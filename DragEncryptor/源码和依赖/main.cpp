#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <windows.h>
#include <commdlg.h>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/err.h>

using namespace std;

// ????
vector<unsigned char> g_key;

// ????
bool endsWith(const string& str, const string& suffix) {
    if (str.length() < suffix.length()) return false;
    return str.compare(str.length() - suffix.length(), suffix.length(), suffix) == 0;
}

// ???????
void loadOrCreateKey() {
    string keyFile = "encryptor.key";
    ifstream in(keyFile, ios::binary);
    if (in.good()) {
        g_key.assign(istreambuf_iterator<char>(in), istreambuf_iterator<char>());
        cout << "Loaded key from: " << keyFile << endl;
    } else {
        g_key.resize(32);
        RAND_bytes(g_key.data(), 32);
        ofstream out(keyFile, ios::binary);
        out.write(reinterpret_cast<char*>(g_key.data()), g_key.size());
        cout << "Generated new key: " << keyFile << endl;
    }
}

// ????
bool encryptFile(const string& inputPath) {
    string outputPath = inputPath + ".enc";
    ifstream in(inputPath, ios::binary);
    if (!in) {
        cerr << "Cannot open: " << inputPath << endl;
        return false;
    }
    in.seekg(0, ios::end);
    size_t fileSize = in.tellg();
    in.seekg(0, ios::beg);
    vector<unsigned char> plaintext(fileSize);
    in.read(reinterpret_cast<char*>(plaintext.data()), fileSize);
    in.close();
    
    cout << "Encrypting: " << inputPath << " (" << fileSize << " bytes)" << endl;
    
    vector<unsigned char> iv(12);
    RAND_bytes(iv.data(), 12);
    
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    vector<unsigned char> ciphertext(fileSize + 16);
    vector<unsigned char> tag(16);
    int len = 0, ciphertext_len = 0;
    
    EVP_EncryptInit_ex(ctx, EVP_aes_256_gcm(), NULL, NULL, NULL);
    EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_IVLEN, 12, NULL);
    EVP_EncryptInit_ex(ctx, NULL, NULL, g_key.data(), iv.data());
    EVP_EncryptUpdate(ctx, ciphertext.data(), &len, plaintext.data(), fileSize);
    ciphertext_len = len;
    EVP_EncryptFinal_ex(ctx, ciphertext.data() + len, &len);
    ciphertext_len += len;
    EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_GET_TAG, 16, tag.data());
    EVP_CIPHER_CTX_free(ctx);
    
    ofstream out(outputPath, ios::binary);
    out.write(reinterpret_cast<char*>(iv.data()), 12);
    out.write(reinterpret_cast<char*>(ciphertext.data()), ciphertext_len);
    out.write(reinterpret_cast<char*>(tag.data()), 16);
    out.close();
    
    cout << "  -> Saved as: " << outputPath << endl;
    return true;
}

// ????
bool decryptFile(const string& inputPath) {
    if (!endsWith(inputPath, ".enc")) {
        cerr << "Not a .enc file: " << inputPath << endl;
        return false;
    }
    
    string outputPath = inputPath.substr(0, inputPath.length() - 4);
    ifstream in(inputPath, ios::binary);
    if (!in) {
        cerr << "Cannot open: " << inputPath << endl;
        return false;
    }
    in.seekg(0, ios::end);
    size_t fileSize = in.tellg();
    in.seekg(0, ios::beg);
    
    if (fileSize < 28) {
        cerr << "File too small: " << inputPath << endl;
        return false;
    }
    
    vector<unsigned char> encrypted(fileSize);
    in.read(reinterpret_cast<char*>(encrypted.data()), fileSize);
    in.close();
    
    cout << "Decrypting: " << inputPath << " (" << fileSize << " bytes)" << endl;
    
    vector<unsigned char> iv(encrypted.begin(), encrypted.begin() + 12);
    vector<unsigned char> tag(encrypted.end() - 16, encrypted.end());
    vector<unsigned char> ciphertext(encrypted.begin() + 12, encrypted.end() - 16);
    
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    vector<unsigned char> plaintext(ciphertext.size());
    int len = 0, plaintext_len = 0;
    
    EVP_DecryptInit_ex(ctx, EVP_aes_256_gcm(), NULL, NULL, NULL);
    EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_IVLEN, 12, NULL);
    EVP_DecryptInit_ex(ctx, NULL, NULL, g_key.data(), iv.data());
    EVP_DecryptUpdate(ctx, plaintext.data(), &len, ciphertext.data(), ciphertext.size());
    plaintext_len = len;
    EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_TAG, 16, tag.data());
    
    int ret = EVP_DecryptFinal_ex(ctx, plaintext.data() + len, &len);
    EVP_CIPHER_CTX_free(ctx);
    
    if (ret != 1) {
        cerr << "  ERROR: Decryption failed! Wrong key or corrupted file." << endl;
        return false;
    }
    plaintext_len += len;
    
    ofstream out(outputPath, ios::binary);
    out.write(reinterpret_cast<char*>(plaintext.data()), plaintext_len);
    out.close();
    
    cout << "  -> Restored as: " << outputPath << endl;
    return true;
}

// ?????????
vector<string> OpenFileDialog() {
    vector<string> files;
    OPENFILENAMEA ofn;
    char szFile[65536] = {0};
    char szDir[MAX_PATH] = {0};
    
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = GetForegroundWindow();
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = "All Files (*.*)\0*.*\0";
    ofn.nFilterIndex = 1;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_ALLOWMULTISELECT;
    ofn.lpstrTitle = "Select files to encrypt/decrypt";
    
    if (!GetOpenFileNameA(&ofn)) {
        return files;
    }
    
    char* p = szFile;
    strcpy_s(szDir, sizeof(szDir), p);
    p += strlen(p) + 1;
    
    if (*p == 0) {
        files.push_back(string(szFile));
    } else {
        while (*p) {
            char szFullPath[MAX_PATH * 2];
            sprintf_s(szFullPath, sizeof(szFullPath), "%s\\%s", szDir, p);
            files.push_back(string(szFullPath));
            p += strlen(p) + 1;
        }
    }
    return files;
}

int main(int argc, char* argv[]) {
    cout << "=== Drag & Drop Encryptor ===" << endl;
    cout << "Usage:" << endl;
    cout << "  1. Drag files onto .exe -> encrypt/decrypt" << endl;
    cout << "  2. Double click -> open file dialog" << endl;
    cout << "  3. Right click -> encrypt/decrypt" << endl;
    cout << endl;
    
    OpenSSL_add_all_algorithms();
    ERR_load_crypto_strings();
    loadOrCreateKey();
    
    cout << "Key: ";
    for (int i = 0; i < 4; i++) printf("%02X", g_key[i]);
    cout << "..." << endl;
    cout << endl;
    
    vector<string> files;
    
    if (argc > 1) {
        for (int i = 1; i < argc; i++) {
            files.push_back(argv[i]);
        }
        cout << "Processing " << files.size() << " file(s)..." << endl;
    } else {
        cout << "Opening file dialog..." << endl;
        files = OpenFileDialog();
        if (files.empty()) {
            cout << "No files selected." << endl;
            cout << "Press Enter to exit...";
            cin.get();
            EVP_cleanup();
            ERR_free_strings();
            return 0;
        }
        cout << "Processing " << files.size() << " file(s)..." << endl;
    }
    
    for (const auto& file : files) {
        if (endsWith(file, ".enc")) {
            decryptFile(file);
        } else {
            encryptFile(file);
        }
    }
    
    cout << endl << "Done!" << endl;
    cout << "Press Enter to exit...";
    cin.get();
    
    EVP_cleanup();
    ERR_free_strings();
    return 0;
}

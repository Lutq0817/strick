#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/err.h>

using namespace std;

class SimpleEncryptor {
private:
    vector<unsigned char> key;
    
    void loadOrCreateKey() {
        string keyFile = "encryptor.key";
        ifstream in(keyFile, ios::binary);
        if (in.good()) {
            key.assign(istreambuf_iterator<char>(in), istreambuf_iterator<char>());
            cout << "Loaded key from: " << keyFile << endl;
        } else {
            key.resize(32);
            RAND_bytes(key.data(), 32);
            ofstream out(keyFile, ios::binary);
            out.write(reinterpret_cast<char*>(key.data()), key.size());
            cout << "Generated new key: " << keyFile << endl;
        }
    }
    
    bool endsWith(const string& str, const string& suffix) {
        if (str.length() < suffix.length()) return false;
        return str.compare(str.length() - suffix.length(), suffix.length(), suffix) == 0;
    }
    
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
        vector<unsigned char> ciphertext(fileSize + 256);
        vector<unsigned char> tag(16);
        
        int len = 0, ciphertext_len = 0;
        
        if (EVP_EncryptInit_ex(ctx, EVP_aes_256_gcm(), NULL, NULL, NULL) != 1 ||
            EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_IVLEN, 12, NULL) != 1 ||
            EVP_EncryptInit_ex(ctx, NULL, NULL, key.data(), iv.data()) != 1) {
            cerr << "Encryption init failed" << endl;
            EVP_CIPHER_CTX_free(ctx);
            return false;
        }
        
        EVP_EncryptUpdate(ctx, ciphertext.data(), &len, plaintext.data(), fileSize);
        ciphertext_len = len;
        EVP_EncryptFinal_ex(ctx, ciphertext.data() + len, &len);
        ciphertext_len += len;
        EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_GET_TAG, 16, tag.data());
        EVP_CIPHER_CTX_free(ctx);
        
        ofstream out(outputPath, ios::binary);
        if (!out) {
            cerr << "Cannot create: " << outputPath << endl;
            return false;
        }
        
        out.write(reinterpret_cast<char*>(iv.data()), 12);
        out.write(reinterpret_cast<char*>(ciphertext.data()), ciphertext_len);
        out.write(reinterpret_cast<char*>(tag.data()), 16);
        out.close();
        
        cout << "  -> Saved as: " << outputPath << endl;
        return true;
    }
    
public:
    SimpleEncryptor() {
        OpenSSL_add_all_algorithms();
        ERR_load_crypto_strings();
        loadOrCreateKey();
    }
    
    ~SimpleEncryptor() {
        EVP_cleanup();
        ERR_free_strings();
    }
    
    void processFile(const string& filepath) {
        if (endsWith(filepath, ".enc")) {
            // ????????????????????
            cout << "This is an encrypted file: " << filepath << endl;
        } else {
            encryptFile(filepath);
        }
    }
    
    void printKeyInfo() {
        cout << "Encryptor ready. Key: ";
        for (int i = 0; i < 4; i++) printf("%02X", key[i]);
        cout << "..." << endl;
    }
};

int main(int argc, char* argv[]) {
    cout << "=== Simple Drag & Drop Encryptor ===" << endl;
    cout << "Drag files onto this .exe to encrypt" << endl;
    cout << "Encrypted files will have .enc suffix" << endl;
    cout << endl;
    
    SimpleEncryptor encryptor;
    encryptor.printKeyInfo();
    
    if (argc > 1) {
        cout << "Processing " << (argc - 1) << " file(s)..." << endl;
        for (int i = 1; i < argc; i++) {
            encryptor.processFile(argv[i]);
        }
        cout << endl << "Done!" << endl;
    } else {
        cout << endl << "No files specified." << endl;
        cout << "Usage: Drag files onto this .exe" << endl;
    }
    
    cout << endl << "Press Enter to exit...";
    cin.get();
    
    return 0;
}

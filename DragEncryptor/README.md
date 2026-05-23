# DragEncryptor

一个拖拽式文件加密工具，使用 AES-256-GCM 加密算法。

## 快速开始（直接使用）

本目录已包含：
- `DragEncryptor_Setup.exe` - 安装包（双击安装即可使用）
- `libcrypto-4-x64.dll` - OpenSSL 加密库
- `libssl-4-x64.dll` - OpenSSL SSL/TLS 库

### 使用方法
1. 双击 `DragEncryptor_Setup.exe` 安装
2. 把文件拖到桌面图标上即可加密/解密

### 从源码编译
1. 安装 OpenSSL 4.0.0 开发库
2. 执行：`g++ -std=c++11 main.cpp -o DragEncryptor.exe -I"OpenSSL/include" -L"OpenSSL/lib" -llibcrypto -llibssl`
3. 将 DLL 放在 exe 同目录下运行

## 文件说明

| 文件 | 说明 |
|------|------|
| `DragEncryptor_Setup.exe` | 安装包（可直接安装） |
| `libcrypto-4-x64.dll` | OpenSSL 运行时库 |
| `libssl-4-x64.dll` | OpenSSL 运行时库 |
| `main.cpp` | 源代码 |
| `encryptor.key` | 密钥文件（首次运行自动生成） |

## ⚠️ 开发者注意

本目录包含一些未删除干净的测试文件，请开发者酌情考虑。

## 作者

Lutq0817

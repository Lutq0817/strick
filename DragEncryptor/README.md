# DragEncryptor

一个拖拽式文件加密工具，使用 AES-256-GCM 加密算法。

## 目录结构

| 文件夹/文件 | 说明 |
|------------|------|
| `源码和依赖/main.cpp` | 源代码 |
| `源码和依赖/libcrypto-4-x64.dll` | OpenSSL 加密库 |
| `源码和依赖/libssl-4-x64.dll` | OpenSSL SSL/TLS 库 |
| `安装包/DragEncryptor_Setup.exe` | 一键安装程序 |

## 使用方法

### 方式一：安装包（推荐）
1. 进入 `安装包` 文件夹
2. 双击 `DragEncryptor_Setup.exe` 安装
3. 把文件拖到桌面图标上即可

### 方式二：源码运行
1. 进入 `源码和依赖` 文件夹
2. 编译：`g++ -std=c++11 main.cpp -o DragEncryptor.exe`
3. 运行 `DragEncryptor.exe`，拖文件上去

## 功能
- 加密：拖文件到 exe → 生成 `.enc`
- 解密：拖 `.enc` 到 exe → 恢复原文件

## 作者
Lutq0817

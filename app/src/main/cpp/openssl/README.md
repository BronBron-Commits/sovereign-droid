# OpenSSL for Android

This directory should contain prebuilt OpenSSL libraries for Android.

## Quick Setup (Recommended)

Download prebuilt OpenSSL 3.0+ for Android from a trusted source and extract here:

```
openssl/
├── arm64-v8a/
│   ├── include/
│   │   └── openssl/
│   │       ├── evp.h
│   │       ├── rand.h
│   │       ├── sha.h
│   │       └── ... (other headers)
│   └── lib/
│       ├── libcrypto.a
│       └── libssl.a
└── armeabi-v7a/
    ├── include/
    └── lib/
```

## Download Sources

1. **Official prebuilts**: https://github.com/openssl/openssl (build from source)
2. **Community prebuilts**: Search for "openssl-android prebuilt" or use vcpkg/conan

## Build from Source (Advanced)

If building OpenSSL from source:
```bash
git clone https://github.com/openssl/openssl.git
cd openssl
./Configure android-arm64 -D__ANDROID_API__=24
make
```

Repeat for armeabi-v7a with `android-arm` target.

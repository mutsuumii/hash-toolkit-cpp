# hash-toolkit-cpp ⚡

File hash toolkit in C++20 with OpenSSL. Compute, verify, and compare hashes.

## Algorithms
MD5, SHA-1, SHA-256, SHA-512 (via OpenSSL EVP)

## Build
```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
./build/hash_toolkit file myfile.bin sha256
./build/hash_toolkit dir ./folder md5
./build/hash_toolkit verify myfile.bin abc123def456...
```

## License
MIT

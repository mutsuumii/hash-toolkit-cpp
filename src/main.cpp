#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <iomanip>
#include <sstream>
#include <filesystem>
#include <openssl/evp.h>

namespace fs = std::filesystem;

std::string hash_file(const std::string& path, const std::string& algo) {
    const EVP_MD* md = EVP_get_digestbyname(algo.c_str());
    if (!md) {
        std::cerr << "Unknown algorithm: " << algo << "\n";
        return "";
    }

    std::ifstream file(path, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Cannot open: " << path << "\n";
        return "";
    }

    EVP_MD_CTX* ctx = EVP_MD_CTX_new();
    EVP_DigestInit_ex(ctx, md, nullptr);

    char buf[8192];
    while (file.read(buf, sizeof(buf)) || file.gcount() > 0) {
        EVP_DigestUpdate(ctx, buf, file.gcount());
    }

    unsigned char hash[EVP_MAX_MD_SIZE];
    unsigned int len = 0;
    EVP_DigestFinal_ex(ctx, hash, &len);
    EVP_MD_CTX_free(ctx);

    std::ostringstream oss;
    for (unsigned int i = 0; i < len; i++) {
        oss << std::hex << std::setfill('0') << std::setw(2)
            << static_cast<int>(hash[i]);
    }
    return oss.str();
}

std::string hash_string(const std::string& text, const std::string& algo) {
    const EVP_MD* md = EVP_get_digestbyname(algo.c_str());
    if (!md) return "Unknown algorithm";

    unsigned char hash[EVP_MAX_MD_SIZE];
    unsigned int len = 0;
    EVP_MD_CTX* ctx = EVP_MD_CTX_new();
    EVP_DigestInit_ex(ctx, md, nullptr);
    EVP_DigestUpdate(ctx, text.c_str(), text.size());
    EVP_DigestFinal_ex(ctx, hash, &len);
    EVP_MD_CTX_free(ctx);

    std::ostringstream oss;
    for (unsigned int i = 0; i < len; i++) {
        oss << std::hex << std::setfill('0') << std::setw(2)
            << static_cast<int>(hash[i]);
    }
    return oss.str();
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cout << "Usage:\n"
                  << "  hash_toolkit file <path> [algo=sha256]\n"
                  << "  hash_toolkit string <text> [algo=sha256]\n"
                  << "  hash_toolkit dir <path> [algo=sha256]\n"
                  << "  hash_toolkit verify <path> <expected> [algo=sha256]\n"
                  << "\nAlgorithms: md5, sha1, sha256, sha512\n";
        return 1;
    }

    std::string cmd = argv[1];
    std::string algo = "sha256";

    if (cmd == "file") {
        if (argc > 3) algo = argv[3];
        auto h = hash_file(argv[2], algo);
        if (!h.empty()) std::cout << algo << ": " << h << "  " << argv[2] << "\n";
    }
    else if (cmd == "string") {
        if (argc > 3) algo = argv[3];
        std::cout << algo << ": " << hash_string(argv[2], algo) << "\n";
    }
    else if (cmd == "dir") {
        if (argc > 3) algo = argv[3];
        for (auto& entry : fs::recursive_directory_iterator(argv[2])) {
            if (entry.is_regular_file()) {
                auto h = hash_file(entry.path().string(), algo);
                if (!h.empty()) std::cout << h << "  " << entry.path().string() << "\n";
            }
        }
    }
    else if (cmd == "verify") {
        if (argc < 4) { std::cerr << "Need: verify <path> <expected>\n"; return 1; }
        if (argc > 4) algo = argv[4];
        auto h = hash_file(argv[2], algo);
        std::string expected = argv[3];
        // lowercase compare
        std::transform(h.begin(), h.end(), h.begin(), ::tolower);
        std::transform(expected.begin(), expected.end(), expected.begin(), ::tolower);
        if (h == expected) {
            std::cout << "[MATCH] " << argv[2] << "\n";
        } else {
            std::cout << "[MISMATCH] " << argv[2] << "\n";
            std::cout << "  Expected: " << expected << "\n";
            std::cout << "  Got:      " << h << "\n";
            return 1;
        }
    }

    return 0;
}

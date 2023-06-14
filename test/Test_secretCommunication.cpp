#include <iostream>
#include <cassert>
#include "secret.h"

int main() {
    std::string input = "Hello, world!";
    std::string key = "password";
    
    // Test encryption and decryption
    std::string encrypted = encrypt(input, key);
    std::string decrypted = decrypt(encrypted, key);
    assert(input == decrypted);
    
    // Test encode and decode
    std::string encoded = encode(input);
    std::string decoded = decode(encoded);
    assert(input == decoded);
    
    std::cout << "All tests passed!" << std::endl;
    
    return 0;
}

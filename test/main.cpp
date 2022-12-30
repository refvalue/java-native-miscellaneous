#include <algorithm>

#include <g6/char8_t_remediation.hpp>
#include <g6/cryptography.hpp>
#include <g6/util/symmetric_homomorphic_decryptor.hpp>
#include <g6/util/symmetric_homomorphic_encryptor.hpp>

using namespace glasssix;

int main() {
    auto key = symmetric_homomorphic_encryptor::generate_secret_key();
    symmetric_homomorphic_encryptor encryptor{key};
    symmetric_homomorphic_decryptor decryptor{key};

    auto x = encryptor.process(U8("fdsfdfsdfsdfsdfsfsfsfsfsf"));
    auto y = encryptor.process(U8("dfgdfgfdg"));

    auto c = x.contains(y);

    auto z = decryptor.process_as_string(y);
    auto z1 = decryptor.process_as_string(x);
    
    auto i = 3;
}

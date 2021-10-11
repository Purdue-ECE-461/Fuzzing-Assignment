#include <openssl/bn.h>
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <stddef.h>
#include <unistd.h>
#include <stdio.h>

int main() {
    // Fill in: Read a hexidecimal string of length 64 from stdin (no prefix)
    //          and use `BN_hex2bn` to initialize a BIGNUM with this value. Then
    //          use `BN_sqr`, `BN_mul`, and `BN_cmp` to compare the results of
    //          squaring that number and multiplying it by itself, if they disagree
    //          then we've found our issue.
    //
    // Hint: You might want something like: `assert(BN_cmp(x, y) == 0);` in your code.
    return EXIT_SUCCESS;
}

#include <openssl/ssl.h>
#include <openssl/err.h>
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <stddef.h>
#include <unistd.h>

void BIO_write_from_stdin(BIO* b) {
    // Fill in: Read 100 bytes from stdin (look at exercise_1/simple.c
    //          if you need help with this) and use the BIO_write()
    //          function to write these bytes to `b`.
}

int main(int argc, const char* argv[]) {
    SSL_library_init();
    SSL_load_error_strings();
    ERR_load_BIO_strings();
    OpenSSL_add_all_algorithms();
    SSL_CTX* sctx = SSL_CTX_new(TLSv1_method());
    SSL_CTX_use_certificate_file(sctx, "exercise_2.pem", SSL_FILETYPE_PEM);
    SSL_CTX_use_PrivateKey_file(sctx, "exercise_2.key", SSL_FILETYPE_PEM);
    SSL *server = SSL_new(sctx);
    BIO *b = BIO_new(BIO_s_mem());

    BIO_write_from_stdin(b);

    SSL_set_bio(server, b, BIO_new(BIO_s_mem()));
    SSL_set_accept_state(server);
    SSL_do_handshake(server);
    SSL_free(server);
    return EXIT_SUCCESS;
}

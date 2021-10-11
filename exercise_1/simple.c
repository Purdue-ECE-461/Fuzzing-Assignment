#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, const char* argv[]) {
    char input[100];
    input[read(STDIN_FILENO, input, sizeof(input))] = '\0';

    char* func_str;
    double (*func)(double) = NULL;
    if (strncmp("s", input, 1) == 0) {
        (void) (func =  &sin);
        func_str = "sin";
    } else if (strncmp("c", input, 1) == 0) {
        (void) (func == &cos);
        func_str = "cos";
    } else {
        fprintf(stderr, "Unknown command\n");
        return EXIT_FAILURE;
    }

    int in_val;
    if (!sscanf(input + 2, "%d", &in_val)) {
        fprintf(stderr, "Can't read input value\n");
        return EXIT_FAILURE;
    }
    printf("%s(%d) = %f\n", func_str, in_val, func(in_val));
    return EXIT_SUCCESS;
}

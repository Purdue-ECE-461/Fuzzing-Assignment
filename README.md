# Fuzzing

## Setup:

This assignment has two main dependencies, AFL and an old version of OpenSSL.

In order to set up AFL, run the following in this directory:
```bash
$ git submodule init
$ git submodule update
$ cd AFL
$ make
$ export AFL_I_DONT_CARE_ABOUT_MISSING_CRASHES=1 # required to run on this VM
$ export AFL_USE_ASAN=1 # check for memory corruption
$ export PATH=$PATH:$(pwd)
$ export AFL_PATH=$(pwd)
$ cd ..
```

In order to set up OpenSSL, run the following:
```bash
$ cd openssl
$ CC=afl-gcc CXX=afl-g++ ./config -d
$ make # This will take 4 minutes, maybe get a cup of tea :)
$ cd ..
```

## Exercise 1:
The first exercise is a simple example of fuzzing a broken program.
This will familiarize you with the basics of running AFL, in the later exercises
the "targets" (what you're fuzzing) will be less trivial.

### Step 1: Compile the program with `afl-gcc`
```bash
$ cd <path to exercise 1>
$ afl-gcc -g simple.c -lm -o simple
```

### Step 2: Run the program with `afl-fuzz`
```bash
$ # `-i in` sets the input directory, which is at least 1 test case
$ # `-o out` sets the output directory
$ # `-m none` removes memory  limits on AFL, which is required when using the address sanitizer 
$ afl-fuzz -i in -o out -m none ./simple
```
You should immediately see at least 1 crash reported on the AFL dashboard.

### Step 3: Understand simple.c

Look at `simple.c`, notice that it reads its input from `stdin` (which is
required for AFL). Investigate lines 14 and 17, does something look off?


## Exercise 2:
As promised, the target for this exercise is less trivial -- we'll
be finding the heartbleed vulnerability in OpenSSL. You also have
more responsibility for this exercise, as you'll be writing a small
"harness" for AFL to use when fuzzing.

### Step 1: Write the harness

Open up `handshake_fuzz.c` and locate the `BIO_write_from_stdin` function.
Don't worry if you don't understand the `main` function, your goal is to
fill in the `BIO_write_from_stdin` function as described in the comment
(this should only take a few lines of code).
Unless you're already fluent in the OpenSSL API, you'll need to Google
the `BIO_write` function. 

### Step 2: Compile the code
```bash
$ cd <path to exercise 2>
$ afl-gcc -g handshake_fuzz.c ../openssl/libssl.a ../openssl/libcrypto.a -o handshake_fuzz -I ../openssl/include/ -ldl
```

### Step 3: Fuzz (vigorously)

OpenSSL is quite a bit bigger than the previous `simple.c` we fuzzed.
To maximize the performance of our fuzzing (and ultimately, not waste your time),
we should run multiple instances of AFL in parallel. In your main terminal, run
the following to spawn a main fuzzer process:
```bash
$ afl-fuzz -i in/ -o out -m none -M fuzzer_01 ./handshake_fuzz
```

Now open 3 new terminal instances and spawn several  follower fuzzers. In each
new terminal run the following (make sure to replace anything in `<>`):
```bash
$ export AFL_I_DONT_CARE_ABOUT_MISSING_CRASHES=1
$ cd <path to exercise_2>
$ ../AFL/afl-fuzz -i in -o out -m none -S <unique fuzzer name> ./handshake_fuzz
```

Once you've spawned 4 total AFL processes (1 leader and 3 followers), monitor
the output of each process. Within about 5 minutes you should see a crash on
the dashboard of a process. You can terminate your processes (and extra terminals)
after you see this crash.

### Step 4: Investigate the crash

Run the `handshake_fuzz` executable with crashing input like so:
```bash
$ # the path and name of the crash will be different for you!
$ ./handshake_fuzz < out/<fuzzer that produced the crash>/crashes/id\:000000.... # truncated
```

You should see error output from the address sanitizer. In this output you should
see something from a `heartbeat`, you've found heartbleed!


## Exercise 3:
So far we've seen fuzzing detect obvious memory error and crashes, but that's not
all fuzzing can do. We can also fuzz to check for violations of invariants in our
code. An example of such an invariant might be "for any integer `x`, the square of
`x` should be equivalent to `x * x`".

Beyond being vulnerable to heartbleed, the version of OpenSSL used in this
assignment is also vulnerable to [CVE-2014-4370](https://nvd.nist.gov/vuln/detail/CVE-2014-3570)
-- a bug where the square of a big number `x` wasn't equal to `x * x`.

In this exercise you'll be finding this failed invariant by comparing `BN_sqr` on
a `BIGNUM` with using `BN_mul` to multiple a `BIGNUM` by itself.

> Note: AFL will treat a failed `assert` statement as a crash.

### Step 1: Write the fuzzing harness

Open up `bn_test.c`, write the `main` function such that it performs to the spec given.
We're not expecting you to read `openssl/bn.h` to understand the API, feel free to
use Google (or Bing if you're into that) to find documentation and example code about OpenSSL `BIGNUM`s. 

### Step 2: Compile `bn_test.c`
```bash
$ cd <path to exercise 3>
$ afl-gcc bn_test.c ../openssl/libcrypto.a -o bn_test -I ../openssl/include/ -ldl
```

After compilation, you should be able to run the following and have no errors:
```bash
$ echo "D3ADB33F" | ./bn_test 
```

### Step 3: Fuzz
```bash
$ afl-fuzz -i in -o out -m none ./bn_test
```

If your harness code works, you should see at least one crash within 3 minutes.

### Step 4: Inspect the offending string

Look at the test case that caused the error:
```bash
$ cat out/crashes/id\:000000...... # truncated
```

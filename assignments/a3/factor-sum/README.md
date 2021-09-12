# sum of smallest non-trivial factors for a set of numbers

* reads in a positive integers in range (2 .. 2^63-1) from standard input
* initialize sum = 0
* for each number on input:
  * if the number is prime, it is ignored
  * determine the smallest factor of the composite number
  * add the smallest factor to a sum
* prints out the sum of all smallest factors

if no composite numbers are found, prints out "0"

compile with:
```sh
$ make
```

quick test:
```console
$ echo "0 3 19 25 4012009 165 1033" | ./sumFactors 5
Using 5 threads.
Sum of divisors = 2011
$ echo "3 5" | ./sumFactors
Using 1 thread.
Sum of divisors = 0

```


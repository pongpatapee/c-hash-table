#include <math.h>

/*
---------
Resizing
---------

The prime calculation will be used for resizing the hash table.

Depending on the load of the table, we want to half or double the size
of the table. However, we want the table size to be a prime number.

To find the prime number for the table size, we will first define a base size,
one in which we will half or double, then we will calculate the next prime
number from the new base size.

We will brute force search from the base size to the next prime number.
Brute forcing here is fine since the number of searches until the next prime
is actually quite low, and will be outweighed by time time spent on re-hashing
items from the table
*/

/*
 * Returns:
 * 1 - is prime
 * 0 - not prime
 * -1 - undefined
 */
int is_prime(const int x) {
  if (x < 2)
    return -1;

  if (x < 4)
    return 1;

  if (x % 2 == 0)
    return 0;

  for (int i = 3; i <= floor(sqrt((double)x)); i += 2) {
    // if x is divisible by a prime number it is not a prime number
    if ((x % i) == 0) {
      return 0;
    }
  }
  return 1;
}

int next_prime(int x) {
  while (is_prime(x) != 1) {
    x++;
  }

  return x;
}

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hash_table.h"

#define HT_PRIME1 151
#define HT_PRIME2 163

// static in C means only visible in the current file
static ht_item* ht_new_item(const char* k, const char* v) {
  ht_item* i = malloc(sizeof(ht_item));
  i->key = strdup(k);
  i->value = strdup(v);

  return i;
}

ht_hash_table* ht_new() {
  ht_hash_table* ht = malloc(sizeof(ht_hash_table));

  ht->size = 53;
  ht->count = 0;
  // Allocate memory with NULL bytes
  ht->items = calloc((size_t)ht->size, sizeof(ht_item*));
  return ht;
}

static void ht_del_item(ht_item* item) {
  free(item->key);
  free(item->value);
  free(item);
}

void ht_del_hash_table(ht_hash_table* ht) {
  for (int i = 0; i < ht->size; i++) {
    if (ht->items[i] != NULL) {
      ht_del_item(ht->items[i]);
    }
  }

  free(ht->items);
  free(ht);
}

/*
-------------
Hashing algo
-------------
Hash function needs to return an even distribution of bucket
indexes. Otherwise it will lead to a higher rate of collision
which will reduce the efficiency of the hashtable


`a` should be a prime that is larger than the number of alphabets
In our case with ASCII, that is 128. Therefore `a` will be 151.

What the hashing function is doing:
1. Generate a really large number
2. Reduce the number to a fixed range with mod
*/

static int ht_hash(const char* s, const int a, const int m) {
  long hash = 0;
  const int len_s = strlen(s);
  for (int i = 0; i < len_s; i++) {
    // s[i] is the int ascii value of the char at position i
    hash += (long)pow(a, len_s - (i + 1)) * s[i];
    hash = hash % m;
  }

  return (int)hash;
}

/*
----------
Collision
----------
Hash functions maps an infinitely large number of inputs
to a finite number of output. Thus, different keys will
map to the same array bucket.

Our collision handler will use open-addressing with
double hashing. Double hashing make use of two hash
to calculate the index after i collisions.

Double hashing:
index = (hash_a(string) + i * (hash_b(string) + 1)) % num_buckets

*/

static int ht_get_hash(const char* s, const int num_buckets,
                       const int attempt) {
  const int hash_a = ht_hash(s, HT_PRIME1, num_buckets);
  const int hash_b = ht_hash(s, HT_PRIME2, num_buckets);

  return (hash_a + attempt * (hash_b + 1)) % num_buckets;
}

int main() {
  printf("C hash table\n");

  // ht_hash_table* ht = ht_new();
  // printf("%p\n", ht);
  // ht_del_hash_table(ht);
  printf("Calling ht_get_hash\n");
  char* key = "some random key";
  int hash = ht_get_hash(key, 40, 10);
  printf("Key: %s, Hash: %d\n", key, hash);

  return 0;
}

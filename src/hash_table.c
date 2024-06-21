#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hash_table.h"
#include "prime.h"

#define HT_PRIME1 151
#define HT_PRIME2 163
#define HT_INITIAL_BASE_SIZE 53

static ht_item HT_DELETED_ITEM = {NULL, NULL};

// static in C means only visible in the current file
static ht_item* ht_new_item(const char* k, const char* v) {
  ht_item* i = malloc(sizeof(ht_item));
  i->key = strdup(k);
  i->value = strdup(v);

  return i;
}

static void ht_del_item(ht_item* item) {
  // printf("Deleting item: %p, k:%s, v:%s\n", item, item->key, item->value);
  free(item->key);
  free(item->value);
  free(item);
}

ht_hash_table* ht_new_sized(const int base_size) {
  ht_hash_table* ht = malloc(sizeof(ht_hash_table));
  ht->base_size = base_size;
  ht->size = next_prime(ht->base_size);
  ht->count = 0;
  // Allocate memory with NULL bytes
  ht->items = calloc((size_t)ht->size, sizeof(ht_item*));
  return ht;
}

ht_hash_table* ht_new() { return ht_new_sized(HT_INITIAL_BASE_SIZE); }

void ht_del_hash_table(ht_hash_table* ht) {
  for (int i = 0; i < ht->size; i++) {
    if (ht->items[i] != NULL && ht->items[i] != &HT_DELETED_ITEM) {
      ht_del_item(ht->items[i]);
    }
  }

  free(ht->items);
  free(ht);
}

static void ht_resize(ht_hash_table* ht, const int base_size) {
  if (base_size < HT_INITIAL_BASE_SIZE) {
    return;
  }

  ht_hash_table* new_ht = ht_new_sized(base_size);

  for (int i = 0; i < ht->size; i++) {
    ht_item* curr_item = ht->items[i];
    if (curr_item != NULL && curr_item != &HT_DELETED_ITEM) {
      ht_insert(new_ht, curr_item->key, curr_item->value);
    }
  }

  // swapping attributes from new_ht to ht instead of returning new_ht,
  // since we want the pointer to the ht to remain the same even after
  // resizing

  ht->base_size = new_ht->base_size;
  ht->count = new_ht->count;

  // Cannot just override value of size because
  // new_ht will need the old size value for item deletion
  const int temp_size = ht->size;
  ht->size = new_ht->size;
  new_ht->size = temp_size;

  ht_item** temp_items = ht->items;
  ht->items = new_ht->items;
  new_ht->items = temp_items;

  printf("Deleting old hash table\n");
  ht_del_hash_table(new_ht);
}

static void ht_resize_up(ht_hash_table* ht) {
  printf("Resizing hash table up\n");
  const int new_size = ht->base_size * 2;
  ht_resize(ht, new_size);
}

static void ht_resize_down(ht_hash_table* ht) {
  printf("Resizing hash table down\n");
  const int new_size = ht->base_size / 2;
  ht_resize(ht, new_size);
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

  int new_hash = (hash_a + attempt * (hash_b + 1)) % num_buckets;

  // printf("Key: %s, attempt: %d, new_ind: %d\n", s, attempt, new_hash);

  return new_hash;
}

// API methods for hash table
void ht_insert(ht_hash_table* ht, const char* key, const char* value) {
  // printf("Inserting k:%s, v:%s\n", key, value);

  const int load = ht->count * 100 / ht->size;
  if (load > 70) {
    ht_resize_up(ht);
  }

  ht_item* item = ht_new_item(key, value);
  int attempts = 0;
  int index = ht_get_hash(key, ht->size, attempts);
  ht_item* curr_item = ht->items[index];

  while (curr_item != NULL) {
    if (curr_item != &HT_DELETED_ITEM) {
      // Update value if key already exists
      if (strcmp(curr_item->key, key) == 0) {
        ht_del_item(curr_item);
        ht->items[index] = item;
        return;
      }
    }
    index = ht_get_hash(key, ht->size, ++attempts);
    curr_item = ht->items[index];
  }

  ht->items[index] = item;
  ht->count++;
}

char* ht_search(ht_hash_table* ht, const char* key) {
  int attempts = 0;
  int index = ht_get_hash(key, ht->size, attempts);
  ht_item* item = ht->items[index];
  while (item != NULL) {
    if (item != &HT_DELETED_ITEM) {
      if (strcmp(item->key, key) == 0) {
        return item->value;
      }
    }
    index = ht_get_hash(key, ht->size, ++attempts);
    item = ht->items[index];
  }

  return NULL;
}

/*
Deletion is more complicated than insertion and seraching
Since the item we're deleting may be part of a collision chain.

If we mark something as NULL, it might make keys at the tail of the
collision chain impossible to find.

Instead we will simply mark the item as deleted using HT_DELETED_ITEM
*/

void ht_delete(ht_hash_table* ht, const char* key) {
  const int load = ht->count * 100 / ht->size;
  if (load < 10) {
    ht_resize_down(ht);
  }

  int attempts = 0;
  int index = ht_get_hash(key, ht->size, attempts);
  ht_item* item = ht->items[index];
  while (item != NULL) {
    if (item != &HT_DELETED_ITEM) {
      if (strcmp(item->key, key) == 0) {
        ht_del_item(item);
        ht->items[index] = &HT_DELETED_ITEM;
      }
    }
    index = ht_get_hash(key, ht->size, ++attempts);
    item = ht->items[index];
  }
  ht->count--;
}

// My custom print functions
void print_ht_item(ht_item* item) {
  printf("k: %s, v: %s\n", item->key, item->value);
}

void print_ht_hash_table(ht_hash_table* ht) {
  printf("Hash Table: \n");
  printf("size: %d, count: %d, items:\n", ht->size, ht->count);
  for (int i = 0; i < ht->size; i++) {
    if (ht->items[i] == NULL || ht->items[i] == &HT_DELETED_ITEM)
      continue;
    printf("\tindex: %d, ", i);
    print_ht_item(ht->items[i]);
  }
}

int main() {
  printf("C hash table\n");
  ht_hash_table* ht = ht_new();
  ht_insert(ht, "1", "value1");
  ht_insert(ht, "2", "value2");
  ht_insert(ht, "some_key", "some value");
  ht_insert(ht, "joe", "mama");
  print_ht_hash_table(ht);

  char* search_key = "some_key";
  printf("\nsearching for key: %s\n", search_key);
  char* val = ht_search(ht, search_key);
  printf("Value found: %s\n", val);

  char* key_to_del = "joe";
  printf("\ndeleting key: %s\n", key_to_del);
  ht_delete(ht, key_to_del);
  print_ht_hash_table(ht);

  char* key_to_update = "2";
  printf("\nupdating value for key: %s\n", key_to_update);
  ht_insert(ht, key_to_update, "new value2");
  print_ht_hash_table(ht);

  // test resizing up
  printf("Adding keys 1 .. 100\n");
  for (int i = 0; i < 100; i++) {
    char key[10];
    char value[30];
    sprintf(value, "value %d", i);
    sprintf(key, "key %d", i);
    ht_insert(ht, key, value);
  }

  print_ht_hash_table(ht);

  // test resizing down
  printf("Deleting keys 1 .. 100\n");
  for (int i = 0; i < 100; i++) {
    char key[10];
    sprintf(key, "key %d", i);
    ht_delete(ht, key);
  }
  print_ht_hash_table(ht);

  return 0;
}

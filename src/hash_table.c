#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hash_table.h"

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

int main() {
  printf("C hash table\n");
  ht_hash_table* ht = ht_new();
  printf("%p\n", ht);
  ht_del_hash_table(ht);

  return 0;
}

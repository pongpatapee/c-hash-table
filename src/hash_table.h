// char* for strings
typedef struct {
  char* key;
  char* value;

} ht_item;

typedef struct {
  int size;
  int count;
  ht_item** items; // Double pointers because we want an Array to item pointers

} ht_hash_table;

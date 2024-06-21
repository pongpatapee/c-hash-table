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

// API for hash_table
void ht_insert(ht_hash_table* ht, const char* key, const char* value);
char* ht_search(ht_hash_table* ht, const char* key);
void ht_delete(ht_hash_table* ht, const char* key);

// My custom print functions
void print_ht_item(ht_item* item);
void print_ht_hash_table(ht_hash_table* ht);

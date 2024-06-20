# Implementing a Hash Table in C

Hash maps/tables are one of the most powerful data structures. Key, value storage is very prominent in a wide variety of problems and a hash table allows us to search for these key, value pairs in O(1) time (on average).

Most high level languages provide this data structure for free. E.g., Python: Dictionary, JavaScript: Object, Go: Maps. Using these languages we've been using the hash maps/tables for free and never thought about how they are able to achieve an O(1) look up time on average. 

Using the following [tutorial](https://github.com/jamesroutley/write-a-hash-table), I will be implementing a hash table in C to get a better understanding of how hash tables work under the hood. For example, how collisions are handled in such a way that it keeps the look up time of a key to remain O(1). 



#ifndef TRIE_H
#define TRIE_H

struct DigitTrieNode;
typedef struct DigitTrieNode dt_node_t;

dt_node_t *dt_init(const size_t length); // returns root
void dt_destroy(dt_node_t *root);
size_t dt_inc(dt_node_t *root, const char *digit_str);
size_t dt_get(dt_node_t *root, const char *digit_str);

#endif // TRIE_H

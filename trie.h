#ifndef TRIE_H
#define TRIE_H

struct DigitTrieNode;
typedef struct DigitTrieNode dt_node_t;

dt_node_t *dt_init(const size_t length);
void dt_destroy(dt_node_t *root);
void dt_set(dt_node_t *root, const char *digit_str, const size_t count);
size_t dt_get(dt_node_t *root, const char *digit_str);

#endif // TRIE_H

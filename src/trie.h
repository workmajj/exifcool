#ifndef DT_TRIE_H
#define DT_TRIE_H

/* DigitTrie */

struct dt_node;
typedef struct dt_node DTNode;

DTNode *dt_init(const size_t length); // returns root
void dt_destroy(DTNode *root);

size_t dt_count(DTNode *root, const char *digit_str);

#endif // DT_TRIE_H
#ifndef TRIE_H
#define TRIE_H

struct DigitTrieNode;
typedef struct DigitTrieNode DTNode;

DTNode *dt_init(const size_t length); // returns root
void dt_destroy(DTNode *root);
size_t dt_inc(DTNode *root, const char *digit_str);
size_t dt_get(DTNode *root, const char *digit_str);

#endif // TRIE_H

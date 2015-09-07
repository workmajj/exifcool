#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "trie.h"

#define DT_SIZE 10 // array size for digits 0-9

/* DigitTrie */

// all keys (digit_str) are strings of digits (e.g., "20140830143607")
// returns 0 (rather than erroring) when getting keys not yet set
// specify fixed string length at init time (e.g., dt_init(14))
// only one instance because of global dt_length state

static size_t dt_length = 0; // uninitialized value

struct DigitTrieNode {
    struct DigitTrieNode *children[DT_SIZE];
    size_t count;
};

/* util */

static int dt_ctoi(const char *c)
{
    char a[2] = {c[0], '\0'};

    return atoi(a);
}

static dt_node_t *dt_alloc()
{
    dt_node_t *n = malloc(sizeof(dt_node_t));
    assert(n != NULL);

    for (size_t i = 0; i < DT_SIZE; i++) n->children[i] = NULL;

    return n;
}

static void dt_free(dt_node_t *n)
{
    assert(n != NULL);

    for (size_t i = 0; i < DT_SIZE; i++) {
        if (n->children[i] != NULL) dt_free(n->children[i]);
    }

    free(n);
}

/* main */

extern dt_node_t *dt_init(const size_t length)
{
    assert(length > 0 && length < SIZE_MAX);

    dt_length = length;

    return dt_alloc();
}

extern void dt_destroy(dt_node_t *root)
{
    assert(dt_length > 0); // init-ed

    dt_length = 0;

    dt_free(root);
}

extern void dt_set(dt_node_t *root, const char *digit_str, const size_t count)
{
    assert(dt_length > 0); // init-ed

    assert(root != NULL);
    assert(digit_str != NULL);
    assert(count < SIZE_MAX);

    size_t len = strnlen(digit_str, dt_length + 1);
    assert(len == dt_length);

    dt_node_t *curr = root;

    for (size_t i = 0; i < len; i++) {
        int idx = dt_ctoi(&digit_str[i]);

        if (!curr->children[idx]) curr->children[idx] = dt_alloc();

        curr = curr->children[idx];
    }

    curr->count = count;
}

extern size_t dt_get(dt_node_t *root, const char *digit_str)
{
    assert(dt_length > 0); // init-ed

    assert(root != NULL);
    assert(digit_str != NULL);

    size_t len = strnlen(digit_str, dt_length + 1);
    assert(len == dt_length);

    dt_node_t *curr = root;

    for (size_t i = 0; i < len; i++) {
        int idx = dt_ctoi(&digit_str[i]);

        if (!curr->children[idx]) return 0; // not yet set

        curr = curr->children[idx];
    }

    return curr->count;
}
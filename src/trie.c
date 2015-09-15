#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "trie.h"

#define DT_SIZE 10 // array size for digits 0-9

// all digit_str keys are strings of digits (e.g., "20140830143607")
// specify fixed strlen at init (doesn't include '\0' byte)
// only one instance allowed because of global dt_length
// func dt_count increments and then returns current count

static size_t dt_length = 0; // uninitialized value

struct dt_node {
    struct dt_node *children[DT_SIZE];
    size_t count;
};

/* util */

static int dt_ctoi(const char *c)
{
    char a[2] = {c[0], 0};

    return atoi(a);
}

static DTNode *dt_alloc()
{
    DTNode *n = malloc(sizeof(DTNode));
    assert(n != NULL);

    for (size_t i = 0; i < DT_SIZE; i++) n->children[i] = NULL;

    return n;
}

static void dt_free(DTNode *n)
{
    assert(n != NULL);

    for (size_t i = 0; i < DT_SIZE; i++) {
        if (n->children[i] != NULL) dt_free(n->children[i]);
    }

    free(n);
}

/* main */

extern DTNode *dt_init(const size_t length)
{
    assert(length > 0);

    dt_length = length;

    return dt_alloc(); // return root
}

extern void dt_destroy(DTNode *root)
{
    assert(dt_length > 0); // init-ed

    dt_length = 0;

    dt_free(root);
}

extern size_t dt_count(DTNode *root, const char *digit_str)
{
    assert(dt_length > 0); // init-ed

    assert(root != NULL);
    assert(digit_str != NULL);

    size_t len = strnlen(digit_str, dt_length + 1);
    assert(len == dt_length);

    DTNode *cur = root;

    for (size_t i = 0; i < len; i++) {
        int idx = dt_ctoi(&digit_str[i]);

        if (!cur->children[idx]) cur->children[idx] = dt_alloc();

        cur = cur->children[idx];
    }

    return cur->count++; // increment then return
}

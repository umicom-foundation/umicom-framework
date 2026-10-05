/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/json_tree/test_value_comparison.c
 * PURPOSE: Check JSON identity comparison across ordering, Unicode, ownership bounds and cancellation.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language_runtime/json_tree.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c)                                                                                             \
    do                                                                                                       \
    {                                                                                                        \
        if (!(c))                                                                                            \
        {                                                                                                    \
            fprintf(stderr, "line %d: %s\n", __LINE__, #c);                                                  \
            return 1;                                                                                        \
        }                                                                                                    \
    } while (0)
typedef struct Vector
{
    const char *name, *left, *right;
    int equal;
} Vector;
static const Vector vectors[] = {{"null", "null", "null", 1},
                                 {"null-bool", "null", "false", 0},
                                 {"bool", "true", "true", 1},
                                 {"bool-change", "true", "false", 0},
                                 {"integer", "123", "123", 1},
                                 {"numeric-spelling", "1", "1.0", 0},
                                 {"negative-zero", "-0", "0", 0},
                                 {"big-integer", "18446744073709551617", "18446744073709551617", 1},
                                 {"string", "\"hello\"", "\"hello\"", 1},
                                 {"escape", "\"a\\u0062\"", "\"ab\"", 1},
                                 {"unicode", "\"\\ud83d\\ude00\"", "\"😀\"", 1},
                                 {"array", "[1,\"b\",null]", "[1,\"b\",null]", 1},
                                 {"array-order", "[1,2]", "[2,1]", 0},
                                 {"array-count", "[1]", "[1,2]", 0},
                                 {"object-order", "{\"b\":2,\"a\":1}", "{\"a\":1,\"b\":2}", 1},
                                 {"key-escape", "{\"\\u0061\":1}", "{\"a\":1}", 1},
                                 {"nested", "{\"x\":[{\"b\":2,\"a\":1}]}", "{\"x\":[{\"a\":1,\"b\":2}]}", 1},
                                 {"key-change", "{\"a\":1}", "{\"b\":1}", 0},
                                 {"value-change", "{\"a\":1}", "{\"a\":2}", 0},
                                 {"object-count", "{}", "{\"a\":1}", 0},
                                 {"empty-array", "[]", "[]", 1},
                                 {"empty-object", "{}", "{}", 1},
                                 {"type-change", "[]", "{}", 0},
                                 {"string-number", "\"1\"", "1", 0}};
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    const char *mode = argv[1], *left = "{\"a\":1}", *right = left;
    int expected = 1, known = 0;
    const char *special[] = {"duplicate-left",
                             "duplicate-right",
                             "duplicate-escaped",
                             "nested-duplicate",
                             "cancel",
                             "null-left",
                             "null-right",
                             "negative-node",
                             "outside-node",
                             "null-output",
                             "subtree",
                             "long-key",
                             "deep",
                             "wide"};
    for (size_t i = 0U; i < sizeof(vectors) / sizeof(vectors[0]); ++i)
        if (strcmp(mode, vectors[i].name) == 0)
        {
            known = 1;
            left = vectors[i].left;
            right = vectors[i].right;
            expected = vectors[i].equal;
        }
    for (size_t i = 0U; i < sizeof(special) / sizeof(special[0]); ++i)
        if (strcmp(mode, special[i]) == 0)
            known = 1;
    CHECK(known);
    UmiStatus wanted = UMI_STATUS_OK;
    char *owned_left = NULL, *owned_right = NULL;
    if (strncmp(mode, "duplicate-", 10U) == 0 || strcmp(mode, "nested-duplicate") == 0)
    {
        wanted = UMI_STATUS_ALREADY_EXISTS;
        if (strcmp(mode, "duplicate-left") == 0)
        {
            left = "{\"a\":1,\"a\":2}";
            right = "{\"a\":1,\"b\":2}";
        }
        if (strcmp(mode, "duplicate-right") == 0)
        {
            right = "{\"a\":1,\"a\":2}";
            left = "{\"a\":1,\"b\":2}";
        }
        if (strcmp(mode, "duplicate-escaped") == 0)
        {
            left = "{\"a\":1,\"\\u0061\":2}";
            right = "{\"a\":1,\"b\":2}";
        }
        if (strcmp(mode, "nested-duplicate") == 0)
            left = right = "[{\"a\":1,\"a\":2}]";
    }
    if (strcmp(mode, "long-key") == 0)
    {
        owned_left = malloc(10020U);
        CHECK(owned_left != NULL);
        owned_left[0] = '{';
        owned_left[1] = '"';
        memset(owned_left + 2, 'x', 10000U);
        strcpy(owned_left + 10002U, "\":true}");
        left = right = owned_left;
    }
    if (strcmp(mode, "deep") == 0)
    {
        owned_left = malloc(514U);
        CHECK(owned_left != NULL);
        memset(owned_left, '[', 128U);
        owned_left[128] = '0';
        memset(owned_left + 129, ']', 128U);
        owned_left[257] = '\0';
        left = right = owned_left;
    }
    if (strcmp(mode, "wide") == 0)
    {
        owned_left = malloc(16384U);
        owned_right = malloc(16384U);
        CHECK(owned_left != NULL && owned_right != NULL);
        size_t a = 1U, b = 1U;
        owned_left[0] = owned_right[0] = '{';
        for (size_t i = 0U; i < 512U; ++i)
        {
            int n = snprintf(owned_left + a, 16384U - a, "%s\"field%zu\":%zu", i ? "," : "", i, i);
            CHECK(n > 0 && (size_t)n < 16384U - a);
            a += (size_t)n;
            n = snprintf(owned_right + b, 16384U - b, "%s\"field%zu\":%zu", i ? "," : "", 511U - i, 511U - i);
            CHECK(n > 0 && (size_t)n < 16384U - b);
            b += (size_t)n;
        }
        strcpy(owned_left + a, "}");
        strcpy(owned_right + b, "}");
        left = owned_left;
        right = owned_right;
    }
    if (strcmp(mode, "subtree") == 0)
    {
        left = "{\"nested\":{\"a\":1}}";
        right = "{\"other\":{\"a\":1}}";
    }
    UmiJsonTree *a = NULL, *b = NULL;
    CHECK(UmiJsonTreeCreate(left, strlen(left), NULL, NULL, &a) == UMI_STATUS_OK);
    CHECK(UmiJsonTreeCreate(right, strlen(right), NULL, NULL, &b) == UMI_STATUS_OK);
    free(owned_left);
    free(owned_right);
    int first = 0, second = 0;
    if (strcmp(mode, "subtree") == 0)
    {
        CHECK(UmiJsonTreeMember(a, 0, "nested", &first) == UMI_STATUS_OK);
        CHECK(UmiJsonTreeMember(b, 0, "other", &second) == UMI_STATUS_OK);
    }
    if (strcmp(mode, "negative-node") == 0)
    {
        first = -1;
        wanted = UMI_STATUS_INVALID_ARGUMENT;
    }
    if (strcmp(mode, "outside-node") == 0)
    {
        second = 9999;
        wanted = UMI_STATUS_INVALID_ARGUMENT;
    }
    if (strncmp(mode, "null-", 5U) == 0 && strcmp(mode, "null-bool") != 0)
        wanted = UMI_STATUS_INVALID_ARGUMENT;
    UmiCancellationToken *cancel = NULL;
    CHECK(umi_cancellation_token_create(&cancel) == UMI_STATUS_OK);
    if (strcmp(mode, "cancel") == 0)
    {
        umi_cancellation_token_request(cancel);
        wanted = UMI_STATUS_CANCELLED;
    }
    int equal = 73;
    CHECK(UmiJsonTreeValuesEqual(strcmp(mode, "null-left") == 0 ? NULL : a, first,
                                 strcmp(mode, "null-right") == 0 ? NULL : b, second, cancel,
                                 strcmp(mode, "null-output") == 0 ? NULL : &equal) == wanted);
    CHECK(equal == (wanted == UMI_STATUS_OK ? expected : 73));
    UmiJsonTreeDestroy(a);
    UmiJsonTreeDestroy(b);
    umi_cancellation_token_destroy(cancel);
    return 0;
}

/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/json_tree/test_tree.c
 * PURPOSE: Exercise owned JSON grammar, traversal, limits and unchanged failure outputs.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language_runtime/json_tree.h"
#include "umicom/language_runtime/json_text.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x)                                                                                             \
    do                                                                                                       \
    {                                                                                                        \
        if (!(x))                                                                                            \
        {                                                                                                    \
            fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x);                                          \
            failed = 1;                                                                                      \
            goto cleanup;                                                                                    \
        }                                                                                                    \
    } while (0)
static UmiStatus Parse(const char *text, UmiJsonTree **out)
{
    return UmiJsonTreeCreate(text, strlen(text), NULL, NULL, out);
}
int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    const char *mode = argv[1];
    int failed = 0;
    UmiJsonTree *tree = NULL;
    UmiCancellationToken *cancel = NULL;
    char *large = NULL;
    UmiJsonTreeLimits limits = UmiJsonTreeDefaultLimits();
    if (strcmp(mode, "ownership") == 0)
    {
        char source[] = "{\"a\":[{\"x\":1},2],\"b\":3}";
        CHECK(UmiJsonTreeCreate(source, strlen(source), NULL, NULL, &tree) == UMI_STATUS_OK);
        memset(source, '?', strlen(source));
        int array = -1, other = -1;
        int64_t number = 0;
        CHECK(UmiJsonTreeCount(tree, 0) == 2U);
        CHECK(UmiJsonTreeMember(tree, 0, "a", &array) == UMI_STATUS_OK);
        CHECK(UmiJsonTreeMember(tree, 0, "b", &other) == UMI_STATUS_OK);
        CHECK(UmiJsonTreeCount(tree, array) == 2U);
        int item = UmiJsonTreeFirst(tree, array);
        CHECK(UmiJsonTreeKind(tree, item) == UMI_LANGUAGE_RUNTIME_JSON_OBJECT);
        item = UmiJsonTreeNext(tree, item);
        CHECK(UmiJsonTreeInteger(tree, item, &number) == UMI_STATUS_OK && number == 2);
        CHECK(UmiJsonTreeNext(tree, item) == -1);
        CHECK(UmiJsonTreeInteger(tree, other, &number) == UMI_STATUS_OK && number == 3);
        CHECK(UmiJsonTreeKind(tree, -1) == UMI_LANGUAGE_RUNTIME_JSON_UNDEFINED);
        CHECK(UmiJsonTreeFirst(NULL, 0) == -1 && UmiJsonTreeNext(tree, 999) == -1);
    }
    else if (strcmp(mode, "grammar") == 0)
    {
        const char *bad[] = {
            "",    " ",  "[1,]", "{\"x\":1,}", "{x:1}",     "[1 2]",    "{\"x\" 1}", "true false", "01",
            "-01", "+1", ".1",   "1.",         "1e",        "1e+",      "NaN",       "Infinity",   "TRUE",
            "[}",  "{]", "\v1",  "1\f",        "/comment/", "{\"x\":}", "[",         "{"};
        for (size_t i = 0U; i < sizeof(bad) / sizeof(bad[0]); ++i)
        {
            CHECK(Parse(bad[i], &tree) == UMI_STATUS_PARSE_ERROR && tree == NULL);
        }
        const char *good[] = {"null",     "true", "false", "-0", "0.25",
                              "-12.5e+7", "1E-9", "[]",    "{}", " \r\n[1,{\"a\":false}]\t"};
        for (size_t i = 0U; i < sizeof(good) / sizeof(good[0]); ++i)
        {
            CHECK(Parse(good[i], &tree) == UMI_STATUS_OK);
            UmiJsonTreeDestroy(tree);
            tree = NULL;
        }
        const char embedded[] = {'1', '\0', '2'};
        CHECK(UmiJsonTreeCreate(embedded, sizeof(embedded), NULL, NULL, &tree) == UMI_STATUS_PARSE_ERROR &&
              tree == NULL);
    }
    else if (strcmp(mode, "unicode") == 0)
    {
        const char *bad[] = {"\"\\u0000\"", "\"\\ud800\"",  "\"\\udc00\"",      "\"\\ud800\\u0041\"",
                             "\"\\x20\"",   "\"\xc0\xaf\"", "\"\xed\xa0\x80\"", "\"\xf4\x90\x80\x80\"",
                             "\"\x80\"",    "\"\n\""};
        for (size_t i = 0U; i < sizeof(bad) / sizeof(bad[0]); ++i)
            CHECK(Parse(bad[i], &tree) == UMI_STATUS_PARSE_ERROR && tree == NULL);
        CHECK(Parse("{\"caf\\u00e9\":\"\\ud83d\\ude03\\n\"}", &tree) == UMI_STATUS_OK);
        int value = -1;
        char text[32] = "retained";
        CHECK(UmiJsonTreeMember(tree, 0, "caf\xc3\xa9", &value) == UMI_STATUS_OK);
        CHECK(UmiJsonTreeText(tree, value, text, sizeof(text)) == UMI_STATUS_OK);
        CHECK(strcmp(text, "\xf0\x9f\x98\x83\n") == 0);
        strcpy(text, "retained");
        CHECK(UmiJsonTreeText(tree, value, text, 2U) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(strcmp(text, "retained") == 0);
    }
    else if (strcmp(mode, "integers") == 0)
    {
        const char *numbers[] = {"9223372036854775807",
                                 "-9223372036854775808",
                                 "0",
                                 "-0",
                                 "9223372036854775808",
                                 "-9223372036854775809",
                                 "1.0",
                                 "1e2",
                                 "true",
                                 "null"};
        for (size_t i = 0U; i < sizeof(numbers) / sizeof(numbers[0]); ++i)
        {
            CHECK(Parse(numbers[i], &tree) == UMI_STATUS_OK);
            int64_t value = 42;
            UmiStatus status = UmiJsonTreeInteger(tree, 0, &value);
            if (i < 4U)
            {
                CHECK(status == UMI_STATUS_OK);
                CHECK(value == (i == 0U ? INT64_MAX : i == 1U ? INT64_MIN : 0));
            }
            else
            {
                CHECK(status == (i < 6U ? UMI_STATUS_CAPACITY_EXCEEDED : UMI_STATUS_PARSE_ERROR));
                CHECK(value == 42);
            }
            UmiJsonTreeDestroy(tree);
            tree = NULL;
        }
    }
    else if (strcmp(mode, "booleans") == 0)
    {
        CHECK(Parse("[true,false,1,null]", &tree) == UMI_STATUS_OK);
        int node = UmiJsonTreeFirst(tree, 0);
        for (size_t i = 0U; i < 4U; ++i, node = UmiJsonTreeNext(tree, node))
        {
            int value = 42;
            UmiStatus status = UmiJsonTreeBoolean(tree, node, &value);
            CHECK(status == (i < 2U ? UMI_STATUS_OK : UMI_STATUS_PARSE_ERROR));
            CHECK(value == (i < 2U ? (i == 0U ? 1 : 0) : 42));
        }
    }
    else if (strcmp(mode, "bounds") == 0)
    {
        limits.bytes = 3U;
        limits.nodes = 2U;
        limits.depth = 1U;
        CHECK(UmiJsonTreeCreate("[0]", 3U, &limits, NULL, &tree) == UMI_STATUS_OK);
        UmiJsonTreeDestroy(tree);
        tree = NULL;
        CHECK(UmiJsonTreeCreate("[0] ", 4U, &limits, NULL, &tree) == UMI_STATUS_CAPACITY_EXCEEDED &&
              tree == NULL);
        limits.bytes = 10U;
        limits.nodes = 1U;
        CHECK(UmiJsonTreeCreate("[0]", 3U, &limits, NULL, &tree) == UMI_STATUS_CAPACITY_EXCEEDED &&
              tree == NULL);
        limits.nodes = 0U;
        CHECK(UmiJsonTreeCreate("0", 1U, &limits, NULL, &tree) == UMI_STATUS_INVALID_ARGUMENT &&
              tree == NULL);
        limits = UmiJsonTreeDefaultLimits();
        limits.depth = 257U;
        CHECK(UmiJsonTreeCreate("0", 1U, &limits, NULL, &tree) == UMI_STATUS_INVALID_ARGUMENT &&
              tree == NULL);
    }
    else if (strcmp(mode, "depth") == 0)
    {
        char nested[514];
        size_t depth = 128U;
        memset(nested, '[', depth);
        memset(nested + depth, ']', depth);
        nested[depth * 2U] = '\0';
        CHECK(Parse(nested, &tree) == UMI_STATUS_OK);
        UmiJsonTreeDestroy(tree);
        tree = NULL;
        limits.depth = 127U;
        CHECK(UmiJsonTreeCreate(nested, depth * 2U, &limits, NULL, &tree) == UMI_STATUS_CAPACITY_EXCEEDED &&
              tree == NULL);
    }
    else if (strcmp(mode, "cancellation") == 0)
    {
        CHECK(umi_cancellation_token_create(&cancel) == UMI_STATUS_OK);
        umi_cancellation_token_request(cancel);
        CHECK(UmiJsonTreeCreate("[]", 2U, NULL, cancel, &tree) == UMI_STATUS_CANCELLED && tree == NULL);
        umi_cancellation_token_reset(cancel);
        CHECK(UmiJsonTreeCreate("[]", 2U, NULL, cancel, &tree) == UMI_STATUS_OK);
    }
    else if (strcmp(mode, "members") == 0)
    {
        CHECK(Parse("{\"a\":1,\"\\u0061\":2,\"b\":3,\"\":4}", &tree) == UMI_STATUS_OK);
        int node = 99;
        CHECK(UmiJsonTreeMember(tree, 0, "a", &node) == UMI_STATUS_ALREADY_EXISTS && node == 99);
        CHECK(UmiJsonTreeMember(tree, 0, "missing", &node) == UMI_STATUS_NOT_FOUND && node == 99);
        CHECK(UmiJsonTreeMember(tree, 0, "b", &node) == UMI_STATUS_OK);
        int64_t value = 0;
        CHECK(UmiJsonTreeInteger(tree, node, &value) == UMI_STATUS_OK && value == 3);
        CHECK(UmiJsonTreeMember(tree, 0, "", &node) == UMI_STATUS_OK);
        CHECK(UmiJsonTreeInteger(tree, node, &value) == UMI_STATUS_OK && value == 4);
    }
    else if (strcmp(mode, "siblings") == 0)
    {
        /* A long array exceeds the former document's byte and token capacities.
         * Nested children must not appear when walking its direct siblings. */
        const size_t count = 20000U;
        large = malloc(count * 4U + 2U);
        CHECK(large != NULL);
        char *at = large;
        *at++ = '[';
        for (size_t i = 0U; i < count; ++i)
        {
            memcpy(at, "[0]", 3U);
            at += 3;
            *at++ = i + 1U == count ? ']' : ',';
        }
        *at = '\0';
        CHECK(Parse(large, &tree) == UMI_STATUS_OK);
        CHECK(UmiJsonTreeCount(tree, 0) == count);
        int node = UmiJsonTreeFirst(tree, 0);
        for (size_t i = 0U; i < count; ++i, node = UmiJsonTreeNext(tree, node))
        {
            CHECK(UmiJsonTreeKind(tree, node) == UMI_LANGUAGE_RUNTIME_JSON_ARRAY &&
                  UmiJsonTreeCount(tree, node) == 1U);
        }
        CHECK(node == -1);
    }
    else if (strcmp(mode, "null") == 0)
    {
        CHECK(Parse("[null,\"null\",false,0,{},[]]", &tree) == UMI_STATUS_OK);
        int node = UmiJsonTreeFirst(tree, 0);
        CHECK(UmiJsonTreeIsNull(tree, node));
        for (node = UmiJsonTreeNext(tree, node); node >= 0; node = UmiJsonTreeNext(tree, node))
            CHECK(!UmiJsonTreeIsNull(tree, node));
        CHECK(!UmiJsonTreeIsNull(NULL, 0) && !UmiJsonTreeIsNull(tree, -1) && !UmiJsonTreeIsNull(tree, 999));
    }
    else if (strcmp(mode, "span") == 0)
    {
        char output[32] = "retained";
        size_t size = 99U;
        const char *encoded = "caf\\u00e9";
        CHECK(UmiLanguageRuntimeJsonTextSpan(encoded, strlen(encoded), NULL, 0U, &size) == UMI_STATUS_OK &&
              size == 5U);
        CHECK(UmiLanguageRuntimeJsonTextSpan(encoded, strlen(encoded), output, sizeof(output), &size) ==
              UMI_STATUS_OK);
        CHECK(strcmp(output, "caf\xc3\xa9") == 0);
        strcpy(output, "retained");
        size = 99U;
        CHECK(UmiLanguageRuntimeJsonTextSpan("\\uD800", 6U, output, sizeof(output), &size) ==
              UMI_STATUS_PARSE_ERROR);
        CHECK(strcmp(output, "retained") == 0 && size == 99U);
        CHECK(UmiLanguageRuntimeJsonTextSpan(NULL, 0U, output, sizeof(output), &size) == UMI_STATUS_OK &&
              size == 0U && output[0] == '\0');
    }
    else
    {
        failed = 2;
    }
cleanup:
    free(large);
    UmiJsonTreeDestroy(tree);
    umi_cancellation_token_destroy(cancel);
    return failed;
}

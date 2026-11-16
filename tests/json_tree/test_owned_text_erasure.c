/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/json_tree/test_owned_text_erasure.c
 *
 * PURPOSE:
 *   Observe owned JSON bytes before allocation release on successful and failed parsing, without reading freed memory.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/platform/cancellation.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static void *observed_text;
static size_t observed_length;
static int cleared, releases, fail_text;
static UmiCancellationToken *cancel_after_allocate;
static void *ObservedMalloc(size_t length)
{
    if (fail_text) return NULL;
    observed_text = malloc(length);
    observed_length = length;
    if (cancel_after_allocate != NULL) umi_cancellation_token_request(cancel_after_allocate);
    return observed_text;
}
static void ObservedFree(void *value)
{
    if (value != NULL && value == observed_text) {
        cleared = 1;
        for (size_t i = 0; i < observed_length; ++i)
            if (((const unsigned char *)value)[i] != 0U) cleared = 0;
        ++releases;
        observed_text = NULL;
    }
    free(value);
}
/* Compile the actual owner with allocator observation. Renamed exports keep
 * this fixture independent of the normal protocol archive's symbols. */
#define UmiJsonTreeDefaultLimits ErasureDefaultLimits
#define UmiJsonTreeCreate ErasureCreate
#define UmiJsonTreeDestroy ErasureDestroy
#define UmiJsonTreeKind ErasureKind
#define UmiJsonTreeFirst ErasureFirst
#define UmiJsonTreeNext ErasureNext
#define UmiJsonTreeCount ErasureCount
#define UmiJsonTreeMember ErasureMember
#define UmiJsonTreeText ErasureText
#define UmiJsonTreeInteger ErasureInteger
#define UmiJsonTreeBoolean ErasureBoolean
#define UmiJsonTreeIsNull ErasureIsNull
#define UmiJsonTreeSourceSpan ErasureSourceSpan
#define malloc ObservedMalloc
#define free ObservedFree
#include "../../src/language_runtime/json_tree.c"
#undef malloc
#undef free
#define CHECK(e) do { if (!(e)) { fprintf(stderr, "%d: %s\n", __LINE__, #e); return 1; } } while (0)
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    const char *input = "{\"private-extension\":\"example response text\"}";
    UmiStatus expected = UMI_STATUS_OK;
    UmiJsonTreeLimits limits = ErasureDefaultLimits();
    if (strcmp(argv[1], "success") == 0) {}
    else if (strcmp(argv[1], "parse-failure") == 0) { input = "{\"private-extension\":\"unfinished"; expected = UMI_STATUS_PARSE_ERROR; }
    else if (strcmp(argv[1], "node-limit") == 0) { limits.nodes = 1; expected = UMI_STATUS_CAPACITY_EXCEEDED; }
    else if (strcmp(argv[1], "cancel") == 0) { CHECK(umi_cancellation_token_create(&cancel_after_allocate) == UMI_STATUS_OK); expected = UMI_STATUS_CANCELLED; }
    else if (strcmp(argv[1], "allocation") == 0) { fail_text = 1; expected = UMI_STATUS_OUT_OF_MEMORY; }
    else if (strcmp(argv[1], "empty") == 0) { input = ""; expected = UMI_STATUS_PARSE_ERROR; }
    else CHECK(0);
    UmiJsonTree *tree = NULL;
    CHECK(ErasureCreate(input, strlen(input), &limits, cancel_after_allocate, &tree) == expected);
    if (expected == UMI_STATUS_OK) CHECK(tree != NULL);
    else CHECK(tree == NULL);
    ErasureDestroy(tree);
    CHECK(releases == (fail_text ? 0 : 1));
    CHECK(fail_text || cleared);
    umi_cancellation_token_destroy(cancel_after_allocate);
    return 0;
}

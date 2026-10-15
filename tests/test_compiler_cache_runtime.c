/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_compiler_cache_runtime.c
 *
 * PURPOSE:
 *   Implement the test compiler cache runtime behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework Batch 58 test | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/compiler/compiler.h"
#include "value_archive/construction_cases.h"
#include <assert.h>
#include <string.h>
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */

/* A rejected replacement must preserve the previously accepted request value. */
static int UmiCompilerRequestConstructionCases(void)
{
    UmiCompilerRequest *value = malloc(sizeof(*value));
    unsigned char *before = malloc(sizeof(*value));
    int result = 1;
    if (value == NULL || before == NULL) goto construction_cleanup;
    UMI_CONSTRUCTION_REQUIRE(umi_compiler_request_init(value, "p", "u", UMI_COMPILER_LANGUAGE_C, "a.c", "a.o") == UMI_STATUS_OK);
    memcpy(before, value, sizeof(*value));
    {
        char oversized[sizeof(value->project_id) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_compiler_request_init(value, oversized, "u", UMI_COMPILER_LANGUAGE_C, "a.c", "a.o") != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_compiler_request_init(value, value->project_id, "u", UMI_COMPILER_LANGUAGE_C, "a.c", "a.o") == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->project_id, "p") == 0);
        memcpy(before, value, sizeof(*value));
    }
    {
        char oversized[sizeof(value->unit_id) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_compiler_request_init(value, "p", oversized, UMI_COMPILER_LANGUAGE_C, "a.c", "a.o") != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_compiler_request_init(value, "p", value->unit_id, UMI_COMPILER_LANGUAGE_C, "a.c", "a.o") == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->unit_id, "u") == 0);
        memcpy(before, value, sizeof(*value));
    }
    {
        char oversized[sizeof(value->source_path) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_compiler_request_init(value, "p", "u", UMI_COMPILER_LANGUAGE_C, oversized, "a.o") != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_compiler_request_init(value, "p", "u", UMI_COMPILER_LANGUAGE_C, value->source_path, "a.o") == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->source_path, "a.c") == 0);
        memcpy(before, value, sizeof(*value));
    }
    {
        char oversized[sizeof(value->output_path) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_compiler_request_init(value, "p", "u", UMI_COMPILER_LANGUAGE_C, "a.c", oversized) != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_compiler_request_init(value, "p", "u", UMI_COMPILER_LANGUAGE_C, "a.c", value->output_path) == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->output_path, "a.o") == 0);
        memcpy(before, value, sizeof(*value));
    }
    result = 0;
construction_cleanup:
    free(before);
    free(value);
    return result;
}

int main(void) {
    if (UmiCompilerRequestConstructionCases() != 0) return 1;
 UmiCompilerCache cache = {0}; UmiCompilerCacheEntry entry = {0}; UmiCompilerRequest request; assert(umi_compiler_request_init(&request,"p","u",UMI_COMPILER_LANGUAGE_C,"a.c","a.o") == UMI_STATUS_OK); entry.key = umi_compiler_request_hash(&request); (void)strcpy(entry.artifact_path,"a.o"); entry.created_at_ns = 5U; entry.valid = true; assert(umi_compiler_cache_put(&cache,&entry) == UMI_STATUS_OK); assert(umi_compiler_cache_get(&cache,entry.key) != NULL); assert(cache.hits == 1U); umi_compiler_cache_invalidate(&cache,6U); assert(umi_compiler_cache_get(&cache,entry.key) == NULL); assert(cache.misses == 1U); return 0; }

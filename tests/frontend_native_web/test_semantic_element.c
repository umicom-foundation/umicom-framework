/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/frontend_native_web/test_semantic_element.c
 *
 * PURPOSE:
 *   Focused regression coverage for native-web semantic element.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <stdio.h>
#include "../value_archive/construction_cases.h"
#include <string.h>
#include "umicom/frontend/native_web/semantic_element.h"
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s at %s:%d\n", #expr, __FILE__, __LINE__); return 1; } } while (0)
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */

/* A rejected replacement must preserve the previously accepted semantic element value. */
static int UmiNativeWebSemanticElementConstructionCases(void)
{
    UmiNativeWebSemanticElement *value = malloc(sizeof(*value));
    unsigned char *before = malloc(sizeof(*value));
    int result = 1;
    if (value == NULL || before == NULL) goto construction_cleanup;
    UMI_CONSTRUCTION_REQUIRE(umi_native_web_semantic_element_init(value, "root", "main") == UMI_STATUS_OK);
    memcpy(before, value, sizeof(*value));
    {
        char oversized[sizeof(value->element_id) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_native_web_semantic_element_init(value, oversized, "main") != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_native_web_semantic_element_init(value, value->element_id, "main") == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->element_id, "root") == 0);
        memcpy(before, value, sizeof(*value));
    }
    {
        char oversized[sizeof(value->tag) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_native_web_semantic_element_init(value, "root", oversized) != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_native_web_semantic_element_init(value, "root", value->tag) == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->tag, "main") == 0);
        memcpy(before, value, sizeof(*value));
    }
    result = 0;
construction_cleanup:
    free(before);
    free(value);
    return result;
}

int main(void)
{
    if (UmiNativeWebSemanticElementConstructionCases() != 0) return 1;

    UmiNativeWebSemanticElement e; CHECK(umi_native_web_semantic_element_init(&e,"root","main") == UMI_STATUS_OK); CHECK(umi_native_web_semantic_element_validate(&e) == UMI_STATUS_OK);
    return 0;
}

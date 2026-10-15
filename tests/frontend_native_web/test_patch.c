/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/frontend_native_web/test_patch.c
 *
 * PURPOSE:
 *   Focused regression coverage for native-web patch.
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
#include "umicom/frontend/native_web/patch.h"
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s at %s:%d\n", #expr, __FILE__, __LINE__); return 1; } } while (0)
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */

/* A rejected replacement must preserve the previously accepted patch value. */
static int UmiNativeWebPatchConstructionCases(void)
{
    UmiNativeWebPatch *value = malloc(sizeof(*value));
    unsigned char *before = malloc(sizeof(*value));
    int result = 1;
    if (value == NULL || before == NULL) goto construction_cleanup;
    UMI_CONSTRUCTION_REQUIRE(umi_native_web_patch_init(value, UMI_NATIVE_WEB_PATCH_SET_TEXT, "x", "", "a", 1U) == UMI_STATUS_OK);
    memcpy(before, value, sizeof(*value));
    {
        char oversized[sizeof(value->target_id) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_native_web_patch_init(value, UMI_NATIVE_WEB_PATCH_SET_TEXT, oversized, "", "a", 1U) != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_native_web_patch_init(value, UMI_NATIVE_WEB_PATCH_SET_TEXT, value->target_id, "", "a", 1U) == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->target_id, "x") == 0);
        memcpy(before, value, sizeof(*value));
    }
    {
        char oversized[sizeof(value->name) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_native_web_patch_init(value, UMI_NATIVE_WEB_PATCH_SET_TEXT, "x", oversized, "a", 1U) != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_native_web_patch_init(value, UMI_NATIVE_WEB_PATCH_SET_TEXT, "x", value->name, "a", 1U) == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->name, "") == 0);
        memcpy(before, value, sizeof(*value));
    }
    {
        char oversized[sizeof(value->value) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_native_web_patch_init(value, UMI_NATIVE_WEB_PATCH_SET_TEXT, "x", "", oversized, 1U) != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_native_web_patch_init(value, UMI_NATIVE_WEB_PATCH_SET_TEXT, "x", "", value->value, 1U) == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->value, "a") == 0);
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
    if (UmiNativeWebPatchConstructionCases() != 0) return 1;

    UmiNativeWebPatch p,q; CHECK(umi_native_web_patch_init(&p,UMI_NATIVE_WEB_PATCH_SET_TEXT,"x","","a",1U)==UMI_STATUS_OK); CHECK(umi_native_web_patch_init(&q,UMI_NATIVE_WEB_PATCH_SET_TEXT,"x","","b",2U)==UMI_STATUS_OK); CHECK(umi_native_web_patch_same_slot(&p,&q));
    return 0;
}

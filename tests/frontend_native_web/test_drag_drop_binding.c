/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/frontend_native_web/test_drag_drop_binding.c
 *
 * PURPOSE:
 *   Focused regression coverage for native-web drag drop binding.
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
#include "umicom/frontend/native_web/drag_drop_binding.h"
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s at %s:%d\n", #expr, __FILE__, __LINE__); return 1; } } while (0)
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */

/* A rejected replacement must preserve the previously accepted drag drop binding value. */
static int UmiNativeWebDragDropBindingConstructionCases(void)
{
    UmiNativeWebDragDropBinding *value = malloc(sizeof(*value));
    unsigned char *before = malloc(sizeof(*value));
    int result = 1;
    if (value == NULL || before == NULL) goto construction_cleanup;
    UMI_CONSTRUCTION_REQUIRE(umi_native_web_drag_drop_binding_init(value, "panel-a", "dock-left", "panel", true) == UMI_STATUS_OK);
    memcpy(before, value, sizeof(*value));
    {
        char oversized[sizeof(value->source_id) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_native_web_drag_drop_binding_init(value, oversized, "dock-left", "panel", true) != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_native_web_drag_drop_binding_init(value, value->source_id, "dock-left", "panel", true) == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->source_id, "panel-a") == 0);
        memcpy(before, value, sizeof(*value));
    }
    {
        char oversized[sizeof(value->target_id) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_native_web_drag_drop_binding_init(value, "panel-a", oversized, "panel", true) != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_native_web_drag_drop_binding_init(value, "panel-a", value->target_id, "panel", true) == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->target_id, "dock-left") == 0);
        memcpy(before, value, sizeof(*value));
    }
    {
        char oversized[sizeof(value->payload_type) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_native_web_drag_drop_binding_init(value, "panel-a", "dock-left", oversized, true) != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_native_web_drag_drop_binding_init(value, "panel-a", "dock-left", value->payload_type, true) == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->payload_type, "panel") == 0);
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
    if (UmiNativeWebDragDropBindingConstructionCases() != 0) return 1;

    UmiNativeWebDragDropBinding b; CHECK(umi_native_web_drag_drop_binding_init(&b,"panel-a","dock-left","panel",true)==UMI_STATUS_OK); CHECK(b.allowed);
    return 0;
}

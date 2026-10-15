/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/frontend_native_web/test_ui_event.c
 *
 * PURPOSE:
 *   Focused regression coverage for native-web ui event.
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
#include "umicom/frontend/native_web/ui_event.h"
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s at %s:%d\n", #expr, __FILE__, __LINE__); return 1; } } while (0)
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */

/* A rejected replacement must preserve the previously accepted ui event value. */
static int UmiNativeWebUiEventConstructionCases(void)
{
    UmiNativeWebUiEvent *value = malloc(sizeof(*value));
    unsigned char *before = malloc(sizeof(*value));
    int result = 1;
    if (value == NULL || before == NULL) goto construction_cleanup;
    UMI_CONSTRUCTION_REQUIRE(umi_native_web_ui_event_init(value, UMI_NATIVE_WEB_EVENT_CLICK, "b", "", "", 1U) == UMI_STATUS_OK);
    memcpy(before, value, sizeof(*value));
    {
        char oversized[sizeof(value->target_id) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_native_web_ui_event_init(value, UMI_NATIVE_WEB_EVENT_CLICK, oversized, "", "", 1U) != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_native_web_ui_event_init(value, UMI_NATIVE_WEB_EVENT_CLICK, value->target_id, "", "", 1U) == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->target_id, "b") == 0);
        memcpy(before, value, sizeof(*value));
    }
    {
        char oversized[sizeof(value->name) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_native_web_ui_event_init(value, UMI_NATIVE_WEB_EVENT_CLICK, "b", oversized, "", 1U) != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_native_web_ui_event_init(value, UMI_NATIVE_WEB_EVENT_CLICK, "b", value->name, "", 1U) == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->name, "") == 0);
        memcpy(before, value, sizeof(*value));
    }
    {
        char oversized[sizeof(value->value) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_native_web_ui_event_init(value, UMI_NATIVE_WEB_EVENT_CLICK, "b", "", oversized, 1U) != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_native_web_ui_event_init(value, UMI_NATIVE_WEB_EVENT_CLICK, "b", "", value->value, 1U) == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->value, "") == 0);
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
    if (UmiNativeWebUiEventConstructionCases() != 0) return 1;

    UmiNativeWebUiEvent e; CHECK(umi_native_web_ui_event_init(&e,UMI_NATIVE_WEB_EVENT_CLICK,"b","","",1U)==UMI_STATUS_OK); CHECK(e.kind==UMI_NATIVE_WEB_EVENT_CLICK);
    return 0;
}

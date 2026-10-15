/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_workstation/test_media_viewport.c
 *
 * PURPOSE:
 *   Implement the test media viewport behavior for
 *   Umicom Framework.
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
#include "umicom/ui/workstation/media_viewport.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */

/* A rejected replacement must preserve the previously accepted media viewport value. */
static int UmiWsMediaViewportConstructionCases(void)
{
    UmiWsMediaViewport *value = malloc(sizeof(*value));
    unsigned char *before = malloc(sizeof(*value));
    int result = 1;
    if (value == NULL || before == NULL) goto construction_cleanup;
    UMI_CONSTRUCTION_REQUIRE(umi_ws_media_viewport_init(value, "preview", 3840, 2160) == UMI_STATUS_OK);
    memcpy(before, value, sizeof(*value));
    {
        char oversized[sizeof(value->viewport_id) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_ws_media_viewport_init(value, oversized, 3840, 2160) != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_ws_media_viewport_init(value, value->viewport_id, 3840, 2160) == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->viewport_id, "preview") == 0);
        memcpy(before, value, sizeof(*value));
    }
    result = 0;
construction_cleanup:
    free(before);
    free(value);
    return result;
}

int main(void) {
    if (UmiWsMediaViewportConstructionCases() != 0) return 1;

    UmiWsMediaViewport v;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ws_media_viewport_init(&v, "preview", 3840, 2160) != UMI_STATUS_OK) return 1;
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_ws_media_viewport_set_zoom(&v, 100.0) != 32.0) return 2;
    umi_ws_media_viewport_set_fit(&v, UMI_WS_VIEWPORT_ONE_TO_ONE);
    /* Apply this branch only when its contract condition is satisfied. */
    if (v.fit != UMI_WS_VIEWPORT_ONE_TO_ONE) return 3;
    puts("media viewport: ok");
    return 0;
}

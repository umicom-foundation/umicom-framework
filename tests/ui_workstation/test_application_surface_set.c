/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_workstation/test_application_surface_set.c
 *
 * PURPOSE:
 *   Implement the test application surface set behavior for
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
#include "../value_archive/construction_cases.h"
#include "umicom/ui/workstation/application_surface_set.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */

/* A rejected replacement must preserve the previously accepted application surface set value. */
static int UmiWsApplicationSurfaceSetConstructionCases(void)
{
    UmiWsApplicationSurfaceSet *value = malloc(sizeof(*value));
    unsigned char *before = malloc(sizeof(*value));
    int result = 1;
    if (value == NULL || before == NULL) goto construction_cleanup;
    UMI_CONSTRUCTION_REQUIRE(umi_ws_application_surface_set_init(value, "desktop", UMI_WS_DOMAIN_GENERIC) == UMI_STATUS_OK);
    memcpy(before, value, sizeof(*value));
    {
        char oversized[sizeof(value->application_id) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_ws_application_surface_set_init(value, oversized, UMI_WS_DOMAIN_GENERIC) != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_ws_application_surface_set_init(value, value->application_id, UMI_WS_DOMAIN_GENERIC) == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->application_id, "desktop") == 0);
        memcpy(before, value, sizeof(*value));
    }
    result = 0;
construction_cleanup:
    free(before);
    free(value);
    return result;
}


/* A rejected replacement must preserve the previously accepted surface descriptor value. */
static int UmiWsSurfaceDescriptorConstructionCases(void)
{
    UmiWsSurfaceDescriptor *value = malloc(sizeof(*value));
    unsigned char *before = malloc(sizeof(*value));
    int result = 1;
    if (value == NULL || before == NULL) goto construction_cleanup;
    UMI_CONSTRUCTION_REQUIRE(umi_ws_surface_descriptor_init(value, "trader.chart", "Chart", UMI_WS_DOMAIN_TRADER, UMI_WS_SURFACE_CHART) == UMI_STATUS_OK);
    memcpy(before, value, sizeof(*value));
    {
        char oversized[sizeof(value->surface_id) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_ws_surface_descriptor_init(value, oversized, "Chart", UMI_WS_DOMAIN_TRADER, UMI_WS_SURFACE_CHART) != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_ws_surface_descriptor_init(value, value->surface_id, "Chart", UMI_WS_DOMAIN_TRADER, UMI_WS_SURFACE_CHART) == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->surface_id, "trader.chart") == 0);
        memcpy(before, value, sizeof(*value));
    }
    {
        char oversized[sizeof(value->label) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_ws_surface_descriptor_init(value, "trader.chart", oversized, UMI_WS_DOMAIN_TRADER, UMI_WS_SURFACE_CHART) != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_ws_surface_descriptor_init(value, "trader.chart", value->label, UMI_WS_DOMAIN_TRADER, UMI_WS_SURFACE_CHART) == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->label, "Chart") == 0);
        memcpy(before, value, sizeof(*value));
    }
    result = 0;
construction_cleanup:
    free(before);
    free(value);
    return result;
}

int main(void) {
    if (UmiWsSurfaceDescriptorConstructionCases() != 0) return 1;

    if (UmiWsApplicationSurfaceSetConstructionCases() != 0) return 1;

    UmiWsApplicationSurfaceSet s;
    UmiWsSurfaceDescriptor d;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ws_application_surface_set_init(&s, "desktop", UMI_WS_DOMAIN_GENERIC) != UMI_STATUS_OK) return 1;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ws_application_surface_set_add(&s, "studio.editor") != UMI_STATUS_OK) return 2;
    /* Apply this branch only when its contract condition is satisfied. */
    if (!umi_ws_application_surface_set_contains(&s, "studio.editor")) return 3;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ws_surface_descriptor_init(&d, "trader.chart", "Chart", UMI_WS_DOMAIN_TRADER, UMI_WS_SURFACE_CHART) != UMI_STATUS_OK) return 4;
    /* Apply this branch only when its contract condition is satisfied. */
    if (!umi_ws_application_surface_set_can_host(&s, &d)) return 5;
    puts("application surface set: ok");
    return 0;
}

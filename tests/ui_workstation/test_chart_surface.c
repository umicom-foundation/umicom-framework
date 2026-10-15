/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_workstation/test_chart_surface.c
 *
 * PURPOSE:
 *   Implement the test chart surface behavior for
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
#include "umicom/ui/workstation/chart_surface.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */

/* A rejected replacement must preserve the previously accepted chart surface value. */
static int UmiWsChartSurfaceConstructionCases(void)
{
    UmiWsChartSurface *value = malloc(sizeof(*value));
    unsigned char *before = malloc(sizeof(*value));
    int result = 1;
    if (value == NULL || before == NULL) goto construction_cleanup;
    UMI_CONSTRUCTION_REQUIRE(umi_ws_chart_surface_init(value, "chart.main", "ES · 1m") == UMI_STATUS_OK);
    memcpy(before, value, sizeof(*value));
    {
        char oversized[sizeof(value->chart_id) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_ws_chart_surface_init(value, oversized, "ES · 1m") != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_ws_chart_surface_init(value, value->chart_id, "ES · 1m") == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->chart_id, "chart.main") == 0);
        memcpy(before, value, sizeof(*value));
    }
    {
        char oversized[sizeof(value->title) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_ws_chart_surface_init(value, "chart.main", oversized) != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_ws_chart_surface_init(value, "chart.main", value->title) == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->title, "ES · 1m") == 0);
        memcpy(before, value, sizeof(*value));
    }
    result = 0;
construction_cleanup:
    free(before);
    free(value);
    return result;
}

/* Exercise rejected edits and borrowed text through the public setter. */
static int UmiWsChartSurfaceTextEditCases(void)
{
    UmiWsChartSurface *value = calloc(1U, sizeof(*value));
    unsigned char *before = malloc(sizeof(*value));
    int result = 1;
    if (value == NULL || before == NULL) goto construction_cleanup;
    UMI_CONSTRUCTION_REQUIRE(umi_ws_chart_surface_set_context_group(value, "prefix-value") == UMI_STATUS_OK);
    memcpy(before, value, sizeof(*value));
    char oversized[sizeof(value->context_group) + 1U];
    memset(oversized, 'x', sizeof(oversized) - 1U);
    oversized[sizeof(oversized) - 1U] = '\0';
    UMI_CONSTRUCTION_REQUIRE(umi_ws_chart_surface_set_context_group(value, oversized) != UMI_STATUS_OK);
    UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
    UMI_CONSTRUCTION_REQUIRE(umi_ws_chart_surface_set_context_group(value, value->context_group + 7U) == UMI_STATUS_OK);
    UMI_CONSTRUCTION_REQUIRE(strcmp(value->context_group, "value") == 0);
    UMI_CONSTRUCTION_REQUIRE(umi_ws_chart_surface_set_context_group(NULL, "value") == UMI_STATUS_INVALID_ARGUMENT);
    result = 0;
construction_cleanup:
    free(before);
    free(value);
    return result;
}

int main(void) {
    if (UmiWsChartSurfaceTextEditCases() != 0) return 1;

    if (UmiWsChartSurfaceConstructionCases() != 0) return 1;

    UmiWsChartSurface c;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ws_chart_surface_init(&c, "chart.main", "ES · 1m") != UMI_STATUS_OK) return 1;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ws_chart_surface_set_context_group(&c, "group.1") != UMI_STATUS_OK) return 2;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ws_chart_surface_set_interval(&c, 300) != UMI_STATUS_OK) return 3;
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_ws_chart_surface_set_zoom(&c, 50.0) != 20.0) return 4;
    puts("chart surface: ok");
    return 0;
}

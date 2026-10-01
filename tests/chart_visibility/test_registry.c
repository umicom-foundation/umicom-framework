/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/chart_visibility/test_registry.c
 * PURPOSE: Exercise atomic pane visibility and retained metadata on real drawing registries.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "../chart_checkpoint/fixture.h"
#include "umicom/chart/drawing_visibility.h"
#include "umicom/chart/drawing_edit.h"

/* Capture actual canonical rows so stale actions cannot mutate newer records. */
int main(int argc, char **argv)
{
    CHECK(argc == 2); const char *name = argv[1];
    UmiChartDrawingRegistry *registry = NULL; CHECK(umi_chart_drawing_registry_create(&registry) == UMI_STATUS_OK);
    UmiChartDrawingSnapshot first = Drawing("first", "NQ"), second = Drawing("second", "NQ"), other = Drawing("other", "ES");
    CHECK(umi_chart_drawing_registry_upsert(registry, &first) == UMI_STATUS_OK);
    CHECK(umi_chart_drawing_registry_upsert(registry, &second) == UMI_STATUS_OK);
    CHECK(umi_chart_drawing_registry_upsert(registry, &other) == UMI_STATUS_OK);
    CHECK(umi_chart_drawing_registry_find(registry, first.id, &first) == UMI_STATUS_OK);
    CHECK(umi_chart_drawing_registry_find(registry, second.id, &second) == UMI_STATUS_OK);
    CHECK(umi_chart_drawing_registry_find(registry, other.id, &other) == UMI_STATUS_OK);
    uint64_t before = umi_chart_drawing_registry_revision(registry); size_t changed = 99U;
    UmiChartDrawingSnapshot actual;
    if (strcmp(name, "single") == 0 || strcmp(name, "duplicate") == 0) {
        CHECK(UmiChartDrawingSetHidden(registry, "NQ", first.id, first.revision, 1) == UMI_STATUS_OK);
        CHECK(umi_chart_drawing_registry_find(registry, first.id, &actual) == UMI_STATUS_OK);
        CHECK(actual.locked && actual.selected && actual.visibility_flags == UMI_CHART_DRAWING_VISIBILITY_HIDDEN);
        CHECK(actual.time1 == first.time1 && actual.time2 == first.time2 && actual.value1 == first.value1 && actual.value2 == first.value2 && strcmp(actual.style, first.style) == 0);
        if (strcmp(name, "duplicate") == 0) {
            CHECK(UmiChartDrawingDuplicate(registry, "NQ", first.id, actual.revision, "copy") == UMI_STATUS_OK);
            CHECK(umi_chart_drawing_registry_find(registry, "copy", &actual) == UMI_STATUS_OK);
            CHECK(actual.visibility_flags == UMI_CHART_DRAWING_VISIBILITY_HIDDEN && !actual.locked && !actual.selected);
        } else {
            uint64_t revision = umi_chart_drawing_registry_revision(registry);
            CHECK(UmiChartDrawingSetHidden(registry, "NQ", first.id, actual.revision, 1) == UMI_STATUS_OK);
            CHECK(umi_chart_drawing_registry_revision(registry) == revision);
            CHECK(UmiChartDrawingSetHidden(registry, "NQ", first.id, actual.revision, 0) == UMI_STATUS_OK);
            CHECK(umi_chart_drawing_registry_find(registry, first.id, &actual) == UMI_STATUS_OK && actual.visibility_flags == 0U);
        }
    } else if (strcmp(name, "stale") == 0) {
        CHECK(UmiChartDrawingSetHidden(registry, "NQ", first.id, first.revision - 1U, 1) == UMI_STATUS_BUSY);
        CHECK(umi_chart_drawing_registry_revision(registry) == before);
    } else if (strcmp(name, "wrong-pane") == 0) {
        CHECK(UmiChartDrawingSetHidden(registry, "ES", first.id, first.revision, 1) == UMI_STATUS_INVALID_STATE);
        CHECK(umi_chart_drawing_registry_revision(registry) == before);
    } else if (strcmp(name, "invalid") == 0) {
        CHECK(UmiChartDrawingSetHidden(registry, "NQ", first.id, first.revision, 2) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiChartDrawingSetHidden(NULL, "NQ", first.id, first.revision, 1) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiChartDrawingSetHidden(registry, NULL, first.id, first.revision, 1) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiChartDrawingSetHidden(registry, "NQ", "missing", first.revision, 1) == UMI_STATUS_NOT_FOUND);
        CHECK(UmiChartDrawingSetPaneHidden(registry, "NQ", before, -1, &changed) == UMI_STATUS_INVALID_ARGUMENT && changed == 99U);
        CHECK(umi_chart_drawing_registry_revision(registry) == before);
    } else if (strcmp(name, "batch-stale") == 0) {
        CHECK(UmiChartDrawingSetPaneHidden(registry, "NQ", before - 1U, 1, &changed) == UMI_STATUS_BUSY && changed == 99U);
        CHECK(umi_chart_drawing_registry_revision(registry) == before);
    } else if (strcmp(name, "empty") == 0) {
        CHECK(UmiChartDrawingSetPaneHidden(registry, "missing", before, 1, &changed) == UMI_STATUS_OK && changed == 0U);
        CHECK(umi_chart_drawing_registry_revision(registry) == before);
    } else if (strcmp(name, "batch-invalid") == 0) {
        second.visibility_flags = 2U; CHECK(umi_chart_drawing_registry_upsert(registry, &second) == UMI_STATUS_OK);
        before = umi_chart_drawing_registry_revision(registry);
        CHECK(UmiChartDrawingSetPaneHidden(registry, "NQ", before, 1, &changed) == UMI_STATUS_INVALID_ARGUMENT && changed == 99U);
        CHECK(umi_chart_drawing_registry_revision(registry) == before);
        CHECK(umi_chart_drawing_registry_find(registry, first.id, &actual) == UMI_STATUS_OK && actual.visibility_flags == 0U);
    } else if (strcmp(name, "batch") == 0 || strcmp(name, "future-tool") == 0) {
        if (strcmp(name, "future-tool") == 0) {
            strcpy(second.tool, "future.annotation"); CHECK(umi_chart_drawing_registry_upsert(registry, &second) == UMI_STATUS_OK);
            before = umi_chart_drawing_registry_revision(registry);
        }
        CHECK(UmiChartDrawingSetPaneHidden(registry, "NQ", before, 1, &changed) == UMI_STATUS_OK && changed == 2U);
        CHECK(umi_chart_drawing_registry_find(registry, first.id, &actual) == UMI_STATUS_OK && actual.visibility_flags == 1U && actual.locked);
        CHECK(umi_chart_drawing_registry_find(registry, second.id, &actual) == UMI_STATUS_OK && actual.visibility_flags == 1U);
        CHECK(strcmp(actual.tool, second.tool) == 0);
        before = umi_chart_drawing_registry_revision(registry);
        CHECK(UmiChartDrawingSetPaneHidden(registry, "NQ", before, 1, &changed) == UMI_STATUS_OK && changed == 0U);
        CHECK(umi_chart_drawing_registry_revision(registry) == before);
        CHECK(UmiChartDrawingSetPaneHidden(registry, "NQ", before, 0, &changed) == UMI_STATUS_OK && changed == 2U);
    } else { umi_chart_drawing_registry_destroy(registry); return 2; }
    CHECK(umi_chart_drawing_registry_find(registry, other.id, &actual) == UMI_STATUS_OK); SameDrawing(&other, &actual);
    umi_chart_drawing_registry_destroy(registry); return 0;
}

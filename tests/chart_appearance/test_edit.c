/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/chart_appearance/test_edit.c
 * PURPOSE: Verify guarded publication, no-op revisions and preservation of drawing properties.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
int main(int argc, char **argv)
{
    CHECK(argc == 2); const char *name = argv[1];
    UmiChartDrawingRegistry *registry = NULL; OK(umi_chart_drawing_registry_create(&registry));
    UmiChartDrawingSnapshot drawing = Drawing(UMI_CHART_DRAWING_RANGE), current;
    drawing.locked = 1; drawing.selected = 1; drawing.visibility_flags = UMI_CHART_DRAWING_VISIBILITY_HIDDEN;
    strcpy(drawing.style, "old-style");
    OK(umi_chart_drawing_registry_upsert(registry, &drawing));
    OK(umi_chart_drawing_registry_at(registry, 0, &drawing));
    UmiChartDrawingAppearance appearance = {51, 170, 255, 25, 30};
    if (strcmp(name, "apply") == 0 || strcmp(name, "no-op") == 0 || strcmp(name, "reset") == 0 || strcmp(name, "duplicate") == 0) {
        OK(UmiChartDrawingSetAppearance(registry, "pane", "drawing", drawing.revision, &appearance));
        OK(umi_chart_drawing_registry_at(registry, 0, &current));
        CHECK(strcmp(current.style, "umi-drawing:1:33AAFF:25:30") == 0 && current.revision > drawing.revision);
        CHECK(current.locked == 1 && current.selected == 1 && current.visibility_flags == 1);
        CHECK(current.time1 == drawing.time1 && current.time2 == drawing.time2 && current.value1 == drawing.value1 && current.value2 == drawing.value2);
        if (strcmp(name, "no-op") == 0) {
            uint64_t version = current.revision;
            OK(UmiChartDrawingSetAppearance(registry, "pane", "drawing", version, &appearance));
            CHECK(umi_chart_drawing_registry_revision(registry) == version);
        } else if (strcmp(name, "reset") == 0) {
            OK(UmiChartDrawingSetAppearance(registry, "pane", "drawing", current.revision, NULL));
            OK(umi_chart_drawing_registry_at(registry, 0, &current)); CHECK(current.style[0] == '\0');
            uint64_t version = current.revision;
            OK(UmiChartDrawingSetAppearance(registry, "pane", "drawing", version, NULL));
            CHECK(umi_chart_drawing_registry_revision(registry) == version);
        } else if (strcmp(name, "duplicate") == 0) {
            OK(UmiChartDrawingDuplicate(registry, "pane", "drawing", current.revision, "copy"));
            UmiChartDrawingSnapshot copy; OK(umi_chart_drawing_registry_find(registry, "copy", &copy));
            CHECK(strcmp(copy.style, current.style) == 0 && !copy.locked && !copy.selected && copy.visibility_flags == 1);
        }
    } else if (strcmp(name, "stale") == 0) {
        CHECK(UmiChartDrawingSetAppearance(registry, "pane", "drawing", drawing.revision - 1, &appearance) == UMI_STATUS_BUSY);
        OK(umi_chart_drawing_registry_remove(registry, "drawing"));
        OK(umi_chart_drawing_registry_upsert(registry, &drawing));
        CHECK(UmiChartDrawingSetAppearance(registry, "pane", "drawing", drawing.revision, &appearance) == UMI_STATUS_BUSY);
        OK(umi_chart_drawing_registry_at(registry, 0, &current)); CHECK(strcmp(current.style, "old-style") == 0);
    } else if (strcmp(name, "wrong-pane") == 0) {
        CHECK(UmiChartDrawingSetAppearance(registry, "other", "drawing", drawing.revision, &appearance) == UMI_STATUS_INVALID_STATE);
        CHECK(umi_chart_drawing_registry_revision(registry) == drawing.revision);
    } else if (strcmp(name, "invalid") == 0) {
        appearance.red = 256;
        CHECK(UmiChartDrawingSetAppearance(registry, "pane", "drawing", drawing.revision, &appearance) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiChartDrawingSetAppearance(registry, "", "drawing", drawing.revision, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiChartDrawingSetAppearance(registry, "pane", "missing", drawing.revision, NULL) == UMI_STATUS_NOT_FOUND);
        CHECK(umi_chart_drawing_registry_revision(registry) == drawing.revision);
    } else if (strcmp(name, "unsupported-tool") == 0) {
        strcpy(drawing.tool, "future-tool"); OK(umi_chart_drawing_registry_upsert(registry, &drawing));
        OK(umi_chart_drawing_registry_at(registry, 0, &drawing));
        CHECK(UmiChartDrawingSetAppearance(registry, "pane", "drawing", drawing.revision, &appearance) == UMI_STATUS_UNAVAILABLE);
        CHECK(umi_chart_drawing_registry_revision(registry) == drawing.revision);
    } else return 2;
    umi_chart_drawing_registry_destroy(registry); return 0;
}

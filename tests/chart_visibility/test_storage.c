/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/chart_visibility/test_storage.c
 * PURPOSE: Keep hidden geometry in immutable documents, transactional saves and recovery.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "../chart_checkpoint/fixture.h"
#include "umicom/chart/drawing_visibility.h"

/* Locate the primary drawing through the public backend for corruption testing. */
static UmiStatus PrimaryKey(const char *key, const char *value, void *data)
{
    (void)value;
    if (strstr(key, ".primary.drawing.0") != NULL) {
        CHECK(strlen(key) < 192U); strcpy(data, key);
    }
    return UMI_STATUS_OK;
}

/* Drive one registered scenario through the public feature owners. */
int main(int argc, char **argv)
{
    CHECK(argc == 2); const char *name = argv[1];
    UmiDataServer *server = NULL; UmiChartDrawingRegistry *registry = NULL;
    CHECK(umi_data_server_create_memory(&server) == UMI_STATUS_OK);
    CHECK(umi_chart_drawing_registry_create(&registry) == UMI_STATUS_OK);
    UmiChartDrawingSnapshot drawing = Drawing("hidden", "NQ"), actual;
    drawing.visibility_flags = UMI_CHART_DRAWING_VISIBILITY_HIDDEN;
    CHECK(umi_chart_drawing_registry_upsert(registry, &drawing) == UMI_STATUS_OK);
    CHECK(umi_chart_drawing_registry_find(registry, drawing.id, &drawing) == UMI_STATUS_OK);
/* The appended timeframe defaults to source bars. This explicit zero retains the fixture semantics when all consumers are rebuilt. The previous implementation remains for engineering review. */
#if 0
    UmiChartNavigation navigation = {20U, 120000, 1};
#endif
    UmiChartNavigation navigation = {20U, 120000, 1, 0U};
    UmiChartDocument *hidden = NULL, *visible = NULL, *loaded = NULL;
    CHECK(UmiChartDocumentCapture(registry, "NQ", &navigation, &hidden) == UMI_STATUS_OK);
    UmiChartCheckpointReport report;
    CHECK(UmiChartCheckpointSave(server, "visibility", hidden, 0U, 1000U, &report) == UMI_STATUS_OK);
    CHECK(UmiChartDrawingSetHidden(registry, "NQ", drawing.id, drawing.revision, 0) == UMI_STATUS_OK);
    CHECK(UmiChartDocumentCapture(registry, "NQ", &navigation, &visible) == UMI_STATUS_OK);
    CHECK(UmiChartDocumentDrawingAt(hidden, 0U, &actual) == UMI_STATUS_OK); SameDrawing(&drawing, &actual);
    uint64_t revision = umi_chart_drawing_registry_revision(registry);
    if (strcmp(name, "recovery") == 0) {
        CHECK(UmiChartCheckpointSave(server, "visibility", visible, report.storage_revision, 2000U, &report) == UMI_STATUS_OK);
        char key[192] = {0}; CHECK(umi_data_server_visit(server, PrimaryKey, key) == UMI_STATUS_OK && key[0]);
        CHECK(umi_data_server_set(server, key, "broken") == UMI_STATUS_OK);
        CHECK(UmiChartCheckpointLoad(server, "visibility", "NQ", &loaded, &report) == UMI_STATUS_OK && report.recovered_last_good);
        CHECK(report.saved_at_ms == 1000U && !report.storage_revision_known);
    } else {
        CHECK(strcmp(name, "roundtrip") == 0 || strcmp(name, "stale") == 0 || strcmp(name, "immutable") == 0);
        CHECK(UmiChartCheckpointLoad(server, "visibility", "NQ", &loaded, &report) == UMI_STATUS_OK && !report.recovered_last_good);
    }
    CHECK(UmiChartDocumentDrawingAt(loaded, 0U, &actual) == UMI_STATUS_OK); SameDrawing(&drawing, &actual);
    if (strcmp(name, "stale") == 0) {
        CHECK(UmiChartDocumentApplyDrawings(loaded, registry, revision - 1U) == UMI_STATUS_INVALID_STATE);
        CHECK(umi_chart_drawing_registry_revision(registry) == revision);
        CHECK(umi_chart_drawing_registry_find(registry, drawing.id, &actual) == UMI_STATUS_OK && actual.visibility_flags == 0U);
    } else {
        CHECK(UmiChartDocumentApplyDrawings(loaded, registry, revision) == UMI_STATUS_OK);
        CHECK(umi_chart_drawing_registry_find(registry, drawing.id, &actual) == UMI_STATUS_OK);
        CHECK(actual.visibility_flags == UMI_CHART_DRAWING_VISIBILITY_HIDDEN && actual.locked && actual.selected && actual.value2 == drawing.value2);
        CHECK(UmiChartDocumentDrawingAt(visible, 0U, &actual) == UMI_STATUS_OK && actual.visibility_flags == 0U);
    }
    UmiChartDocumentDestroy(hidden); UmiChartDocumentDestroy(visible); UmiChartDocumentDestroy(loaded);
    umi_chart_drawing_registry_destroy(registry); umi_data_server_destroy(server); return 0;
}

/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/chart_checkpoint/test_sqlite.c
 * PURPOSE: Check restart durability, real transaction rollback and full drawing capacity.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
int main(int argc, char **argv)
{
    CHECK(argc == 3);
    /* Reserve a new test-owned file; never truncate someone else's database. */
    FILE *reserved = fopen(argv[2], "wx"); CHECK(reserved != NULL); CHECK(fclose(reserved) == 0);
    UmiDataServer *server = NULL;
    UmiStatus status = umi_data_server_create_sqlite(argv[2], &server);
    if (status == UMI_STATUS_UNAVAILABLE) { CHECK(remove(argv[2]) == 0); return 77; }
    CHECK(status == UMI_STATUS_OK);
    UmiChartDocument *first = Document("NQ", 110), *second = Document("NQ", 120), *loaded = NULL;
    /* The proposed second save also changes visibility. Restart must retain
     * it; rollback must keep the first visible drawing and its earlier value. */
    UmiChartDrawingSnapshot proposed;
    CHECK(UmiChartDocumentDrawingAt(second, 0U, &proposed) == UMI_STATUS_OK);
    proposed.visibility_flags = UMI_CHART_DRAWING_VISIBILITY_HIDDEN;
    UmiChartDocumentSummary proposedSummary;
    CHECK(UmiChartDocumentGetSummary(second, &proposedSummary) == UMI_STATUS_OK);
    UmiChartDocument *hiddenSecond = NULL;
    CHECK(UmiChartDocumentCreate("NQ", &proposedSummary.navigation, &proposed, 1U,
        proposedSummary.source_revision, &hiddenSecond) == UMI_STATUS_OK);
    UmiChartDocumentDestroy(second); second = hiddenSecond;
    UmiChartCheckpointReport report;
    CHECK(UmiChartCheckpointSave(server, "local", first, 0U, 1000U, &report) == UMI_STATUS_OK && report.durable);
    if (strcmp(argv[1], "rollback") == 0 || strcmp(argv[1], "automatic-abort") == 0) {
        /* Fail after backup publication and primary deletion, at the new
         * primary manifest. One transaction must restore every earlier row. */
        const char *trigger = strcmp(argv[1], "automatic-abort") == 0
            ? "CREATE TRIGGER reject_chart BEFORE INSERT ON umicom_kv WHEN NEW.key LIKE '%.primary.manifest' BEGIN SELECT RAISE(ROLLBACK, 'fixture failure'); END;"
            : "CREATE TRIGGER reject_chart BEFORE INSERT ON umicom_kv WHEN NEW.key LIKE '%.primary.manifest' BEGIN SELECT RAISE(ABORT, 'fixture failure'); END;";
        CHECK(umi_data_server_execute(server, trigger) == UMI_STATUS_OK);
        CHECK(UmiChartCheckpointSave(server, "local", second, 1U, 2000U, &report) != UMI_STATUS_OK);
        CHECK(!umi_data_server_in_transaction(server));
        CHECK(umi_data_server_execute(server, "DROP TRIGGER reject_chart;") == UMI_STATUS_OK);
        CHECK(UmiChartCheckpointLoad(server, "local", "NQ", &loaded, &report) == UMI_STATUS_OK && Value(loaded) == 110 && report.storage_revision == 1U);
        CHECK(umi_data_server_count(server) == 2U);
        UmiChartDrawingSnapshot retained;
        CHECK(UmiChartDocumentDrawingAt(loaded, 0U, &retained) == UMI_STATUS_OK && retained.visibility_flags == 0U);
    } else if (strcmp(argv[1], "capacity") == 0) {
        UmiChartDrawingSnapshot *many = calloc(UMI_CHART_DRAWING_CAPACITY, sizeof(*many)); CHECK(many != NULL);
        for (size_t i = 0U; i < UMI_CHART_DRAWING_CAPACITY; ++i) {
            many[i] = Drawing("placeholder", "NQ"); (void)snprintf(many[i].id, sizeof(many[i].id), "drawing.%zu", i);
            many[i].value2 = (double)i; memset(many[i].style, '%', sizeof(many[i].style) - 1U);
            many[i].visibility_flags = (i % 2U) ? UMI_CHART_DRAWING_VISIBILITY_HIDDEN : 0U;
        }
/* The appended timeframe defaults to source bars. This explicit zero retains the fixture semantics when all consumers are rebuilt. The previous implementation remains for engineering review. */
#if 0
        UmiChartNavigation navigation = {4096U, INT64_MIN, 1}; UmiChartDocument *full = NULL;
#endif
        UmiChartNavigation navigation = {4096U, INT64_MIN, 1, 0U}; UmiChartDocument *full = NULL;
        CHECK(UmiChartDocumentCreate("NQ", &navigation, many, UMI_CHART_DRAWING_CAPACITY, 99U, &full) == UMI_STATUS_OK);
        CHECK(UmiChartCheckpointSave(server, "local", full, 1U, 2000U, &report) == UMI_STATUS_OK);
        CHECK(UmiChartCheckpointSave(server, "local", full, 2U, 3000U, &report) == UMI_STATUS_OK);
        CHECK(umi_data_server_count(server) == 2U * (UMI_CHART_DRAWING_CAPACITY + 1U));
        CHECK(UmiChartCheckpointLoad(server, "local", "NQ", &loaded, &report) == UMI_STATUS_OK && report.drawing_count == UMI_CHART_DRAWING_CAPACITY);
        UmiChartDrawingSnapshot final;
        CHECK(UmiChartDocumentDrawingAt(loaded, UMI_CHART_DRAWING_CAPACITY - 1U, &final) == UMI_STATUS_OK);
        SameDrawing(&many[UMI_CHART_DRAWING_CAPACITY - 1U], &final); UmiChartDocumentDestroy(full); free(many);
    } else {
        CHECK(strcmp(argv[1], "restart") == 0);
        CHECK(UmiChartCheckpointSave(server, "local", second, 1U, 2000U, &report) == UMI_STATUS_OK);
        umi_data_server_destroy(server); server = NULL;
        CHECK(umi_data_server_create_sqlite(argv[2], &server) == UMI_STATUS_OK);
        CHECK(UmiChartCheckpointLoad(server, "local", "NQ", &loaded, &report) == UMI_STATUS_OK);
        CHECK(Value(loaded) == 120 && report.storage_revision == 2U && report.saved_at_ms == 2000U && report.durable);
        UmiChartDrawingSnapshot restarted;
        CHECK(UmiChartDocumentDrawingAt(loaded, 0U, &restarted) == UMI_STATUS_OK);
        CHECK(restarted.visibility_flags == UMI_CHART_DRAWING_VISIBILITY_HIDDEN);
        UmiDataServer *other = NULL; CHECK(umi_data_server_create_sqlite(argv[2], &other) == UMI_STATUS_OK);
        CHECK(UmiChartCheckpointSave(other, "local", first, 1U, 3000U, &report) == UMI_STATUS_INVALID_STATE);
        CHECK(umi_data_server_begin(server) == UMI_STATUS_OK);
        CHECK(UmiChartCheckpointSave(other, "local", first, 2U, 3000U, &report) == UMI_STATUS_BUSY);
        CHECK(umi_data_server_rollback(server) == UMI_STATUS_OK); umi_data_server_destroy(other);
    }
    UmiChartDocumentDestroy(first); UmiChartDocumentDestroy(second); UmiChartDocumentDestroy(loaded);
    umi_data_server_destroy(server); CHECK(remove(argv[2]) == 0); return 0;
}

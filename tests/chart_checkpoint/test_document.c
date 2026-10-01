/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/chart_checkpoint/test_document.c
 * PURPOSE: Check immutable ownership and all-or-nothing replacement of a single chart pane.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    UmiChartDrawingRegistry *registry = NULL;
    CHECK(umi_chart_drawing_registry_create(&registry) == UMI_STATUS_OK);
    UmiChartDrawingSnapshot first = Drawing("one", "NQ"), foreign = Drawing("foreign", "ES"), output;
    CHECK(umi_chart_drawing_registry_upsert(registry, &first) == UMI_STATUS_OK);
    CHECK(umi_chart_drawing_registry_upsert(registry, &foreign) == UMI_STATUS_OK);
    CHECK(umi_chart_drawing_registry_find(registry, "foreign", &foreign) == UMI_STATUS_OK);
/* The appended timeframe defaults to source bars. This explicit zero retains the fixture semantics when all consumers are rebuilt. The previous implementation remains for engineering review. */
#if 0
    UmiChartNavigation navigation = {12U, INT64_MIN, 1}; UmiChartDocument *document = NULL;
#endif
    UmiChartNavigation navigation = {12U, INT64_MIN, 1, 0U}; UmiChartDocument *document = NULL;
    CHECK(UmiChartDocumentCapture(registry, "NQ", &navigation, &document) == UMI_STATUS_OK);
    uint64_t revision = umi_chart_drawing_registry_revision(registry);
    if (strcmp(argv[1], "ownership") == 0) {
        first.value2 = 999; CHECK(umi_chart_drawing_registry_upsert(registry, &first) == UMI_STATUS_OK);
        CHECK(Value(document) == 110.125);
        CHECK(UmiChartDocumentDrawingAt(document, 1U, &output) == UMI_STATUS_NOT_FOUND);
    } else if (strcmp(argv[1], "replace") == 0) {
        CHECK(UmiChartDocumentApplyDrawings(document, registry, revision) == UMI_STATUS_OK);
        CHECK(umi_chart_drawing_registry_find(registry, "foreign", &output) == UMI_STATUS_OK); SameDrawing(&foreign, &output);
        CHECK(umi_chart_drawing_registry_find(registry, "one", &output) == UMI_STATUS_OK);
        CHECK(output.locked && output.selected && output.revision == revision + 1U);
        CHECK(UmiChartDocumentApplyDrawings(document, registry, revision) == UMI_STATUS_INVALID_STATE);
        CHECK(umi_chart_drawing_registry_revision(registry) == revision + 1U);
    } else if (strcmp(argv[1], "empty") == 0) {
        UmiChartDocument *empty = NULL;
        CHECK(UmiChartDocumentCreate("NQ", &navigation, NULL, 0U, 99U, &empty) == UMI_STATUS_OK);
        CHECK(UmiChartDocumentApplyDrawings(empty, registry, revision) == UMI_STATUS_OK);
        CHECK(umi_chart_drawing_registry_count(registry) == 1U);
        CHECK(umi_chart_drawing_registry_at(registry, 0U, &output) == UMI_STATUS_OK); SameDrawing(&foreign, &output);
        UmiChartDocumentDestroy(empty);
    } else if (strcmp(argv[1], "collision") == 0) {
        strcpy(first.id, "foreign");
        CHECK(UmiChartDrawingRegistryReplacePane(registry, "NQ", &first, 1U, revision) == UMI_STATUS_ALREADY_EXISTS);
        CHECK(umi_chart_drawing_registry_revision(registry) == revision && umi_chart_drawing_registry_count(registry) == 2U);
    } else if (strcmp(argv[1], "invalid") == 0) {
        UmiChartDocument *invalid = document;
        first.value2 = NAN;
        CHECK(UmiChartDocumentCreate("NQ", &navigation, &first, 1U, 0U, &invalid) == UMI_STATUS_INVALID_ARGUMENT && invalid == NULL);
        CHECK(UmiChartDrawingRegistryReplacePane(registry, "NQ", &first, 1U, revision) == UMI_STATUS_INVALID_ARGUMENT);
        first.value2 = 1; first.locked = 2;
        CHECK(UmiChartDrawingValidateGeometry(&first) == UMI_STATUS_INVALID_ARGUMENT);
        first.locked = 1; navigation.pinned = -1;
        CHECK(UmiChartDocumentCreate("NQ", &navigation, &first, 1U, 0U, &invalid) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_chart_drawing_registry_revision(registry) == revision);
    } else {
        CHECK(strcmp(argv[1], "capacity") == 0);
        UmiChartDrawingSnapshot *many = calloc(UMI_CHART_DRAWING_CAPACITY, sizeof(*many)); CHECK(many != NULL);
        for (size_t i = 0U; i < UMI_CHART_DRAWING_CAPACITY; ++i) { many[i] = first; (void)snprintf(many[i].id, sizeof(many[i].id), "item.%zu", i); }
        CHECK(UmiChartDrawingRegistryReplacePane(registry, "NQ", many, UMI_CHART_DRAWING_CAPACITY, revision) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(umi_chart_drawing_registry_revision(registry) == revision);
        CHECK(umi_chart_drawing_registry_remove(registry, "foreign") == UMI_STATUS_OK);
        CHECK(UmiChartDrawingRegistryReplacePane(registry, "NQ", many, UMI_CHART_DRAWING_CAPACITY, revision + 1U) == UMI_STATUS_OK);
        CHECK(umi_chart_drawing_registry_count(registry) == UMI_CHART_DRAWING_CAPACITY); free(many);
    }
    UmiChartDocumentDestroy(document); umi_chart_drawing_registry_destroy(registry); return 0;
}

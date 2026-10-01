/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/chart_checkpoint/test_codec.c
 * PURPOSE: Exercise exact numeric round trips and malformed persisted records.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
#include "../../src/chart/checkpoint_private.h"
#include <float.h>
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    UmiWorkbenchLayoutDataFieldSet *fields = calloc(1U, sizeof(*fields)); CHECK(fields != NULL);
    UmiChartDrawingSnapshot input = Drawing("a%\n=b", "NQ"), output = {0};
    input.time1 = INT64_MIN; input.time2 = INT64_MAX; input.value1 = -0.0; input.value2 = DBL_MAX; input.revision = UINT64_MAX;
    char text[UMI_CHART_CHECKPOINT_VALUE], changed[UMI_CHART_CHECKPOINT_VALUE * 2U];
    CHECK(UmiChartCheckpointEncodeDrawing(fields, &input, text) == UMI_STATUS_OK);
    if (strcmp(argv[1], "exact") == 0) {
        CHECK(UmiChartCheckpointDecodeDrawing(fields, text, &output) == UMI_STATUS_OK); SameDrawing(&input, &output);
        input.value2 = DBL_MIN;
        CHECK(UmiChartCheckpointEncodeDrawing(fields, &input, text) == UMI_STATUS_OK);
        CHECK(UmiChartCheckpointDecodeDrawing(fields, text, &output) == UMI_STATUS_OK); SameDrawing(&input, &output);
    } else if (strcmp(argv[1], "duplicate") == 0) {
        (void)snprintf(changed, sizeof(changed), "%sid=overwritten\n", text);
        CHECK(UmiChartCheckpointDecodeDrawing(fields, changed, &output) == UMI_STATUS_PARSE_ERROR);
    } else if (strcmp(argv[1], "nul") == 0) {
        char *value = strstr(text, "style="); CHECK(value != NULL);
        (void)snprintf(changed, sizeof(changed), "%.*sstyle=hidden%%00tail\n%s", (int)(value - text), text, strchr(value, '\n') + 1);
        CHECK(UmiChartCheckpointDecodeDrawing(fields, changed, &output) == UMI_STATUS_PARSE_ERROR);
    } else if (strcmp(argv[1], "numeric") == 0) {
        const char *bad[] = {"-1", "+1", "", " 1", "18446744073709551616", "1x"};
        char *value = strstr(text, "revision="); CHECK(value != NULL);
        for (size_t i = 0U; i < sizeof(bad) / sizeof(bad[0]); ++i) {
            (void)snprintf(changed, sizeof(changed), "%.*srevision=%s\n", (int)(value - text), text, bad[i]);
            CHECK(UmiChartCheckpointDecodeDrawing(fields, changed, &output) == UMI_STATUS_PARSE_ERROR);
        }
    } else {
        CHECK(strcmp(argv[1], "metadata") == 0);
        ChartCheckpointMetadata metadata = {0}, decoded = {0}; strcpy(metadata.scope, "profile.one"); strcpy(metadata.summary.pane_id, "NQ");
/* The appended timeframe defaults to source bars. This explicit zero retains the fixture semantics when all consumers are rebuilt. The previous implementation remains for engineering review. */
#if 0
        metadata.summary.navigation = (UmiChartNavigation){4096U, INT64_MIN, 1};
#endif
        metadata.summary.navigation = (UmiChartNavigation){4096U, INT64_MIN, 1, 0U};
        metadata.storageRevision = UINT64_MAX; metadata.savedAtMs = UINT64_MAX; metadata.summary.source_revision = UINT64_MAX;
        metadata.summary.drawing_count = 4096U;
        CHECK(UmiChartCheckpointEncodeMetadata(fields, &metadata, text) == UMI_STATUS_OK);
        CHECK(UmiChartCheckpointDecodeMetadata(fields, text, &decoded) == UMI_STATUS_OK);
        CHECK(decoded.storageRevision == UINT64_MAX && decoded.summary.navigation.anchor_ms == INT64_MIN && decoded.summary.drawing_count == 4096U);
        text[strlen(text)-1U] = '\0'; CHECK(UmiChartCheckpointDecodeMetadata(fields, text, &decoded) == UMI_STATUS_PARSE_ERROR);
    }
    free(fields); return 0;
}

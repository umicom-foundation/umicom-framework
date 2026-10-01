/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/chart_visibility/test_codec.c
 * PURPOSE: Verify legacy visibility defaults and exact version-two record validation.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "../chart_checkpoint/fixture.h"
#include "../../src/chart/checkpoint_private.h"

/* Remove complete named lines when creating genuine old-format records. */
static void Without(const char *input, const char *key, char *output)
{
    size_t used = 0U, keyBytes = strlen(key);
    while (*input != '\0') {
        const char *end = strchr(input, '\n'); CHECK(end != NULL);
        size_t length = (size_t)(end - input) + 1U;
        if (!(length > keyBytes && strncmp(input, key, keyBytes) == 0 && input[keyBytes] == '=')) {
            CHECK(used + length < UMI_CHART_CHECKPOINT_VALUE);
            memcpy(output + used, input, length); used += length;
        }
        input = end + 1U;
    }
    output[used] = '\0';
}

/* Leave the output sentinel untouched when any part of a record is invalid. */
int main(int argc, char **argv)
{
    CHECK(argc == 2); const char *name = argv[1];
    UmiWorkbenchLayoutDataFieldSet *fields = calloc(1U, sizeof *fields); CHECK(fields != NULL);
    UmiChartDrawingSnapshot input = Drawing("quoted%=\n", "NQ"), output = {0};
    input.visibility_flags = strcmp(name, "visible") == 0 ? 0U : 1U;
    char text[UMI_CHART_CHECKPOINT_VALUE], intermediate[UMI_CHART_CHECKPOINT_VALUE], changed[UMI_CHART_CHECKPOINT_VALUE];
    CHECK(UmiChartCheckpointEncodeDrawing(fields, &input, text) == UMI_STATUS_OK);
    if (strcmp(name, "legacy") == 0) {
        Without(text, "drawing_schema", intermediate); Without(intermediate, "visibility_flags", changed);
        CHECK(UmiChartCheckpointDecodeDrawing(fields, changed, &output) == UMI_STATUS_OK);
        input.visibility_flags = 0U; SameDrawing(&input, &output);
    } else if (strcmp(name, "hidden") == 0 || strcmp(name, "visible") == 0) {
        CHECK(UmiChartCheckpointDecodeDrawing(fields, text, &output) == UMI_STATUS_OK); SameDrawing(&input, &output);
    } else {
        strcpy(changed, text);
        if (strcmp(name, "partial") == 0) Without(text, "visibility_flags", changed);
        else if (strcmp(name, "missing-schema") == 0) Without(text, "drawing_schema", changed);
        else if (strcmp(name, "version") == 0 || strcmp(name, "flags") == 0) {
            CHECK(umi_workbench_layout_data_value_decode(text, fields) == UMI_STATUS_OK);
            CHECK(umi_workbench_layout_data_field_set_put(fields, strcmp(name, "version") == 0 ? "drawing_schema" : "visibility_flags", "3") == UMI_STATUS_OK);
            CHECK(umi_workbench_layout_data_value_encode(fields, changed, sizeof changed, NULL) == UMI_STATUS_OK);
        } else if (strcmp(name, "duplicate") == 0) {
            CHECK(strlen(text) + sizeof("visibility_flags=0\n") < sizeof changed);
            strcat(changed, "visibility_flags=0\n");
        } else { free(fields); return 2; }
        UmiChartDrawingSnapshot sentinel; memset(&sentinel, 0x5a, sizeof sentinel); output = sentinel;
        CHECK(UmiChartCheckpointDecodeDrawing(fields, changed, &output) == UMI_STATUS_PARSE_ERROR);
        CHECK(memcmp(&sentinel, &output, sizeof output) == 0);
    }
    free(fields); return 0;
}

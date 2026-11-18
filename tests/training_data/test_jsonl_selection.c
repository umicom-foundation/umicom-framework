/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/training_data/test_jsonl_selection.c
 *
 * PURPOSE:
 *   Check selected-row order, canonical values, exact membership, capacity and all-or-nothing dataset output.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/ai/training/jsonl_selection.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr,"%d: %s\n",__LINE__,#x); return 1; } } while (0)
#define A "{\"completion\":\"A\",\"prompt\":\"one\",\"source_id\":\"\\u0061\"}\r\n"
#define B "{\"source_id\":\"b\",\"prompt\":\"two\",\"completion\":\"B\"}\n"
#define C "{\"source_id\":\"a\",\"prompt\":\"three\",\"completion\":\"C\"}"
#define EXPECT_A "{\"source_id\":\"a\",\"prompt\":\"one\",\"completion\":\"A\"}\n"
#define EXPECT_C C "\n"

int main(int argc, char **argv)
{
    CHECK(argc == 2);
    const char *mode = argv[1], *input = A B C;
    unsigned char membership[4] = {0U, 1U, 0U, 0U};
    char output[512], before[512];
    memset(output, '~', sizeof(output)); memcpy(before, output, sizeof(output));
    UmiAiTrainingJsonlSelectionSummary summary, original;
    memset(&summary, 0x5a, sizeof(summary)); original = summary;
    UmiAiTrainingJsonlReport report;
    if (strcmp(mode, "training") == 0 || strcmp(mode, "evaluation") == 0) {
        unsigned char side = strcmp(mode, "training") == 0 ? 0U : 1U;
        const char *expected = side == 0U ? EXPECT_A EXPECT_C : B;
        CHECK(UmiAiTrainingJsonlSelectionWrite(input, strlen(input), membership, 3U, side,
            NULL, output, sizeof(output), &summary, &report) == UMI_STATUS_OK);
        CHECK(strcmp(output, expected) == 0 && output[strlen(expected) + 1U] == '~');
        CHECK(summary.inputRows == 3U && summary.selectedRows == (side == 0U ? 2U : 1U));
        CHECK(summary.outputBytes == strlen(expected) && report.acceptedRows == 3U);
    } else if (strcmp(mode, "all-and-none") == 0) {
        memset(membership, 0, sizeof(membership));
        CHECK(UmiAiTrainingJsonlSelectionWrite(input, strlen(input), membership, 3U, 1U,
            NULL, output, 1U, &summary, &report) == UMI_STATUS_OK);
        CHECK(output[0] == 0 && output[1] == '~' && summary.selectedRows == 0U && summary.outputBytes == 0U);
        CHECK(UmiAiTrainingJsonlSelectionWrite(input, strlen(input), membership, 3U, 0U,
            NULL, output, sizeof(output), &summary, &report) == UMI_STATUS_OK);
        CHECK(strcmp(output, EXPECT_A B EXPECT_C) == 0 && summary.selectedRows == 3U);
    } else if (strcmp(mode, "capacity") == 0) {
        const size_t needed = strlen(EXPECT_A EXPECT_C);
        CHECK(UmiAiTrainingJsonlSelectionWrite(input, strlen(input), membership, 3U, 0U,
            NULL, output, needed, &summary, &report) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(memcmp(output, before, sizeof(output)) == 0 && memcmp(&summary, &original, sizeof(summary)) == 0);
        CHECK(report.stage == UMI_AI_TRAINING_JSONL_COMPLETE);
        CHECK(UmiAiTrainingJsonlSelectionWrite(input, strlen(input), membership, 3U, 0U,
            NULL, output, needed + 1U, &summary, &report) == UMI_STATUS_OK);
        CHECK(strcmp(output, EXPECT_A EXPECT_C) == 0 && output[needed + 1U] == '~');
    } else if (strcmp(mode, "membership-count") == 0) {
        for (size_t count = 2U; count <= 4U; count += 2U) {
            CHECK(UmiAiTrainingJsonlSelectionWrite(input, strlen(input), membership, count, 0U,
                NULL, output, sizeof(output), &summary, &report) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(memcmp(output, before, sizeof(output)) == 0 && memcmp(&summary, &original, sizeof(summary)) == 0);
        }
    } else if (strcmp(mode, "invalid-unselected") == 0) {
        input = A "{}\n" C;
        CHECK(UmiAiTrainingJsonlSelectionWrite(input, strlen(input), membership, 3U, 0U,
            NULL, output, sizeof(output), &summary, &report) == UMI_STATUS_PARSE_ERROR);
        CHECK(report.acceptedRows == 1U && report.failedLine == 2U);
        CHECK(memcmp(output, before, sizeof(output)) == 0 && memcmp(&summary, &original, sizeof(summary)) == 0);
    } else if (strcmp(mode, "escaped-values") == 0) {
        input = "{\"source_id\":\"\\u0061\",\"prompt\":\"line\\nquote\\\"\",\"completion\":\"caf\\u00e9\"}";
        const char *expected = "{\"source_id\":\"a\",\"prompt\":\"line\\nquote\\\"\",\"completion\":\"caf\xc3\xa9\"}\n";
        CHECK(UmiAiTrainingJsonlSelectionWrite(input, strlen(input), membership, 1U, 0U,
            NULL, output, sizeof(output), &summary, &report) == UMI_STATUS_OK);
        CHECK(strcmp(output, expected) == 0 && summary.selectedRows == 1U);
    } else if (strcmp(mode, "cancel") == 0) {
        UmiCancellationToken *token = NULL;
        CHECK(umi_cancellation_token_create(&token) == UMI_STATUS_OK);
        umi_cancellation_token_request(token);
        UmiStatus status = UmiAiTrainingJsonlSelectionWrite(input, strlen(input), membership, 3U, 0U,
            token, output, sizeof(output), &summary, &report);
        umi_cancellation_token_destroy(token);
        CHECK(status == UMI_STATUS_CANCELLED && report.acceptedRows == 0U);
        CHECK(memcmp(output, before, sizeof(output)) == 0 && memcmp(&summary, &original, sizeof(summary)) == 0);
    } else if (strcmp(mode, "row-bound") == 0) {
        const size_t stride = strlen(B), rows = UMI_AI_TRAINING_JSONL_ROWS;
        char *many = malloc(stride * (rows + 1U));
        unsigned char *flags = calloc(rows, 1U);
        CHECK(many != NULL && flags != NULL);
        for (size_t i = 0U; i <= rows; ++i) memcpy(many + i * stride, B, stride);
        UmiStatus accepted = UmiAiTrainingJsonlSelectionWrite(many, stride * rows, flags, rows, 1U,
            NULL, output, sizeof(output), &summary, &report);
        UmiStatus rejected = UmiAiTrainingJsonlSelectionWrite(many, stride * (rows + 1U), flags, rows, 1U,
            NULL, output, sizeof(output), &summary, &report);
        free(flags); free(many);
        CHECK(accepted == UMI_STATUS_OK && rejected == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(summary.inputRows == rows && summary.selectedRows == 0U && output[0] == 0 && output[1] == '~');
    } else if (strcmp(mode, "arguments") == 0) {
        CHECK(UmiAiTrainingJsonlSelectionWrite(NULL, 1U, membership, 3U, 0U, NULL,
            output, sizeof(output), &summary, &report) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiAiTrainingJsonlSelectionWrite(input, 0U, membership, 3U, 0U, NULL,
            output, sizeof(output), &summary, &report) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiAiTrainingJsonlSelectionWrite(input, strlen(input), NULL, 3U, 0U, NULL,
            output, sizeof(output), &summary, &report) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiAiTrainingJsonlSelectionWrite(input, strlen(input), membership, 3U, 2U, NULL,
            output, sizeof(output), &summary, &report) == UMI_STATUS_INVALID_ARGUMENT);
        membership[1] = 2U;
        CHECK(UmiAiTrainingJsonlSelectionWrite(input, strlen(input), membership, 3U, 0U, NULL,
            output, sizeof(output), &summary, &report) == UMI_STATUS_INVALID_ARGUMENT);
        membership[1] = 1U;
        CHECK(UmiAiTrainingJsonlSelectionWrite(input, strlen(input), membership, 3U, 0U, NULL,
            NULL, sizeof(output), &summary, &report) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiAiTrainingJsonlSelectionWrite(input, strlen(input), membership, 3U, 0U, NULL,
            output, sizeof(output), NULL, &report) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiAiTrainingJsonlSelectionWrite(input, strlen(input), membership, 3U, 0U, NULL,
            output, sizeof(output), &summary, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(memcmp(output, before, sizeof(output)) == 0 && memcmp(&summary, &original, sizeof(summary)) == 0);
    } else return 2;
    return 0;
}

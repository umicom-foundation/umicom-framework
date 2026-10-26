/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_training_supervised_dataset_rejections.c
 * PURPOSE: Test tampering, invalid JSON, full-source split admission, capacity
 *          refusals and failure-atomic caller output.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "supervised_fixture.h"
#include <stdio.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "dataset refusal:%d %s\n", __LINE__, #x); return 1; } } while (0)
int main(void)
{
    UmiTestSupervisedFixture fixture;
    UmiAiTrainingSupervisedDataset *dataset = NULL, *refused = NULL;
    CHECK(UmiTestSupervisedFixturePrepare(&fixture) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingSupervisedDatasetCreate(fixture.records, fixture.lengths,
          fixture.evidence, 4U, 8U, &dataset) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingSupervisedDatasetCreate(fixture.records, fixture.lengths,
          fixture.evidence, 4U, 8U, &dataset) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingSupervisedDatasetCreate(fixture.records, fixture.lengths,
          fixture.evidence, 4U, 0U, &refused) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(refused == NULL);
    /* Reject source tampering before parsing. */
    char corrupted[UMI_TEST_SUPERVISED_JSON_CAP];
    memcpy(corrupted, fixture.records[0], fixture.lengths[0] + 1U);
    corrupted[12] = corrupted[12] == 'Q' ? 'P' : 'Q';
    const char *invalidRows[4] = {
        corrupted, fixture.records[1], fixture.records[2], fixture.records[3]
    };
    CHECK(UmiAiTrainingSupervisedDatasetCreate(invalidRows, fixture.lengths,
          fixture.evidence, 4U, 8U, &refused) != UMI_STATUS_OK);
    CHECK(refused == NULL);
    /* Even a record with an updated matching SHA is refused if it does not
     * obey the canonical producer's exact two-field schema. CorpusAudit owns
     * the hash; SupervisedDataset owns the deliberately narrow JSON reader. */
    const char foreignSchema[] =
        "{\"prompt\":\"Question\",\"unknown\":\"Answer\"}\n";
    const char *unrecognised[4] = {
        foreignSchema, fixture.records[1], fixture.records[2], fixture.records[3]
    };
    size_t foreignSizes[4] = {
        sizeof(foreignSchema) - 1U, fixture.lengths[1],
        fixture.lengths[2], fixture.lengths[3]
    };
    UmiAiTrainingReviewedExampleEvidence foreignEvidence[4];
    memcpy(foreignEvidence, fixture.evidence, sizeof(foreignEvidence));
    foreignEvidence[0].jsonBytes = sizeof(foreignSchema) - 1U;
    CHECK(UmiSha256Buffer(foreignSchema, sizeof(foreignSchema) - 1U,
          foreignEvidence[0].jsonSha256) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingSupervisedDatasetCreate(unrecognised, foreignSizes,
          foreignEvidence, 4U, 8U, &refused) == UMI_STATUS_PARSE_ERROR);
    CHECK(refused == NULL);

    /* A verified record may be valid JSONL yet too long for this decoder.
     * It must be refused, never truncated into a silently different answer. */
    CHECK(UmiAiTrainingSupervisedDatasetCreate(fixture.records, fixture.lengths,
          fixture.evidence, 4U, 2U, &refused) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(refused == NULL);
    size_t sentinel[4] = {123U, 456U, 789U, 101U}, count = 777U;
    CHECK(UmiAiTrainingSupervisedDatasetOrder(dataset, UMI_AI_TRAINING_PARTITION_TRAIN,
          3U, 2U, sentinel, 1U, &count) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(sentinel[0] == 123U && sentinel[1] == 456U && count == 777U);
    CHECK(UmiAiTrainingSupervisedDatasetOrder(dataset, (UmiAiTrainingPartition)99,
          3U, 2U, sentinel, 4U, &count) == UMI_STATUS_INVALID_ARGUMENT);
    UmiAiTrainingSupervisedExample output;
    memset(&output, 0x5a, sizeof(output));
    UmiAiTrainingSupervisedExample previous = output;
    CHECK(UmiAiTrainingSupervisedDatasetGet(dataset, 99U, &output) == UMI_STATUS_NOT_FOUND);
    CHECK(memcmp(&output, &previous, sizeof(output)) == 0);
    UmiAiTrainingSupervisedDatasetDestroy(dataset);
    puts("Tampered source, oversized window, invalid split and atomic rejection passed");
    return 0;
}

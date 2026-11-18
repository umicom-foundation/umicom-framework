/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/training_data/main.c
 *
 * PURPOSE:
 *   Demonstrate bounded training-record validation and visible partial progress without model execution.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/ai/training/jsonl_pairs.h"
#include "umicom/ai/training/jsonl_summary.h"
#include "umicom/ai/training/jsonl_partition.h"
#include "umicom/ai/training/jsonl_selection.h"
#include "umicom/ai/training/jsonl_overlap.h"
#include "umicom/ai/training/jsonl_review.h"
#include "umicom/ai/training/prediction_metrics.h"
#include <stdio.h>
#include <string.h>

typedef struct Summary {
    size_t records;
    size_t completionBytes;
} Summary;

static UmiStatus InspectPair(void *context, const UmiAiTrainingPair *pair, size_t line)
{
    Summary *summary = context;
    if (line != summary->records + 1U) return UMI_STATUS_INVALID_STATE;
    /* This example counts decoded bytes. A model tokenizer may produce a very
     * different token count, so these numbers cannot size a model context. */
    ++summary->records;
    summary->completionBytes += strlen(pair->completion);
    return UMI_STATUS_OK;
}

int main(void)
{
    /* Small original examples are embedded so this exercise cannot silently
     * discover local documents or send their contents to a provider. */
    const char examples[] =
        "{\"source_id\":\"hardware-note\",\"prompt\":\"What stores working values?\","
        "\"completion\":\"RAM holds working values while a program runs.\"}\n"
        "{\"source_id\":\"compiler-note\",\"prompt\":\"What does a compiler do?\","
        "\"completion\":\"It translates source code into another executable form.\"}\n";
    Summary summary = {0};
    UmiAiTrainingJsonlReport report;
    UmiStatus status = UmiAiTrainingJsonlVisit(examples, sizeof(examples)-1U,
        NULL, InspectPair, &summary, &report);
    if (status != UMI_STATUS_OK || summary.records != 2U ||
        report.acceptedRows != 2U || report.consumedBytes != sizeof(examples)-1U)
        return 1;
    printf("Accepted %zu records; %zu decoded completion bytes.\n",
        summary.records, summary.completionBytes);

    /* Keep a separate accumulator for the failing input. A scan reports
     * completed callbacks; it cannot roll back arbitrary caller side effects. */
    const char invalid[] =
        "{\"source_id\":\"a\",\"prompt\":\"p\",\"completion\":\"c\"}\n"
        "{\"source_id\":\"b\",\"prompt\":\"p\",\"completion\":42}";
    Summary partial = {0};
    status = UmiAiTrainingJsonlVisit(invalid, sizeof(invalid)-1U,
        NULL, InspectPair, &partial, &report);
    if (status != UMI_STATUS_PARSE_ERROR || partial.records != 1U ||
        report.acceptedRows != 1U || report.failedLine != 2U)
        return 2;
    puts("Rejected line 2; the earlier accepted row is reported explicitly.");

    /* A caller may construct a reviewed example directly. Emission escapes
     * controls and quotes through Framework; it publishes no partial record. */
    UmiAiTrainingPair prepared = {0};
    strcpy(prepared.sourceId, "original-note");
    strcpy(prepared.prompt, "Name two kinds of memory.");
    strcpy(prepared.completion, "Working memory: RAM.\nPersistent storage: SSD.");
    char serialized[512];
    size_t serializedBytes = 0U;
    if (UmiAiTrainingPairWrite(&prepared, serialized, sizeof(serialized), &serializedBytes) != UMI_STATUS_OK)
        return 3;
    UmiAiTrainingPair decoded;
    if (UmiAiTrainingPairRead(serialized, serializedBytes, NULL, &decoded) != UMI_STATUS_OK ||
        strcmp(prepared.completion, decoded.completion) != 0)
        return 4;
    puts("A prepared record round-tripped without changing its decoded answer.");


    /* Dataset statistics publish only after every record is valid. Unlike the
     * callback exercise above, callers cannot accidentally use a partial sum. */
    UmiAiTrainingJsonlSummary dataset = {0};
    if (UmiAiTrainingJsonlSummarize(examples, sizeof(examples)-1U,
            NULL, &dataset, &report) != UMI_STATUS_OK ||
        dataset.rows != 2U || dataset.sourceGroups != 2U ||
        dataset.largestSourceGroupRows != 1U ||
        dataset.completionBytes != summary.completionBytes)
        return 5;
    printf("Dataset: %zu rows; %zu source groups; largest group %zu rows.\n",
        dataset.rows, dataset.sourceGroups, dataset.largestSourceGroupRows);
    UmiAiTrainingJsonlSummary saved = dataset;
    if (UmiAiTrainingJsonlSummarize(invalid, sizeof(invalid)-1U,
            NULL, &dataset, &report) != UMI_STATUS_PARSE_ERROR ||
        memcmp(&saved, &dataset, sizeof(dataset)) != 0 || report.failedLine != 2U)
        return 6;
    puts("Rejected data left the previous complete summary unchanged.");

    /* Two rows from one note must stay together. The seed selects groups;
     * a one-group holdout can therefore contain one or two rows here. */
    const char related[] =
        "{\"source_id\":\"note-a\",\"prompt\":\"a1\",\"completion\":\"one\"}\n"
        "{\"source_id\":\"note-b\",\"prompt\":\"b1\",\"completion\":\"two\"}\n"
        "{\"source_id\":\"note-a\",\"prompt\":\"a2\",\"completion\":\"three\"}\n"
        "{\"source_id\":\"note-c\",\"prompt\":\"c1\",\"completion\":\"four\"}\n";
    unsigned char evaluation[4] = {0};
    UmiAiTrainingJsonlPartitionSummary partition;
    if (UmiAiTrainingJsonlPartition(related, sizeof(related)-1U, 1U, 42U,
            NULL, evaluation, sizeof(evaluation), &partition, &report) != UMI_STATUS_OK ||
        partition.rows != 4U || partition.sourceGroups != 3U ||
        evaluation[0] != evaluation[2] ||
        evaluation[0] + evaluation[1] + evaluation[3] != 1U)
        return 7;
    printf("Source partition: %zu training rows; %zu evaluation rows; related rows stayed together.\n",
        partition.trainingRows, partition.evaluationRows);

    /* Membership is tied to this exact row order. Both outputs are private
     * memory buffers; application file publication is a separate operation. */
    char training[512], holdout[512];
    UmiAiTrainingJsonlSelectionSummary trainingSelection, holdoutSelection;
    if (UmiAiTrainingJsonlSelectionWrite(related, sizeof(related)-1U,
            evaluation, partition.rows, 0U, NULL, training, sizeof(training),
            &trainingSelection, &report) != UMI_STATUS_OK ||
        UmiAiTrainingJsonlSelectionWrite(related, sizeof(related)-1U,
            evaluation, partition.rows, 1U, NULL, holdout, sizeof(holdout),
            &holdoutSelection, &report) != UMI_STATUS_OK)
        return 8;
    Summary trainingCheck = {0}, holdoutCheck = {0};
    if (UmiAiTrainingJsonlVisit(training, trainingSelection.outputBytes, NULL,
            InspectPair, &trainingCheck, &report) != UMI_STATUS_OK ||
        UmiAiTrainingJsonlVisit(holdout, holdoutSelection.outputBytes, NULL,
            InspectPair, &holdoutCheck, &report) != UMI_STATUS_OK ||
        trainingCheck.records != partition.trainingRows ||
        holdoutCheck.records != partition.evaluationRows ||
        trainingCheck.records + holdoutCheck.records != partition.rows)
        return 9;
    printf("Selected JSONL: %zu training bytes; %zu evaluation bytes; all four rows retained.\n",
        trainingSelection.outputBytes, holdoutSelection.outputBytes);

    /* These invented logits test the metric contract, not model quality.
     * Keep numerator and denominator together; no selected rows has no ratio. */
    const float logits[] = {4.0f, 1.0f, 0.0f, 2.0f, 3.0f, 1.0f};
    const size_t targets[] = {0U, 0U};
    const uint8_t selected[] = {1U, 1U};
    UmiAiTrainingPredictionCounts counts;
    if (UmiAiTrainingPredictionScore(logits, 2U, 3U, targets, selected,
            2U, 2U, &counts) != UMI_STATUS_OK ||
        counts.selectedRows != 2U || counts.topOneCorrect != 1U ||
        counts.topKCorrect != 2U)
        return 10;
    printf("Invented predictions: top-one %zu/%zu; top-%zu %zu/%zu.\n",
        counts.topOneCorrect, counts.selectedRows, counts.topK,
        counts.topKCorrect, counts.selectedRows);
    const uint8_t excluded[] = {0U, 0U};
    if (UmiAiTrainingPredictionScore(logits, 2U, 3U, targets, excluded,
            2U, 2U, &counts) != UMI_STATUS_OK || counts.selectedRows != 0U)
        return 11;
    puts("No selected predictions: accuracy unavailable; no division performed.");

    /* Source grouping and content comparison answer different questions.
     * This private in-memory audit uses the exact reviewed membership above. */
    UmiAiTrainingJsonlOverlapSummary overlap;
    if (UmiAiTrainingJsonlAuditOverlap(related, sizeof(related)-1U,
            evaluation, partition.rows, 4096U, NULL, &overlap, &report) != UMI_STATUS_OK ||
        overlap.sharedPairs != 0U || overlap.distinctPairs != 4U)
        return 12;
    puts("Related-source split: no exact prompt/completion pair crosses its sides.");
    const char repeated[] =
        "{\"source_id\":\"original\",\"prompt\":\"What stores working values?\",\"completion\":\"RAM.\"}\n"
        "{\"source_id\":\"copied-note\",\"prompt\":\"What stores working values?\",\"completion\":\"RAM.\"}\n";
    const unsigned char reviewed[] = {0U, 1U};
    if (UmiAiTrainingJsonlAuditOverlap(repeated, sizeof(repeated)-1U,
            reviewed, 2U, 4096U, NULL, &overlap, &report) != UMI_STATUS_OK ||
        overlap.sharedPairs != 1U || overlap.trainingRowsInSharedPairs != 1U ||
        overlap.evaluationRowsInSharedPairs != 1U ||
        overlap.firstTrainingLine != 1U || overlap.firstEvaluationLine != 2U)
        return 13;
    printf("Repeated pair: training line %zu and evaluation line %zu; %zu staging bytes.\n",
        overlap.firstTrainingLine, overlap.firstEvaluationLine, overlap.stagingBytes);

    /* Composed review publishes membership only after the content audit also
     * succeeds. The source remains the same borrowed in-memory snapshot. */
    UmiAiTrainingJsonlReviewOptions reviewOptions={1U,42U,4096U};
    UmiAiTrainingJsonlReviewSummary combined,previousCombined;
    unsigned char combinedMembership[2]={0x5a,0x5a};
    if(UmiAiTrainingJsonlReview(repeated,sizeof(repeated)-1U,&reviewOptions,NULL,
        combinedMembership,sizeof(combinedMembership),&combined,&report)!=UMI_STATUS_OK ||
        combined.partition.rows!=2U || combined.partition.sourceGroups!=2U ||
        combined.overlap.sharedPairs!=1U || combinedMembership[0]==combinedMembership[1])
        return 14;
    previousCombined=combined;
    unsigned char previousMembership[2];memcpy(previousMembership,combinedMembership,sizeof(previousMembership));
    reviewOptions.maxStagingBytes=1U;
    if(UmiAiTrainingJsonlReview(repeated,sizeof(repeated)-1U,&reviewOptions,NULL,
        combinedMembership,sizeof(combinedMembership),&combined,&report)!=UMI_STATUS_CAPACITY_EXCEEDED ||
        memcmp(&combined,&previousCombined,sizeof(combined))!=0 ||
        memcmp(combinedMembership,previousMembership,sizeof(combinedMembership))!=0)
        return 15;
    puts("Composed review: one shared pair observed; later budget refusal preserved prior outputs.");

    puts("No tokenizer, model training, network or file changes were performed.");
    return 0;
}

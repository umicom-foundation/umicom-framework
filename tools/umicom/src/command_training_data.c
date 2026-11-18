/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tools/umicom/src/command_training_data.c
 *
 * PURPOSE:
 *   Present bounded local dataset statistics without exposing corpus content or starting training.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "command_training_data.h"
#include "umicom/ai/training/jsonl_file.h"
#include "umicom/base/ascii_number.h"
#include "umicom/security/secrets.h"
#include <stdio.h>
#include <string.h>

static void TrainingHelp(void)
{
    puts("umicom training-data inspect ABSOLUTE_JSONL_FILE\n"
         "umicom training-data context ABSOLUTE_JSONL_FILE CONTEXT_ROWS\n"
         "umicom training-data review ABSOLUTE_JSONL_FILE EVALUATION_GROUPS SEED STAGING_BYTES\n"
         "Review groups: 1..4095; seed: unsigned 64-bit decimal; staging bytes: positive decimal.\n"
         "Review staging limits overlap storage only, not total process memory.\n"
         "Context rows: 1..32 for the native byte teaching decoder; no truncation.\n"
         "Inspect strict source_id/prompt/completion records in one local file.\n"
         "Limits: 16 MiB, 4096 rows; source ID 128 decoded bytes,\n"
         "prompt/completion 4096 decoded bytes each; JSON record 32768 bytes.\n"
         "Shows aggregate UTF-8 byte counts, not tokens. No model or training starts.\n"
         "Exit: 0 accepted, 1 input refused, 2 command usage error.");
}
/* A native command is a thin client: Framework owns file admission, exact
 * number parsing and context statistics. No raw path or record text is echoed. */
static int InspectContext(const char *path,const char *contextText)
{
    uint64_t rows=0U;
    if(umi_ascii_uint64_parse(contextText,strlen(contextText),&rows)!=UMI_STATUS_OK ||
        rows==0U || rows>UMI_AI_DECODER_MAX_CONTEXT) {
        fputs("Context rows must be a decimal integer from 1 through 32.\n",stderr);
        return 2;
    }
    UmiAiTrainingJsonlContextSummary summary;
    UmiAiTrainingJsonlReport report;
    UmiStatus status=UmiAiTrainingJsonlInspectContextFile(path,(size_t)rows,NULL,&summary,&report);
    if(status!=UMI_STATUS_OK) {
        fprintf(stderr,"Training context refused: status %d; stage %d; accepted rows %zu; failed line %zu.\n",
            (int)status,(int)report.stage,report.acceptedRows,report.failedLine);
        return 1;
    }
    printf("Native byte context: %zu | Records: %zu | Fitting: %zu | Overlong: %zu | First overlong line: %zu\n",
        summary.contextLength,summary.records,summary.fittingRecords,summary.overlongRecords,summary.firstOverlongLine);
    printf("Required rows (all): %zu | Shortest: %zu | Longest: %zu\n"
           "Fitting valid rows: %zu | Fitting selected rows: %zu | Fitting padding rows: %zu\n",
        summary.requiredRows,summary.shortestRequiredRows,summary.longestRequiredRows,
        summary.fittingValidRows,summary.fittingSelectedRows,summary.fittingPaddingRows);
    puts("Native byte-token format only. No text was truncated and no model or training started.");
    return ferror(stdout)?1:0;
}

/* The command exposes counts only. Framework owns source grouping, exact pair
 * comparison and transactional publication; a shared pair is an observation,
 * not an instruction to silently delete data or approve a training run. */
static int ReviewDataset(const char *path,const char *groupsText,const char *seedText,const char *budgetText)
{
    uint64_t groups=0U,seed=0U,budget=0U;
    if(umi_ascii_uint64_parse(groupsText,strlen(groupsText),&groups)!=UMI_STATUS_OK ||
        umi_ascii_uint64_parse(seedText,strlen(seedText),&seed)!=UMI_STATUS_OK ||
        umi_ascii_uint64_parse(budgetText,strlen(budgetText),&budget)!=UMI_STATUS_OK ||
        groups==0U || groups>=UMI_AI_TRAINING_JSONL_ROWS || budget==0U || budget>SIZE_MAX) {
        fputs("Review requires decimal groups 1..4095, an unsigned 64-bit seed and a positive staging byte budget fitting this platform.\n",stderr);
        return 2;
    }
    const UmiAiTrainingJsonlReviewOptions options={(size_t)groups,seed,(size_t)budget};
    unsigned char membership[UMI_AI_TRAINING_JSONL_ROWS];
    UmiAiTrainingJsonlReviewSummary summary;
    UmiAiTrainingJsonlReport report;
    const UmiStatus status=UmiAiTrainingJsonlReviewFile(path,&options,NULL,
        membership,sizeof(membership),&summary,&report);
    umi_secret_clear(membership,sizeof(membership));
    if(status!=UMI_STATUS_OK) {
        fprintf(stderr,"Dataset review refused: status %d; stage %d; accepted rows %zu; failed line %zu.\n",
            (int)status,(int)report.stage,report.acceptedRows,report.failedLine);
        return 1;
    }
    printf("Rows: %zu | Source groups: %zu | Training rows: %zu | Evaluation rows: %zu\n",
        summary.partition.rows,summary.partition.sourceGroups,summary.partition.trainingRows,summary.partition.evaluationRows);
    printf("Distinct pairs: %zu | Duplicate rows: %zu | Shared pairs: %zu\n"
           "Shared-pair training rows: %zu | Evaluation rows: %zu | First witness lines: %zu, %zu\n"
           "Overlap staging bytes: %zu\n",
        summary.overlap.distinctPairs,summary.overlap.duplicateRows,summary.overlap.sharedPairs,
        summary.overlap.trainingRowsInSharedPairs,summary.overlap.evaluationRowsInSharedPairs,
        summary.overlap.firstTrainingLine,summary.overlap.firstEvaluationLine,summary.overlap.stagingBytes);
    puts("Review completed, not dataset approval. Exact pairs only; no files, model or training were created.");
    return ferror(stdout)?1:0;
}

int umi_cli_command_training_data(int argc, char **argv)
{
    if (argc == 0 || (argc == 1 && strcmp(argv[0], "--help") == 0)) {
        TrainingHelp();
        return 0;
    }
    if (argc == 5 && strcmp(argv[0], "review") == 0)
        return ReviewDataset(argv[1],argv[2],argv[3],argv[4]);
    if (argc == 3 && strcmp(argv[0], "context") == 0)
        return InspectContext(argv[1],argv[2]);
    if (argc != 2 || strcmp(argv[0], "inspect") != 0) {
        /* Keep the existing inspect usage and expose the additive context route. */
        fputs("Use: umicom training-data inspect ABSOLUTE_JSONL_FILE\n"
              "  or: umicom training-data context ABSOLUTE_JSONL_FILE CONTEXT_ROWS\n"
              "  or: umicom training-data review ABSOLUTE_JSONL_FILE EVALUATION_GROUPS SEED STAGING_BYTES\n", stderr);
        return 2;
    }
    UmiAiTrainingJsonlSummary summary;
    UmiAiTrainingJsonlReport report;
    UmiStatus status = UmiAiTrainingJsonlInspectFile(argv[1], NULL, &summary, &report);
    if (status != UMI_STATUS_OK) {
        /* Never echo untrusted corpus text or path bytes into the terminal.
         * Progress is diagnostic only; no partial summary is presented. */
        fprintf(stderr, "Training data refused: status %d; stage %d; accepted rows %zu; failed line %zu.\n",
            (int)status, (int)report.stage, report.acceptedRows, report.failedLine);
        return 1;
    }
    puts("Umicom training-data inspection: strict teaching schema accepted.");
    printf("Rows: %zu | Source groups: %zu | Largest group rows: %zu | Empty prompts: %zu\n",
        summary.rows, summary.sourceGroups, summary.largestSourceGroupRows, summary.emptyPrompts);
    printf("Prompt bytes: %zu | Completion bytes: %zu\n"
           "Longest prompt bytes: %zu | Longest completion bytes: %zu\n",
        summary.promptBytes, summary.completionBytes,
        summary.longestPromptBytes, summary.longestCompletionBytes);
    puts("Counts are decoded UTF-8 bytes, not tokens. No model or training started.");
    return 0;
}

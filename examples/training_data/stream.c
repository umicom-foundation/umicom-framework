/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/training_data/stream.c
 *
 * PURPOSE:
 *   Inspect one explicitly selected local JSONL file with bounded input retention and publish aggregate counts only after success.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/ai/training/jsonl_stream.h"
#include "umicom/platform/native_arguments.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
typedef struct Counts { size_t rows,promptBytes,completionBytes; } Counts;
static UmiStatus CountPair(void *opaque,const UmiAiTrainingPair *pair,size_t line)
{
    Counts *counts=opaque;
    const size_t prompt=strlen(pair->prompt),completion=strlen(pair->completion);
    if(line!=counts->rows+1U || prompt>SIZE_MAX-counts->promptBytes ||
        completion>SIZE_MAX-counts->completionBytes) return UMI_STATUS_CAPACITY_EXCEEDED;
    ++counts->rows;
    counts->promptBytes+=prompt;
    counts->completionBytes+=completion;
    return UMI_STATUS_OK;
}
static int Run(int argc,char **argv)
{
    if(argc!=2) {
        fputs("Usage: umicom-sdk-training-stream ABSOLUTE_JSONL_FILE\n",stderr);
        return 2;
    }
    Counts candidate={0};
    UmiAiTrainingJsonlReport report;
    UmiStatus status=UmiAiTrainingJsonlVisitFile(argv[1],NULL,CountPair,&candidate,&report);
    if(status!=UMI_STATUS_OK) {
        /* Callback progress is diagnostic only. Keep provisional aggregate
         * statistics private so a rejected tail cannot look like a small corpus. */
        fprintf(stderr,"Inspection refused: status %d; accepted rows %zu; failed line %zu\n",
            (int)status,report.acceptedRows,report.failedLine);
        return 1;
    }
    printf("rows: %zu\nprompt bytes: %zu\ncompletion bytes: %zu\n",
        candidate.rows,candidate.promptBytes,candidate.completionBytes);
    return 0;
}
int main(int argc,char **argv)
{
    int result=2;
    UmiStatus status=UmiNativeArgumentsDispatch(argc,argv,Run,&result);
    return status==UMI_STATUS_OK?result:2;
}

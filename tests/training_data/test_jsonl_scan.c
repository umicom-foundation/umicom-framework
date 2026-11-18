/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/training_data/test_jsonl_scan.c
 *
 * PURPOSE:
 *   Verify JSONL progress, callback lifetimes, cancellation and input bounds independently of model execution.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/ai/training/jsonl_pairs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%d: %s\n", __LINE__, #x); return 1; } } while (0)
#define ROW "{\"source_id\":\"a\",\"prompt\":\"p\",\"completion\":\"c\"}"
typedef struct Context { size_t calls, reject; UmiCancellationToken *cancel; int wrong; } Context;
static UmiStatus Visit(void *opaque, const UmiAiTrainingPair *pair, size_t line)
{
    Context *context = opaque;
    ++context->calls;
    if (line != context->calls || strcmp(pair->sourceId,"a") != 0) context->wrong = 1;
    if (context->cancel != NULL) umi_cancellation_token_request(context->cancel);
    return context->calls == context->reject ? UMI_STATUS_IO_ERROR : UMI_STATUS_OK;
}
int main(int argc, char **argv)
{
    CHECK(argc==2); const char *mode=argv[1]; Context context={0};
    UmiAiTrainingJsonlReport report;
    if (strcmp(mode,"lf")==0 || strcmp(mode,"crlf")==0 || strcmp(mode,"last-line")==0) {
        const char *input=strcmp(mode,"lf")==0 ? ROW "\n" ROW "\n" :
            strcmp(mode,"crlf")==0 ? ROW "\r\n" ROW "\r\n" : ROW "\n" ROW;
        CHECK(UmiAiTrainingJsonlVisit(input,strlen(input),NULL,Visit,&context,&report)==UMI_STATUS_OK);
        CHECK(context.calls==2U&&!context.wrong&&report.acceptedRows==2U);
        CHECK(report.consumedBytes==strlen(input)&&report.failedLine==0U);
        CHECK(report.stage==UMI_AI_TRAINING_JSONL_COMPLETE);
    } else if (strcmp(mode,"blank")==0 || strcmp(mode,"bad-second")==0 || strcmp(mode,"callback")==0) {
        const char *input=strcmp(mode,"blank")==0 ? ROW "\n\n" : strcmp(mode,"bad-second")==0 ? ROW "\n{}" : ROW "\n" ROW;
        if (strcmp(mode,"callback")==0) context.reject=2U;
        UmiStatus expected=context.reject ? UMI_STATUS_IO_ERROR : UMI_STATUS_PARSE_ERROR;
        CHECK(UmiAiTrainingJsonlVisit(input,strlen(input),NULL,Visit,&context,&report)==expected);
        CHECK(report.acceptedRows==1U&&report.failedLine==2U&&report.consumedBytes==strlen(ROW)+1U);
        CHECK(context.calls==(context.reject?2U:1U)&&!context.wrong);
        CHECK(report.stage==(context.reject?UMI_AI_TRAINING_JSONL_VISITOR:UMI_AI_TRAINING_JSONL_RECORD));
    } else if (strcmp(mode,"cancel-between")==0 || strcmp(mode,"cancel-before")==0) {
        UmiCancellationToken *token=NULL; CHECK(umi_cancellation_token_create(&token)==UMI_STATUS_OK);
        context.cancel=token;
        if(strcmp(mode,"cancel-before")==0)umi_cancellation_token_request(token);
        CHECK(UmiAiTrainingJsonlVisit(ROW "\n" ROW,sizeof(ROW "\n" ROW)-1U,token,Visit,&context,&report)==UMI_STATUS_CANCELLED);
        size_t accepted=strcmp(mode,"cancel-before")==0?0U:1U;
        CHECK(context.calls==accepted&&report.acceptedRows==accepted&&report.failedLine==accepted+1U);
        CHECK(report.stage==UMI_AI_TRAINING_JSONL_RECORD); umi_cancellation_token_destroy(token);
    } else if (strcmp(mode,"rows")==0) {
        const size_t stride=strlen(ROW "\n"), count=UMI_AI_TRAINING_JSONL_ROWS+1U;
        char *input=malloc(stride*count); CHECK(input!=NULL);
        for(size_t i=0;i<count;++i)memcpy(input+i*stride,ROW "\n",stride);
        CHECK(UmiAiTrainingJsonlVisit(input,stride*UMI_AI_TRAINING_JSONL_ROWS,NULL,Visit,&context,&report)==UMI_STATUS_OK);
        CHECK(report.acceptedRows==UMI_AI_TRAINING_JSONL_ROWS);
        context=(Context){0};
        CHECK(UmiAiTrainingJsonlVisit(input,stride*count,NULL,Visit,&context,&report)==UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(report.acceptedRows==UMI_AI_TRAINING_JSONL_ROWS&&report.failedLine==count);
        CHECK(report.consumedBytes==stride*UMI_AI_TRAINING_JSONL_ROWS); free(input);
    } else if (strcmp(mode,"line-limit")==0) {
        char *input=malloc(UMI_AI_TRAINING_PAIR_LINE_BYTES+1U); CHECK(input!=NULL);
        memset(input,' ',UMI_AI_TRAINING_PAIR_LINE_BYTES+1U);memcpy(input,ROW,strlen(ROW));
        CHECK(UmiAiTrainingJsonlVisit(input,UMI_AI_TRAINING_PAIR_LINE_BYTES,NULL,Visit,&context,&report)==UMI_STATUS_OK);
        context=(Context){0};
        CHECK(UmiAiTrainingJsonlVisit(input,UMI_AI_TRAINING_PAIR_LINE_BYTES+1U,NULL,Visit,&context,&report)==UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(report.acceptedRows==0U&&report.consumedBytes==0U&&context.calls==0U);free(input);
    } else if (strcmp(mode,"line-limit-crlf")==0 || strcmp(mode,"line-limit-final-cr")==0 || strcmp(mode,"line-limit-double-cr")==0) {
        size_t bytes = UMI_AI_TRAINING_PAIR_LINE_BYTES;
        char *input = malloc(bytes + 2U); CHECK(input != NULL);
        memset(input, ' ', bytes); memcpy(input, ROW, strlen(ROW));
        input[bytes] = '\r';
        input[bytes + 1U] = strcmp(mode,"line-limit-double-cr")==0 ? '\r' : '\n';
        size_t length = bytes + (strcmp(mode,"line-limit-final-cr")==0 ? 1U : 2U);
        UmiStatus expected = strcmp(mode,"line-limit-double-cr")==0 ? UMI_STATUS_CAPACITY_EXCEEDED : UMI_STATUS_OK;
        CHECK(UmiAiTrainingJsonlVisit(input,length,NULL,Visit,&context,&report)==expected);
        if (expected == UMI_STATUS_OK) {
            CHECK(context.calls==1U&&report.acceptedRows==1U&&report.consumedBytes==length);
        } else { CHECK(context.calls==0U&&report.acceptedRows==0U&&report.consumedBytes==0U); }
        free(input);
    } else if (strcmp(mode,"multiline")==0) {
        const char *input="{\n\"source_id\":\"a\",\"prompt\":\"p\",\"completion\":\"c\"}";
        CHECK(UmiAiTrainingJsonlVisit(input,strlen(input),NULL,Visit,&context,&report)==UMI_STATUS_PARSE_ERROR);
        CHECK(context.calls==0U&&report.failedLine==1U&&report.consumedBytes==0U);
    } else if (strcmp(mode,"arguments")==0) {
        CHECK(UmiAiTrainingJsonlVisit(NULL,1U,NULL,Visit,&context,&report)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(report.stage==UMI_AI_TRAINING_JSONL_INPUT&&report.acceptedRows==0U);
        CHECK(UmiAiTrainingJsonlVisit(ROW,0U,NULL,Visit,&context,&report)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiAiTrainingJsonlVisit(ROW,strlen(ROW),NULL,NULL,&context,&report)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiAiTrainingJsonlVisit(ROW,strlen(ROW),NULL,Visit,&context,NULL)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiAiTrainingJsonlVisit(ROW,UMI_AI_TRAINING_JSONL_BYTES+1U,NULL,Visit,&context,&report)==UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(context.calls==0U&&report.consumedBytes==0U&&report.failedLine==0U);
    } else return 2;
    return 0;
}

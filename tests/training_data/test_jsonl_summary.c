/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/training_data/test_jsonl_summary.c
 *
 * PURPOSE:
 *   Verify dataset statistics, exact group identity, transactional refusal and bounded source inventory.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/ai/training/jsonl_summary.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%d: %s\n", __LINE__, #x); return 1; } } while (0)
#define ROW "{\"source_id\":\"a\",\"prompt\":\"p\",\"completion\":\"c\"}"
static int Inspect(const char *input, UmiAiTrainingJsonlSummary *summary)
{
    UmiAiTrainingJsonlReport report;
    CHECK(UmiAiTrainingJsonlSummarize(input,strlen(input),NULL,summary,&report)==UMI_STATUS_OK);
    CHECK(summary->rows==report.acceptedRows&&report.consumedBytes==strlen(input));
    CHECK(report.failedLine==0U&&report.stage==UMI_AI_TRAINING_JSONL_COMPLETE);
    return 0;
}
int main(int argc,char **argv)
{
    CHECK(argc==2); const char *mode=argv[1];
    UmiAiTrainingJsonlSummary summary, before;
    memset(&summary,0x5a,sizeof(summary)); memcpy(&before,&summary,sizeof(before));
    UmiAiTrainingJsonlReport report;
    if(strcmp(mode,"counts")==0) {
        const char *input=ROW "\n{\"source_id\":\"b\",\"prompt\":\"\",\"completion\":\"xyz\"}\n"
            "{\"source_id\":\"a\",\"prompt\":\"uv\",\"completion\":\"cd\"}";
        CHECK(Inspect(input,&summary)==0);
        CHECK(summary.rows==3U&&summary.sourceGroups==2U&&summary.largestSourceGroupRows==2U);
        CHECK(summary.emptyPrompts==1U&&summary.promptBytes==3U&&summary.completionBytes==6U);
        CHECK(summary.longestPromptBytes==2U&&summary.longestCompletionBytes==3U);
    } else if(strcmp(mode,"escaped-group")==0) {
        CHECK(Inspect(ROW "\n{\"source_id\":\"\\u0061\",\"prompt\":\"p\",\"completion\":\"c\"}",&summary)==0);
        CHECK(summary.sourceGroups==1U&&summary.largestSourceGroupRows==2U);
    } else if(strcmp(mode,"unicode-bytes")==0) {
        CHECK(Inspect("{\"source_id\":\"one\",\"prompt\":\"\\u00e9\",\"completion\":\"\\ud83d\\ude80\"}",&summary)==0);
        CHECK(summary.promptBytes==2U&&summary.completionBytes==4U);
        CHECK(summary.longestPromptBytes==2U&&summary.longestCompletionBytes==4U);
    } else if(strcmp(mode,"exact-identity")==0) {
        CHECK(Inspect(ROW "\n{\"source_id\":\"A\",\"prompt\":\"p\",\"completion\":\"c\"}\n"
            "{\"source_id\":\"\\u00e9\",\"prompt\":\"p\",\"completion\":\"c\"}\n"
            "{\"source_id\":\"e\\u0301\",\"prompt\":\"p\",\"completion\":\"c\"}",&summary)==0);
        CHECK(summary.sourceGroups==4U&&summary.largestSourceGroupRows==1U);
    } else if(strcmp(mode,"reordered")==0) {
        UmiAiTrainingJsonlSummary other;
        const char *one="{\"source_id\":\"z\",\"prompt\":\"\",\"completion\":\"two\"}\n" ROW "\n" ROW;
        const char *two=ROW "\n{\"source_id\":\"z\",\"prompt\":\"\",\"completion\":\"two\"}\n" ROW;
        CHECK(Inspect(one,&summary)==0&&Inspect(two,&other)==0);
        CHECK(summary.rows==other.rows&&summary.sourceGroups==other.sourceGroups);
        CHECK(summary.largestSourceGroupRows==other.largestSourceGroupRows);
        CHECK(summary.promptBytes==other.promptBytes&&summary.completionBytes==other.completionBytes);
    } else if(strcmp(mode,"partial-refusal")==0) {
        const char *input=ROW "\n{}";
        CHECK(UmiAiTrainingJsonlSummarize(input,strlen(input),NULL,&summary,&report)==UMI_STATUS_PARSE_ERROR);
        CHECK(memcmp(&summary,&before,sizeof(summary))==0);
        CHECK(report.acceptedRows==1U&&report.failedLine==2U);
    } else if(strcmp(mode,"cancel")==0) {
        UmiCancellationToken *token=NULL;
        CHECK(umi_cancellation_token_create(&token)==UMI_STATUS_OK);
        umi_cancellation_token_request(token);
        CHECK(UmiAiTrainingJsonlSummarize(ROW,strlen(ROW),token,&summary,&report)==UMI_STATUS_CANCELLED);
        CHECK(memcmp(&summary,&before,sizeof(summary))==0);
        CHECK(report.acceptedRows==0U&&report.stage==UMI_AI_TRAINING_JSONL_INPUT);
        umi_cancellation_token_destroy(token);
    } else if(strcmp(mode,"rows")==0) {
        size_t stride=strlen(ROW "\n"),rows=UMI_AI_TRAINING_JSONL_ROWS;
        char *input=malloc(stride*(rows+1U)); CHECK(input!=NULL);
        for(size_t i=0U;i<=rows;++i)memcpy(input+i*stride,ROW "\n",stride);
        CHECK(UmiAiTrainingJsonlSummarize(input,stride*rows,NULL,&summary,&report)==UMI_STATUS_OK);
        CHECK(summary.rows==rows&&summary.sourceGroups==1U&&summary.largestSourceGroupRows==rows);
        before=summary;
        CHECK(UmiAiTrainingJsonlSummarize(input,stride*(rows+1U),NULL,&summary,&report)==UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(memcmp(&summary,&before,sizeof(summary))==0&&report.acceptedRows==rows);
        free(input);
    } else if(strcmp(mode,"unique-capacity")==0) {
        size_t capacity=UMI_AI_TRAINING_JSONL_ROWS*96U,used=0U;
        char *input=malloc(capacity);CHECK(input!=NULL);
        for(size_t i=0U;i<UMI_AI_TRAINING_JSONL_ROWS;++i) {
            int n=snprintf(input+used,capacity-used,"{\"source_id\":\"%zu\",\"prompt\":\"p\",\"completion\":\"c\"}\n",i);
            CHECK(n>0&&(size_t)n<capacity-used);used+=(size_t)n;
        }
        CHECK(UmiAiTrainingJsonlSummarize(input,used,NULL,&summary,&report)==UMI_STATUS_OK);
        CHECK(summary.rows==UMI_AI_TRAINING_JSONL_ROWS&&summary.sourceGroups==summary.rows);
        CHECK(summary.largestSourceGroupRows==1U); free(input);
    } else if(strcmp(mode,"long-id")==0) {
        UmiAiTrainingPair pair={.prompt="p",.completion="c"};
        memset(pair.sourceId,'a',UMI_AI_TRAINING_PAIR_SOURCE_BYTES);
        char input[1024];size_t count=0U;
        CHECK(UmiAiTrainingPairWrite(&pair,input,sizeof(input),&count)==UMI_STATUS_OK);
        CHECK(Inspect(input,&summary)==0&&summary.sourceGroups==1U);
    } else if(strcmp(mode,"arguments")==0) {
        CHECK(UmiAiTrainingJsonlSummarize(NULL,1U,NULL,&summary,&report)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(report.stage==UMI_AI_TRAINING_JSONL_INPUT);
        CHECK(UmiAiTrainingJsonlSummarize(ROW,0U,NULL,&summary,&report)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiAiTrainingJsonlSummarize(ROW,strlen(ROW),NULL,NULL,&report)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiAiTrainingJsonlSummarize(ROW,strlen(ROW),NULL,&summary,NULL)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiAiTrainingJsonlSummarize(ROW,(size_t)UMI_AI_TRAINING_JSONL_BYTES+1U,NULL,&summary,&report)==UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(memcmp(&summary,&before,sizeof(summary))==0&&report.acceptedRows==0U);
    } else return 2;
    return 0;
}

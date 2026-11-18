/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/training_data/test_jsonl_partition.c
 *
 * PURPOSE:
 *   Exercise source-group invariants, bounded admission and atomic JSONL partition outputs.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/ai/training/jsonl_partition.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr,"%d: %s\n",__LINE__,#x); return 1; } } while (0)
#define ROW(id) "{\"source_id\":\"" id "\",\"prompt\":\"p\",\"completion\":\"c\"}\n"
static int Partition(const char *text, unsigned char *membership, size_t capacity,
    UmiAiTrainingJsonlPartitionSummary *summary)
{
    UmiAiTrainingJsonlReport report;
    CHECK(UmiAiTrainingJsonlPartition(text,strlen(text),1U,42U,NULL,
        membership,capacity,summary,&report)==UMI_STATUS_OK);
    CHECK(report.acceptedRows==summary->rows&&report.consumedBytes==strlen(text));
    CHECK(report.stage==UMI_AI_TRAINING_JSONL_COMPLETE&&report.failedLine==0U);
    CHECK(summary->trainingRows+summary->evaluationRows==summary->rows);
    return 0;
}
int main(int argc,char **argv)
{
    CHECK(argc==2);const char *mode=argv[1];
    unsigned char membership[UMI_AI_TRAINING_JSONL_ROWS+1U],before[sizeof(membership)];
    memset(membership,0x5a,sizeof(membership));memcpy(before,membership,sizeof(before));
    UmiAiTrainingJsonlPartitionSummary summary,original;
    memset(&summary,0x5a,sizeof(summary));memcpy(&original,&summary,sizeof(original));
    UmiAiTrainingJsonlReport report;
    const char *text=ROW("c") ROW("a") ROW("b") ROW("a") ROW("c");
    if(strcmp(mode,"groups")==0) {
        CHECK(Partition(text,membership,sizeof(membership),&summary)==0);
        CHECK(summary.rows==5U&&summary.sourceGroups==3U);
        CHECK(membership[0]==membership[4]&&membership[1]==membership[3]);
        CHECK(membership[0]+membership[1]+membership[2]==1U);
        CHECK(membership[5]==0x5a);
    } else if(strcmp(mode,"reordered")==0) {
        unsigned char reordered[5];UmiAiTrainingJsonlPartitionSummary other;
        CHECK(Partition(text,membership,sizeof(membership),&summary)==0);
        CHECK(Partition(ROW("a") ROW("c") ROW("c") ROW("b") ROW("a"),reordered,sizeof(reordered),&other)==0);
        const size_t map[]={1U,0U,4U,2U,3U};
        for(size_t i=0;i<5U;++i)CHECK(reordered[i]==membership[map[i]]);
        CHECK(summary.evaluationRows==other.evaluationRows);
    } else if(strcmp(mode,"repeated-rows")==0) {
        unsigned char minimal[3];UmiAiTrainingJsonlPartitionSummary other;
        CHECK(Partition(text,membership,sizeof(membership),&summary)==0);
        CHECK(Partition(ROW("a") ROW("b") ROW("c"),minimal,sizeof(minimal),&other)==0);
        CHECK(minimal[0]==membership[1]&&minimal[1]==membership[2]&&minimal[2]==membership[0]);
        CHECK(other.evaluationRows==1U&&summary.evaluationRows<summary.rows);
    } else if(strcmp(mode,"escaped-id")==0) {
        CHECK(Partition(ROW("a") ROW("\\u0061") ROW("b"),membership,sizeof(membership),&summary)==0);
        CHECK(summary.sourceGroups==2U&&membership[0]==membership[1]&&membership[0]!=membership[2]);
    } else if(strcmp(mode,"exact-id")==0) {
        CHECK(Partition(ROW("a") ROW("A") ROW("\\u00e9") ROW("e\\u0301"),membership,sizeof(membership),&summary)==0);
        CHECK(summary.sourceGroups==4U&&summary.evaluationRows==1U);
    } else if(strcmp(mode,"capacity")==0) {
        CHECK(UmiAiTrainingJsonlPartition(text,strlen(text),1U,42U,NULL,membership,4U,&summary,&report)==UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(report.stage==UMI_AI_TRAINING_JSONL_COMPLETE&&report.acceptedRows==5U);
        CHECK(memcmp(membership,before,sizeof(before))==0&&memcmp(&summary,&original,sizeof(summary))==0);
    } else if(strcmp(mode,"partial-refusal")==0) {
        text=ROW("a") "{}";
        CHECK(UmiAiTrainingJsonlPartition(text,strlen(text),1U,42U,NULL,membership,sizeof(membership),&summary,&report)==UMI_STATUS_PARSE_ERROR);
        CHECK(report.acceptedRows==1U&&report.failedLine==2U);
        CHECK(memcmp(membership,before,sizeof(before))==0&&memcmp(&summary,&original,sizeof(summary))==0);
    } else if(strcmp(mode,"group-count")==0) {
        const char *one=ROW("a") ROW("a");
        CHECK(UmiAiTrainingJsonlPartition(one,strlen(one),1U,42U,NULL,membership,sizeof(membership),&summary,&report)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiAiTrainingJsonlPartition(text,strlen(text),3U,42U,NULL,membership,sizeof(membership),&summary,&report)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(memcmp(membership,before,sizeof(before))==0&&memcmp(&summary,&original,sizeof(summary))==0);
    } else if(strcmp(mode,"cancel")==0) {
        UmiCancellationToken *token=NULL;CHECK(umi_cancellation_token_create(&token)==UMI_STATUS_OK);
        umi_cancellation_token_request(token);
        CHECK(UmiAiTrainingJsonlPartition(text,strlen(text),1U,42U,token,membership,sizeof(membership),&summary,&report)==UMI_STATUS_CANCELLED);
        CHECK(report.acceptedRows==0U&&report.stage==UMI_AI_TRAINING_JSONL_INPUT);
        CHECK(memcmp(membership,before,sizeof(before))==0&&memcmp(&summary,&original,sizeof(summary))==0);
        umi_cancellation_token_destroy(token);
    } else if(strcmp(mode,"row-limit")==0) {
        size_t stride=strlen(ROW("a")),rows=UMI_AI_TRAINING_JSONL_ROWS;
        char *input=malloc(stride*(rows+1U));CHECK(input!=NULL);
        for(size_t i=0U;i<=rows;++i)memcpy(input+i*stride,i%2U?ROW("a"):ROW("b"),stride);
        CHECK(UmiAiTrainingJsonlPartition(input,stride*rows,1U,42U,NULL,membership,sizeof(membership),&summary,&report)==UMI_STATUS_OK);
        CHECK(summary.rows==rows&&summary.evaluationRows==rows/2U&&membership[rows]==0x5a);
        memcpy(before,membership,sizeof(before));original=summary;
        CHECK(UmiAiTrainingJsonlPartition(input,stride*(rows+1U),1U,42U,NULL,membership,sizeof(membership),&summary,&report)==UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(memcmp(membership,before,sizeof(before))==0&&memcmp(&summary,&original,sizeof(summary))==0);free(input);
    } else if(strcmp(mode,"arguments")==0) {
        CHECK(UmiAiTrainingJsonlPartition(NULL,1U,1U,42U,NULL,membership,sizeof(membership),&summary,&report)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiAiTrainingJsonlPartition(text,0U,1U,42U,NULL,membership,sizeof(membership),&summary,&report)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiAiTrainingJsonlPartition(text,strlen(text),0U,42U,NULL,membership,sizeof(membership),&summary,&report)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiAiTrainingJsonlPartition(text,strlen(text),1U,42U,NULL,NULL,0U,&summary,&report)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiAiTrainingJsonlPartition(text,strlen(text),1U,42U,NULL,membership,sizeof(membership),NULL,&report)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiAiTrainingJsonlPartition(text,strlen(text),1U,42U,NULL,membership,sizeof(membership),&summary,NULL)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiAiTrainingJsonlPartition(text,(size_t)UMI_AI_TRAINING_JSONL_BYTES+1U,1U,42U,NULL,membership,sizeof(membership),&summary,&report)==UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(memcmp(membership,before,sizeof(before))==0&&memcmp(&summary,&original,sizeof(summary))==0);
    } else return 2;
    return 0;
}

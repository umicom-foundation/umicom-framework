/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/training_data/test_jsonl_overlap.c
 *
 * PURPOSE:
 *   Verify exact pair identity, partition witnesses, memory budgets and transactional audit refusal.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/ai/training/jsonl_overlap.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%d: %s\n", __LINE__, #x); return 1; } } while (0)
#define ROW(id,p,c) "{\"source_id\":\"" id "\",\"prompt\":\"" p "\",\"completion\":\"" c "\"}"
#define A ROW("a","prompt","answer")
static int Audit(const char *input, const unsigned char *membership, size_t count,
    UmiAiTrainingJsonlOverlapSummary *summary)
{
    UmiAiTrainingJsonlReport report;
    CHECK(UmiAiTrainingJsonlAuditOverlap(input,strlen(input),membership,count,SIZE_MAX,NULL,summary,&report)==UMI_STATUS_OK);
    CHECK(summary->rows==count && report.acceptedRows==count && report.failedLine==0U);
    CHECK(report.consumedBytes==strlen(input) && report.stage==UMI_AI_TRAINING_JSONL_COMPLETE);
    CHECK(summary->trainingRows+summary->evaluationRows==count);
    CHECK(summary->duplicateRows+summary->distinctPairs==count);
    return 0;
}
int main(int argc, char **argv)
{
    CHECK(argc==2);
    const char *mode=argv[1];
    UmiAiTrainingJsonlOverlapSummary summary,before;
    memset(&summary,0x5a,sizeof(summary));memcpy(&before,&summary,sizeof(before));
    UmiAiTrainingJsonlReport report;
    const unsigned char sides[3]={0U,1U,0U};
    if(!strcmp(mode,"distinct")) {
        CHECK(Audit(A "\n" ROW("a","prompt","different"),sides,2,&summary)==0);
        CHECK(summary.distinctPairs==2 && summary.duplicateRows==0 && summary.sharedPairs==0);
        CHECK(summary.firstTrainingLine==0 && summary.firstEvaluationLine==0);
    } else if(!strcmp(mode,"shared")) {
        const char *input=ROW("z","Z","z") "\n" ROW("a","A","a") "\n" ROW("b","B","b") "\n"
            ROW("a2","A","a") "\n" ROW("z2","Z","z") "\n" ROW("a3","A","a");
        const unsigned char membership[]={0,1,0,0,1,1};
        CHECK(Audit(input,membership,6,&summary)==0);
        CHECK(summary.distinctPairs==3 && summary.duplicateRows==3 && summary.sharedPairs==2);
        CHECK(summary.trainingRows==3 && summary.evaluationRows==3);
        CHECK(summary.trainingRowsInSharedPairs==2 && summary.evaluationRowsInSharedPairs==3);
        CHECK(summary.firstTrainingLine==1 && summary.firstEvaluationLine==5);
    } else if(!strcmp(mode,"same-side")) {
        const unsigned char membership[]={1,1,1};
        CHECK(Audit(A "\n" A "\n" A,membership,3,&summary)==0);
        CHECK(summary.distinctPairs==1 && summary.duplicateRows==2 && summary.sharedPairs==0);
        CHECK(summary.trainingRows==0 && summary.evaluationRows==3);
        CHECK(summary.trainingRowsInSharedPairs==0 && summary.evaluationRowsInSharedPairs==0);
    } else if(!strcmp(mode,"escaped")) {
        CHECK(Audit(ROW("left","\\u0070","\\n") "\r\n" ROW("right","p","\\u000a"),sides,2,&summary)==0);
        CHECK(summary.sharedPairs==1 && summary.firstTrainingLine==1 && summary.firstEvaluationLine==2);
        CHECK(Audit(ROW("a","\\u00e9","c") "\n" ROW("b","\xc3\xa9","c"),sides,2,&summary)==0);
        CHECK(summary.sharedPairs==1);
    } else if(!strcmp(mode,"boundaries")) {
        CHECK(Audit(ROW("a","ab","c") "\n" ROW("b","a","bc"),sides,2,&summary)==0);
        CHECK(summary.distinctPairs==2 && summary.sharedPairs==0);
        CHECK(Audit(ROW("a","","abc") "\n" ROW("b","a","bc"),sides,2,&summary)==0);
        CHECK(summary.distinctPairs==2);
    } else if(!strcmp(mode,"normalization")) {
        CHECK(Audit(ROW("a","\\u00e9","c") "\n" ROW("a","e\\u0301","c"),sides,2,&summary)==0);
        CHECK(summary.sharedPairs==0);
        CHECK(Audit(ROW("a","P","c") "\n" ROW("a","p","c") "\n" ROW("a","p ","c"),sides,3,&summary)==0);
        CHECK(summary.distinctPairs==3);
    } else if(!strcmp(mode,"budget")) {
        CHECK(Audit(A "\n" A,sides,2,&summary)==0);
        const size_t exact=summary.stagingBytes;
        CHECK(exact>2U*(strlen("prompt")+strlen("answer")+2U));before=summary;
        CHECK(UmiAiTrainingJsonlAuditOverlap(A "\n" A,strlen(A "\n" A),sides,2,exact-1,NULL,&summary,&report)==UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(memcmp(&summary,&before,sizeof(summary))==0 && report.stage==UMI_AI_TRAINING_JSONL_COMPLETE);
        CHECK(UmiAiTrainingJsonlAuditOverlap(A "\n" A,strlen(A "\n" A),sides,2,exact,NULL,&summary,&report)==UMI_STATUS_OK);
        CHECK(summary.stagingBytes==exact);
    } else if(!strcmp(mode,"membership")) {
        const unsigned char invalid[]={0,2};
        CHECK(UmiAiTrainingJsonlAuditOverlap(A,strlen(A),invalid,2,1000,NULL,&summary,&report)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(report.stage==UMI_AI_TRAINING_JSONL_INPUT && report.acceptedRows==0);
        CHECK(UmiAiTrainingJsonlAuditOverlap(A,strlen(A),sides,2,1000,NULL,&summary,&report)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(report.stage==UMI_AI_TRAINING_JSONL_COMPLETE && report.acceptedRows==1);
        CHECK(UmiAiTrainingJsonlAuditOverlap(A "\n" A,strlen(A "\n" A),sides,1,1000,NULL,&summary,&report)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(report.acceptedRows==2 && memcmp(&summary,&before,sizeof(summary))==0);
    } else if(!strcmp(mode,"partial")) {
        const char *input=A "\n{}";
        CHECK(UmiAiTrainingJsonlAuditOverlap(input,strlen(input),sides,2,1000,NULL,&summary,&report)==UMI_STATUS_PARSE_ERROR);
        CHECK(report.acceptedRows==1 && report.failedLine==2 && report.consumedBytes==strlen(A)+1);
        CHECK(memcmp(&summary,&before,sizeof(summary))==0);
    } else if(!strcmp(mode,"cancel")) {
        UmiCancellationToken *token=NULL;CHECK(umi_cancellation_token_create(&token)==UMI_STATUS_OK);
        umi_cancellation_token_request(token);
        CHECK(UmiAiTrainingJsonlAuditOverlap(A,strlen(A),sides,1,1000,token,&summary,&report)==UMI_STATUS_CANCELLED);
        CHECK(report.acceptedRows==0 && report.stage==UMI_AI_TRAINING_JSONL_INPUT);
        CHECK(memcmp(&summary,&before,sizeof(summary))==0);umi_cancellation_token_destroy(token);
    } else if(!strcmp(mode,"rows")) {
        const size_t rows=UMI_AI_TRAINING_JSONL_ROWS,stride=strlen(A "\n");
        char *input=malloc(stride*(rows+1));unsigned char *membership=malloc(rows);CHECK(input && membership);
        for(size_t i=0;i<=rows;++i)memcpy(input+i*stride,A "\n",stride);
        for(size_t i=0;i<rows;++i)membership[i]=(unsigned char)(i%2);
        CHECK(UmiAiTrainingJsonlAuditOverlap(input,stride*rows,membership,rows,SIZE_MAX,NULL,&summary,&report)==UMI_STATUS_OK);
        CHECK(summary.rows==rows && summary.distinctPairs==1 && summary.sharedPairs==1);
        CHECK(summary.trainingRowsInSharedPairs==rows/2 && summary.evaluationRowsInSharedPairs==rows/2);
        CHECK(summary.firstTrainingLine==1 && summary.firstEvaluationLine==2);before=summary;
        CHECK(UmiAiTrainingJsonlAuditOverlap(input,stride*(rows+1),membership,rows,SIZE_MAX,NULL,&summary,&report)==UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(report.acceptedRows==rows && memcmp(&summary,&before,sizeof(summary))==0);
        free(membership);free(input);
    } else if(!strcmp(mode,"owned")) {
        char input[]=A "\n" ROW("b","prompt","answer");char saved[sizeof(input)];memcpy(saved,input,sizeof(input));
        unsigned char membership[]={1,0};
        for(size_t i=0;i<300;++i) {
            CHECK(Audit(input,membership,2,&summary)==0);
            CHECK(summary.firstTrainingLine==2 && summary.firstEvaluationLine==1);
            CHECK(memcmp(input,saved,sizeof(input))==0 && membership[0]==1 && membership[1]==0);
        }
        before=summary;memset(input,0,sizeof(input));memset(membership,0,sizeof(membership));
        CHECK(memcmp(&summary,&before,sizeof(summary))==0);
    } else if(!strcmp(mode,"arguments")) {
        CHECK(UmiAiTrainingJsonlAuditOverlap(NULL,1,sides,1,1000,NULL,&summary,&report)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiAiTrainingJsonlAuditOverlap(A,0,sides,1,1000,NULL,&summary,&report)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiAiTrainingJsonlAuditOverlap(A,strlen(A),NULL,1,1000,NULL,&summary,&report)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiAiTrainingJsonlAuditOverlap(A,strlen(A),sides,0,1000,NULL,&summary,&report)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiAiTrainingJsonlAuditOverlap(A,strlen(A),sides,1,0,NULL,&summary,&report)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiAiTrainingJsonlAuditOverlap(A,strlen(A),sides,1,1000,NULL,NULL,&report)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiAiTrainingJsonlAuditOverlap(A,strlen(A),sides,1,1000,NULL,&summary,NULL)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiAiTrainingJsonlAuditOverlap(A,UMI_AI_TRAINING_JSONL_BYTES+1U,sides,1,1000,NULL,&summary,&report)==UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(UmiAiTrainingJsonlAuditOverlap(A,strlen(A),sides,UMI_AI_TRAINING_JSONL_ROWS+1U,1000,NULL,&summary,&report)==UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(memcmp(&summary,&before,sizeof(summary))==0);
    } else return 2;
    return 0;
}

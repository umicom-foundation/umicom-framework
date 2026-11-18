/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/training_data/test_jsonl_review.c
 *
 * PURPOSE:
 *   Verify atomic review, source grouping, exact overlap and staging-budget refusal.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/ai/training/jsonl_review.h"
#include <stdio.h>
#include <string.h>
#define CHECK(x) do { if(!(x)){fprintf(stderr,"%d: %s\n",__LINE__,#x);return 1;} } while(0)
static const char rows[]=
    "{\"source_id\":\"a\",\"prompt\":\"p\",\"completion\":\"c\"}\n"
    "{\"source_id\":\"a\",\"prompt\":\"q\",\"completion\":\"r\"}\n"
    "{\"source_id\":\"b\",\"prompt\":\"p\",\"completion\":\"c\"}\n";
static int success(uint64_t seed)
{
    unsigned char member[8];memset(member,0x5a,sizeof(member));
    UmiAiTrainingJsonlReviewSummary summary;
    UmiAiTrainingJsonlReport report;
    UmiAiTrainingJsonlReviewOptions options={1U,seed,65536U};
    CHECK(UmiAiTrainingJsonlReview(rows,sizeof(rows)-1U,&options,NULL,member,sizeof(member),&summary,&report)==UMI_STATUS_OK);
    CHECK(summary.partition.rows==3U && summary.partition.sourceGroups==2U);
    CHECK(member[0]==member[1] && member[0]!=member[2] && member[0]<=1U && member[2]<=1U);
    CHECK(summary.partition.evaluationRows==(size_t)(member[0]+member[1]+member[2]));
    CHECK(summary.partition.trainingRows+summary.partition.evaluationRows==3U);
    CHECK(summary.overlap.rows==3U && summary.overlap.distinctPairs==2U && summary.overlap.duplicateRows==1U);
    CHECK(summary.overlap.sharedPairs==1U && summary.overlap.trainingRowsInSharedPairs==1U && summary.overlap.evaluationRowsInSharedPairs==1U);
    CHECK(summary.overlap.firstTrainingLine==(member[0]==0U?1U:3U));
    CHECK(summary.overlap.firstEvaluationLine==(member[0]==1U?1U:3U));
    CHECK(summary.overlap.trainingRows==summary.partition.trainingRows && summary.overlap.evaluationRows==summary.partition.evaluationRows);
    CHECK(report.acceptedRows==3U && report.stage==UMI_AI_TRAINING_JSONL_COMPLETE);
    for(size_t i=3U;i<sizeof(member);++i)CHECK(member[i]==0x5a);
    unsigned char repeat[8]={0};UmiAiTrainingJsonlReviewSummary again;
    CHECK(UmiAiTrainingJsonlReview(rows,sizeof(rows)-1U,&options,NULL,repeat,sizeof(repeat),&again,&report)==UMI_STATUS_OK);
    CHECK(memcmp(member,repeat,3U)==0 && again.overlap.stagingBytes==summary.overlap.stagingBytes);
    return 0;
}
int main(int argc,char **argv)
{
    CHECK(argc==2);
    if(!strcmp(argv[1],"shared"))return success(42U);
    if(!strcmp(argv[1],"seed-boundaries")){CHECK(success(0U)==0);CHECK(success(UINT64_MAX)==0);return 0;}
    unsigned char member[8],beforeMember[8];memset(member,0x5a,sizeof(member));memcpy(beforeMember,member,sizeof(member));
    UmiAiTrainingJsonlReviewSummary summary,before;memset(&summary,0x5a,sizeof(summary));before=summary;
    UmiAiTrainingJsonlReport report;
    UmiAiTrainingJsonlReviewOptions options={1U,42U,65536U};
    UmiStatus status=UMI_STATUS_OK;
    if(!strcmp(argv[1],"capacity")){
        status=UmiAiTrainingJsonlReview(rows,sizeof(rows)-1U,&options,NULL,member,2U,&summary,&report);
        CHECK(status==UMI_STATUS_CAPACITY_EXCEEDED && report.stage==UMI_AI_TRAINING_JSONL_COMPLETE);
    }else if(!strcmp(argv[1],"budget")){
        options.maxStagingBytes=1U;
        status=UmiAiTrainingJsonlReview(rows,sizeof(rows)-1U,&options,NULL,member,sizeof(member),&summary,&report);
        CHECK(status==UMI_STATUS_CAPACITY_EXCEEDED && report.acceptedRows==3U && report.stage==UMI_AI_TRAINING_JSONL_COMPLETE);
    }else if(!strcmp(argv[1],"budget-boundary")){
        UmiAiTrainingJsonlReviewSummary measured;unsigned char temporary[8];
        CHECK(UmiAiTrainingJsonlReview(rows,sizeof(rows)-1U,&options,NULL,temporary,8U,&measured,&report)==UMI_STATUS_OK);
        options.maxStagingBytes=measured.overlap.stagingBytes;
        CHECK(UmiAiTrainingJsonlReview(rows,sizeof(rows)-1U,&options,NULL,temporary,8U,&measured,&report)==UMI_STATUS_OK);
        --options.maxStagingBytes;
        status=UmiAiTrainingJsonlReview(rows,sizeof(rows)-1U,&options,NULL,member,8U,&summary,&report);
        CHECK(status==UMI_STATUS_CAPACITY_EXCEEDED);
    }else if(!strcmp(argv[1],"late-malformed")){
        const char bad[]="{\"source_id\":\"a\",\"prompt\":\"p\",\"completion\":\"c\"}\n{}\n";
        status=UmiAiTrainingJsonlReview(bad,sizeof(bad)-1U,&options,NULL,member,8U,&summary,&report);
        CHECK(status!=UMI_STATUS_OK && report.acceptedRows==1U && report.failedLine==2U && report.stage==UMI_AI_TRAINING_JSONL_RECORD);
    }else if(!strcmp(argv[1],"group-count")){
        options.evaluationGroupCount=2U;
        status=UmiAiTrainingJsonlReview(rows,sizeof(rows)-1U,&options,NULL,member,8U,&summary,&report);
        CHECK(status==UMI_STATUS_INVALID_ARGUMENT && report.stage==UMI_AI_TRAINING_JSONL_COMPLETE);
    }else if(!strcmp(argv[1],"cancel")){
        UmiCancellationToken *token=NULL;CHECK(umi_cancellation_token_create(&token)==UMI_STATUS_OK);
        umi_cancellation_token_request(token);
        status=UmiAiTrainingJsonlReview(rows,sizeof(rows)-1U,&options,token,member,8U,&summary,&report);
        umi_cancellation_token_destroy(token);
        CHECK(status==UMI_STATUS_CANCELLED && report.acceptedRows==0U && report.stage==UMI_AI_TRAINING_JSONL_INPUT);
    }else if(!strcmp(argv[1],"arguments")){
        CHECK(UmiAiTrainingJsonlReview(NULL,1U,&options,NULL,member,8U,&summary,&report)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiAiTrainingJsonlReview(rows,0U,&options,NULL,member,8U,&summary,&report)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiAiTrainingJsonlReview(rows,sizeof(rows)-1U,NULL,NULL,member,8U,&summary,&report)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiAiTrainingJsonlReview(rows,sizeof(rows)-1U,&options,NULL,NULL,8U,&summary,&report)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiAiTrainingJsonlReview(rows,sizeof(rows)-1U,&options,NULL,member,8U,NULL,&report)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiAiTrainingJsonlReview(rows,sizeof(rows)-1U,&options,NULL,member,8U,&summary,NULL)==UMI_STATUS_INVALID_ARGUMENT);
        options.evaluationGroupCount=0U;
        CHECK(UmiAiTrainingJsonlReview(rows,sizeof(rows)-1U,&options,NULL,member,8U,&summary,&report)==UMI_STATUS_INVALID_ARGUMENT);
        options.evaluationGroupCount=UMI_AI_TRAINING_JSONL_ROWS;
        CHECK(UmiAiTrainingJsonlReview(rows,sizeof(rows)-1U,&options,NULL,member,8U,&summary,&report)==UMI_STATUS_INVALID_ARGUMENT);
        options.evaluationGroupCount=1U;options.maxStagingBytes=0U;
        CHECK(UmiAiTrainingJsonlReview(rows,sizeof(rows)-1U,&options,NULL,member,8U,&summary,&report)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(report.acceptedRows==0U && report.stage==UMI_AI_TRAINING_JSONL_INPUT);
    }else if(!strcmp(argv[1],"source-bound")){
        status=UmiAiTrainingJsonlReview(rows,UMI_AI_TRAINING_JSONL_BYTES+1U,&options,NULL,member,8U,&summary,&report);
        CHECK(status==UMI_STATUS_CAPACITY_EXCEEDED && report.acceptedRows==0U);
    }else return 2;
    CHECK(memcmp(member,beforeMember,sizeof(member))==0);
    CHECK(memcmp(&summary,&before,sizeof(summary))==0);
    return 0;
}

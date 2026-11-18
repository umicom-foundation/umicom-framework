/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/training_data/test_jsonl_file.c
 *
 * PURPOSE:
 *   Verify file admission, output preservation and shared CLI presentation using native argument ownership.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/ai/training/jsonl_file.h"
#include "umicom/platform/native_arguments.h"
#include "umicom/platform/output_file.h"
#include "../../tools/umicom/src/command_training_data.h"
#include <stdio.h>
#include <string.h>
#define CHECK(condition) do { if (!(condition)) { \
    fprintf(stderr, "%d: %s\n", __LINE__, #condition); return 1; } } while (0)

static int Service(const char *scenario, const char *file)
{
    UmiAiTrainingJsonlSummary summary, before;
    UmiAiTrainingJsonlReport report;
    memset(&summary, 0x5a, sizeof summary);
    before = summary;
    memset(&report, 0x5a, sizeof report);
    if (strcmp(scenario, "arguments") == 0) {
        CHECK(UmiAiTrainingJsonlInspectFile(NULL, NULL, &summary, &report) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(report.acceptedRows == 0U && report.failedLine == 0U && report.stage == UMI_AI_TRAINING_JSONL_INPUT);
        CHECK(UmiAiTrainingJsonlInspectFile(file, NULL, NULL, &report) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiAiTrainingJsonlInspectFile(file, NULL, &summary, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(memcmp(&summary, &before, sizeof summary) == 0);
        return 0;
    }
    if (strcmp(scenario, "cancel") == 0) {
        UmiCancellationToken *token = NULL;
        CHECK(umi_cancellation_token_create(&token) == UMI_STATUS_OK);
        umi_cancellation_token_request(token);
        UmiStatus status = UmiAiTrainingJsonlInspectFile(file, token, &summary, &report);
        umi_cancellation_token_destroy(token);
        CHECK(status == UMI_STATUS_CANCELLED);
        CHECK(memcmp(&summary, &before, sizeof summary) == 0);
        CHECK(report.acceptedRows == 0U && report.consumedBytes == 0U && report.stage == UMI_AI_TRAINING_JSONL_INPUT);
        return 0;
    }
    UmiStatus status = UmiAiTrainingJsonlInspectFile(file, NULL, &summary, &report);
    if (strcmp(scenario, "valid") == 0 || strcmp(scenario, "unicode") == 0 || strcmp(scenario, "crlf") == 0) {
        CHECK(status == UMI_STATUS_OK);
        CHECK(summary.rows == 2U && summary.sourceGroups == 1U && summary.largestSourceGroupRows == 2U);
        CHECK(summary.emptyPrompts == 1U && summary.promptBytes == 17U && summary.completionBytes == 37U);
        CHECK(report.acceptedRows == 2U && report.failedLine == 0U && report.stage == UMI_AI_TRAINING_JSONL_COMPLETE);
    } else {
        CHECK(status != UMI_STATUS_OK);
        CHECK(memcmp(&summary, &before, sizeof summary) == 0);
        if (strcmp(scenario, "invalid-second") == 0) {
            CHECK(report.acceptedRows == 1U && report.failedLine == 2U && report.stage == UMI_AI_TRAINING_JSONL_RECORD);
        } else if (strcmp(scenario, "oversize") == 0) {
            CHECK(status == UMI_STATUS_CAPACITY_EXCEEDED && report.stage == UMI_AI_TRAINING_JSONL_INPUT);
        } else if (strcmp(scenario, "missing") == 0) {
            CHECK(status == UMI_STATUS_NOT_FOUND && report.acceptedRows == 0U);
        } else if (strcmp(scenario, "relative") == 0) {
            CHECK(status == UMI_STATUS_INVALID_ARGUMENT && report.acceptedRows == 0U);
        }
    }
    return 0;
}

/* The context-file service shares input ownership with summary inspection.
 * Sentinel outputs distinguish an atomic refusal from partial publication. */
static int ContextService(const char *scenario,const char *file)
{
    UmiAiTrainingJsonlContextSummary summary,before;
    UmiAiTrainingJsonlReport report;
    memset(&summary,0x5a,sizeof summary);
    before=summary;
    memset(&report,0x5a,sizeof report);
    if(strcmp(scenario,"arguments")==0) {
        CHECK(UmiAiTrainingJsonlInspectContextFile(NULL,4U,NULL,&summary,&report)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(report.acceptedRows==0U && report.stage==UMI_AI_TRAINING_JSONL_INPUT);
        CHECK(UmiAiTrainingJsonlInspectContextFile(file,4U,NULL,NULL,&report)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiAiTrainingJsonlInspectContextFile(file,4U,NULL,&summary,NULL)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiAiTrainingJsonlInspectContextFile(file,0U,NULL,&summary,&report)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiAiTrainingJsonlInspectContextFile(file,33U,NULL,&summary,&report)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(report.acceptedRows==0U && report.consumedBytes==0U);
        CHECK(memcmp(&summary,&before,sizeof summary)==0);
        return 0;
    }
    if(strcmp(scenario,"cancel")==0) {
        UmiCancellationToken *token=NULL;
        CHECK(umi_cancellation_token_create(&token)==UMI_STATUS_OK);
        umi_cancellation_token_request(token);
        UmiStatus status=UmiAiTrainingJsonlInspectContextFile(file,4U,token,&summary,&report);
        umi_cancellation_token_destroy(token);
        CHECK(status==UMI_STATUS_CANCELLED);
        CHECK(memcmp(&summary,&before,sizeof summary)==0);
        CHECK(report.acceptedRows==0U && report.consumedBytes==0U && report.stage==UMI_AI_TRAINING_JSONL_INPUT);
        return 0;
    }
    UmiStatus status=UmiAiTrainingJsonlInspectContextFile(file,4U,NULL,&summary,&report);
    if(strcmp(scenario,"valid")==0 || strcmp(scenario,"unicode")==0 || strcmp(scenario,"crlf")==0) {
        CHECK(status==UMI_STATUS_OK);
        CHECK(summary.contextLength==4U && summary.records==3U);
        CHECK(summary.fittingRecords==2U && summary.overlongRecords==1U && summary.firstOverlongLine==3U);
        CHECK(summary.requiredRows==17U && summary.shortestRequiredRows==3U && summary.longestRequiredRows==10U);
        CHECK(summary.fittingValidRows==7U && summary.fittingSelectedRows==6U && summary.fittingPaddingRows==1U);
        CHECK(report.acceptedRows==3U && report.stage==UMI_AI_TRAINING_JSONL_COMPLETE);
    } else {
        CHECK(status!=UMI_STATUS_OK);
        CHECK(memcmp(&summary,&before,sizeof summary)==0);
        if(strcmp(scenario,"invalid-second")==0) {
            CHECK(report.acceptedRows==1U && report.failedLine==2U && report.stage==UMI_AI_TRAINING_JSONL_RECORD);
        } else {
            CHECK(report.acceptedRows==0U && report.consumedBytes==0U && report.stage==UMI_AI_TRAINING_JSONL_INPUT);
            if(strcmp(scenario,"missing")==0) CHECK(status==UMI_STATUS_NOT_FOUND);
            if(strcmp(scenario,"relative")==0) CHECK(status==UMI_STATUS_INVALID_ARGUMENT);
            if(strcmp(scenario,"oversize")==0) CHECK(status==UMI_STATUS_CAPACITY_EXCEEDED);
        }
    }
    return 0;
}


/* Synthetic fixtures use binary output so CRLF is not rewritten by a shell. */
static int ReviewFixture(const char *kind,const char *path)
{
    UmiOutputFile *file=NULL;
    CHECK(UmiOutputFileCreate(path,&file)==UMI_STATUS_OK);
    UmiStatus status=UMI_STATUS_OK;
    if(strcmp(kind,"oversize")==0){
        char spaces[4096];memset(spaces,' ',sizeof(spaces));
        size_t remaining=UMI_AI_TRAINING_JSONL_BYTES+1U;
        while(remaining>0U && status==UMI_STATUS_OK){
            size_t chunk=remaining<sizeof(spaces)?remaining:sizeof(spaces);
            status=UmiOutputFileWrite(file,spaces,chunk);remaining-=chunk;
        }
    }else{
        const char *first="{\"source_id\":\"private-one\",\"prompt\":\"private-question\",\"completion\":\"private-answer\"}";
        const char *second=strcmp(kind,"late-invalid")==0?"{}":
            strcmp(kind,"distinct")==0?"{\"source_id\":\"private-two\",\"prompt\":\"another-question\",\"completion\":\"private-answer\"}":
            "{\"source_id\":\"private-two\",\"prompt\":\"private-question\",\"completion\":\"private-answer\"}";
        const char *ending=strcmp(kind,"crlf")==0?"\r\n":"\n";
        status=UmiOutputFileWrite(file,first,strlen(first));
        if(status==UMI_STATUS_OK)status=UmiOutputFileWrite(file,ending,strlen(ending));
        if(status==UMI_STATUS_OK)status=UmiOutputFileWrite(file,second,strlen(second));
        if(status==UMI_STATUS_OK)status=UmiOutputFileWrite(file,ending,strlen(ending));
    }
    const UmiStatus closed=UmiOutputFileClose(file);UmiOutputFileDestroy(file);
    CHECK(status==UMI_STATUS_OK && closed==UMI_STATUS_OK);
    return 0;
}
static int ReviewService(const char *scenario,const char *file)
{
    UmiAiTrainingJsonlReviewSummary summary,before;memset(&summary,0x5a,sizeof(summary));before=summary;
    unsigned char membership[8],original[8];memset(membership,0x5a,sizeof(membership));memcpy(original,membership,sizeof(original));
    UmiAiTrainingJsonlReport report;
    UmiAiTrainingJsonlReviewOptions options={1U,42U,65536U};
    size_t capacity=sizeof(membership);
    if(strcmp(scenario,"arguments")==0){
        CHECK(UmiAiTrainingJsonlReviewFile(NULL,&options,NULL,membership,capacity,&summary,&report)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiAiTrainingJsonlReviewFile(file,NULL,NULL,membership,capacity,&summary,&report)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiAiTrainingJsonlReviewFile(file,&options,NULL,NULL,capacity,&summary,&report)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiAiTrainingJsonlReviewFile(file,&options,NULL,membership,capacity,NULL,&report)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiAiTrainingJsonlReviewFile(file,&options,NULL,membership,capacity,&summary,NULL)==UMI_STATUS_INVALID_ARGUMENT);
        options.evaluationGroupCount=0U;
        CHECK(UmiAiTrainingJsonlReviewFile(file,&options,NULL,membership,capacity,&summary,&report)==UMI_STATUS_INVALID_ARGUMENT);
        options.evaluationGroupCount=UMI_AI_TRAINING_JSONL_ROWS;
        CHECK(UmiAiTrainingJsonlReviewFile(file,&options,NULL,membership,capacity,&summary,&report)==UMI_STATUS_INVALID_ARGUMENT);
        options.evaluationGroupCount=1U;options.maxStagingBytes=0U;
        CHECK(UmiAiTrainingJsonlReviewFile(file,&options,NULL,membership,capacity,&summary,&report)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(report.acceptedRows==0U && report.stage==UMI_AI_TRAINING_JSONL_INPUT);
    }else{
        if(strcmp(scenario,"capacity")==0)capacity=1U;
        if(strcmp(scenario,"budget")==0)options.maxStagingBytes=1U;
        if(strcmp(scenario,"groups")==0)options.evaluationGroupCount=2U;
        UmiCancellationToken *token=NULL;
        if(strcmp(scenario,"cancel")==0){CHECK(umi_cancellation_token_create(&token)==UMI_STATUS_OK);umi_cancellation_token_request(token);}
        const UmiStatus status=UmiAiTrainingJsonlReviewFile(file,&options,token,membership,capacity,&summary,&report);
        umi_cancellation_token_destroy(token);
        if(strcmp(scenario,"valid")==0 || strcmp(scenario,"unicode")==0 || strcmp(scenario,"crlf")==0 || strcmp(scenario,"distinct")==0){
            CHECK(status==UMI_STATUS_OK && summary.partition.rows==2U && summary.partition.sourceGroups==2U);
            CHECK(summary.partition.trainingRows==1U && summary.partition.evaluationRows==1U);
            CHECK(membership[0]<=1U && membership[1]<=1U && membership[0]!=membership[1]);
            CHECK(summary.overlap.sharedPairs==(strcmp(scenario,"distinct")==0?0U:1U));
            CHECK(report.acceptedRows==2U && report.stage==UMI_AI_TRAINING_JSONL_COMPLETE);
            for(size_t i=2U;i<sizeof(membership);++i)CHECK(membership[i]==0x5a);
            return 0;
        }
        CHECK(status!=UMI_STATUS_OK);
        if(strcmp(scenario,"budget")==0 || strcmp(scenario,"capacity")==0){CHECK(status==UMI_STATUS_CAPACITY_EXCEEDED && report.stage==UMI_AI_TRAINING_JSONL_COMPLETE);}
        else if(strcmp(scenario,"groups")==0){CHECK(status==UMI_STATUS_INVALID_ARGUMENT && report.stage==UMI_AI_TRAINING_JSONL_COMPLETE);}
        else if(strcmp(scenario,"late-invalid")==0){CHECK(report.acceptedRows==1U && report.failedLine==2U && report.stage==UMI_AI_TRAINING_JSONL_RECORD);}
        else{
            CHECK(report.acceptedRows==0U && report.stage==UMI_AI_TRAINING_JSONL_INPUT);
            if(strcmp(scenario,"cancel")==0)CHECK(status==UMI_STATUS_CANCELLED);
            if(strcmp(scenario,"missing")==0)CHECK(status==UMI_STATUS_NOT_FOUND);
            if(strcmp(scenario,"relative")==0)CHECK(status==UMI_STATUS_INVALID_ARGUMENT);
            if(strcmp(scenario,"oversize")==0)CHECK(status==UMI_STATUS_CAPACITY_EXCEEDED);
        }
    }
    CHECK(memcmp(&summary,&before,sizeof(summary))==0 && memcmp(membership,original,sizeof(membership))==0);
    return 0;
}

static int Run(int argc, char **argv)
{
    if (argc == 4 && strcmp(argv[1], "review-fixture") == 0) return ReviewFixture(argv[2],argv[3]);
    if (argc == 4 && strcmp(argv[1], "review-service") == 0) return ReviewService(argv[2],argv[3]);
    if (argc == 4 && strcmp(argv[1], "context-service") == 0) return ContextService(argv[2], argv[3]);
    if (argc == 4 && strcmp(argv[1], "service") == 0) return Service(argv[2], argv[3]);
    if (argc >= 2 && strcmp(argv[1], "training-data") == 0)
        return umi_cli_command_training_data(argc - 2, argv + 2);
    return 2;
}
int main(int argc, char **argv)
{
    int result = 2;
    UmiStatus status = UmiNativeArgumentsDispatch(argc, argv, Run, &result);
    return status == UMI_STATUS_OK ? result : 2;
}

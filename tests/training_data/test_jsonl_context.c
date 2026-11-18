/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/training_data/test_jsonl_context.c
 *
 * PURPOSE:
 *   Verify exact decoded-byte context statistics, full-input refusal and overflow-safe pair measurement.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ai/training/jsonl_context.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#define CHECK(x) do {if(!(x)){fprintf(stderr,"%d: %s\n",__LINE__,#x);return 1;}}while(0)
#define ROW "{\"source_id\":\"a\",\"prompt\":\"p\",\"completion\":\"c\"}"
static int Inspect(const char *text,size_t context,UmiAiTrainingJsonlContextSummary *out)
{
    UmiAiTrainingJsonlReport report;
    CHECK(UmiAiTrainingJsonlInspectContext(text,strlen(text),context,NULL,out,&report)==UMI_STATUS_OK);
    CHECK(report.stage==UMI_AI_TRAINING_JSONL_COMPLETE && report.failedLine==0);
    CHECK(report.acceptedRows==out->records && report.consumedBytes==strlen(text));
    return 0;
}
int main(int argc,char **argv)
{
    CHECK(argc==2);const char *mode=argv[1];
    UmiAiTrainingJsonlContextSummary out,saved;memset(&out,0x5a,sizeof out);memcpy(&saved,&out,sizeof out);
    UmiAiTrainingJsonlReport report;
    if(!strcmp(mode,"golden")) {
        const char text[]=ROW "\n{\"source_id\":\"b\",\"prompt\":\"\",\"completion\":\"xyz\"}\n"
            "{\"source_id\":\"c\",\"prompt\":\"long\",\"completion\":\"reply\"}";
        CHECK(Inspect(text,4,&out)==0);
        CHECK(out.contextLength==4 && out.records==3 && out.fittingRecords==2 && out.overlongRecords==1);
        CHECK(out.firstOverlongLine==3 && out.shortestRequiredRows==3 && out.longestRequiredRows==10);
        CHECK(out.requiredRows==17 && out.fittingValidRows==7 && out.fittingSelectedRows==6 && out.fittingPaddingRows==1);
    }else if(!strcmp(mode,"escapes")) {
        const char text[]="{\"source_id\":\"a\",\"prompt\":\"\\u00e9\",\"completion\":\"\\ud83d\\ude80\"}";
        CHECK(Inspect(text,7,&out)==0);CHECK(out.requiredRows==7 && out.fittingSelectedRows==5 && out.fittingPaddingRows==0);
        CHECK(Inspect(text,6,&out)==0);CHECK(out.overlongRecords==1 && out.fittingSelectedRows==0 && out.firstOverlongLine==1);
    }else if(!strcmp(mode,"exact")) {
        CHECK(Inspect(ROW "\r\n" ROW,3,&out)==0);
        CHECK(out.requiredRows==6 && out.fittingRecords==2 && out.overlongRecords==0 && out.firstOverlongLine==0);
        CHECK(out.fittingSelectedRows==4 && out.fittingPaddingRows==0);
        CHECK(Inspect(ROW,32,&out)==0);CHECK(out.fittingPaddingRows==29);
    }else if(!strcmp(mode,"all-overlong")) {
        CHECK(Inspect(ROW "\n" ROW,1,&out)==0);
        CHECK(out.records==2 && out.overlongRecords==2 && out.firstOverlongLine==1 && out.requiredRows==6);
        CHECK(out.fittingRecords==0 && out.fittingValidRows==0 && out.fittingSelectedRows==0 && out.fittingPaddingRows==0);
    }else if(!strcmp(mode,"malformed")) {
        const char text[]=ROW "\n{}";
        CHECK(UmiAiTrainingJsonlInspectContext(text,sizeof text-1,2,NULL,&out,&report)!=UMI_STATUS_OK);
        CHECK(memcmp(&out,&saved,sizeof out)==0 && report.acceptedRows==1 && report.failedLine==2);
        CHECK(report.consumedBytes==strlen(ROW)+1 && report.stage==UMI_AI_TRAINING_JSONL_RECORD);
    }else if(!strcmp(mode,"cancel")) {
        UmiCancellationToken *token=NULL;CHECK(umi_cancellation_token_create(&token)==UMI_STATUS_OK);
        umi_cancellation_token_request(token);
        UmiStatus status=UmiAiTrainingJsonlInspectContext(ROW,strlen(ROW),3,token,&out,&report);
        umi_cancellation_token_destroy(token);
        CHECK(status==UMI_STATUS_CANCELLED && memcmp(&out,&saved,sizeof out)==0 && report.acceptedRows==0);
    }else if(!strcmp(mode,"arguments")) {
        CHECK(UmiAiTrainingJsonlInspectContext(NULL,1,3,NULL,&out,&report)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(memcmp(&out,&saved,sizeof out)==0 && report.stage==UMI_AI_TRAINING_JSONL_INPUT);
        CHECK(UmiAiTrainingJsonlInspectContext(ROW,strlen(ROW),0,NULL,&out,&report)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiAiTrainingJsonlInspectContext(ROW,strlen(ROW),33,NULL,&out,&report)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiAiTrainingJsonlInspectContext(ROW,strlen(ROW),3,NULL,NULL,&report)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiAiTrainingJsonlInspectContext(ROW,strlen(ROW),3,NULL,&out,NULL)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(memcmp(&out,&saved,sizeof out)==0);
    }else if(!strcmp(mode,"bounds")) {
        const size_t rowBytes=strlen(ROW)+1U;
        char *text=malloc(rowBytes*(UMI_AI_TRAINING_JSONL_ROWS+1U));CHECK(text!=NULL);
        for(size_t i=0;i<UMI_AI_TRAINING_JSONL_ROWS+1U;++i){memcpy(text+i*rowBytes,ROW,rowBytes-1U);text[(i+1U)*rowBytes-1U]='\n';}
        UmiStatus status=UmiAiTrainingJsonlInspectContext(text,rowBytes*UMI_AI_TRAINING_JSONL_ROWS,3,NULL,&out,&report);
        CHECK(status==UMI_STATUS_OK && out.records==UMI_AI_TRAINING_JSONL_ROWS);
        CHECK(out.requiredRows==3U*UMI_AI_TRAINING_JSONL_ROWS && out.fittingSelectedRows==2U*UMI_AI_TRAINING_JSONL_ROWS);
        memcpy(&saved,&out,sizeof out);
        status=UmiAiTrainingJsonlInspectContext(text,rowBytes*(UMI_AI_TRAINING_JSONL_ROWS+1U),3,NULL,&out,&report);
        free(text);
        CHECK(status==UMI_STATUS_CAPACITY_EXCEEDED && memcmp(&out,&saved,sizeof out)==0);
        CHECK(report.acceptedRows==UMI_AI_TRAINING_JSONL_ROWS);
    }else if(!strcmp(mode,"measure")) {
        UmiAiTrainingBytePairShape shape,before;memset(&shape,0x4b,sizeof shape);memcpy(&before,&shape,sizeof shape);
        CHECK(UmiAiTrainingBytePairMeasure(0,0,&shape)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiAiTrainingBytePairMeasure(SIZE_MAX,1,&shape)==UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(UmiAiTrainingBytePairMeasure(0,SIZE_MAX,&shape)==UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(UmiAiTrainingBytePairMeasure(SIZE_MAX-1U,1,&shape)==UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(memcmp(&shape,&before,sizeof shape)==0);
        CHECK(UmiAiTrainingBytePairMeasure(1,1,NULL)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiAiTrainingBytePairMeasure(SIZE_MAX-2U,1,&shape)==UMI_STATUS_OK);
        CHECK(shape.validRows==SIZE_MAX && shape.selectedRows==2);
        CHECK(UmiAiTrainingBytePairMeasure(0,SIZE_MAX-1U,&shape)==UMI_STATUS_OK);
        CHECK(shape.validRows==SIZE_MAX && shape.selectedRows==SIZE_MAX);
    }else return 2;
    return 0;
}

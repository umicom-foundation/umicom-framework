/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/training_data/test_jsonl_writer.c
 *
 * PURPOSE:
 *   Verify exact output capacity, Unicode escaping, round trips and transactional rejection.
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
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr,"%d: %s\n",__LINE__,#x);return 1; } } while(0)
static int Refuse(const UmiAiTrainingPair *pair,size_t capacity,UmiStatus expected)
{
    char output[UMI_AI_TRAINING_PAIR_LINE_BYTES+2U], before[sizeof(output)];
    memset(output,'?',sizeof(output));memcpy(before,output,sizeof(output));
    size_t count=777U;
    CHECK(UmiAiTrainingPairWrite(pair,output,capacity,&count)==expected);
    CHECK(count==777U&&memcmp(output,before,sizeof(output))==0);
    return 0;
}
int main(int argc,char **argv)
{
    CHECK(argc==2);const char *mode=argv[1];
    UmiAiTrainingPair pair={0};strcpy(pair.sourceId,"source");strcpy(pair.prompt,"why");strcpy(pair.completion,"answer");
    if(strcmp(mode,"roundtrip")==0||strcmp(mode,"escapes")==0||strcmp(mode,"empty-prompt")==0){
        if(strcmp(mode,"escapes")==0){
            strcpy(pair.sourceId,"caf\xc3\xa9");
            strcpy(pair.prompt,"quote \" and backslash \\");
            strcpy(pair.completion,"line\nbreak\ttab\rreturn\1");
        }
        if(strcmp(mode,"empty-prompt")==0)pair.prompt[0]=0;
        char output[1024];size_t count=0U;
        CHECK(UmiAiTrainingPairWrite(&pair,output,sizeof(output),&count)==UMI_STATUS_OK);
        CHECK(count==strlen(output)&&output[count-1U]=='\n');
        CHECK(memchr(output,'\n',count-1U)==NULL);
        UmiAiTrainingPair decoded;
        CHECK(UmiAiTrainingPairRead(output,count,NULL,&decoded)==UMI_STATUS_OK);
        CHECK(strcmp(pair.sourceId,decoded.sourceId)==0&&strcmp(pair.prompt,decoded.prompt)==0&&strcmp(pair.completion,decoded.completion)==0);
    }else if(strcmp(mode,"exact-capacity")==0){
        char output[1024];size_t count=0U;
        CHECK(UmiAiTrainingPairWrite(&pair,output,sizeof(output),&count)==UMI_STATUS_OK);
        size_t needed=count+1U;memset(output,'?',sizeof(output));
        CHECK(UmiAiTrainingPairWrite(&pair,output,needed,&count)==UMI_STATUS_OK);
        CHECK(output[needed-1U]==0&&output[needed]=='?');
        CHECK(Refuse(&pair,needed-1U,UMI_STATUS_CAPACITY_EXCEEDED)==0);
        CHECK(Refuse(&pair,0U,UMI_STATUS_CAPACITY_EXCEEDED)==0);
    }else if(strcmp(mode,"late-utf8")==0){
        pair.completion[0]=(char)0xff;pair.completion[1]=0;
        CHECK(Refuse(&pair,1024U,UMI_STATUS_INVALID_ARGUMENT)==0);
    }else if(strcmp(mode,"unterminated")==0){
        memset(pair.prompt,'x',sizeof(pair.prompt));
        CHECK(Refuse(&pair,1024U,UMI_STATUS_INVALID_ARGUMENT)==0);
    }else if(strcmp(mode,"empty-required")==0){
        pair.sourceId[0]=0;CHECK(Refuse(&pair,1024U,UMI_STATUS_INVALID_ARGUMENT)==0);
        strcpy(pair.sourceId,"source");pair.completion[0]=0;
        CHECK(Refuse(&pair,1024U,UMI_STATUS_INVALID_ARGUMENT)==0);
    }else if(strcmp(mode,"expansion")==0){
        memset(pair.prompt,1,UMI_AI_TRAINING_PAIR_TEXT_BYTES);
        memset(pair.completion,2,UMI_AI_TRAINING_PAIR_TEXT_BYTES);
        CHECK(Refuse(&pair,UMI_AI_TRAINING_PAIR_LINE_BYTES+2U,UMI_STATUS_CAPACITY_EXCEEDED)==0);
    }else if(strcmp(mode,"arguments")==0){
        char output[64];size_t count;
        CHECK(UmiAiTrainingPairWrite(NULL,output,sizeof(output),&count)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiAiTrainingPairWrite(&pair,NULL,sizeof(output),&count)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiAiTrainingPairWrite(&pair,output,sizeof(output),NULL)==UMI_STATUS_INVALID_ARGUMENT);
    }else return 2;
    return 0;
}

/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/training_metrics/test_prediction_metrics.c
 *
 * PURPOSE:
 *   Compare selected token metrics with literal ranks and an independent sorted-row oracle, including bounded refusal cases.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/ai/training/prediction_metrics.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr,"prediction metrics check: %d\n",__LINE__); return 1; } } while (0)
static const float scores[]={3,1,2,0, -4,-1,-2,-3, 1,1,1,1, -0.0F,0.0F,-0.0F,0.0F};
static const size_t targets[]={0,2,2,3};
static float largeScores[UMI_AI_TRAINING_MAX_ELEMENTS];
static size_t largeTargets[UMI_AI_TRAINING_MAX_ELEMENTS];
static int Refused(const float *values,size_t rows,size_t vocabulary,const size_t *truth,
    const uint8_t *selected,size_t selectedCount,size_t topK,UmiStatus expected)
{
    UmiAiTrainingPredictionCounts out,before;
    memset(&out,0xA7,sizeof out);memcpy(&before,&out,sizeof before);
    return UmiAiTrainingPredictionScore(values,rows,vocabulary,truth,selected,selectedCount,topK,&out)==expected && !memcmp(&before,&out,sizeof out);
}
int main(int argc,char **argv)
{
    CHECK(argc==2);const char *mode=argv[1];UmiAiTrainingPredictionCounts out;
    if(!strcmp(mode,"gold")) {
        CHECK(UmiAiTrainingPredictionScore(scores,4,4,targets,NULL,0,2,&out)==UMI_STATUS_OK);
        CHECK(out.rows==4 && out.selectedRows==4 && out.topOneCorrect==1 && out.topKCorrect==2 && out.topK==2);
    } else if(!strcmp(mode,"selection")) {
        uint8_t selected[]={0,1,1,0};
        CHECK(UmiAiTrainingPredictionScore(scores,4,4,targets,selected,4,2,&out)==UMI_STATUS_OK);
        CHECK(out.rows==4 && out.selectedRows==2 && out.topOneCorrect==0 && out.topKCorrect==1);
    } else if(!strcmp(mode,"empty-selection")) {
        uint8_t selected[]={0,0,0,0};
        CHECK(UmiAiTrainingPredictionScore(scores,4,4,targets,selected,4,4,&out)==UMI_STATUS_OK);
        CHECK(out.rows==4 && out.selectedRows==0 && out.topOneCorrect==0 && out.topKCorrect==0);
    } else if(!strcmp(mode,"ties")) {
        for(size_t target=0;target<4;++target)for(size_t k=1;k<=4;++k) {
            CHECK(UmiAiTrainingPredictionScore(scores+12,1,4,&target,NULL,0,k,&out)==UMI_STATUS_OK);
            CHECK(out.topOneCorrect==(target==0U?1U:0U));CHECK(out.topKCorrect==(target<k?1U:0U));
        }
    } else if(!strcmp(mode,"reference")) {
        /* Independent oracle fully sorts token IDs using stable insertion.
         * Repeated integer-valued logits deliberately create many ties. */
        for(uint32_t seed=1U;seed<=128U;++seed) {
            float values[19U*16U];size_t truth[19];uint8_t chosen[19];
            size_t rows=1U+seed%19U,vocabulary=1U+seed%16U,k=1U+seed%vocabulary;
            size_t selected=0,one=0,top=0;uint32_t state=seed;
            for(size_t row=0;row<rows;++row) {
                size_t sorted[16];
                for(size_t col=0;col<vocabulary;++col) {
                    state=state*UINT32_C(1664525)+UINT32_C(1013904223);
                    values[row*vocabulary+col]=(float)((int)(state%7U)-3);sorted[col]=col;
                }
                truth[row]=state%vocabulary;chosen[row]=(uint8_t)((row+seed)%3U!=0U);
                for(size_t i=1;i<vocabulary;++i) {
                    size_t id=sorted[i],j=i;
                    while(j>0U && (values[row*vocabulary+id]>values[row*vocabulary+sorted[j-1U]] ||
                        (values[row*vocabulary+id]==values[row*vocabulary+sorted[j-1U]] && id<sorted[j-1U]))) {
                        sorted[j]=sorted[j-1U];--j;
                    }
                    sorted[j]=id;
                }
                if(chosen[row]) {
                    ++selected;if(sorted[0]==truth[row])++one;
                    for(size_t i=0;i<k;++i)if(sorted[i]==truth[row])++top;
                }
            }
            CHECK(UmiAiTrainingPredictionScore(values,rows,vocabulary,truth,chosen,rows,k,&out)==UMI_STATUS_OK);
            CHECK(out.selectedRows==selected && out.topOneCorrect==one && out.topKCorrect==top);
        }
    } else if(!strcmp(mode,"maximum")) {
        CHECK(UmiAiTrainingPredictionScore(largeScores,UMI_AI_TRAINING_MAX_ELEMENTS,1,largeTargets,NULL,0,1,&out)==UMI_STATUS_OK);
        CHECK(out.topOneCorrect==UMI_AI_TRAINING_MAX_ELEMENTS && out.selectedRows==UMI_AI_TRAINING_MAX_ELEMENTS);
        size_t last=UMI_AI_TRAINING_MAX_ELEMENTS-1U;
        CHECK(UmiAiTrainingPredictionScore(largeScores,1,UMI_AI_TRAINING_MAX_ELEMENTS,&last,NULL,0,UMI_AI_TRAINING_MAX_ELEMENTS,&out)==UMI_STATUS_OK);
        CHECK(out.topOneCorrect==0 && out.topKCorrect==1);
    } else if(!strcmp(mode,"arguments")) {
        CHECK(Refused(NULL,4,4,targets,NULL,0,2,UMI_STATUS_INVALID_ARGUMENT));
        CHECK(Refused(scores,4,4,NULL,NULL,0,2,UMI_STATUS_INVALID_ARGUMENT));
        CHECK(Refused(scores,0,4,targets,NULL,0,2,UMI_STATUS_INVALID_ARGUMENT));
        CHECK(Refused(scores,4,0,targets,NULL,0,1,UMI_STATUS_INVALID_ARGUMENT));
        CHECK(Refused(scores,4,4,targets,NULL,0,0,UMI_STATUS_INVALID_ARGUMENT));
        CHECK(Refused(scores,4,4,targets,NULL,0,5,UMI_STATUS_INVALID_ARGUMENT));
        CHECK(UmiAiTrainingPredictionScore(scores,4,4,targets,NULL,0,2,NULL)==UMI_STATUS_INVALID_ARGUMENT);
    } else if(!strcmp(mode,"capacity")) {
        CHECK(Refused(scores,SIZE_MAX,2,targets,NULL,0,1,UMI_STATUS_CAPACITY_EXCEEDED));
        CHECK(Refused(scores,1,SIZE_MAX,targets,NULL,0,1,UMI_STATUS_CAPACITY_EXCEEDED));
        CHECK(Refused(scores,UMI_AI_TRAINING_MAX_ELEMENTS/4U+1U,4,targets,NULL,0,1,UMI_STATUS_CAPACITY_EXCEEDED));
    } else if(!strcmp(mode,"selection-refusal")) {
        uint8_t chosen[]={1,1,1,2};
        CHECK(Refused(scores,4,4,targets,NULL,4,2,UMI_STATUS_INVALID_ARGUMENT));
        CHECK(Refused(scores,4,4,targets,chosen,3,2,UMI_STATUS_INVALID_ARGUMENT));
        CHECK(Refused(scores,4,4,targets,chosen,4,2,UMI_STATUS_INVALID_ARGUMENT));
        chosen[3]=255;CHECK(Refused(scores,4,4,targets,chosen,4,2,UMI_STATUS_INVALID_ARGUMENT));
    } else if(!strcmp(mode,"nonfinite")) {
        uint8_t chosen[]={1,1,1,0};float values[16];
        for(size_t i=0;i<16;++i)for(unsigned which=0;which<3;++which) {
            memcpy(values,scores,sizeof values);values[i]=which==0U?NAN:(which==1U?INFINITY:-INFINITY);
            CHECK(Refused(values,4,4,targets,chosen,4,2,UMI_STATUS_INVALID_ARGUMENT));
        }
    } else if(!strcmp(mode,"target-refusal")) {
        size_t truth[4];uint8_t chosen[]={0,0,0,0};
        for(size_t row=0;row<4;++row) {
            memcpy(truth,targets,sizeof truth);truth[row]=4;
            CHECK(Refused(scores,4,4,truth,chosen,4,2,UMI_STATUS_INVALID_ARGUMENT));
        }
    } else return 2;
    return 0;
}

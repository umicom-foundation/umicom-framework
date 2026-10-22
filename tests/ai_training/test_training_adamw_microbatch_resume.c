/*-----------------------------------------------------------------------------
 * Umicom Framework | Native AI regression
 * PURPOSE: Prove microbatched AdamW experiments resume with byte-identical
 * weights AND optimiser moments under an unchanged absolute-step schedule.
 * AUTHOR AND ORGANISATION: Sammy Hegab | Umicom Foundation | MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/experiment_adamw.h"
#include "umicom/ai/training/optimizer_checkpoint.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef UMI_AI_TEST_MANIFEST_PATH
#error UMI_AI_TEST_MANIFEST_PATH required
#endif
#define CHECK(x) do {if(!(x)){fprintf(stderr,"line %d: %s\n",__LINE__,#x);return 1;}} while(0)
static int CheckIdentity(const UmiAiTrainingDecoder *a,
                         const UmiAiTrainingAdamW *ao,
                         const UmiAiTrainingDecoder *b,
                         const UmiAiTrainingAdamW *bo)
{
    size_t sa=0U,sb=0U,wa=0U,wb=0U;
    uint8_t *ab=NULL,*bb=NULL;
    CHECK(UmiAiTrainingAdamWCheckpointSize(a,ao,&sa)==UMI_STATUS_OK);
    CHECK(UmiAiTrainingAdamWCheckpointSize(b,bo,&sb)==UMI_STATUS_OK && sa==sb);
    ab=malloc(sa);bb=malloc(sb);CHECK(ab!=NULL && bb!=NULL);
    CHECK(UmiAiTrainingAdamWCheckpointEncode(a,ao,ab,sa,&wa)==UMI_STATUS_OK);
    CHECK(UmiAiTrainingAdamWCheckpointEncode(b,bo,bb,sb,&wb)==UMI_STATUS_OK);
    CHECK(wa==wb && wa==sa && memcmp(ab,bb,sa)==0);
    free(ab);free(bb);return 0;
}
int main(void)
{
    UmiAiTrainingDataset *dataset=NULL;
    UmiAiTrainingDecoder *full=NULL,*partial=NULL,*resumed=NULL;
    UmiAiTrainingAdamW *fo=NULL,*po=NULL,*ro=NULL;
    UmiAiTrainingExperimentConfig config,intermediate;
    UmiAiTrainingAdamWConfig options;
    UmiAiTrainingLearningRateSchedule schedule;
    UmiAiTrainingExperimentResult fr,pr,rr;
    uint64_t completed=0U;
    size_t stateBytes=0U;
    CHECK(UmiAiTrainingExperimentDefaults(&config)==UMI_STATUS_OK);
    CHECK(UmiAiTrainingAdamWDefaults(&options)==UMI_STATUS_OK);
    CHECK(UmiAiTrainingDatasetLoadManifest(UMI_AI_TEST_MANIFEST_PATH,&dataset)==UMI_STATUS_OK);
    config.steps=6U;config.evalInterval=2U;config.evalBatches=1U;
    config.batchSequences=4U;config.learningRate=0.016F;
    schedule=(UmiAiTrainingLearningRateSchedule){UMI_AI_TRAINING_SCHEDULE_COSINE,0.016F,0.003F,2U,6U};
    CHECK(UmiAiTrainingExperimentRunAdamWAccumulated(dataset,&config,&options,&schedule,
        2U,NULL,NULL,&full,&fo,&fr)==UMI_STATUS_OK);
    intermediate=config;intermediate.steps=2U;
    CHECK(UmiAiTrainingExperimentRunAdamWAccumulated(dataset,&intermediate,&options,&schedule,
        2U,NULL,NULL,&partial,&po,&pr)==UMI_STATUS_OK);
    CHECK(UmiAiTrainingExperimentResumeAdamWAccumulated(dataset,&config,&options,&schedule,
        partial,po,2U,NULL,NULL,&resumed,&ro,&rr)==UMI_STATUS_OK);
    CHECK(fr.completedSteps==6U && pr.completedSteps==2U && rr.completedSteps==6U);
    CHECK(CheckIdentity(full,fo,resumed,ro)==0);
    CHECK(UmiAiTrainingAdamWInspect(po,&completed,&stateBytes)==UMI_STATUS_OK && completed==2U);
    UmiAiTrainingAdamWDestroy(ro);UmiAiTrainingAdamWDestroy(po);UmiAiTrainingAdamWDestroy(fo);
    UmiAiTrainingDecoderDestroy(resumed);UmiAiTrainingDecoderDestroy(partial);UmiAiTrainingDecoderDestroy(full);
    UmiAiTrainingDatasetDestroy(dataset);
    puts("microbatch AdamW experiment: exact combined checkpoint after resume PASS");
    return 0;
}

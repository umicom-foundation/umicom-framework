/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/training_decoder_mask/test_decoder_adamw_selection.c
 *
 * PURPOSE:
 *   Verify selected AdamW loss, complete checkpoint invariance, refusal rollback and graph lifetimes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/ai/training/optimizer.h"
#include "umicom/ai/training/optimizer_checkpoint.h"
#include "../../src/ai/training/decoder_private.h"
#include "../../src/ai/training/optimizer_internal.h"
#include <float.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr,"%d: %s\n",__LINE__,#x); return 1; } } while (0)
#define OK(x) CHECK((x)==UMI_STATUS_OK)
static const UmiAiTrainingDecoderConfig config={8,3,4,8,2,42};
static const size_t ids[6]={1,2,3,4,5,6}, targets[6]={2,3,4,5,6,7};
static const unsigned char attended[6]={1,0,1,0,1,1}, scored[6]={0,0,1,0,0,1};
static const unsigned char all[6]={1,1,1,1,1,1};
typedef struct Pair { UmiAiTrainingDecoder *model; UmiAiTrainingAdamW *optimizer; } Pair;
typedef struct Snapshot { unsigned char bytes[65536]; size_t count; } Snapshot;
static int create(Pair *p,float decay) {
    UmiAiTrainingAdamWConfig options;
    *p=(Pair){0}; OK(UmiAiTrainingAdamWDefaults(&options)); options.weightDecay=decay;
    OK(UmiAiTrainingDecoderCreate(&config,&p->model));
    OK(UmiAiTrainingDecoderAdamWCreate(p->model,&options,&p->optimizer)); return 0;
}
static void destroy(Pair *p) {
    UmiAiTrainingAdamWDestroy(p->optimizer); UmiAiTrainingDecoderDestroy(p->model); *p=(Pair){0};
}
static int save(const Pair *p,Snapshot *s) {
    OK(UmiAiTrainingAdamWCheckpointEncode(p->model,p->optimizer,s->bytes,sizeof s->bytes,&s->count)); return 0;
}
static int same(const Pair *p,const Snapshot *s) {
    Snapshot after; CHECK(save(p,&after)==0);
    CHECK(after.count==s->count && !memcmp(after.bytes,s->bytes,after.count)); return 0;
}
static int stable(const Pair *p,const UmiAiTrainingTapeMemory *before) {
    UmiAiTrainingTapeMemory after; OK(UmiAiTrainingDecoderMemoryInspect(p->model,&after));
    CHECK(after.currentBytes==before->currentBytes && after.auxiliaryBytes==before->auxiliaryBytes &&
        after.tensorCount==before->tensorCount); return 0;
}
static UmiStatus step(Pair *p,const size_t *input,const size_t *target,
    const unsigned char *attention,const unsigned char *selected,float *loss) {
    return UmiAiTrainingDecoderTrainAdamWPaddedStep(p->model,p->optimizer,
        input,target,6,attention,6,selected,6,0.02F,loss);
}
/* Compare complete model-and-moment state, not merely the printed scalar loss. */
static int parity(int objectiveOnly) {
    Pair a,b; Snapshot state; float la,lb;
    CHECK(create(&a,0.01F)==0 && create(&b,0.01F)==0);
    for(size_t i=0;i<12;++i) {
        if(objectiveOnly) {
            OK(UmiAiTrainingDecoderTrainAdamWMaskedStep(a.model,a.optimizer,ids,targets,6,scored,6,0.02F,&la));
            OK(step(&b,ids,targets,all,scored,&lb));
        } else {
            OK(UmiAiTrainingDecoderTrainAdamWStep(a.model,a.optimizer,ids,targets,6,0.02F,&la));
            OK(UmiAiTrainingDecoderTrainAdamWMaskedStep(b.model,b.optimizer,ids,targets,6,all,6,0.02F,&lb));
        }
        CHECK(la==lb && save(&a,&state)==0 && same(&b,&state)==0);
    }
    destroy(&b); destroy(&a); return 0;
}
static int reference(void) {
    Pair p; float expected,observed; CHECK(create(&p,0.01F)==0);
    for(size_t i=0;i<10;++i) {
        OK(UmiAiTrainingDecoderEvaluatePadded(p.model,ids,targets,6,attended,6,scored,6,&expected));
        OK(step(&p,ids,targets,attended,scored,&observed));
        CHECK(expected==observed && p.model->completedSteps==i+1);
    }
    destroy(&p); return 0;
}
static int invariant(int objectiveOnly) {
    Pair a,b; Snapshot state; float la,lb;
    const size_t otherIds[6]={1,7,3,0,5,6}, otherTargets[6]={7,0,4,7,1,7};
    CHECK(create(&a,0.01F)==0 && create(&b,0.01F)==0);
    for(size_t i=0;i<12;++i) {
        if(objectiveOnly) {
            OK(UmiAiTrainingDecoderTrainAdamWMaskedStep(a.model,a.optimizer,ids,targets,6,scored,6,0.02F,&la));
            OK(UmiAiTrainingDecoderTrainAdamWMaskedStep(b.model,b.optimizer,ids,otherTargets,6,scored,6,0.02F,&lb));
        } else {
            OK(step(&a,ids,targets,attended,scored,&la));
            OK(step(&b,otherIds,otherTargets,attended,scored,&lb));
        }
        CHECK(la==lb && save(&a,&state)==0 && same(&b,&state)==0);
    }
    destroy(&b); destroy(&a); return 0;
}
static int rejections(void) {
    Pair p; Snapshot state; UmiAiTrainingTapeMemory before; float loss=-77;
    const unsigned char invalid[6]={1,0,1,0,2,1}, empty[6]={0};
    size_t badIds[6]; memcpy(badIds,ids,sizeof ids); badIds[1]=8;
    CHECK(create(&p,0.01F)==0 && save(&p,&state)==0);
    OK(UmiAiTrainingDecoderMemoryInspect(p.model,&before));
    CHECK(step(&p,ids,targets,invalid,scored,&loss)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(step(&p,ids,targets,attended,invalid,&loss)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(step(&p,ids,targets,attended,empty,&loss)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(step(&p,ids,targets,attended,all,&loss)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(step(&p,ids,targets,empty,scored,&loss)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(step(&p,ids,targets,NULL,scored,&loss)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(step(&p,ids,targets,attended,NULL,&loss)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(step(&p,badIds,targets,attended,scored,&loss)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(step(&p,ids,badIds,attended,scored,&loss)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(step(&p,NULL,targets,attended,scored,&loss)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(step(&p,ids,NULL,attended,scored,&loss)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(step(&p,ids,targets,attended,scored,NULL)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingDecoderTrainAdamWPaddedStep(p.model,p.optimizer,ids,targets,6,attended,5,scored,6,0.02F,&loss)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingDecoderTrainAdamWPaddedStep(p.model,p.optimizer,ids,targets,6,attended,6,scored,5,0.02F,&loss)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingDecoderTrainAdamWPaddedStep(p.model,p.optimizer,ids,targets,5,attended,5,scored,5,0.02F,&loss)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingDecoderTrainAdamWMaskedStep(p.model,p.optimizer,ids,targets,6,NULL,6,0.02F,&loss)==UMI_STATUS_INVALID_ARGUMENT);
    const float rates[]={0,-1,NAN,INFINITY};
    for(size_t i=0;i<sizeof rates/sizeof rates[0];++i)
        CHECK(UmiAiTrainingDecoderTrainAdamWPaddedStep(p.model,p.optimizer,ids,targets,6,attended,6,scored,6,rates[i],&loss)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(loss==-77 && same(&p,&state)==0 && stable(&p,&before)==0);
    OK(step(&p,ids,targets,attended,scored,&loss)); destroy(&p); return 0;
}
static int binding(void) {
    Pair a,b; Snapshot initialA,initialB; float loss=-77;
    CHECK(create(&a,0.01F)==0 && create(&b,0.01F)==0);
    CHECK(save(&a,&initialA)==0 && save(&b,&initialB)==0);
    CHECK(UmiAiTrainingDecoderTrainAdamWPaddedStep(a.model,b.optimizer,ids,targets,6,attended,6,scored,6,0.02F,&loss)==UMI_STATUS_INVALID_STATE);
    CHECK(loss==-77 && same(&a,&initialA)==0 && same(&b,&initialB)==0);
    CHECK(UmiAiTrainingDecoderTrainAdamWPaddedStep(NULL,a.optimizer,ids,targets,6,attended,6,scored,6,0.02F,&loss)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingDecoderTrainAdamWPaddedStep(a.model,NULL,ids,targets,6,attended,6,scored,6,0.02F,&loss)==UMI_STATUS_INVALID_ARGUMENT);
    a.model->completedSteps=1; /* Simulate a caller updating weights through another path. */
    CHECK(step(&a,ids,targets,attended,scored,&loss)==UMI_STATUS_INVALID_STATE);
    a.model->completedSteps=0;
    CHECK(loss==-77 && same(&a,&initialA)==0);
    destroy(&b); destroy(&a); return 0;
}
static int budget(void) {
    Pair p; Snapshot state; UmiAiTrainingTapeMemory before,measured; float loss=-77;
    CHECK(create(&p,0.01F)==0 && save(&p,&state)==0);
    OK(UmiAiTrainingDecoderMemoryInspect(p.model,&before));
    OK(UmiAiTrainingDecoderEvaluatePadded(p.model,ids,targets,6,attended,6,scored,6,&loss));
    OK(UmiAiTrainingDecoderMemoryInspect(p.model,&measured));
    const size_t limits[]={before.currentBytes,before.currentBytes+1,
        before.currentBytes+(measured.peakBytes-before.currentBytes)/2,measured.peakBytes-1};
    for(size_t i=0;i<sizeof limits/sizeof limits[0];++i) {
        OK(UmiAiTrainingDecoderSetMemoryBudget(p.model,limits[i])); loss=-77;
        CHECK(step(&p,ids,targets,attended,scored,&loss)==UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(loss==-77 && same(&p,&state)==0 && stable(&p,&before)==0);
    }
    OK(UmiAiTrainingDecoderSetMemoryBudget(p.model,0));
    OK(step(&p,ids,targets,attended,scored,&loss));
    CHECK(stable(&p,&before)==0); destroy(&p); return 0;
}
static int repeat(void) {
    Pair p; UmiAiTrainingTapeMemory before; float first,last,loss; uint64_t steps;
    size_t stateBytes,afterBytes; CHECK(create(&p,0.01F)==0);
    OK(UmiAiTrainingDecoderMemoryInspect(p.model,&before));
    OK(UmiAiTrainingAdamWInspect(p.optimizer,&steps,&stateBytes));
    OK(UmiAiTrainingDecoderEvaluatePadded(p.model,ids,targets,6,attended,6,scored,6,&first));
    for(size_t i=0;i<100;++i) {
        OK(step(&p,ids,targets,attended,scored,&loss)); CHECK(stable(&p,&before)==0);
    }
    OK(UmiAiTrainingDecoderEvaluatePadded(p.model,ids,targets,6,attended,6,scored,6,&last));
    OK(UmiAiTrainingAdamWInspect(p.optimizer,&steps,&afterBytes));
    CHECK(last<first && steps==100 && p.model->completedSteps==100 && stateBytes==afterBytes);
    destroy(&p); return 0;
}
static int resume(void) {
    Pair a,b={0}; Snapshot state; float la,lb; CHECK(create(&a,0.01F)==0);
    for(size_t i=0;i<3;++i) OK(step(&a,ids,targets,attended,scored,&la));
    CHECK(save(&a,&state)==0);
    OK(UmiAiTrainingAdamWCheckpointDecode(state.bytes,state.count,&b.model,&b.optimizer));
    for(size_t i=0;i<12;++i) {
        OK(step(&a,ids,targets,attended,scored,&la)); OK(step(&b,ids,targets,attended,scored,&lb));
        CHECK(la==lb && save(&a,&state)==0 && same(&b,&state)==0);
    }
    destroy(&b); destroy(&a); return 0;
}
static int updateFailure(void) {
    Pair p; Snapshot state; UmiAiTrainingTapeMemory before; float loss=-77;
    CHECK(create(&p,FLT_MAX)==0 && save(&p,&state)==0);
    OK(UmiAiTrainingDecoderMemoryInspect(p.model,&before));
    CHECK(UmiAiTrainingDecoderTrainAdamWPaddedStep(p.model,p.optimizer,ids,targets,6,attended,6,scored,6,FLT_MAX,&loss)!=UMI_STATUS_OK);
    CHECK(loss==-77 && same(&p,&state)==0 && stable(&p,&before)==0);
    destroy(&p); return 0;
}
static int overflow(void) {
    Pair p; Snapshot state; float loss=-77; UmiAiTrainingAdamWConfig options;
    uint64_t steps; size_t parameterCount,elements; const size_t *counts; const double *first,*second;
    CHECK(create(&p,0.01F)==0);
    OK(UmiAiTrainingAdamWInternalInspect(p.optimizer,&options,&steps,&parameterCount,&elements,&counts,&first,&second));
    OK(UmiAiTrainingAdamWInternalRestore(p.optimizer,first,second,elements,UINT64_MAX));
    p.model->completedSteps=UINT64_MAX; CHECK(save(&p,&state)==0);
    CHECK(step(&p,ids,targets,attended,scored,&loss)==UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(loss==-77 && same(&p,&state)==0); destroy(&p); return 0;
}
int main(int argc,char **argv) {
    if(argc!=2) return 2;
    if(!strcmp(argv[1],"legacy-parity")) return parity(0);
    if(!strcmp(argv[1],"selection-parity")) return parity(1);
    if(!strcmp(argv[1],"reference")) return reference();
    if(!strcmp(argv[1],"target-invariance")) return invariant(1);
    if(!strcmp(argv[1],"padding-invariance")) return invariant(0);
    if(!strcmp(argv[1],"rejections")) return rejections();
    if(!strcmp(argv[1],"binding")) return binding();
    if(!strcmp(argv[1],"budget")) return budget();
    if(!strcmp(argv[1],"repeat")) return repeat();
    if(!strcmp(argv[1],"resume")) return resume();
    if(!strcmp(argv[1],"update-failure")) return updateFailure();
    if(!strcmp(argv[1],"step-overflow")) return overflow();
    return 2;
}

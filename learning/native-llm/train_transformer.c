/*-----------------------------------------------------------------------------
 * Umicom Framework | learning/native-llm/train_transformer.c
 *
 * PURPOSE:
 *   Teach one native pre-LN transformer block with two causal attention heads,
 *   two residual paths, trainable layer normalisation and feed-forward layers.
 *   Token embeddings and next-token logits are trained with cross-entropy.
 *
 * LIMITATION:
 *   Four examples from two memorisable sequences are NOT held-out evaluation
 *   and do not demonstrate generalisation. No checkpoint, positional embedding,
 *   model stacking, inference sampler or reliable coding assistant is claimed.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/tape.h"
#include "umicom/ai/training/initialisation.h"
#include "umicom/ai/training/transformer.h"

#include <math.h>
#include <stdio.h>

enum {
    VOCAB=4, CHANNELS=4, HIDDEN=8, ROWS=4,
    P_EMBEDDING = 0, P_LN_1_SCALE, P_LN_1_BIAS,
    P_QUERY, P_KEY, P_VALUE, P_PROJECTION,
    P_LN_2_SCALE, P_LN_2_BIAS,
    P_UP, P_UP_BIAS, P_DOWN, P_DOWN_BIAS, P_LOGITS,
    PARAMETER_COUNT
};

#define REQUIRE(call) do { \
    UmiStatus result = (call); \
    if (result != UMI_STATUS_OK) { \
        fprintf(stderr, "transformer demo failure line %d: %s (%d)\n", \
                __LINE__, #call, (int)result); \
        UmiAiTrainingTapeDestroy(tape); return 1; \
    } \
} while (0)

static size_t ParameterRows(size_t index)
{
    if (index==P_LN_1_SCALE || index==P_LN_1_BIAS || index==P_LN_2_SCALE ||
        index==P_LN_2_BIAS || index==P_UP_BIAS || index==P_DOWN_BIAS) return 1U;
    if (index==P_DOWN) return HIDDEN;
    return CHANNELS;
}
static size_t ParameterColumns(size_t index)
{
    if(index==P_UP || index==P_UP_BIAS) return HIDDEN;
    return CHANNELS;
}

int main(void)
{
    static const size_t tokens[ROWS] = {0U,1U,2U,1U};
    static const size_t targets[ROWS] = {1U,3U,1U,0U};
    static const char *const names[VOCAB] = {
        "framework", "learns", "calypso", "C23"
    };
    UmiAiTrainingTape *tape=NULL;
    UmiAiTrainingTensor *parameters[PARAMETER_COUNT]={NULL};
    UmiAiTrainingRandomState rng;
    UmiAiTrainingTransformerBlockParameters block;
    const UmiAiTrainingTransformerBlockConfig config={2U,2U,1e-4F};
    float first=0.0F,last=0.0F;
    REQUIRE(UmiAiTrainingTapeCreate(&tape));
    REQUIRE(UmiAiTrainingRandomSeed(&rng,0x5A77U));
    for(size_t p=0U;p<PARAMETER_COUNT;++p){
        size_t rows=ParameterRows(p);
        size_t cols=ParameterColumns(p);
        size_t count=rows*cols;
        float values[CHANNELS*HIDDEN]={0};
        if(p==P_LN_1_SCALE || p==P_LN_2_SCALE){
            for(size_t i=0U;i<count;++i)values[i]=1.0F;
        } else if(p!=P_LN_1_BIAS && p!=P_LN_2_BIAS &&
                  p!=P_UP_BIAS && p!=P_DOWN_BIAS){
            REQUIRE(UmiAiTrainingRandomFillUniform(&rng,-0.35F,0.35F,
                                                    values,count));
        }
        REQUIRE(UmiAiTrainingTensorCreate(tape,rows,cols,values,count,1,
                                          &parameters[p]));
    }
    block=(UmiAiTrainingTransformerBlockParameters){
        parameters[P_LN_1_SCALE],parameters[P_LN_1_BIAS],
        parameters[P_QUERY],parameters[P_KEY],parameters[P_VALUE],
        parameters[P_PROJECTION],
        parameters[P_LN_2_SCALE],parameters[P_LN_2_BIAS],
        parameters[P_UP],parameters[P_UP_BIAS],
        parameters[P_DOWN],parameters[P_DOWN_BIAS]
    };

    puts("Umicom native C23 decoder block: 2-head causal attention, 2 residual paths");
    puts("Training from seeded parameters. The data are memorisable, not held-out.\n");
    for(size_t epoch=0U;epoch<1200U;++epoch){
        UmiAiTrainingTensor *embedded=NULL,*state=NULL,*logits=NULL,*loss=NULL;
        REQUIRE(UmiAiTrainingEmbeddingLookup(tape,parameters[P_EMBEDDING],
                                              tokens,ROWS,&embedded));
        REQUIRE(UmiAiTrainingTransformerBlockForward(tape,embedded,&block,
                                                       &config,&state));
        REQUIRE(UmiAiTrainingMatmul(tape,state,parameters[P_LOGITS],&logits));
        REQUIRE(UmiAiTrainingCrossEntropy(tape,logits,targets,ROWS,&loss));
        REQUIRE(UmiAiTrainingTensorValues(loss,&last,1U));
        if(epoch==0U)first=last;
        if(epoch==0U || epoch==49U || epoch==199U || epoch==499U || epoch==1199U)
            printf("epoch %4zu: training cross-entropy %.6f\n",epoch+1U,(double)last);
        REQUIRE(UmiAiTrainingBackward(tape,loss));
        for(size_t p=0U;p<PARAMETER_COUNT;++p)
            REQUIRE(UmiAiTrainingSgdStep(tape,parameters[p],0.20F));
        REQUIRE(UmiAiTrainingTapeRewind(tape,PARAMETER_COUNT));
    }
    printf("\nCorpus: '%s %s' => '%s' and '%s %s' => '%s'\n",
           names[0],names[1],names[3],names[2],names[1],names[0]);
    printf("loss %.6f -> %.6f\n",(double)first,(double)last);
    UmiAiTrainingTapeDestroy(tape);
    if(!isfinite(last) || !(last<first*0.25F)){
        fputs("The synthetic corpus did not converge; inspect the training contract.\n",stderr);
        return 1;
    }
    puts("Training improved on seen data; no generalisation claim.");
    return 0;
}

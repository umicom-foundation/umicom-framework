/*-----------------------------------------------------------------------------
 * Umicom Framework | tests/ai_training/transformer_fixture.h
 * PURPOSE: Shared fixed-size deterministic C23 test fixture for real Framework
 *          Transformer operators; not an application implementation.
 * AUTHOR: Sammy Hegab / Umicom Foundation | LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_TRAINING_TRANSFORMER_TEST_FIXTURE_H
#define UMICOM_AI_TRAINING_TRANSFORMER_TEST_FIXTURE_H

#include "umicom/ai/training/transformer.h"
#include <stddef.h>

/* Indices are deliberately explicit so gradient tests identify each weight. */
enum {
    T_NORM_SCALE_1, T_NORM_BIAS_1, T_QUERY, T_KEY, T_VALUE, T_OUTPUT,
    T_NORM_SCALE_2, T_NORM_BIAS_2, T_FF_UP, T_FF_UP_BIAS,
    T_FF_DOWN, T_FF_DOWN_BIAS, T_PARAMETER_COUNT
};
enum {T_ROW_COUNT=4, T_CHANNELS=4, T_HIDDEN=6, T_MAX_VALUES=24};

typedef struct UmiTransformerTestData {
    float input[16];
    float values[T_PARAMETER_COUNT][T_MAX_VALUES];
} UmiTransformerTestData;

typedef struct UmiTransformerTestFixture {
    UmiAiTrainingTape *tape;
    UmiAiTrainingTensor *input;
    UmiAiTrainingTensor *parameters[T_PARAMETER_COUNT];
    UmiAiTrainingTransformerBlockParameters block;
    UmiAiTrainingTransformerBlockConfig config;
} UmiTransformerTestFixture;

static inline size_t TestRows(size_t index)
{
    switch(index){
    case T_NORM_SCALE_1:case T_NORM_BIAS_1:
    case T_NORM_SCALE_2:case T_NORM_BIAS_2:
    case T_FF_UP_BIAS:case T_FF_DOWN_BIAS: return 1U;
    case T_FF_DOWN: return T_HIDDEN;
    default: return T_CHANNELS;
    }
}
static inline size_t TestColumns(size_t index)
{
    switch(index){
    case T_FF_UP:case T_FF_UP_BIAS: return T_HIDDEN;
    case T_FF_DOWN: return T_CHANNELS;
    default: return T_CHANNELS;
    }
}
static inline void InitialiseTestData(UmiTransformerTestData *data)
{
    static const float starting[16] = {
        0.1F,-0.2F,0.5F,0.7F,
        -0.3F,0.2F,0.1F,0.8F,
        0.5F,-0.4F,0.3F,-0.1F,
        -0.2F,0.6F,-0.1F,0.4F
    };
    for(size_t j=0U;j<16U;++j)data->input[j]=starting[j];
    for(size_t parameter=0U;parameter<T_PARAMETER_COUNT;++parameter){
        size_t count=TestRows(parameter)*TestColumns(parameter);
        for(size_t index=0U;index<count;++index){
            float value=(float)((int)((parameter*17U+index*13U)%31U)-15)*0.027F;
            if(parameter==T_NORM_SCALE_1 || parameter==T_NORM_SCALE_2)
                value=1.0F + value*0.13F;
            if(parameter==T_NORM_BIAS_1 || parameter==T_NORM_BIAS_2 ||
               parameter==T_FF_UP_BIAS || parameter==T_FF_DOWN_BIAS)
                value*=0.13F;
            data->values[parameter][index]=value;
        }
    }
}
static inline UmiStatus CreateTestFixture(UmiTransformerTestFixture *f,
                                         const UmiTransformerTestData *data)
{
    UmiStatus s;
    if(f==NULL || data==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    f->tape=NULL;f->input=NULL;
    for(size_t i=0U;i<T_PARAMETER_COUNT;++i)f->parameters[i]=NULL;
    s=UmiAiTrainingTapeCreate(&f->tape);
    if(s!=UMI_STATUS_OK)return s;
    s=UmiAiTrainingTensorCreate(f->tape,T_ROW_COUNT,T_CHANNELS,
                                data->input,16U,1,&f->input);
    for(size_t i=0U;i<T_PARAMETER_COUNT && s==UMI_STATUS_OK;++i){
        s=UmiAiTrainingTensorCreate(f->tape,TestRows(i),TestColumns(i),
                                    data->values[i],TestRows(i)*TestColumns(i),
                                    1,&f->parameters[i]);
    }
    if(s!=UMI_STATUS_OK){UmiAiTrainingTapeDestroy(f->tape);f->tape=NULL;return s;}
    f->block=(UmiAiTrainingTransformerBlockParameters){
        f->parameters[T_NORM_SCALE_1],f->parameters[T_NORM_BIAS_1],
        f->parameters[T_QUERY],f->parameters[T_KEY],
        f->parameters[T_VALUE],f->parameters[T_OUTPUT],
        f->parameters[T_NORM_SCALE_2],f->parameters[T_NORM_BIAS_2],
        f->parameters[T_FF_UP],f->parameters[T_FF_UP_BIAS],
        f->parameters[T_FF_DOWN],f->parameters[T_FF_DOWN_BIAS]
    };
    f->config=(UmiAiTrainingTransformerBlockConfig){2U,2U,1e-4F};
    return UMI_STATUS_OK;
}
static inline void FreeTestFixture(UmiTransformerTestFixture *f)
{
    if(f!=NULL){UmiAiTrainingTapeDestroy(f->tape);f->tape=NULL;}
}
#endif

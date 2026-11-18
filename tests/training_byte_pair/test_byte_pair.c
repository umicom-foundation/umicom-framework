/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/training_byte_pair/test_byte_pair.c
 *
 * PURPOSE:
 *   Check exact shifted byte tokens, independent masks, bounded refusal and copied example ownership.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/ai/training/byte_pair_sequence.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#define CHECK(c) do { if(!(c)) { fprintf(stderr,"line %d: %s\n",__LINE__,#c); return 1; } } while(0)
static int golden(void)
{
    UmiAiTrainingBytePairSequence out;
    const uint8_t prompt[]={'A'}, completion[]={'B','C'};
    const size_t input[]={1,69,70,71,0,0}, target[]={69,70,71,2,0,0};
    const unsigned char attention[]={1,1,1,1,0,0}, selected[]={0,1,1,1,0,0};
    CHECK(UmiAiTrainingBytePairPrepare(prompt,1,completion,2,6,&out)==UMI_STATUS_OK);
    CHECK(out.context_length==6 && out.valid_rows==4 && out.selected_rows==3);
    CHECK(memcmp(out.inputs,input,sizeof input)==0 && memcmp(out.targets,target,sizeof target)==0);
    CHECK(memcmp(out.attention,attention,sizeof attention)==0 && memcmp(out.selected,selected,sizeof selected)==0);
    return 0;
}
static int empty(void)
{
    UmiAiTrainingBytePairSequence out; const uint8_t data=42;
    CHECK(UmiAiTrainingBytePairPrepare(NULL,0,&data,1,2,&out)==UMI_STATUS_OK);
    CHECK(out.inputs[0]==1 && out.inputs[1]==46 && out.targets[0]==46 && out.targets[1]==2);
    CHECK(out.valid_rows==2 && out.selected_rows==2 && out.attention[0]==1 && out.attention[1]==1);
    CHECK(out.selected[0]==1 && out.selected[1]==1);
    return 0;
}
static int binary(void)
{
    UmiAiTrainingBytePairSequence out;const uint8_t p[]={0,255},c[]={13,10};
    CHECK(UmiAiTrainingBytePairPrepare(p,2,c,2,5,&out)==UMI_STATUS_OK);
    const size_t in[]={1,4,259,17,14},targets[]={4,259,17,14,2};
    CHECK(memcmp(out.inputs,in,sizeof in)==0 && memcmp(out.targets,targets,sizeof targets)==0);
    CHECK(out.selected[0]==0 && out.selected[1]==0 && out.selected[2]==1 && out.selected[4]==1);
    return 0;
}
static int padding(void)
{
    UmiAiTrainingBytePairSequence out;const uint8_t byte=1;
    memset(&out,0xa5,sizeof out);
    CHECK(UmiAiTrainingBytePairPrepare(NULL,0,&byte,1,UMI_AI_DECODER_MAX_CONTEXT,&out)==UMI_STATUS_OK);
    CHECK(out.context_length==32 && out.valid_rows==2);
    for(size_t i=2;i<UMI_AI_DECODER_MAX_CONTEXT;++i)
        CHECK(out.inputs[i]==0 && out.targets[i]==0 && out.attention[i]==0 && out.selected[i]==0);
    memset(&out,0xa5,sizeof out);
    CHECK(UmiAiTrainingBytePairPrepare(NULL,0,&byte,1,2,&out)==UMI_STATUS_OK);
    for(size_t i=2;i<UMI_AI_DECODER_MAX_CONTEXT;++i)
        CHECK(out.inputs[i]==0 && out.targets[i]==0 && out.attention[i]==0 && out.selected[i]==0);
    return 0;
}
static int limits(void)
{
    UmiAiTrainingBytePairSequence out;uint8_t data[32];memset(data,7,sizeof data);
    CHECK(UmiAiTrainingBytePairPrepare(data,30,data,1,32,&out)==UMI_STATUS_OK);
    CHECK(out.valid_rows==32 && out.selected_rows==2 && out.inputs[31]==11 && out.targets[31]==2);
    CHECK(out.selected[29]==0 && out.selected[30]==1 && out.selected[31]==1);
    CHECK(UmiAiTrainingBytePairPrepare(NULL,0,data,31,32,&out)==UMI_STATUS_OK && out.selected_rows==32);
    return 0;
}
static int nulls(void)
{
    UmiAiTrainingBytePairSequence out,saved;const uint8_t b=1;memset(&out,0xa5,sizeof out);saved=out;
    CHECK(UmiAiTrainingBytePairPrepare(&b,1,&b,1,3,NULL)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingBytePairPrepare(NULL,1,&b,1,3,&out)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingBytePairPrepare(&b,1,NULL,1,3,&out)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingBytePairPrepare(&b,1,NULL,0,3,&out)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingBytePairPrepare(&b,1,&b,0,3,&out)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingBytePairPrepare(&b,1,&b,1,0,&out)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingBytePairPrepare(&b,1,&b,1,33,&out)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(memcmp(&out,&saved,sizeof out)==0);
    return 0;
}
static int refusal(void)
{
    UmiAiTrainingBytePairSequence out,saved;const uint8_t b=1;memset(&out,0xa5,sizeof out);saved=out;
    CHECK(UmiAiTrainingBytePairPrepare(&b,31,&b,1,32,&out)==UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(UmiAiTrainingBytePairPrepare(&b,SIZE_MAX,&b,1,32,&out)==UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(UmiAiTrainingBytePairPrepare(&b,1,&b,SIZE_MAX,32,&out)==UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(UmiAiTrainingBytePairPrepare(NULL,0,&b,1,1,&out)==UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(memcmp(&out,&saved,sizeof out)==0);
    return 0;
}
static int ownership(void)
{
    UmiAiTrainingBytePairSequence out;uint8_t p[]={10},c[]={20};
    CHECK(UmiAiTrainingBytePairPrepare(p,1,c,1,3,&out)==UMI_STATUS_OK);
    p[0]=200;c[0]=201;
    CHECK(out.inputs[1]==14 && out.inputs[2]==24 && out.targets[1]==24);
    return 0;
}
static int grid(void)
{
    uint8_t p[32],c[32];for(size_t i=0;i<32;++i){p[i]=(uint8_t)(i+10);c[i]=(uint8_t)(200-i);}
    for(size_t pn=0;pn<31;++pn)for(size_t cn=1;cn<=31-pn;++cn) {
        size_t stream[33],length=0;stream[length++]=1;
        for(size_t i=0;i<pn;++i)stream[length++]=(size_t)p[i]+4;
        for(size_t i=0;i<cn;++i)stream[length++]=(size_t)c[i]+4;
        stream[length++]=2;
        for(size_t context=length-1;context<=32;++context) {
            UmiAiTrainingBytePairSequence out;
            CHECK(UmiAiTrainingBytePairPrepare(p,pn,c,cn,context,&out)==UMI_STATUS_OK);
            CHECK(out.valid_rows==length-1 && out.context_length==context && out.selected_rows==cn+1);
            size_t selected=0;
            for(size_t row=0;row<32;++row) {
                const int valid=row+1<length;
                CHECK(out.inputs[row]==(valid?stream[row]:0));
                CHECK(out.targets[row]==(valid?stream[row+1]:0));
                CHECK(out.attention[row]==(valid?1:0));
                const int contributes=valid && row+1>=pn+1;
                CHECK(out.selected[row]==(contributes?1:0));
                selected+=out.selected[row];
            }
            CHECK(selected==cn+1);
            UmiAiTrainingBytePairShape shape;
            CHECK(UmiAiTrainingBytePairMeasure(pn,cn,&shape)==UMI_STATUS_OK);
            CHECK(shape.validRows==out.valid_rows && shape.selectedRows==out.selected_rows);
        }
    }
    return 0;
}
static int vocabulary(void)
{
    for(unsigned i=0;i<256;++i) {
        uint8_t byte=(uint8_t)i;UmiAiTrainingBytePairSequence out;
        CHECK(UmiAiTrainingBytePairPrepare(NULL,0,&byte,1,2,&out)==UMI_STATUS_OK);
        CHECK(out.targets[0]==i+UMI_AI_BYTE_OFFSET && out.targets[0]<UMI_AI_BYTE_VOCABULARY);
        uint32_t token=(uint32_t)out.targets[0];uint8_t decoded=0;size_t written=0;
        CHECK(UmiAiByteDecode(&token,1,&decoded,1,&written)==UMI_STATUS_OK && written==1 && decoded==byte);
    }
    return 0;
}
int main(int argc,char **argv)
{
    if(argc!=2)return 2;
    struct Case {const char *name;int (*run)(void);} cases[]={
        {"golden",golden},{"empty",empty},{"binary",binary},{"padding",padding},{"limits",limits},
        {"nulls",nulls},{"refusal",refusal},{"ownership",ownership},{"grid",grid},{"vocabulary",vocabulary}};
    for(size_t i=0;i<sizeof cases/sizeof cases[0];++i)if(strcmp(argv[1],cases[i].name)==0)return cases[i].run();
    return 2;
}

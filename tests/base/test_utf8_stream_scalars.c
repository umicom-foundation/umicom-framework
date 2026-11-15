/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/base/test_utf8_stream_scalars.c
 *
 * PURPOSE:
 *   Check every Unicode scalar through byte-by-byte reconstruction and staged state copies.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/base/utf8_stream.h"
#include "umicom/base/utf8.h"
#include <stdio.h>
#include <string.h>
#define CHECK(c) do {if(!(c)){fprintf(stderr,"%d: scalar U+%06x: %s\n",__LINE__,(unsigned)scalar,#c);return 1;}}while(0)
int main(void)
{
    uint64_t scalars=0U,bytes=0U;
    UmiUtf8Stream aggregate={0};
    for(uint32_t scalar=0U;scalar<=0x10ffffU;++scalar){
        if(scalar>=0xd800U && scalar<=0xdfffU)continue;
        uint8_t encoded[4],output[4];size_t width;
        if(scalar<=0x7fU){encoded[0]=(uint8_t)scalar;width=1U;}
        else if(scalar<=0x7ffU){
            encoded[0]=(uint8_t)(0xc0U|(scalar>>6U));
            encoded[1]=(uint8_t)(0x80U|(scalar&63U));width=2U;
        }else if(scalar<=0xffffU){
            encoded[0]=(uint8_t)(0xe0U|(scalar>>12U));
            encoded[1]=(uint8_t)(0x80U|((scalar>>6U)&63U));
            encoded[2]=(uint8_t)(0x80U|(scalar&63U));width=3U;
        }else{
            encoded[0]=(uint8_t)(0xf0U|(scalar>>18U));
            encoded[1]=(uint8_t)(0x80U|((scalar>>12U)&63U));
            encoded[2]=(uint8_t)(0x80U|((scalar>>6U)&63U));
            encoded[3]=(uint8_t)(0x80U|(scalar&63U));width=4U;
        }
        /* Ordinary decoding checks the independently encoded test value.
         * Incremental publication must preserve those exact original bytes. */
        size_t used=0U;uint32_t decoded=UINT32_MAX;
        CHECK(UmiUtf8DecodeScalar(encoded,width,&used,&decoded));
        CHECK(used==width && decoded==scalar);
        for(size_t index=0U;index<width;++index){
            UmiUtf8Stream staged=aggregate;size_t produced=99U;
            memset(output,0xa5,sizeof output);
            CHECK(UmiUtf8StreamAppend(&staged,encoded[index],output,sizeof output,&produced)==UMI_STATUS_OK);
            if(index+1U==width)CHECK(produced==width && memcmp(output,encoded,width)==0);
            else CHECK(produced==0U && output[0]==0xa5U);
            aggregate=staged;
        }
        ++scalars;bytes+=(uint64_t)width;
        UmiUtf8StreamInfo info;
        CHECK(UmiUtf8StreamFinish(&aggregate)==UMI_STATUS_OK);
        CHECK(UmiUtf8StreamInspect(&aggregate,&info)==UMI_STATUS_OK);
        CHECK(info.completedScalars==scalars && info.completedBytes==bytes && info.bytesNeeded==0U);
    }
    printf("Reconstructed %llu Unicode scalars (%llu bytes).\n",
        (unsigned long long)scalars,(unsigned long long)bytes);
    return 0;
}

/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/model_formats/main.c
 *
 * PURPOSE:
 *   Demonstrate metadata-only model inspection through an independently installed Framework SDK.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/model_formats/safetensors.h"
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void)
{
    const char json[]="{\"example\":{\"dtype\":\"F32\",\"shape\":[2],\"data_offsets\":[0,8]}}";
    size_t header=strlen(json),size=8U+header;
    unsigned char *prefix=malloc(size);
    if(prefix==NULL) return 1;
    for(size_t i=0U;i<8U;++i) prefix[i]=(unsigned char)((uint64_t)header>>(8U*i));
    memcpy(prefix+8U,json,header);
    /* Synthetic declared payload length; no model file or tensor bytes exist.
     * A real caller must obtain this size from the same owned file it hashes. */
    UmiSafetensorsHeaderInfo info;
    UmiStatus status=UmiSafetensorsHeaderInspect(prefix,size,(uint64_t)size+8U,NULL,&info);
    free(prefix);
    if(status!=UMI_STATUS_OK) {
        fprintf(stderr,"Header inspection refused: %d\n",(int)status);
        return 1;
    }
    printf("Header describes %zu tensor, %" PRIu64 " elements and %" PRIu64 " payload bytes.\n",
        info.tensors,info.elements,info.data_bytes);
    puts("Metadata only: no tensor data read and no model executed.");
    return fflush(stdout)==0 && !ferror(stdout)?0:1;
}

/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/creative_audio/client/main.c
 * PURPOSE:
 *   Independent SDK client; input contains two signed PCM16 frames.
 * ORGANISATION: Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/* Umicom Foundation | Sammy Hegab | MIT
 * Independent SDK client; input contains two signed PCM16 frames. */
#include "umicom/creative_workspace/audio.h"
#include <stdio.h>
int main(void)
{
    const unsigned char wave[]={'R','I','F','F',40,0,0,0,'W','A','V','E','f','m','t',' ',16,0,0,0,
        1,0,1,0,128,187,0,0,0,119,1,0,2,0,16,0,'d','a','t','a',4,0,0,0,0,128,255,127};
    UmiCreativeAudioClip *clip=NULL;
    UmiCreativeExport output={0};
    UmiStatus status=UmiCreativeAudioDecode(wave,sizeof(wave),&clip);
    UmiCreativeAudioEdit edit={0,2,500,0,0};
    if(status==UMI_STATUS_OK)status=UmiCreativeAudioRender(clip,&edit,&output);
    UmiCreativeAudioDestroy(clip);clip=NULL;
    if(status==UMI_STATUS_OK)status=UmiCreativeAudioDecode(output.bytes,output.size,&clip);
    int16_t sample=0;
    if(status==UMI_STATUS_OK)status=UmiCreativeAudioSample(clip,0,0,&sample);
    UmiCreativeAudioDestroy(clip);UmiCreativeExportFree(&output);
    if(status!=UMI_STATUS_OK||sample!=-16384)return 1;
    puts("Public SDK client: half-gain sample -16384; every owner released.");return 0;
}

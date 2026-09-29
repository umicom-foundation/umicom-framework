/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT */
#include "umicom/creative_workspace/audio.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static long countdown=-1;
void *__real_malloc(size_t n);void *__real_calloc(size_t n,size_t s);
void *__wrap_malloc(size_t n){if(countdown==0)return NULL;if(countdown>0)--countdown;return __real_malloc(n);}
void *__wrap_calloc(size_t n,size_t s){if(countdown==0)return NULL;if(countdown>0)--countdown;return __real_calloc(n,s);}
#define CHECK(x) do{if(!(x)){countdown=-1;fprintf(stderr,"allocation check failed: %s\n",#x);return 1;}}while(0)
int main(void)
{
    const unsigned char wave[]={ 'R','I','F','F',40,0,0,0,'W','A','V','E','f','m','t',' ',16,0,0,0,
        1,0,1,0,128,187,0,0,0,119,1,0,2,0,16,0,'d','a','t','a',4,0,0,0,0,128,255,127};
    for(long n=0;n<=2;++n){UmiCreativeAudioClip *clip=NULL;countdown=n;
        UmiStatus s=UmiCreativeAudioDecode(wave,sizeof(wave),&clip);countdown=-1;
        if(n<2)CHECK(s==UMI_STATUS_OUT_OF_MEMORY&&clip==NULL);else CHECK(s==UMI_STATUS_OK&&clip!=NULL);
        UmiCreativeAudioDestroy(clip);
    }
    UmiCreativeAudioClip *clip=NULL;CHECK(UmiCreativeAudioDecode(wave,sizeof(wave),&clip)==UMI_STATUS_OK);
    UmiCreativeAudioEdit edit={0,2,1000,0,0};UmiCreativeExport out={0};countdown=0;
    UmiStatus s=UmiCreativeAudioRender(clip,&edit,&out);countdown=-1;
    CHECK(s==UMI_STATUS_OUT_OF_MEMORY&&out.bytes==NULL&&out.size==0);
    CHECK(UmiCreativeAudioRender(clip,&edit,&out)==UMI_STATUS_OK);
    UmiCreativeExportFree(&out);UmiCreativeAudioDestroy(clip);
    puts("Decode and render allocation failures preserve ownership and outputs.");return 0;
}

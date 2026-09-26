/* Umicom Framework tests | Sammy Hegab, Umicom Foundation | MIT
 * Synthetic ELF headers are parser fixtures, never guest-boot evidence. */
#include "test_support.h"
static void Header(unsigned char h[64])
{
    memset(h,0,64U);memcpy(h,"\177ELF\2\1\1",7U);h[16]=2U;h[18]=62U;h[20]=1U;h[32]=64U;h[52]=64U;h[54]=56U;h[56]=1U;
}
int TestElfAccepted(void)
{
    unsigned char h[64];Header(h);uint16_t machine;
    unsigned values[]={62U,183U,243U};
    for(size_t i=0U;i<3U;++i){h[18]=(unsigned char)values[i];CHECK(UmiNativeStageElfCheck(h,64U,120U,&machine)==UMI_STATUS_OK);CHECK(machine==values[i]);}
    h[16]=3U;CHECK(UmiNativeStageElfCheck(h,64U,120U,&machine)==UMI_STATUS_OK);return 0;
}
int TestElfRejected(void)
{
    unsigned char h[64],test[64];Header(h);uint16_t machine;
    const size_t positions[]={0U,4U,5U,6U,16U,18U,20U,52U,54U,56U};
    for(size_t i=0U;i<sizeof positions/sizeof positions[0];++i){memcpy(test,h,sizeof test);test[positions[i]]=0U;CHECK(UmiNativeStageElfCheck(test,sizeof test,120U,&machine)!=UMI_STATUS_OK);CHECK(machine==0U);}
    for(size_t i=0U;i<64U;++i)CHECK(UmiNativeStageElfCheck(h,i,120U,&machine)!=UMI_STATUS_OK);
    CHECK(UmiNativeStageElfCheck(h,64U,119U,&machine)!=UMI_STATUS_OK);return 0;
}
int TestElfBounds(void)
{
    unsigned char h[64];Header(h);uint16_t machine;
    memset(h+32U,255,8U);CHECK(UmiNativeStageElfCheck(h,64U,120U,&machine)!=UMI_STATUS_OK);
    Header(h);h[56]=255U;h[57]=255U;CHECK(UmiNativeStageElfCheck(h,64U,UMI_NATIVE_STAGE_BINARY_LIMIT,&machine)!=UMI_STATUS_OK);
    Header(h);CHECK(UmiNativeStageElfCheck(h,64U,UMI_NATIVE_STAGE_BINARY_LIMIT+1U,&machine)!=UMI_STATUS_OK);return 0;
}
int TestPngBrand(void){size_t n;const unsigned char *icon=UmiNativeBrandIcon(&n);CHECK(UmiNativeStagePngCheck(icon,n)==UMI_STATUS_OK);return 0;}
int TestPngTruncation(void){size_t n;const unsigned char *icon=UmiNativeBrandIcon(&n);for(size_t i=0U;i<n;++i)CHECK(UmiNativeStagePngCheck(icon,i)!=UMI_STATUS_OK);return 0;}
int TestPngMutation(void)
{
    size_t n;const unsigned char *icon=UmiNativeBrandIcon(&n);unsigned char *copy=malloc(n+1U);CHECK(copy!=NULL);
    for(size_t i=0U;i<n;i+=7U){memcpy(copy,icon,n);copy[i]^=1U;CHECK(UmiNativeStagePngCheck(copy,n)!=UMI_STATUS_OK);}
    memcpy(copy,icon,n);copy[n]=0U;CHECK(UmiNativeStagePngCheck(copy,n+1U)!=UMI_STATUS_OK);free(copy);return 0;
}
int TestPngOverflow(void)
{
    size_t n;const unsigned char *icon=UmiNativeBrandIcon(&n);unsigned char *copy=malloc(n);CHECK(copy!=NULL);memcpy(copy,icon,n);memset(copy+8U,255,4U);
    CHECK(UmiNativeStagePngCheck(copy,n)!=UMI_STATUS_OK);free(copy);return 0;
}
int TestCliArguments(void)
{
    char *a[]={"tool","stage","--desk","/a","--desk","/b",NULL};CHECK(UmiNativeStageMain(6,a)==2);
    char *b[]={"tool","verify","--output",NULL};CHECK(UmiNativeStageMain(3,b)==2);
    char *c[]={"tool","plan","--output","/a","--expect-plan","bad",NULL};CHECK(UmiNativeStageMain(6,c)==2);
    char *d[]={"tool","unknown",NULL};CHECK(UmiNativeStageMain(2,d)==2);
    char *e[]={"tool","digest","--file","",NULL};CHECK(UmiNativeStageMain(4,e)==2);
    CHECK(UmiNativeStageMain(0,NULL)==2);return 0;
}
int TestPlanNulls(void)
{
    CHECK(UmiNativeStagePlanManifest(NULL)==NULL);CHECK(UmiNativeStagePlanEntry(NULL)==NULL);CHECK(UmiNativeStagePlanFingerprint(NULL)==NULL);
    UmiNativeStagePlanDestroy(NULL);return 0;
}

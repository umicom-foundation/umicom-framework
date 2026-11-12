/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/boot_media/test_read_close.c
 *
 * PURPOSE:
 *   Verify read-only media observations publish success only after checked native release.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "../support/assertions.h"
#include "../../src/boot_media/internal.h"
#include "umicom/setup_centre/files.h"
#include <assert.h>
static const char *mode;
/* Production transfer functions are included below. Keep synthetic file
 * objects distinct from their local source/target pointers; retain the former
 * declaration for review without changing release ordering or error outcomes. */
#if 0
static BmFile source,target;
#endif
static BmFile captured_source_file,captured_target_file;
static UmiBootMediaImage fixture;
static unsigned sourceClosed,targetClosed,targetOpened;
static UmiStatus ReadOpen(const char *path,BmFile **out,uint64_t *bytes)
{
    if(!strcmp(path,"source"))*out=&captured_source_file;
    else if(!strcmp(path,"target")){*out=&captured_target_file;++targetOpened;}
    else return UMI_STATUS_INVALID_ARGUMENT;
    *bytes=fixture.bytes;return UMI_STATUS_OK;
}
static UmiStatus ReadInspect(BmFile *file,uint64_t bytes,UmiBootMediaImage *out)
{
    assert(file==&captured_source_file&&bytes==fixture.bytes);*out=fixture;return UMI_STATUS_OK;
}
static UmiStatus ReadHash(BmFile *file,uint64_t bytes,char hex[65],
    UmiBootMediaProgress callback,void *context,UmiBootMediaReport *report)
{
    (void)callback;(void)context;(void)report;
    assert(bytes==fixture.bytes);
    if((file==&captured_source_file&&!strcmp(mode,"inspect-read-and-close-error"))||
        (file==&captured_target_file&&!strcmp(mode,"verify-read-and-close-error")))
        return UMI_STATUS_INVALID_STATE;
    strcpy(hex,fixture.sha256);return UMI_STATUS_OK;
}
static UmiStatus ReadSize(BmFile *file,uint64_t bytes)
{
    assert(file==&captured_source_file||file==&captured_target_file);
    return bytes==fixture.bytes?UMI_STATUS_OK:UMI_STATUS_INVALID_STATE;
}
static UmiStatus ReadClose(BmFile *file)
{
    if(file==NULL)return UMI_STATUS_OK;
    if(file==&captured_source_file){
        assert(++sourceClosed==1U);
        if(!strcmp(mode,"inspect-close-error")||!strcmp(mode,"inspect-read-and-close-error")||
            !strcmp(mode,"verify-source-close-error"))return UMI_STATUS_IO_ERROR;
    }else{
        assert(file==&captured_target_file&&++targetClosed==1U);
        if(!strcmp(mode,"verify-target-close-error")||!strcmp(mode,"verify-read-and-close-error"))
            return UMI_STATUS_PERMISSION_DENIED;
    }
    return UMI_STATUS_OK;
}
/* Keep the real report clearing, structural comparison, source-first ordering
 * and first-error policy. Only transport outcomes are injected; no file is opened. */
#define BmSourceOpen ReadOpen
#define BmInspect ReadInspect
#define BmHash ReadHash
#define BmCheckRegularSize ReadSize
#define BmCloseResult ReadClose
#include "../../src/boot_media/transfer.c"
#undef BmCloseResult
#undef BmCheckRegularSize
#undef BmHash
#undef BmInspect
#undef BmSourceOpen
int main(int argc,char **argv)
{
    if(argc!=2)return 2;
    mode=argv[1];
    const char *cases[]={"inspect-success", "inspect-close-error", "inspect-read-and-close-error", "verify-success", "verify-source-close-error", "verify-target-close-error", "verify-read-and-close-error", "null-report"};
    int known=0;
    for(size_t i=0;i<sizeof cases/sizeof cases[0];++i)if(!strcmp(mode,cases[i]))known=1;
    if(!known)return 2;
    fixture.bytes=512;fixture.mbrLayout=1;fixture.partitions=1;
    memset(fixture.sha256,'a',64);fixture.sha256[64]=0;
    UmiBootMediaReport report;memset(&report,0x5a,sizeof report);
    UmiBootMediaImage image;memset(&image,0x5a,sizeof image);
    int inspect=!strncmp(mode,"inspect-",8U);
    UmiStatus status=inspect?UmiBootMediaInspect("source",&image,NULL,NULL,&report):
        UmiBootMediaVerifyFile("source","target",NULL,NULL,!strcmp(mode,"null-report")?NULL:&report);
    UmiStatus expected=UMI_STATUS_OK;
    if(!strcmp(mode,"inspect-close-error")||!strcmp(mode,"verify-source-close-error"))expected=UMI_STATUS_IO_ERROR;
    if(!strcmp(mode,"verify-target-close-error"))expected=UMI_STATUS_PERMISSION_DENIED;
    if(strstr(mode,"read-and-close-error"))expected=UMI_STATUS_INVALID_STATE;
    assert(status==expected&&sourceClosed==1U);
    if(strcmp(mode,"null-report")) {
        assert(report.status==expected&&!report.outputCreated&&!report.writeStarted);
        assert(report.verified==(!inspect&&expected==UMI_STATUS_OK));
        assert(report.detail[0]!=0);
    }
    if(inspect){
        assert(!targetOpened&&!targetClosed);
        if(status==UMI_STATUS_OK)assert(image.bytes==512U&&image.mbrLayout);
        else{
            const unsigned char *cleared=(const unsigned char *)&image;
            for(size_t i=0;i<sizeof image;++i)assert(cleared[i]==0U);
        }
    }else{
        unsigned expectedTarget=strcmp(mode,"verify-source-close-error")?1U:0U;
        assert(targetOpened==expectedTarget&&targetClosed==expectedTarget);
    }
    return 0;
}

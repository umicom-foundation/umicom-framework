/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/boot_media/test_creation_effects.c
 *
 * PURPOSE:
 *   Verify transfer reports distinguish created practice files, attempted writes and verified bytes under injected native outcomes.
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
#include <stdio.h>
static const char *mode;
static BmFile sourceFile,targetFile;
static UmiBootMediaImage sourceImage;
static unsigned createCalls,writeCalls,hashCalls,sourceCloses,targetCloses;
static int createEffect=1;
static UmiStatus createStatus=UMI_STATUS_OK;
static UmiStatus FixtureSourceOpen(const char *path,BmFile **out,uint64_t *bytes)
{
    (void)path;*out=&sourceFile;*bytes=sourceImage.bytes;return UMI_STATUS_OK;
}
static UmiStatus FixtureCreate(const char *path,BmFile **out,int *created)
{
    (void)path;++createCalls;
    *out=createStatus==UMI_STATUS_OK?&targetFile:NULL;
    if(created)*created=createEffect;
    return createStatus;
}
static UmiStatus FixtureHash(BmFile *file,uint64_t bytes,char hex[65],
    UmiBootMediaProgress progress,void *context,UmiBootMediaReport *report)
{
    (void)bytes;++hashCalls;
    UmiStatus status=BmTick(report,report->phase,0,sourceImage.bytes,progress,context);
    if(status!=UMI_STATUS_OK)return status;
    strcpy(hex,sourceImage.sha256);
    if(file==&targetFile&&!strcmp(mode,"verify-error"))
        hex[0]=hex[0]=='a'?'b':'a';
    return UMI_STATUS_OK;
}
static UmiStatus FixtureInspect(BmFile *file,uint64_t bytes,UmiBootMediaImage *out)
{
    (void)file;(void)bytes;*out=sourceImage;return UMI_STATUS_OK;
}
static UmiStatus FixtureSize(BmFile *file,uint64_t bytes)
{
    (void)file;return bytes==sourceImage.bytes?UMI_STATUS_OK:UMI_STATUS_INVALID_STATE;
}
static UmiStatus FixtureRead(BmFile *file,uint64_t offset,void *data,size_t bytes)
{
    (void)file;(void)offset;memset(data,0,bytes);return UMI_STATUS_OK;
}
static UmiStatus FixtureWrite(BmFile *file,uint64_t offset,const void *data,size_t bytes)
{
    (void)file;(void)offset;(void)data;(void)bytes;++writeCalls;
/* Exercise operation-error precedence when release also fails. The previous implementation is retained for review. */
#if 0
    return !strcmp(mode,"write-error")?UMI_STATUS_IO_ERROR:UMI_STATUS_OK;
#endif
    return (!strcmp(mode,"write-error")||!strcmp(mode,"write-and-close-error"))?
        UMI_STATUS_IO_ERROR:UMI_STATUS_OK;
}
static UmiStatus FixtureFlush(BmFile *file)
{
    (void)file;return !strcmp(mode,"flush-error")?UMI_STATUS_IO_ERROR:UMI_STATUS_OK;
}
/* The release seam now returns native outcomes so completion can be tested after byte verification. Preserve the former void seam for review. The previous implementation is retained for review. */
#if 0
static void FixtureClose(BmFile *file)
{
    if(file==&sourceFile)++sourceCloses;
    if(file==&targetFile)++targetCloses;
}

#endif
static UmiStatus FixtureClose(BmFile *file)
{
    if(file==&sourceFile) {
        ++sourceCloses;
        if(!strcmp(mode,"source-close-error")||!strcmp(mode,"both-close-errors"))
            return UMI_STATUS_IO_ERROR;
    }
    if(file==&targetFile) {
        ++targetCloses;
        if(!strcmp(mode,"target-close-error")||!strcmp(mode,"both-close-errors")||
            !strcmp(mode,"write-and-close-error")||!strcmp(mode,"cancelled-and-close-error"))
            return UMI_STATUS_PERMISSION_DENIED;
    }
    return UMI_STATUS_OK;
}
static void *FixtureMalloc(size_t bytes)
{
    if(!strcmp(mode,"buffer-no-memory")&&bytes==BM_CHUNK)return NULL;
    return malloc(bytes);
}
/* Only controlled transport outcomes are injected. The real plan consumption,
 * streaming digest, progress policy and report publication remain in use. */
#define BmSourceOpen FixtureSourceOpen
#define BmCreateObserved FixtureCreate
#define BmHash FixtureHash
#define BmInspect FixtureInspect
#define BmCheckRegularSize FixtureSize
#define BmRead FixtureRead
#define BmWrite FixtureWrite
#define BmFlush FixtureFlush
/* The checked release replaces the former void seam. */
#if 0
#define BmClose FixtureClose
#endif
#define BmCloseResult FixtureClose
#define malloc FixtureMalloc
#include "../../src/boot_media/transfer.c"
#undef malloc
#undef BmCloseResult
#undef BmFlush
#undef BmWrite
#undef BmRead
#undef BmCheckRegularSize
#undef BmInspect
#undef BmHash
#undef BmCreateObserved
#undef BmSourceOpen
static unsigned finishedCalls;
static int Progress(const UmiBootMediaReport *report,void *context)
{
    (void)context;
    if(report->phase==UMI_BOOT_MEDIA_FINISHED)++finishedCalls;
    return (!strcmp(mode,"cancel-before-acquire")&&report->phase==UMI_BOOT_MEDIA_RECHECK)||
/* Keep cancellation primary when target release later fails. The previous implementation is retained for review. */
#if 0
        (!strcmp(mode,"cancel-before-payload")&&report->phase==UMI_BOOT_MEDIA_WRITE)||
#endif
        ((!strcmp(mode,"cancel-before-payload")||!strcmp(mode,"cancelled-and-close-error"))&&
            report->phase==UMI_BOOT_MEDIA_WRITE)||
        (!strcmp(mode,"finished-cancel")&&report->phase==UMI_BOOT_MEDIA_FINISHED);
}
int main(int argc,char **argv)
{
    if(argc!=2)return 2;
    mode=argv[1];
/* Extend the existing transfer scenarios with checked release outcomes. The previous implementation is retained for review. */
#if 0
    const char *cases[]={ "complete", "open-refused", "existing", "created-no-memory", "created-invalid", "created-lock-refused", "buffer-no-memory", "cancel-before-acquire", "cancel-before-payload", "write-error", "flush-error", "verify-error", "finished-cancel" };
#endif
    const char *cases[]={ "complete", "open-refused", "existing", "created-no-memory", "created-invalid", "created-lock-refused", "buffer-no-memory", "cancel-before-acquire", "cancel-before-payload", "write-error", "flush-error", "verify-error", "finished-cancel", "target-close-error", "source-close-error", "both-close-errors", "write-and-close-error", "cancelled-and-close-error" };
    int known=0;
    for(size_t i=0;i<sizeof cases/sizeof cases[0];++i)if(!strcmp(mode,cases[i]))known=1;
    if(!known)return 2;
    unsigned char payload[512]={0};
    sourceImage.bytes=sizeof payload;sourceImage.mbrLayout=1;sourceImage.partitions=1;
    assert(UmiNativeSha256Buffer(payload,sizeof payload,sourceImage.sha256)==UMI_STATUS_OK);
    if(!strcmp(mode,"open-refused")){createStatus=UMI_STATUS_PERMISSION_DENIED;createEffect=0;}
    if(!strcmp(mode,"existing")){createStatus=UMI_STATUS_ALREADY_EXISTS;createEffect=0;}
    if(!strcmp(mode,"created-no-memory"))createStatus=UMI_STATUS_OUT_OF_MEMORY;
    if(!strcmp(mode,"created-invalid"))createStatus=UMI_STATUS_INVALID_ARGUMENT;
    if(!strcmp(mode,"created-lock-refused"))createStatus=UMI_STATUS_PERMISSION_DENIED;
    UmiBootMediaPlan plan={0};
    plan.image=sourceImage;
    strcpy(plan.source,"injected source");strcpy(plan.destination,"injected practice output");
    strcpy(plan.fingerprint,"reviewed");strcpy(plan.confirmation,"CREATE reviewed");
    UmiBootMediaReport report;
    memset(&report,0x5a,sizeof report);
    UmiStatus status=UmiBootMediaApply(&plan,plan.fingerprint,plan.confirmation,Progress,NULL,&report);
    UmiStatus expected=createStatus;
    if(!strcmp(mode,"buffer-no-memory"))expected=UMI_STATUS_OUT_OF_MEMORY;
    if(!strcmp(mode,"write-error")||!strcmp(mode,"flush-error"))expected=UMI_STATUS_IO_ERROR;
    if(!strcmp(mode,"verify-error"))expected=UMI_STATUS_INVALID_STATE;
    if(!strncmp(mode,"cancel-",7U)||!strcmp(mode,"finished-cancel"))expected=UMI_STATUS_CANCELLED;
    if(!strcmp(mode,"source-close-error")||!strcmp(mode,"write-and-close-error"))
        expected=UMI_STATUS_IO_ERROR;
    if(!strcmp(mode,"target-close-error")||!strcmp(mode,"both-close-errors"))
        expected=UMI_STATUS_PERMISSION_DENIED;
    if(!strcmp(mode,"cancelled-and-close-error"))expected=UMI_STATUS_CANCELLED;
    assert(status==expected&&report.status==expected&&plan.consumed);
    assert(createCalls==(!strcmp(mode,"cancel-before-acquire")?0U:1U));
    assert(report.outputCreated==(createCalls?createEffect:0));
    assert(report.writeStarted==(writeCalls!=0U));
    assert(report.verified==(!strcmp(mode,"complete")));
    assert(sourceCloses==1U);
    assert(finishedCalls==(!strcmp(mode,"complete")||!strcmp(mode,"finished-cancel")?1U:0U));
    assert(targetCloses==(createCalls&&createStatus==UMI_STATUS_OK?1U:0U));
    assert(hashCalls>=1U&&report.detail[0]!='\0');
    /* A refused retry must replace old effect flags without acquiring again. */
    unsigned before=createCalls;
    assert(UmiBootMediaApply(&plan,plan.fingerprint,plan.confirmation,NULL,NULL,&report)==UMI_STATUS_INVALID_STATE);
    assert(createCalls==before&&!report.outputCreated&&!report.writeStarted&&!report.verified);
    return 0;
}

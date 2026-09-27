/*-----------------------------------------------------------------------------
 * Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * File: src/boot_media/transfer.c
 * Review -> recheck -> acquire -> write -> flush -> read back. The plan is
 * single-use, and verified is set only at the final boundary. Physical writes
 * have no rollback; cancellation after the first write is explicitly partial.
 *---------------------------------------------------------------------------*/

#include "internal.h"
#include "umicom/setup_centre/files.h"
#include <inttypes.h>
UmiStatus BmMessage(UmiBootMediaReport *r,UmiStatus s,const char *fmt,...)
{
    if (r) {
        r->status=s;
        va_list a;
        va_start(a,fmt);
        (void)vsnprintf(r->detail,sizeof r->detail,fmt,a);
        va_end(a);
    }
    return s;
}

UmiStatus BmTick(UmiBootMediaReport *r,UmiBootMediaPhase phase,uint64_t done,uint64_t total,UmiBootMediaProgress cb,
    void *ctx)
{
    if (r) {
        r->phase=phase;
        r->bytesDone=done;
        r->bytesTotal=total;
    }
    return cb&&cb(r,ctx)?UMI_STATUS_CANCELLED:UMI_STATUS_OK;
}

int BmHashValid(const char *h) {
    if (!h||strlen(h)!=64U)return 0;
    for (size_t i=0;i<64U;++i)if (!((h[i]>='0'&&h[i]<='9')||(h[i]>='a'&&h[i]<='f')))return 0;
    return 1;
}

static int DeviceStringsValid(const UmiBootMediaDevice *d)
{
    return d && memchr(d->path, 0, sizeof d->path) &&
        memchr(d->identity, 0, sizeof d->identity) &&
        memchr(d->model, 0, sizeof d->model) &&
        memchr(d->serial, 0, sizeof d->serial) &&
        memchr(d->reason, 0, sizeof d->reason);
}

static int SameImageStructure(const UmiBootMediaImage *a, const UmiBootMediaImage *b)
{
    return a->bytes == b->bytes && a->partitions == b->partitions &&
        a->mbrLayout == b->mbrLayout && a->gptLayout == b->gptLayout &&
        a->iso9660 == b->iso9660 && a->elTorito == b->elTorito &&
        a->biosEntry == b->biosEntry && a->efiEntry == b->efiEntry;
}

int BmSameDevice(const UmiBootMediaDevice *a,const UmiBootMediaDevice *b)
{
    return !strcmp(a->path,b->path)&&!strcmp(a->identity,b->identity)&&!strcmp(a->serial,b->serial)&&
        !strcmp(a->model,b->model)&&a->bytes==b->bytes&&a->sectorBytes==b->sectorBytes&&a->usb==b->usb&&a->removable==b->removable;
}

UmiStatus UmiBootMediaCheckDevicePolicy(const UmiBootMediaImage *im,const UmiBootMediaDevice *d,UmiBootMediaReport *r)
{
    if (!im||!d)return UMI_STATUS_INVALID_ARGUMENT;
    if (!DeviceStringsValid(d))return UMI_STATUS_INVALID_ARGUMENT;
    if (!d->identity[0]||!d->serial[0]||!d->path[0]||!d->model[0])return BmMessage(r,UMI_STATUS_PERMISSION_DENIED,
        "A whole-device identity, model and serial are required.");
    if (!d->usb||!d->removable||d->protectedDevice||d->inUse||d->readOnly)return BmMessage(r,UMI_STATUS_PERMISSION_DENIED,
        "Device is not an unused, writable, identified removable USB disk: %s",d->reason);
    if (d->sectorBytes<512U||d->sectorBytes>4096U||(d->sectorBytes&(d->sectorBytes-1U))||!d->bytes||d->bytes%d->sectorBytes||d->bytes>UMI_BOOT_MEDIA_MAX_DEVICE)return BmMessage(r,
        UMI_STATUS_INVALID_ARGUMENT,"Unsupported sector geometry or device size.");
    if (im->bytes<512U||im->bytes>UMI_BOOT_MEDIA_MAX_IMAGE||im->bytes%512U||im->bytes>d->bytes)return BmMessage(r,
        UMI_STATUS_CAPACITY_EXCEEDED,"The complete image must fit the selected disk.");
    if (!im->mbrLayout&&!im->gptLayout)return BmMessage(r,UMI_STATUS_UNAVAILABLE,"An optical-only ISO is not accepted as a USB disk image.");
    if (d->sectorBytes!=512U)return BmMessage(r,UMI_STATUS_UNAVAILABLE,"This disk-image profile uses 512-byte logical sectors; 4Kn media is not qualified.");
    if (im->gptLayout&&im->bytes!=d->bytes)return BmMessage(r,UMI_STATUS_UNAVAILABLE,"GPT images require an exactly sized target; automatic GPT relocation is not implemented.");
    return UMI_STATUS_OK;
}

UmiStatus UmiBootMediaInspect(const char *path,UmiBootMediaImage *out,UmiBootMediaProgress cb,void *ctx,
    UmiBootMediaReport *r)
{
    UmiBootMediaReport local= {
        0
    };
    if (!r)r=&local;
    memset(r,0,sizeof *r);
    if (!out)return UMI_STATUS_INVALID_ARGUMENT;
    memset(out,0,sizeof *out);
    BmFile *f=NULL;
    uint64_t bytes=0;
    UmiStatus s=BmSourceOpen(path,&f,&bytes);
    if (s==UMI_STATUS_OK)s=BmInspect(f,bytes,out);
    if (s==UMI_STATUS_OK)s=BmHash(f,bytes,out->sha256,cb,ctx,r);

    /* Re-read structural metadata after the streaming digest. Conventional
     * Windows writers are denied sharing; Linux locks are cooperative. This
     * catches a changed layout, but is not a hostile-writer snapshot service. */
    UmiBootMediaImage checked = {
        0
    };
    if (s==UMI_STATUS_OK)s=BmInspect(f,bytes,&checked);
    if (s==UMI_STATUS_OK&&!SameImageStructure(out,&checked))s=UMI_STATUS_INVALID_STATE;
    BmClose(f);
    if (s!=UMI_STATUS_OK)memset(out,0,sizeof *out);
    return BmMessage(r,s,s==UMI_STATUS_OK?"Image structure and SHA-256 recorded. No boot code was executed.":"Image inspection failed or was cancelled. No destination was opened.");
}

static UmiStatus Fingerprint(UmiBootMediaPlan *p)
{
    size_t capacity=2U*UMI_BOOT_MEDIA_PATH+2U*UMI_BOOT_MEDIA_ID+1024U;
    char *text=malloc(capacity);
    if (!text)return UMI_STATUS_OUT_OF_MEMORY;
    int n=snprintf(text,capacity,"UMICOM_MEDIA_PLAN 1\n%d\n%zu:%s\n%zu:%s\n%s\n%" PRIu64 "\n%zu:%s\n%zu:%s\n%" PRIu64 "\n%u\n%zu:%s\n",
        p->physical,strlen(p->source),p->source,strlen(p->destination),p->destination,p->image.sha256,p->image.bytes,
        strlen(p->device.identity),p->device.identity,strlen(p->device.serial),p->device.serial,p->device.bytes,
        p->device.sectorBytes,strlen(p->device.model),p->device.model);
    UmiStatus s=n<0||(size_t)n>=capacity?UMI_STATUS_CAPACITY_EXCEEDED:UmiNativeSha256Buffer(text,(size_t)n,
        p->fingerprint);
    free(text);
    if (s==UMI_STATUS_OK)(void)snprintf(p->confirmation,sizeof p->confirmation,"%s %.32s",p->physical?"ERASE":"CREATE",
        p->fingerprint);
    return s;
}

static UmiStatus Review(const char *source,const char *dest,const UmiBootMediaDevice *device,UmiBootMediaPlan **out,
    UmiBootMediaProgress cb,void *ctx,UmiBootMediaReport *r)
{
    if (r)memset(r,0,sizeof *r);
    if (!out)return BmMessage(r,UMI_STATUS_INVALID_ARGUMENT,"An output-plan pointer is required.");
    *out=NULL;
    if (BmPath(source)!=UMI_STATUS_OK||(!device&&BmPath(dest)!=UMI_STATUS_OK)||!dest||strlen(dest)>=UMI_BOOT_MEDIA_PATH)return UMI_STATUS_INVALID_ARGUMENT;
    UmiBootMediaPlan *p=calloc(1,sizeof *p);
    if (!p)return UMI_STATUS_OUT_OF_MEMORY;
    strcpy(p->source,source);
    strcpy(p->destination,dest);
    p->physical=device!=NULL;
    UmiStatus s=UmiBootMediaInspect(source,&p->image,cb,ctx,r);
    if (s==UMI_STATUS_OK&&device) {
        s=BmDeviceCheck(device->path,&p->device);
        if (s==UMI_STATUS_OK&&!BmSameDevice(device,&p->device))s=UMI_STATUS_INVALID_STATE;
        if (s==UMI_STATUS_OK)s=UmiBootMediaCheckDevicePolicy(&p->image,&p->device,r);
    }
    else if (s==UMI_STATUS_OK) {
        UmiStatus exists=UmiSetupFileCheck(dest,0,NULL);
        if (exists!=UMI_STATUS_NOT_FOUND)s=exists==UMI_STATUS_OK?UMI_STATUS_ALREADY_EXISTS:exists;
    }
    if (s==UMI_STATUS_OK)s=Fingerprint(p);
    if (s!=UMI_STATUS_OK) {
        free(p);
        return BmMessage(r,s,"Review refused. Check the image, destination, device identity and policy.");
    }
    *out=p;
    return BmMessage(r,s,p->physical?"Reviewed USB image transfer. Existing partitions and files will be destroyed if applied.":"Reviewed a new-file practice copy. No file has been created.");
}

UmiStatus UmiBootMediaReviewFile(const char *s,const char *d,UmiBootMediaPlan **p,UmiBootMediaProgress cb,
    void *c,UmiBootMediaReport *r) {
    return Review(s,d,NULL,p,cb,c,r);
}

UmiStatus UmiBootMediaReviewDevice(const char *s,const UmiBootMediaDevice *d,UmiBootMediaPlan **p,UmiBootMediaProgress cb,
    void *c,UmiBootMediaReport *r) {
    if (!DeviceStringsValid(d)) {
        if (p)*p=NULL;
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return Review(s,d->path,d,p,cb,c,r);
}

const char *UmiBootMediaPlanFingerprint(const UmiBootMediaPlan *p) {
    return p?p->fingerprint:NULL;
}

const char *UmiBootMediaPlanConfirmation(const UmiBootMediaPlan *p) {
    return p?p->confirmation:NULL;
}

const char *UmiBootMediaPlanDestination(const UmiBootMediaPlan *p) {
    return p?p->destination:NULL;
}

const UmiBootMediaImage *UmiBootMediaPlanImage(const UmiBootMediaPlan *p) {
    return p?&p->image:NULL;
}

void UmiBootMediaPlanDestroy(UmiBootMediaPlan *p) {
    free(p);
}

int UmiBootMediaDeviceWritesEnabled(void)
{
#ifdef UMICOM_BOOT_MEDIA_ENABLE_DEVICE_WRITES
    return 1;

#else
    return 0;

#endif
}

UmiStatus UmiBootMediaApply(UmiBootMediaPlan *p,const char *expected,const char *confirmation,UmiBootMediaProgress cb,
    void *ctx,UmiBootMediaReport *r)
{
    UmiBootMediaReport local= {
        0
    };
    if (!r)r=&local;
    memset(r,0,sizeof *r);
    if (!p||!expected||!confirmation||p->consumed||strcmp(expected,p->fingerprint)||strcmp(confirmation,p->confirmation))return BmMessage(r,
        UMI_STATUS_INVALID_STATE,"Use an unused plan and its exact fingerprint and confirmation.");
    p->consumed=1;
    if (p->physical&&!UmiBootMediaDeviceWritesEnabled())return BmMessage(r,UMI_STATUS_UNAVAILABLE,"Physical writing is disabled in this build; inspection and new-file practice remain available.");
    BmFile *source=NULL,*target=NULL;
    uint64_t bytes=0;
    char hash[65];
    UmiStatus s=BmSourceOpen(p->source,&source,&bytes);
    if (s==UMI_STATUS_OK&&bytes!=p->image.bytes)s=UMI_STATUS_INVALID_STATE;
    r->phase=UMI_BOOT_MEDIA_RECHECK;
    if (s==UMI_STATUS_OK)s=BmHash(source,bytes,hash,cb,ctx,r);
    if (s==UMI_STATUS_OK&&strcmp(hash,p->image.sha256))s=UMI_STATUS_INVALID_STATE;
    UmiBootMediaImage checked = {
        0
    };
    if (s==UMI_STATUS_OK)s=BmInspect(source,bytes,&checked);
    if (s==UMI_STATUS_OK&&!SameImageStructure(&p->image,&checked))s=UMI_STATUS_INVALID_STATE;
    if (s==UMI_STATUS_OK)s=BmTick(r,UMI_BOOT_MEDIA_RECHECK,bytes,bytes,cb,ctx);
    if (s==UMI_STATUS_OK) {
        s=p->physical?BmDeviceOpen(&p->device,1,&target):BmCreate(p->destination,&target);
        if (s==UMI_STATUS_OK&&!p->physical)r->outputCreated=1;
    }
    unsigned char *buffer=NULL;
    if (s==UMI_STATUS_OK) {
        buffer=malloc(BM_CHUNK);
        if (!buffer)s=UMI_STATUS_OUT_OF_MEMORY;
    }
    UmiNativeSha256 streamed;
    UmiNativeSha256Init(&streamed);
    for (uint64_t at=0;at<bytes&&s==UMI_STATUS_OK;) {
        size_t n=bytes-at>BM_CHUNK?BM_CHUNK:(size_t)(bytes-at);
        s=BmTick(r,UMI_BOOT_MEDIA_WRITE,at,bytes,cb,ctx);
        if (s==UMI_STATUS_OK)s=BmRead(source,at,buffer,n);
        if (s==UMI_STATUS_OK)s=UmiNativeSha256Update(&streamed,buffer,n);
        if (s==UMI_STATUS_OK) {
            r->writeStarted=1;
            s=BmWrite(target,at,buffer,n);
        }
        at+=n;
    }
    unsigned char raw[32];
    if (s==UMI_STATUS_OK)s=UmiNativeSha256Final(&streamed,raw);
    if (s==UMI_STATUS_OK) {
        UmiNativeSha256Hex(raw,hash);
        if (strcmp(hash,p->image.sha256))s=UMI_STATUS_INVALID_STATE;
    }

    /* Retire stale trailing partition signatures on a larger MBR target. The
     * untouched middle is NOT erased securely. The plan/guide state this rule. */
    uint64_t tail=0,tailAt=bytes;
    if (s==UMI_STATUS_OK&&p->physical&&target->bytes>bytes) {
        tail=target->bytes-bytes>BM_TAIL?BM_TAIL:target->bytes-bytes;
        tailAt=target->bytes-tail;
        memset(buffer,0,(size_t)tail);
        s=BmTick(r,UMI_BOOT_MEDIA_WRITE,bytes,bytes+tail,cb,ctx);
        if (s==UMI_STATUS_OK)s=BmWrite(target,tailAt,buffer,(size_t)tail);
    }
    if (s==UMI_STATUS_OK)s=BmTick(r,UMI_BOOT_MEDIA_FLUSH,bytes,bytes,cb,ctx);
    if (s==UMI_STATUS_OK)s=BmFlush(target);
    if (s==UMI_STATUS_OK)r->phase=UMI_BOOT_MEDIA_VERIFY;
    if (s==UMI_STATUS_OK)s=BmHash(target,bytes,hash,cb,ctx,r);
    if (s==UMI_STATUS_OK&&strcmp(hash,p->image.sha256))s=UMI_STATUS_INVALID_STATE;
    if (s==UMI_STATUS_OK&&tail) {
        s=BmRead(target,tailAt,buffer,(size_t)tail);
        if (s==UMI_STATUS_OK)for (size_t i=0;i<(size_t)tail;++i)if (buffer[i]) {
            s=UMI_STATUS_INVALID_STATE;
            break;
        }
    }
    free(buffer);
    BmClose(target);
    BmClose(source);
    if (s==UMI_STATUS_OK)s=BmTick(r,UMI_BOOT_MEDIA_FINISHED,bytes+tail,bytes+tail,cb,ctx);
    r->verified=s==UMI_STATUS_OK;
    return BmMessage(r,s,s==UMI_STATUS_OK?"Transfer flushed and read back. Image bytes agree; booting has not been tested.":r->writeStarted?"Transfer incomplete. Written media may be unbootable. No rollback or automatic retry was attempted.":"Stopped before any payload write. A newly created practice file may remain.");
}

UmiStatus UmiBootMediaVerifyFile(const char *a,const char *b,UmiBootMediaProgress cb,void *ctx,UmiBootMediaReport *r)
{
    UmiBootMediaReport local= {
        0
    };
    if (!r)r=&local;
    memset(r,0,sizeof *r);
    UmiBootMediaImage image;
    UmiStatus s=UmiBootMediaInspect(a,&image,cb,ctx,r);
    BmFile *f=NULL;
    uint64_t bytes=0;
    char hash[65];
    if (s==UMI_STATUS_OK)r->phase=UMI_BOOT_MEDIA_VERIFY;
    if (s==UMI_STATUS_OK)s=BmSourceOpen(b,&f,&bytes);
    if (s==UMI_STATUS_OK&&bytes!=image.bytes)s=UMI_STATUS_INVALID_STATE;
    if (s==UMI_STATUS_OK)s=BmHash(f,bytes,hash,cb,ctx,r);
    if (s==UMI_STATUS_OK&&strcmp(hash,image.sha256))s=UMI_STATUS_INVALID_STATE;
    BmClose(f);
    r->verified=s==UMI_STATUS_OK;
    return BmMessage(r,s,s==UMI_STATUS_OK?"Whole-file bytes agree. No boot code was executed.":"The new-file copy does not verify or the check was cancelled.");
}

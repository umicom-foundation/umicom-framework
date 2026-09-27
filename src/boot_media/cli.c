/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Native CLI adapter; no shell commands or automatic privileged relaunch. */

#include "internal.h"
#include <inttypes.h>
static void Help(void)
{
    puts("Umicom Boot Media\n"
        "  inspect --image ABSOLUTE_IMAGE\n"
        "  list\n"
        "  review --image ABSOLUTE_IMAGE --output NEW_ABSOLUTE_FILE\n"
        "  review --image ABSOLUTE_IMAGE --device EXACT_DISCOVERED_DEVICE\n"
        "  copy --image IMAGE --output NEW_FILE --expect HASH --confirm \"CREATE TOKEN\"\n"
        "  write --image IMAGE --device DEVICE --expect HASH --confirm \"ERASE TOKEN\"\n"
        "  verify --image IMAGE --output COPIED_FILE\n"
        "  --self-test | --help\n"
        "A write destroys existing device data. Device writing is build-gated.\n"
        "Use list again after reconnecting a device. There is no burn or eject command.\n"
        "Structural inspection and matching bytes do not establish bootability.");
}

static void Image(const UmiBootMediaImage *i)
{
    printf("Bytes: %" PRIu64 "\nSHA-256: %s\nMBR: %s; GPT: %s; ISO9660: %s; El Torito: %s\n",
        i->bytes,i->sha256,i->mbrLayout?"yes":"no",i->gptLayout?"yes":"no",i->iso9660?"yes":"no",i->elTorito?"yes":"no");
}

static int Result(UmiStatus s,const UmiBootMediaReport *r)
{
    if (r&&r->detail[0])fprintf(s==UMI_STATUS_OK?stdout:stderr,"%s\n",r->detail);
    return s==UMI_STATUS_OK?0:s==UMI_STATUS_UNAVAILABLE?77:1;
}

int UmiBootMediaMain(int argc,char **argv)
{
    if (argc==2&&(!strcmp(argv[1],"--help")||!strcmp(argv[1],"help"))) {
        Help();
        return 0;
    }
    if (argc==2&&!strcmp(argv[1],"--self-test")) {
        if (BmCrc32("123456789",9U)!=UINT32_C(0xcbf43926))return 1;
        puts("Native media CRC self-check passed. No file or device was opened.");
        return 0;
    }
    UmiBootMediaReport report= {
        0
    };
    if (argc==2&&!strcmp(argv[1],"list")) {
        UmiBootMediaInventory *items=calloc(1,sizeof *items);
        if (!items)return 1;
        UmiStatus s=UmiBootMediaDiscover(items,&report);
        for (size_t i=0;i<items->count;++i) {
            const UmiBootMediaDevice*d=&items->devices[i];
            printf("%s\n  Model: %s\n  Serial: %s\n  Identity: %s\n  Bytes: %" PRIu64 "; logical sector: %u\n  %s\n",
                d->path,d->model,d->serial,d->identity,d->bytes,d->sectorBytes,d->reason);
        }
        free(items);
        printf("Physical writes: %s\n",UmiBootMediaDeviceWritesEnabled()?"explicitly enabled":"disabled");
        return Result(s,&report);
    }
    if (argc<4) {
        Help();
        return 2;
    }
    const char *image=NULL,*output=NULL,*device=NULL,*hash=NULL,*confirmation=NULL;
    for (int i=2;i<argc;i+=2) {
        if (i+1>=argc)return 2;
        const char **slot=NULL;
        if (!strcmp(argv[i],"--image"))slot=&image;
        else if (!strcmp(argv[i],"--output"))slot=&output;
        else if (!strcmp(argv[i],"--device"))slot=&device;
        else if (!strcmp(argv[i],"--expect"))slot=&hash;
        else if (!strcmp(argv[i],"--confirm"))slot=&confirmation;
        else return 2;
        if (*slot||!argv[i+1][0])return 2;
        *slot=argv[i+1];
    }
    if (!image)return 2;
    if (!strcmp(argv[1],"inspect")) {
        if (output||device||hash||confirmation)return 2;
        UmiBootMediaImage info;
        UmiStatus s=UmiBootMediaInspect(image,&info,NULL,NULL,&report);
        if (s==UMI_STATUS_OK)Image(&info);
        return Result(s,&report);
    }
    if (!strcmp(argv[1],"verify")) {
        if (!output||device||hash||confirmation)return 2;
        return Result(UmiBootMediaVerifyFile(image,output,NULL,NULL,&report),&report);
    }
    int review=!strcmp(argv[1],"review"),copy=!strcmp(argv[1],"copy"),write=!strcmp(argv[1],"write");
    if (!review&&!copy&&!write)return 2;
    if ((!output)==(!device)||(copy&&!output)||(write&&!device)||((copy||write)&&(!hash||!confirmation))||(review&&(hash||confirmation)))return 2;
    UmiBootMediaPlan *plan=NULL;
    UmiStatus s;
    if (device) {
        UmiBootMediaDevice observed;
        s=BmDeviceCheck(device,&observed);
        if (s==UMI_STATUS_OK)s=UmiBootMediaReviewDevice(image,&observed,&plan,NULL,NULL,&report);
    }
    else s=UmiBootMediaReviewFile(image,output,&plan,NULL,NULL,&report);
    if (s==UMI_STATUS_OK) {
        if (review) {
            Image(UmiBootMediaPlanImage(plan));
            printf("Destination: %s\nFingerprint: %s\nConfirmation: %s\n",UmiBootMediaPlanDestination(plan),UmiBootMediaPlanFingerprint(plan),
                UmiBootMediaPlanConfirmation(plan));
        }
        else s=UmiBootMediaApply(plan,hash,confirmation,NULL,NULL,&report);
    }
    UmiBootMediaPlanDestroy(plan);
    return Result(s,&report);
}

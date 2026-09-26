/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Native command dispatch. Printing a plan does not run it. All filesystem
 * behaviour remains in the shared stage API, not in application wrappers. */
#include "internal.h"
#include <inttypes.h>
#include <stdio.h>
#include <string.h>
static int Usage(void)
{
    puts("Umicom native launcher delivery\n"
         "  umicom-session-stage plan --desk PATH --launcher PATH --output NEW-DIRECTORY [--icon PNG]\n"
         "  umicom-session-stage stage --desk PATH --launcher PATH --output NEW-DIRECTORY [--icon PNG] [--expect-plan SHA256]\n"
         "  umicom-session-stage verify --output DIRECTORY\n"
         "  umicom-session-stage digest --file ABSOLUTE-LOCAL-PATH\n"
         "  umicom-session-stage --self-test\n"
         "Linux staging is opt-in. No autostart, installation or process launch is performed.\n"
         "Exit codes: 0 completed, 1 failed, 2 invalid arguments, 77 unsupported host.");
    return ferror(stdout)?1:0;
}
static int Result(UmiStatus status,const UmiNativeStageReport *report)
{
    if(status==UMI_STATUS_OK){puts(report->detail);return ferror(stdout)?1:0;}
    fprintf(stderr,"Not completed [%s / %s]: %s",report->phase,UmiNativeStageStatusText(status),report->detail);
    if(report->systemError!=0)fprintf(stderr," (native error %d)",report->systemError);
    fputc('\n',stderr);
    if(report->outputCreated)fputs("Partial output was retained. Inspect it and use a new destination for a later attempt.\n",stderr);
    return status==UMI_STATUS_UNAVAILABLE?77:1;
}
int UmiNativeStageMain(int argc,char **argv)
{
    if(argc<1||argv==NULL)return 2;
    if(argc==2 && strcmp(argv[1],"--help")==0)return Usage();
    if(argc==2 && strcmp(argv[1],"--self-test")==0) {
        char hash[65];
        if(UmiNativeSha256Buffer("abc",3U,hash)!=UMI_STATUS_OK||strcmp(hash,"ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad")!=0)return 1;
        puts("Native C SHA-256 self-check passed. No file, subprocess or network request was made.");return ferror(stdout)?1:0;
    }
    if(argc<2){(void)Usage();return 2;}
    enum {PLAN,STAGE,VERIFY,DIGEST} operation;
    if(strcmp(argv[1],"plan")==0)operation=PLAN;
    else if(strcmp(argv[1],"stage")==0)operation=STAGE;
    else if(strcmp(argv[1],"verify")==0)operation=VERIFY;
    else if(strcmp(argv[1],"digest")==0)operation=DIGEST;
    else {fputs("Unknown operation. Use --help.\n",stderr);return 2;}
    UmiNativeStageConfig config={0};const char *expected=NULL,*file=NULL;unsigned seen=0U;
    for(int i=2;i<argc;i+=2) {
        if(i+1>=argc||argv[i+1][0]=='\0'){fputs("Every option needs a non-empty value.\n",stderr);return 2;}
        unsigned bit;const char **slot;
        if(strcmp(argv[i],"--desk")==0){bit=1U;slot=&config.deskPath;}
        else if(strcmp(argv[i],"--launcher")==0){bit=2U;slot=&config.launcherPath;}
        else if(strcmp(argv[i],"--output")==0){bit=4U;slot=&config.outputPath;}
        else if(strcmp(argv[i],"--icon")==0){bit=8U;slot=&config.iconPath;}
        else if(strcmp(argv[i],"--expect-plan")==0){bit=16U;slot=&expected;}
        else if(strcmp(argv[i],"--file")==0){bit=32U;slot=&file;}
        else {fputs("Unknown option. Use --help.\n",stderr);return 2;}
        if((seen&bit)!=0U){fputs("Duplicate option rejected.\n",stderr);return 2;}
        seen|=bit;*slot=argv[i+1];
    }
    if((operation==VERIFY&&seen!=4U)||(operation==DIGEST&&seen!=32U)
        ||(operation==PLAN&&((seen&7U)!=7U||(seen&~15U)!=0U))
        ||(operation==STAGE&&((seen&7U)!=7U||(seen&~31U)!=0U))) {
        fputs("Options do not match this operation. Use --help.\n",stderr);return 2;
    }
    UmiNativeStageReport report={0};UmiStatus status;
    if(operation==DIGEST){char hash[65];uint64_t bytes=0U;status=UmiNativeStageDigestFile(file,hash,&bytes,&report);
        if(status==UMI_STATUS_OK)printf("%s  %s\nBytes: %" PRIu64 "\n",hash,file,bytes);
        return Result(status,&report);
    }
    if(operation==VERIFY)return Result(UmiNativeStageVerify(config.outputPath,&report),&report);
    UmiNativeStagePlan *plan=NULL;status=UmiNativeStagePlanCreate(&config,&plan,&report);
    if(status!=UMI_STATUS_OK)return Result(status,&report);
    if(operation==PLAN) {
        const UmiNativeStageManifest *m=UmiNativeStagePlanManifest(plan);
        printf("Plan fingerprint: %s\nDesk: %s\nDesk SHA-256: %s\nLauncher: %s\nLauncher SHA-256: %s\n\n%s",
            UmiNativeStagePlanFingerprint(plan),m->desk.path,m->desk.sha256,m->launcher.path,m->launcher.sha256,UmiNativeStagePlanEntry(plan));
    } else status=UmiNativeStageCommit(plan,expected,&report);
    UmiNativeStagePlanDestroy(plan);return Result(status,&report);
}

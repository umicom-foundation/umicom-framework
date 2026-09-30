/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/os_image/cli.c
 * PURPOSE:
 *   Native command-line presentation. Domain work remains in the library.
 * ORGANISATION: Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Native command-line presentation. Domain work remains in the library. */
#include "internal.h"
static void Help(void) {
    puts("Umicom native OS image tool\n" "  prepare --os ABS --framework ABS --buildroot ABS --output NEW --arch riscv64|x86_64 --git ABS\n" "  configure --output ABS --make ABS\n" "  build --output ABS --make ABS [--jobs 2]\n" "  legal-info --output ABS --make ABS\n" "  pack --output ABS --bundle NEW\n" "  verify --bundle ABS\n" "  boot --bundle ABS --qemu ABS --mode normal|recovery --result NEW [--timeout 90]\n" "  archive --file ABS\n" "  elf --file ABS --arch riscv64|x86_64\n" "  kernel --file ABS --arch riscv64|x86_64\n" "  assess --file ABS --source SHA256 --mode normal|recovery --exit NUMBER\n" "  --self-test\n" "All outputs are new paths. No disk writer, mount, network client or shell feature.\n" "prepare/configure/build run on Linux/WSL. Buildroot itself needs its normal host tools.\n" "archive accepts canonical newc or native stored-gzip; it never extracts.\n" "assess checks supplied text only; it never establishes that QEMU ran.");
}
int UmiOsImageMainWithCancellation(int argc,char **argv,const UmiCancellationToken *cancel) {
    if(argc==2&&!strcmp(argv[1],"--help")) {
        Help();
        return 0;
    }
    if(argc==2&&!strcmp(argv[1],"--self-test")) {
        UmiOsImageEntry entries[]= {
            {
                "etc",OI_DIR|0755U,0,0,NULL,0
            }
            , {
                "etc/notice",OI_REG|0444U,0,0,(const unsigned char*)"Umicom\n",7
            }
        }
        ;
        unsigned char *data=NULL;
        size_t size=0;
        UmiOsImageArchive *a=NULL;
        UmiStatus s=UmiOsImageArchiveBuild(entries,2,&data,&size);
        if(s==UMI_STATUS_OK)s=UmiOsImageArchiveOpen(data,size,&a);
        if(s==UMI_STATUS_OK&&UmiOsImageArchiveCount(a)!=2U)s=UMI_STATUS_INTERNAL_ERROR;
        free(data);
        UmiOsImageArchiveDestroy(a);
        puts(s==UMI_STATUS_OK?"Native archive self-check passed. No file, build or guest was started.":"Native archive self-check failed.");
        return s==UMI_STATUS_OK?0:1;
    }
    enum {
        OS,FW,BR,OUTPUT,ARCH,GIT,MAKE,JOBS,BUNDLE,QEMU,MODE,RESULT,TIMEOUT,FILE_PATH,SOURCE,EXIT_CODE,KEY_COUNT
    }
    ;
    const char *keys[]= {
        "--os","--framework","--buildroot","--output","--arch","--git","--make","--jobs","--bundle","--qemu","--mode","--result","--timeout","--file","--source","--exit"
    }
    ;
    const char *values[KEY_COUNT]= {
        0
    }
    ;
    uint32_t seen=0,allowed=0,required=0;
    if(argc<2) {
        Help();
        return 2;
    }
    for(int i=2;i<argc;i+=2) {
        if(i+1>=argc)return 2;
        size_t k=0;
        for(;k<KEY_COUNT;++k)if(!strcmp(argv[i],keys[k]))break;
        if(k==KEY_COUNT||(seen&(UINT32_C(1)<<k))||!argv[i+1][0])return 2;
        seen|=UINT32_C(1)<<k;
        values[k]=argv[i+1];
    }
#define B(k) (UINT32_C(1)<<(k))
    const char *cmd=argv[1];
    if(!strcmp(cmd,"prepare"))required=B(OS)|B(FW)|B(BR)|B(OUTPUT)|B(ARCH)|B(GIT);
    else if(!strcmp(cmd,"configure")||!strcmp(cmd,"legal-info"))required=B(OUTPUT)|B(MAKE);
    else if(!strcmp(cmd,"build")) {
        required=B(OUTPUT)|B(MAKE);
        allowed=B(JOBS);
    }
    else if(!strcmp(cmd,"pack"))required=B(OUTPUT)|B(BUNDLE);
    else if(!strcmp(cmd,"verify"))required=B(BUNDLE);
    else if(!strcmp(cmd,"boot")) {
        required=B(BUNDLE)|B(QEMU)|B(MODE)|B(RESULT);
        allowed=B(TIMEOUT);
    }
    else if(!strcmp(cmd,"archive"))required=B(FILE_PATH);
    else if(!strcmp(cmd,"elf")||!strcmp(cmd,"kernel"))required=B(FILE_PATH)|B(ARCH);
    else if(!strcmp(cmd,"assess"))required=B(FILE_PATH)|B(SOURCE)|B(MODE)|B(EXIT_CODE);
    else {
        Help();
        return 2;
    }
    allowed|=required;
    if((seen&required)!=required||(seen&~allowed))return 2;
#undef B
    UmiOsImageArch arch=UMI_OS_IMAGE_RISCV64;
    UmiOsImageMode mode=UMI_OS_IMAGE_NORMAL;
    if(values[ARCH]) {
        if(!strcmp(values[ARCH],"x86_64"))arch=UMI_OS_IMAGE_X86_64;
        else if(strcmp(values[ARCH],"riscv64"))return 2;
    }
    if(values[MODE]) {
        if(!strcmp(values[MODE],"recovery"))mode=UMI_OS_IMAGE_RECOVERY;
        else if(strcmp(values[MODE],"normal"))return 2;
    }
    uint64_t jobs=2,timeout=90,exitCode=0;
    if(values[JOBS]&&(!OiNumber(values[JOBS],&jobs)||jobs<1U||jobs>64U))return 2;
    if(values[TIMEOUT]&&(!OiNumber(values[TIMEOUT],&timeout)||timeout<5U||timeout>600U))return 2;
    if(values[EXIT_CODE]&&(!OiNumber(values[EXIT_CODE],&exitCode)||exitCode>255U))return 2;
    UmiOsImageReport report;
    OiInit(&report);
    UmiStatus s=UMI_STATUS_INVALID_ARGUMENT;
    if(!strcmp(cmd,"prepare"))s=UmiOsImagePrepare(values[OS],values[FW],values[BR],values[OUTPUT],arch,values[GIT],&report);
    else if(!strcmp(cmd,"configure"))s=UmiOsImageConfigure(values[OUTPUT],values[MAKE],cancel,&report);
    else if(!strcmp(cmd,"build"))s=UmiOsImageBuild(values[OUTPUT],values[MAKE],(unsigned)jobs,cancel,&report);
    else if(!strcmp(cmd,"legal-info"))s=UmiOsImageLegalInfo(values[OUTPUT],values[MAKE],cancel,&report);
    else if(!strcmp(cmd,"pack"))s=UmiOsImagePack(values[OUTPUT],values[BUNDLE],&report);
    else if(!strcmp(cmd,"verify"))s=UmiOsImageVerify(values[BUNDLE],&report);
    else if(!strcmp(cmd,"boot"))s=UmiOsImageBoot(values[BUNDLE],values[QEMU],mode,(unsigned)timeout,values[RESULT],cancel,&report);
    else {
        unsigned char *data=NULL;
        size_t size=0;
        s=OiRead(values[FILE_PATH],UMI_OS_IMAGE_MAX_ARCHIVE+16384U,&data,&size);
        if(s==UMI_STATUS_OK&&!strcmp(cmd,"archive")) {
            UmiOsImageArchive *a=NULL;
            s=UmiOsImageArchiveOpen(data,size,&a);
            if(s==UMI_STATUS_OK) {
                for(size_t i=0;i<UmiOsImageArchiveCount(a);++i) {
                    const UmiOsImageEntry *e=UmiOsImageArchiveAt(a,i);
                    printf("%06" PRIo32 " %zu %s\n",e->mode,e->size,e->name);
                }
                OiReport(&report,s,"Archive inspected without extraction or guest execution.");
            }
            UmiOsImageArchiveDestroy(a);
        }
        else if(s==UMI_STATUS_OK&&(!strcmp(cmd,"elf")||!strcmp(cmd,"kernel"))) {
            s=!strcmp(cmd,"elf")?UmiOsImageValidateElf(data,size,arch):UmiOsImageValidateKernel(data,size,arch);
            OiReport(&report,s,s==UMI_STATUS_OK?"Image structure agrees. No program or guest was executed.":"Image structure is outside the selected profile.");
        }
        else if(s==UMI_STATUS_OK)s=UmiOsImageAssessBoot(data,size,values[SOURCE],mode,(int)exitCode,&report);
        free(data);
    }
    printf("%s\nstatus=%d completed=%d process-launched=%d exit=%d\n",report.detail[0]?report.detail:"Operation stopped; check paths, schema and command arguments.",(int)s,report.completed,report.processLaunched,report.exitCode);
    if(report.sourceId[0])printf("source=%s\n",report.sourceId);
    return s==UMI_STATUS_OK?0:s==UMI_STATUS_UNAVAILABLE?77:1;
}

int UmiOsImageMain(int argc,char **argv)
{
    return UmiOsImageMainWithCancellation(argc,argv,NULL);
}

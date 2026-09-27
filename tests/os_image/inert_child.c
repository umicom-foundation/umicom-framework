/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * INERT TEST EXECUTABLE. It is neither make/Buildroot nor QEMU. It emits small
 * synthetic files/transcripts to exercise the production process adapter and
 * stage-state checks. It is never installed or distributed as a guest runtime. */
#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#endif
#include "fixtures.h"
#ifndef _WIN32
#include <time.h>
#endif
static int WriteFile(const char *root,const char *name,const void *data,size_t size) {
    CHECK(OiParents(root,name)==UMI_STATUS_OK);
    char path[UMI_OS_IMAGE_PATH];
    CHECK(OiJoin(root,name,path)==UMI_STATUS_OK);
    CHECK(OiWrite(path,data,size)==UMI_STATUS_OK);
    return 0;
}
static int MakeFixture(int argc,char **argv) {
    CHECK(argc==7);
    CHECK(!strncmp(argv[3],"O=",2));
    CHECK(!strncmp(argv[4],"BR2_EXTERNAL=",13));
    const char *root=argv[3]+2,*action=argv[6];
    if(strstr(action,"defconfig")) {
        OiText config;
        OiTextInit(&config);
        OiPrint(&config,"BR2_INIT_NONE=y\nBR2_STATIC_LIBS=y\nBR2_TOOLCHAIN_BUILDROOT_MUSL=y\nBR2_PACKAGE_UMICOM_OS_INIT=y\nBR2_PACKAGE_UMICOM_FRAMEWORK_PROBE=y\nBR2_DOWNLOAD_FORCE_CHECK_HASHES=y\nBR2_REPRODUCIBLE=y\nBR2_LINUX_KERNEL_CUSTOM_VERSION_VALUE=\"6.12.109\"\n%s=y\n",strstr(action,"riscv")?"BR2_riscv":"BR2_x86_64");
        int result=WriteFile(root,".config",config.data,config.size);
        OiTextClear(&config);
        return result;
    }
    if(!strcmp(action,"legal-info")) {
        puts("Inert legal-info fixture: no vendor build or licence collection.");
        return 0;
    }
    CHECK(!strncmp(action,"BR2_JLEVEL=",11));
    CHECK(!strncmp(argv[5],"BR2_DL_DIR=",11));
    const char *downloads=argv[5]+11;
    UmiStatus made=OiDirectory(downloads,1);
    CHECK(made==UMI_STATUS_OK||made==UMI_STATUS_ALREADY_EXISTS);
    CHECK(!WriteFile(downloads,"linux/linux-6.12.109.tar.xz",TEST_ARCHIVE_TEXT,sizeof TEST_ARCHIVE_TEXT-1U));
    unsigned char *config=NULL;
    size_t n=0;
    CHECK(OiReadJoined(root,".config",4096,&config,&n)==UMI_STATUS_OK);
    UmiOsImageArch arch=strstr((char*)config,"BR2_riscv=y")?UMI_OS_IMAGE_RISCV64:UMI_OS_IMAGE_X86_64;
    free(config);
    unsigned char elf[4096],kernel[8192];
    TestElf(elf,arch);
    TestKernel(kernel,arch);
    CHECK(!WriteFile(root,"target/init",elf,sizeof elf));
    CHECK(!WriteFile(root,"target/usr/libexec/umicom-platform-check",elf,sizeof elf));
    CHECK(!WriteFile(root,"target/usr/libexec/umicom-framework-probe",elf,sizeof elf));
    CHECK(!WriteFile(root,arch==UMI_OS_IMAGE_RISCV64?"images/Image":"images/bzImage",kernel,sizeof kernel));
    OiText t;
    OiTextInit(&t);
    OiPrint(&t,"CONFIG_BLK_DEV_INITRD=y\nCONFIG_RD_GZIP=y\nCONFIG_BINFMT_ELF=y\nCONFIG_PROC_FS=y\nCONFIG_SYSFS=y\nCONFIG_TMPFS=y\nCONFIG_DEVTMPFS=y\nCONFIG_SERIAL_8250_CONSOLE=y\n# CONFIG_MODULES is not set\n# CONFIG_NET is not set\n%s=y\n",arch==UMI_OS_IMAGE_RISCV64?"CONFIG_RISCV":"CONFIG_X86_64");
    int result=WriteFile(root,"build/linux-6.12.109/.config",t.data,t.size);
    OiTextClear(&t);
    return result;
}
static int QemuFixture(int argc,char **argv) {
    const char *initrd=NULL,*append=NULL;
    int noNetwork=0,noMonitor=0,noDisplay=0,tcg=0;
    for(int i=1;i<argc;++i) {
        if(!strcmp(argv[i],"-drive")||!strcmp(argv[i],"-hda")||!strcmp(argv[i],"-virtfs"))return 9;
        if(i+1<argc) {
            if(!strcmp(argv[i],"-initrd"))initrd=argv[i+1];
            if(!strcmp(argv[i],"-append"))append=argv[i+1];
            if(!strcmp(argv[i],"-nic")&&!strcmp(argv[i+1],"none"))noNetwork=1;
            if(!strcmp(argv[i],"-monitor")&&!strcmp(argv[i+1],"none"))noMonitor=1;
            if(!strcmp(argv[i],"-display")&&!strcmp(argv[i+1],"none"))noDisplay=1;
            if(!strcmp(argv[i],"-accel")&&!strcmp(argv[i+1],"tcg"))tcg=1;
        }
    }
    CHECK(initrd&&append&&noNetwork&&noMonitor&&noDisplay&&tcg&&strstr(append,"umicom.autopoweroff=1"));
    unsigned char *data=NULL;
    size_t size=0;
    CHECK(OiRead(initrd,UMI_OS_IMAGE_MAX_ARCHIVE+16384U,&data,&size)==UMI_STATUS_OK);
    UmiOsImageArchive *archive=NULL;
    CHECK(UmiOsImageArchiveOpen(data,size,&archive)==UMI_STATUS_OK);
    char source[65]= {
        0
    }
    ;
    for(size_t i=0;i<UmiOsImageArchiveCount(archive);++i) {
        const UmiOsImageEntry *entry=UmiOsImageArchiveAt(archive,i);
        if(!strcmp(entry->name,"etc/umicom/source-id")) {
            CHECK(entry->size==65U);
            memcpy(source,entry->data,64);
        }
    }
    CHECK(OiHash(source,64));
    OiText t;
    OiTextInit(&t);
    TestTranscript(&t,source,strstr(append,"umicom.recovery=1")!=NULL);
    CHECK(fwrite(t.data,1,t.size,stdout)==t.size);
    OiTextClear(&t);
    UmiOsImageArchiveDestroy(archive);
    free(data);
    return 0;
}
int main(int argc,char **argv) {
    if(argc>1&&!strcmp(argv[1],"-C"))return MakeFixture(argc,argv);
    if(argc>1&&!strcmp(argv[1],"-M"))return QemuFixture(argc,argv);
    if(argc==2&&!strcmp(argv[1],"--fail"))return 17;
    if(argc==2&&!strcmp(argv[1],"--large")) {
        for(unsigned i=0;i<70000U;++i)puts("Inert test output abcdefghijklmnopqrstuvwxyz 01234567890123456789");
        return 0;
    }
#ifndef _WIN32
    if(argc==2&&!strcmp(argv[1],"--wait")) {
        struct timespec t= {
            5,0
        }
        ;
        (void)nanosleep(&t,NULL);
        return 0;
    }
#endif
    for(int i=1;i<argc;++i)printf("%zu:%s\n",strlen(argv[i]),argv[i]);
    return 0;
}

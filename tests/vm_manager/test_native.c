/*-----------------------------------------------------------------------------
 * Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Actual process/filesystem/Data Server integration with explicitly inert
 * native peers. Synthetic kernel/disk bytes are never guest-boot evidence.
 *---------------------------------------------------------------------------*/
#define _GNU_SOURCE
#include "../../src/vm_manager/internal.h"
#include "../../src/os_image/internal.h"
#include "../../src/setup_centre/internal.h"
#include "umicom/vm_manager/qmp_channel.h"
#include <unistd.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <time.h>
#define CHECK(c) do{if(!(c)){fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#c);return 1;}}while(0)
#define OK(c) do{UmiStatus _s=(c);if(_s!=UMI_STATUS_OK){fprintf(stderr,"%d: %s returned %d\n",__LINE__,#c,(int)_s);return 1;}}while(0)
static int Put(const char*r,const char*n,const void*d,size_t z){
    char p[4096];
    OK(UmiSetupFileParents(r,n,NULL));
    OK(UmiSetupPathJoin(r,n,p));
    OK(UmiSetupFileWriteNew(p,d,z,NULL));
    return 0;
}
static int Change(const char*p,const void*d,size_t n){
    FILE*f=fopen(p,"wb");
    CHECK(f);
    CHECK(fwrite(d,1,n,f)==n);
    CHECK(!fclose(f));
    return 0;
}
static int Copy(const char*from,const char*r,const char*n){
    char p[4096],h[65];
    uint64_t z;
    OK(UmiSetupFileDigest(from,h,&z,NULL));
    OK(UmiSetupFileParents(r,n,NULL));
    OK(UmiSetupPathJoin(r,n,p));
    OK(UmiSetupFileCopyChecked(from,p,h,z,NULL));
    return 0;
}
static int Runtime(const char*r,const char*child,char out[4096],int windows){
    char source[4096],inventory[4096];
    OK(UmiSetupPathJoin(r,"publisher-input",source));
    OK(UmiSetupDirectoryCreate(source,NULL));
    CHECK(!Copy(child,source,"bin/qemu-system"));
    CHECK(!Copy(child,source,"bin/qemu-img"));
    CHECK(!Put(source,"firmware/README","Inert firmware fixture\n",23));
    CHECK(!Put(source,"COPYING","Fixture licence notice\n",23));
    CHECK(!Put(source,"SOURCE","Fixture source provenance\n",26));
    char text[2048];
    int n=snprintf(text,sizeof text,"UMICOM_QEMU_INPUT\t1\nhost\t%s\narchitecture\t1\nversion\tINERT TEST FIXTURE\nemulator\tbin/qemu-system\nimage-tool\tbin/qemu-img\nfirmware\tfirmware\nlicence\tCOPYING\nsource-notice\tSOURCE\nfile\tbin/qemu-system\nfile\tbin/qemu-img\nfile\tfirmware/README\nfile\tCOPYING\nfile\tSOURCE\n",windows?"windows-amd64":VmHost());
    CHECK(n>0&&(size_t)n<sizeof text);
    OK(UmiSetupPathJoin(r,"publisher.tsv",inventory));
    OK(UmiSetupFileWriteNew(inventory,text,(size_t)n,NULL));
    OK(UmiSetupPathJoin(r,"sealed-runtime",out));
    UmiVmReport report;
    OK(UmiVmRuntimePack(source,inventory,out,&report));
    OK(UmiVmRuntimeVerify(out,&report));
    return 0;
}
static void Le16(unsigned char*p,unsigned v){
    p[0]=(unsigned char)v;
    p[1]=(unsigned char)(v>>8);
}
static void Le64(unsigned char*p,uint64_t v){
    for(unsigned i=0;i<8;++i)p[i]=(unsigned char)(v>>(i*8U));
}
static int Bundle(const char*r,char out[4096]){
    char target[4096],overlay[4096],kernel[4096];
    OK(OiJoin(r,"target-fixture",target));
    OK(OiDirectory(target,1));
    OK(OiJoin(r,"overlay-fixture",overlay));
    OK(OiDirectory(overlay,1));
    OK(OiJoin(r,"synthetic-kernel",kernel));
    unsigned char elf[4096]={
        0
    },k[8192]={
        0
    };
    memcpy(elf,"\177ELF\2\1\1",7);
    Le16(elf+16,2);
    Le16(elf+18,62);
    OiPut32(elf+20,1);
    Le64(elf+24,0x400100);
    Le64(elf+32,64);
    Le16(elf+52,64);
    Le16(elf+54,56);
    Le16(elf+56,1);
    OiPut32(elf+64,1);
    OiPut32(elf+68,5);
    Le64(elf+80,0x400000);
    Le64(elf+96,4096);
    Le64(elf+104,4096);
    Le64(elf+112,4096);
    k[0x1f1]=4;
    k[0x1fe]=0x55;
    k[0x1ff]=0xaa;
    memcpy(k+0x202,"HdrS",4);
    Le16(k+0x206,0x20c);
    Le16(k+0x236,1);
    OK(OiWrite(kernel,k,sizeof k));
    const char*programs[]={
        "init","usr/libexec/umicom-framework-probe","usr/libexec/umicom-platform-check"
    };
    OiText evidence;
    OiTextInit(&evidence);
    char hash[65];
    OK(UmiNativeSha256Buffer(elf,sizeof elf,hash));
    for(size_t i=0;i<3;++i){
        CHECK(!Put(target,programs[i],elf,sizeof elf));
        OiPrint(&evidence,"file\t%s\t%zu\tbuild/target/%s\n",hash,sizeof elf,programs[i]);
    }
    OK(UmiNativeSha256Buffer(k,sizeof k,hash));
    OiPrint(&evidence,"file\t%s\t%zu\tbuild/images/bzImage\n",hash,sizeof k);
    const char*names[]={
        "etc/group","etc/os-release","etc/passwd","etc/shadow","etc/umicom/boot.conf"
    };
    const char*bytes="inert test configuration\n";
    OK(UmiNativeSha256Buffer(bytes,strlen(bytes),hash));
    OiText manifest;
    OiTextInit(&manifest);
    OiPrint(&manifest,"UMICOM_OS_INPUTS\t1\narch\tx86_64\nbuildroot\t0123456789abcdef0123456789abcdef01234567\nlinux\t6.12.109\nlinux-sha256\t%s\n",hash);
    for(size_t i=0;i<5;++i){
        CHECK(!Put(overlay,names[i],bytes,strlen(bytes)));
        OiPrint(&manifest,"file\t%s\t%zu\tumicomOS/rootfs/foundation/%s\n",hash,strlen(bytes),names[i]);
    }
    OiPlan*p=calloc(1,sizeof *p);
    CHECK(p);
    OK(OiPlanDecode(manifest.data,manifest.size,p));
    OK(OiJoin(r,"image-bundle",out));
    UmiOsImageReport report;
    UmiStatus status=OiPackFiles(target,overlay,kernel,manifest.data,manifest.size,p,&evidence,out,&report);
    free(p);
    OiTextClear(&manifest);
    OiTextClear(&evidence);
    OK(status);
    OK(UmiOsImageVerify(out,&report));
    return 0;
}
static int Protocol(const char*test,const char*root,const char*child){
    const char*mode=test;
    UmiProcessChannel*c=NULL;
    UmiProcessChannelRequest req={
        child,&mode,1,root
    };
    OK(UmiProcessChannelOpen(&req,&c));
    UmiVmSession*s=NULL;
    UmiVmReport r;
    UmiStatus status=UmiVmQmpAdoptChannel(c,&s,&r);
    if(!strcmp(test,"bad-greeting")||!strcmp(test,"negotiate-error")||!strcmp(test,"query-timeout")||!strcmp(test,"oversize")||!strcmp(test,"malformed")){
        CHECK(status!=UMI_STATUS_OK&&!s);
        UmiProcessChannelDestroy(c);
        return 0;
    }
    OK(status);
    c=NULL;
    UmiVmSnapshot state;
    OK(UmiVmObserve(s,&state));
    CHECK(!state.guestRunning&&state.controlAvailable&&state.processRunning);
    unsigned char data[2048];
    size_t n;
    if(!strcmp(test,"resume-error")){
        CHECK(UmiVmControl(s,UMI_VM_RESUME,NULL,0,data,sizeof data,&n,&r)==UMI_STATUS_INVALID_STATE);
        OK(UmiVmControl(s,UMI_VM_QUERY,NULL,0,data,sizeof data,&n,&r));
        OK(UmiVmObserve(s,&state));
        CHECK(!state.guestRunning&&state.controlAvailable);
    }
    else if(!strcmp(test,"console-type")){
        CHECK(UmiVmControl(s,UMI_VM_CONSOLE_READ,NULL,0,data,sizeof data,&n,&r)==UMI_STATUS_PARSE_ERROR);
        OK(UmiVmObserve(s,&state));
        CHECK(!state.controlAvailable);
    }
    else{
        OK(UmiVmControl(s,UMI_VM_RESUME,NULL,0,data,sizeof data,&n,&r));
        OK(UmiVmObserve(s,&state));
        CHECK(state.guestRunning&&!strcmp(state.state,"running"));
        OK(UmiVmControl(s,UMI_VM_PAUSE,NULL,0,data,sizeof data,&n,&r));
        OK(UmiVmObserve(s,&state));
        CHECK(!state.guestRunning);
        OK(UmiVmControl(s,UMI_VM_CONSOLE_READ,NULL,0,data,sizeof data,&n,&r));
        CHECK(n==21&&!memcmp(data,"Umicom Notes fixture\n",21));
        const char input[]="poweroff\n\"\\{}";
        OK(UmiVmControl(s,UMI_VM_CONSOLE_WRITE,input,sizeof input-1,data,sizeof data,&n,&r));
        OK(UmiVmControl(s,UMI_VM_POWERDOWN,NULL,0,data,sizeof data,&n,&r));
        OK(UmiVmObserve(s,&state));
        CHECK(state.shutdownRequested&&state.processRunning);
        OK(UmiVmControl(s,UMI_VM_QUIT,NULL,0,data,sizeof data,&n,&r));
    }
    UmiVmSessionDestroy(s);
    return 0;
}
static int Channel(const char*test,const char*root,const char*child){
    UmiProcessChannel*c=NULL;
    const char*args[]={
        "arguments","space here","quote\" slash\\","\xc3\xa9"
    };
    UmiProcessChannelRequest req={
        child,args,4,root
    };
    if(!strcmp(test,"channel_script")){
        char p[4096];
        OK(UmiSetupPathJoin(root,"script",p));
        OK(UmiSetupFileWriteNew(p,"#!/bin/sh\nexit 0\n",17,NULL));
        CHECK(!chmod(p,0700));
        req.program=p;
        CHECK(UmiProcessChannelOpen(&req,&c)!=UMI_STATUS_OK&&!c);
        return 0;
    }
    if(!strcmp(test,"channel_missing")){
        req.program="/does/not/exist";
        CHECK(UmiProcessChannelOpen(&req,&c)==UMI_STATUS_UNAVAILABLE);
        return 0;
    }
    if(!strcmp(test,"channel_arguments")){
        CHECK(!setenv("UMICOM_INJECTED_SECRET","must not inherit",1));
        OK(UmiProcessChannelOpen(&req,&c));
        char data[2048];
        size_t used=0,n=0;
        for(;;){
            OK(UmiProcessChannelRead(c,data+used,sizeof data-1-used,&n,3000));
            if(!n)break;
            used+=n;
        }
        data[used]=0;
        CHECK(strstr(data,"10:space here")&&strstr(data,"injected=absent"));
        UmiProcessChannelDestroy(c);
        return 0;
    }
    const char*mode=!strcmp(test,"channel_descendants")?"descendant":"silent";
    req.arguments=&mode;
    req.argumentCount=1;
    OK(UmiProcessChannelOpen(&req,&c));
    if(!strcmp(test,"channel_descendants")){
        char data[128];
        size_t n;
        OK(UmiProcessChannelRead(c,data,sizeof data-1,&n,3000));
        CHECK(n>0);
        data[n]=0;
        pid_t childPid=(pid_t)strtol(data,NULL,10);
        CHECK(childPid>1);
        for(unsigned i=0;i<100;++i){
            UmiProcessChannelSnapshot snapshot;
            OK(UmiProcessChannelPoll(c,&snapshot));
            if(!snapshot.running)break;
            usleep(10000);
        }
        UmiProcessChannelDestroy(c);
        /* A terminated descendant can remain a zombie awaiting the host's init. */
        char path[128];
        snprintf(path,sizeof path,"/proc/%ld/stat",(long)childPid);
        int stopped=0;
        for(unsigned attempt=0;attempt<100U;++attempt){
            FILE*f=fopen(path,"r");
            if(!f){
                stopped=1;
                break;
            }
            char line[512];
            char*got=fgets(line,sizeof line,f);
            fclose(f);
            if(got){
                char*end=strrchr(line,')');
                if(end&&end[2]=='Z'){
                    stopped=1;
                    break;
                }
            }
            usleep(10000);
        }
        CHECK(stopped);
        return 0;
    }
    char byte;
    size_t n;
    CHECK(UmiProcessChannelRead(c,&byte,1,&n,30)==UMI_STATUS_TIMEOUT&&n==0);
    OK(UmiProcessChannelTerminate(c));
    UmiProcessChannelSnapshot snapshot;
    OK(UmiProcessChannelPoll(c,&snapshot));
    CHECK(!snapshot.running&&snapshot.terminated);
    UmiProcessChannelDestroy(c);
    return 0;
}
static int Files(const char*test,const char*r,const char*child){
    char runtime[4096],path[4096];
    CHECK(!Runtime(r,child,runtime,!strcmp(test,"component")));
    UmiVmReport report;
    if(!strcmp(test,"runtime_integrity")){
        OK(UmiVmRuntimeVerify(runtime,&report));
        CHECK(strlen(report.fingerprint)==64);
        return 0;
    }
    if(!strcmp(test,"runtime_extra")){
        CHECK(!Put(runtime,"bin/injected.dll","unexpected",10));
        CHECK(UmiVmRuntimeVerify(runtime,&report)==UMI_STATUS_INVALID_STATE);
        return 0;
    }
    if(!strcmp(test,"runtime_tamper")){
        OK(UmiSetupPathJoin(runtime,"SOURCE",path));
        CHECK(!Change(path,"changed",7));
        CHECK(UmiVmRuntimeVerify(runtime,&report)==UMI_STATUS_INVALID_STATE);
        return 0;
    }
    if(!strcmp(test,"runtime_symlink")){
        OK(UmiSetupPathJoin(runtime,"extra",path));
        CHECK(!symlink("SOURCE",path));
        CHECK(UmiVmRuntimeVerify(runtime,&report)==UMI_STATUS_PERMISSION_DENIED);
        return 0;
    }
    if(!strcmp(test,"component")){
        OK(UmiSetupPathJoin(r,"component.tsv",path));
        OK(UmiVmRuntimeComponent(runtime,path,&report));
        char*text=NULL;
        size_t n;
        OK(UmiSetupFileRead(path,100000,&text,&n,NULL));
        CHECK(strstr(text,"owned\tumicom-vm-manager\tshare/umicom/qemu/bin/qemu-system\t"));
        free(text);
        CHECK(UmiVmRuntimeComponent(runtime,path,&report)==UMI_STATUS_ALREADY_EXISTS);
        return 0;
    }
    char disk[4096],copy[4096];
    OK(UmiSetupPathJoin(r,"disk",disk));
    OK(UmiSetupPathJoin(r,"checkpoint",copy));
    OK(UmiVmDiskCreate(runtime,disk,UINT64_C(8388608),&report));
    uint64_t virtualSize;
    OK(VmDiskValidate(disk,&virtualSize));
    CHECK(virtualSize==8388608);
    if(!strcmp(test,"disk_existing")){
        CHECK(UmiVmDiskCreate(runtime,disk,8388608,&report)==UMI_STATUS_ALREADY_EXISTS);
        return 0;
    }
    if(!strcmp(test,"disk_lease")){
        void*lease=NULL;
        OK(VmDiskLease(disk,&lease));
        CHECK(UmiVmDiskCheckpoint(runtime,disk,copy,&report)==UMI_STATUS_BUSY);
        VmDiskUnlease(lease);
        return 0;
    }
    if(!strcmp(test,"disk_backing")){
        OK(UmiSetupPathJoin(disk,"disk.qcow2",path));
        FILE*f=fopen(path,"r+b");
        CHECK(f);
        CHECK(!fseek(f,15,SEEK_SET));
        CHECK(fputc(1,f)==1);
        CHECK(!fclose(f));
        CHECK(VmDiskValidate(disk,&virtualSize)==UMI_STATUS_INVALID_STATE);
        return 0;
    }
    if(!strcmp(test,"disk_checkpoint")){
        OK(UmiVmDiskCheckpoint(runtime,disk,copy,&report));
        char a[4096],b[4096],ha[65],hb[65];
        uint64_t na,nb;
        OK(UmiSetupPathJoin(disk,"disk.qcow2",a));
        OK(UmiSetupPathJoin(copy,"disk.qcow2",b));
        OK(UmiSetupFileDigest(a,ha,&na,NULL));
        OK(UmiSetupFileDigest(b,hb,&nb,NULL));
        CHECK(na==nb&&!strcmp(ha,hb));
        return 0;
    }
    return 2;
}
static int Launch(const char*test,const char*r,const char*child){
    char runtime[4096],image[4096],run[4096];
    CHECK(!Runtime(r,child,runtime,0));
    CHECK(!Bundle(r,image));
    OK(UmiSetupPathJoin(r,"run",run));
    UmiVmProfile p;
    UmiVmProfileInit(&p);
    strcpy(p.id,"notes-lab");
    strcpy(p.name,"Inert launch integration");
    CHECK(strlen(runtime)<sizeof p.runtimeDirectory&&strlen(image)<sizeof p.imageBundle);
    strcpy(p.runtimeDirectory,runtime);
    strcpy(p.imageBundle,image);
    UmiVmReport report;
    OK(UmiVmReview(&p,run,&report));
    char fingerprint[65];
    strcpy(fingerprint,report.fingerprint);
    UmiVmSession*s=NULL;
    if(!strcmp(test,"launch_stale")){
        p.memoryMiB=2048;
        CHECK(UmiVmStart(&p,run,fingerprint,&s,&report)==UMI_STATUS_INVALID_STATE);
        CHECK(!s&&!report.outputCreated);
        return 0;
    }
    if(!strcmp(test,"launch_changed_runtime")){
        char path[4096];
        OK(UmiSetupPathJoin(runtime,"SOURCE",path));
        CHECK(!Change(path,"changed",7));
        CHECK(UmiVmStart(&p,run,fingerprint,&s,&report)==UMI_STATUS_INVALID_STATE);
        CHECK(!s&&!report.processLaunched);
        return 0;
    }
    if(!strcmp(test,"launch_overlap")){
        CHECK(UmiVmReview(&p,runtime,&report)==UMI_STATUS_INVALID_ARGUMENT);
        return 0;
    }
    OK(UmiVmStart(&p,run,fingerprint,&s,&report));
    CHECK(s&&report.processLaunched&&report.outputCreated);
    UmiVmSnapshot state;
    OK(UmiVmObserve(s,&state));
    CHECK(!state.guestRunning&&state.controlAvailable);
    unsigned char output[2048];
    size_t used;
    OK(UmiVmControl(s,UMI_VM_RESUME,NULL,0,output,sizeof output,&used,&report));
    OK(UmiVmControl(s,UMI_VM_PAUSE,NULL,0,output,sizeof output,&used,&report));
    OK(UmiVmControl(s,UMI_VM_QUIT,NULL,0,output,sizeof output,&used,&report));
    UmiVmSessionDestroy(s);
    CHECK(UmiVmReview(&p,run,&report)==UMI_STATUS_ALREADY_EXISTS);
    puts("Inert process/QMP launch integration passed; no QEMU or guest was executed.");
    return 0;
}
int main(int argc,char**argv){
    CHECK(argc==3);
    char root[]="/tmp/umicom-vm-test-XXXXXX";
    CHECK(mkdtemp(root));
    const char*test=argv[1];
    if(!strncmp(test,"channel_",8))return Channel(test,root,argv[2]);
    if(!strncmp(test,"runtime_",8)||!strncmp(test,"disk_",5)||!strcmp(test,"component"))return Files(test,root,argv[2]);
    if(!strncmp(test,"launch_",7))return Launch(test,root,argv[2]);
    return Protocol(test,root,argv[2]);
}

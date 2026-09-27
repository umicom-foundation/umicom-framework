/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * The only disk format attached here is a standalone qcow2 in a managed
 * directory. No host device, backing chain, encryption or external data file.
 * Cold checkpoints create independent copies; they never overwrite a disk. */
#include "internal.h"
static uint32_t Be32(const unsigned char*p){
    return ((uint32_t)p[0]<<24)|((uint32_t)p[1]<<16)|((uint32_t)p[2]<<8)|p[3];
}
static uint64_t Be64(const unsigned char*p){
    return ((uint64_t)Be32(p)<<32)|Be32(p+4);
}
UmiStatus VmDiskValidate(const char*root,uint64_t*bytes){
    if(!VmPath(root,0)||!bytes)return UMI_STATUS_INVALID_ARGUMENT;
    char path[UMI_SETUP_PATH_CAPACITY];
    unsigned char header[104];
    size_t n=0;
    UmiStatus s=UmiSetupPathJoin(root,"disk.qcow2",path);
    if(s==UMI_STATUS_OK)s=UmiSetupFileSingleLink(path,NULL);
    if(s==UMI_STATUS_OK)s=UmiSetupFileReadHeader(path,header,sizeof header,&n,NULL);
    if(s!=UMI_STATUS_OK)return s;
    if(n!=104U||Be32(header)!=UINT32_C(0x514649fb)||Be32(header+4)!=3U||Be64(header+8)||Be32(header+16)||Be32(header+20)<12U||Be32(header+20)>21U||Be32(header+32)||Be64(header+72)||Be32(header+96)!=4U||Be32(header+100)<104U)return UMI_STATUS_INVALID_STATE;
    *bytes=Be64(header+24);
    if(*bytes<UINT64_C(1048576)||*bytes>UINT64_C(137438953472)||*bytes%512U)return UMI_STATUS_CAPACITY_EXCEEDED;
    char *record=NULL;
    size_t size=0;
    s=VmReadJoined(root,"disk.umi",512,&record,&size);
    if(s==UMI_STATUS_OK){
        char expected[256];
        int count=snprintf(expected,sizeof expected,"UMICOM_VM_DISK\t1\nformat\tqcow2\nvirtual-bytes\t%" PRIu64 "\n",*bytes);
        if(count<0||(size_t)count!=size||memcmp(record,expected,size))s=UMI_STATUS_INVALID_STATE;
    }
    free(record);
    return s;
}
static UmiStatus Record(const char*root,uint64_t n){
    char bytes[256];
    int count=snprintf(bytes,sizeof bytes,"UMICOM_VM_DISK\t1\nformat\tqcow2\nvirtual-bytes\t%" PRIu64 "\n",n);
    return count<0||(size_t)count>=sizeof bytes?UMI_STATUS_INTERNAL_ERROR:VmWriteJoined(root,"disk.umi",bytes,(size_t)count);
}
UmiStatus VmDiskRun(const char*root,const char*cwd,const char*const*args,size_t count,UmiVmReport*r){
    VmRuntime runtime;
    UmiStatus s=VmRuntimeRead(root,&runtime,1);
    char program[UMI_SETUP_PATH_CAPACITY];
    if(s==UMI_STATUS_OK&&strcmp(runtime.host,VmHost()))s=UMI_STATUS_UNAVAILABLE;
    if(s==UMI_STATUS_OK)s=UmiSetupPathJoin(root,runtime.imageTool,program);
    if(s==UMI_STATUS_OK)s=UmiSetupNativeProgramCheck(program,NULL);
    if(s==UMI_STATUS_OK){
        /* Disk tools use the same private environment and process lifetime as
                                 * the interactive provider. Never inherit LD_PRELOAD or a developer PATH. */
        UmiProcessChannelRequest request={
            program,args,count,cwd
        };
        UmiProcessChannel *channel=NULL;
        s=UmiProcessChannelOpen(&request,&channel);
        if(s==UMI_STATUS_OK&&r)r->processLaunched=1;
        uint64_t deadline=VmClock()+60000U;
        size_t captured=0;
        while(s==UMI_STATUS_OK){
            unsigned char output[4096];
            size_t used=0;
            UmiStatus read=UmiProcessChannelRead(channel,output,sizeof output,&used,100);
            if(read!=UMI_STATUS_OK&&read!=UMI_STATUS_TIMEOUT){
                s=read;
                break;
            }
            if(used>65536U-captured){
                s=UMI_STATUS_CAPACITY_EXCEEDED;
                break;
            }
            captured+=used;
            UmiProcessChannelSnapshot observation;
            s=UmiProcessChannelPoll(channel,&observation);
            if(s!=UMI_STATUS_OK)break;
            if(!observation.running){
                if(observation.exitCode||observation.diagnosticsTruncated)s=UMI_STATUS_INVALID_STATE;
                break;
            }
            if(VmClock()>=deadline){
                s=UMI_STATUS_TIMEOUT;
                break;
            }
        }
        UmiProcessChannelDestroy(channel);
    }
    VmRuntimeFree(&runtime);
    return s;
}
UmiStatus UmiVmDiskCreate(const char*runtime,const char*destination,uint64_t bytes,UmiVmReport*r){
    if(r)memset(r,0,sizeof *r);
    if(!VmPath(runtime,0)||!VmPath(destination,0)||bytes<UINT64_C(1048576)||bytes>UINT64_C(137438953472)||bytes%512U)return VmReport(r,UMI_STATUS_INVALID_ARGUMENT,"Choose a new managed directory and a whole-sector capacity from 1 MiB to 128 GiB.");
    UmiStatus s=UmiVmRuntimeVerify(runtime,r);
    if(s==UMI_STATUS_OK)s=UmiSetupDirectoryCreate(destination,NULL);
    if(s==UMI_STATUS_OK&&r)r->outputCreated=1;
    void*lease=NULL;
    if(s==UMI_STATUS_OK)s=VmDiskLease(destination,&lease);
    char capacity[32];
    snprintf(capacity,sizeof capacity,"%" PRIu64,bytes);
    const char*args[]={
        "create","-q","-f","qcow2","-o","compat=1.1,lazy_refcounts=off","disk.qcow2",capacity
    };
    if(s==UMI_STATUS_OK)s=VmDiskRun(runtime,destination,args,sizeof args/sizeof args[0],r);
    if(s==UMI_STATUS_OK)s=Record(destination,bytes);
    uint64_t actual=0;
    if(s==UMI_STATUS_OK)s=VmDiskValidate(destination,&actual);
    if(s==UMI_STATUS_OK&&actual!=bytes)s=UMI_STATUS_INVALID_STATE;
    VmDiskUnlease(lease);
    return VmReport(r,s,s==UMI_STATUS_OK?"Blank managed qcow2 created. No partition, filesystem or guest installation was created.":"Disk creation stopped. A new partial directory may remain; no existing destination was replaced.");
}
UmiStatus UmiVmDiskCheckpoint(const char*runtime,const char*source,const char*destination,UmiVmReport*r){
    if(r)memset(r,0,sizeof *r);
    if(!VmPath(runtime,0)||!VmPath(source,0)||!VmPath(destination,0)||!strcmp(source,destination))return UMI_STATUS_INVALID_ARGUMENT;
    void*lease=NULL;
    UmiStatus s=VmDiskLease(source,&lease);
    uint64_t capacity=0,size=0;
    char from[UMI_SETUP_PATH_CAPACITY],to[UMI_SETUP_PATH_CAPACITY],hash[65];
    if(s==UMI_STATUS_OK)s=VmDiskValidate(source,&capacity);
    const char*args[]={
        "check","-f","qcow2","disk.qcow2"
    };
    if(s==UMI_STATUS_OK)s=VmDiskRun(runtime,source,args,4,r);
    if(s==UMI_STATUS_OK)s=UmiSetupPathJoin(source,"disk.qcow2",from);
    if(s==UMI_STATUS_OK)s=UmiSetupFileDigest(from,hash,&size,NULL);
    if(s==UMI_STATUS_OK)s=UmiSetupDirectoryCreate(destination,NULL);
    if(s==UMI_STATUS_OK&&r)r->outputCreated=1;
    if(s==UMI_STATUS_OK)s=UmiSetupPathJoin(destination,"disk.qcow2",to);
    if(s==UMI_STATUS_OK)s=UmiSetupFileCopyChecked(from,to,hash,size,NULL);
    if(s==UMI_STATUS_OK)s=Record(destination,capacity);
    if(s==UMI_STATUS_OK&&r)strcpy(r->fingerprint,hash);
    VmDiskUnlease(lease);
    return VmReport(r,s,s==UMI_STATUS_OK?"Cold disk checkpoint copied to a new directory. It contains no RAM/CPU state; guest filesystem consistency is a separate check.":"Checkpoint refused or incomplete. Stop managed disk users and retain any partial output for inspection.");
}

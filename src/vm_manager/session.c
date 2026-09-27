/*-----------------------------------------------------------------------------
 * Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * QEMU adapter and supervised session. There is no public TCP monitor, arbitrary
 * QMP command box, HMP string injection, automatic reconnect or unknown retry.
 * A command result must match its ID; observed guest state comes from QMP.
 *---------------------------------------------------------------------------*/
#include "internal.h"
#include "umicom/os_image/bundle_info.h"
#include "umicom/vm_manager/qmp_channel.h"
#include <limits.h>
struct UmiVmSession {
    UmiProcessChannel *channel;
    UmiVmSnapshot snapshot;
    unsigned char buffer[UMI_VM_QMP_FRAME];
    size_t used;
    uint64_t nextId;
    void *diskLease;
    char runDirectory[UMI_VM_PATH];
};
static UmiStatus Frame(UmiVmSession*s,UmiVmQmpMessage*m,uint64_t deadline){
    for(;;){
        unsigned char *end=memchr(s->buffer,'\n',s->used);
        if(end){
            size_t length=(size_t)(end-s->buffer)+1U;
            UmiStatus result=UmiVmQmpDecode(s->buffer,length,m);
            memmove(s->buffer,s->buffer+length,s->used-length);
            s->used-=length;
            return result;
        }
        if(s->used==sizeof s->buffer)return UMI_STATUS_CAPACITY_EXCEEDED;
        uint64_t now=VmClock();
        if(now>=deadline)return UMI_STATUS_TIMEOUT;
        size_t n=0;
        unsigned remaining=(unsigned)(deadline-now);
        if(remaining>100U)remaining=100U;
        UmiStatus result=UmiProcessChannelRead(s->channel,s->buffer+s->used,sizeof s->buffer-s->used,&n,remaining);
        if(result==UMI_STATUS_TIMEOUT)continue;
        if(result!=UMI_STATUS_OK)return result;
        if(!n)return UMI_STATUS_IO_ERROR;
        s->used+=n;
    }
}
static UmiStatus Exchange(UmiVmSession*s,UmiVmCommand c,const void*data,size_t n,UmiVmQmpMessage*out){
    if(!s->snapshot.controlAvailable&&c!=UMI_VM_NEGOTIATE)return UMI_STATUS_INVALID_STATE;
    if(s->nextId==INT64_MAX)return UMI_STATUS_CAPACITY_EXCEEDED;
    uint64_t id=++s->nextId;
    char frame[4096];
    size_t size=0;
    UmiStatus result=UmiVmQmpEncode(c,id,data,n,frame,sizeof frame,&size);
    if(result!=UMI_STATUS_OK)return result;
    result=UmiProcessChannelWrite(s->channel,frame,size,2500);
    if(result!=UMI_STATUS_OK)goto lost;
    uint64_t deadline=VmClock()+2500U;
    for(unsigned received=0;received<128U;++received){
        result=Frame(s,out,deadline);
        if(result!=UMI_STATUS_OK)goto lost;
        if(out->kind==UMI_VM_QMP_EVENT){
            ++s->snapshot.events;
            strcpy(s->snapshot.lastEvent,out->event);
            continue;
        }
        if(!out->hasId){
            result=UMI_STATUS_PARSE_ERROR;
            goto lost;
        }
        if(out->id!=id)continue;
        if(out->kind==UMI_VM_QMP_ERROR){
            snprintf(s->snapshot.lastError,sizeof s->snapshot.lastError,"%.90s: %.400s",out->errorClass,out->description);
            return UMI_STATUS_INVALID_STATE;
        }
        if(out->kind!=UMI_VM_QMP_RETURN){
            result=UMI_STATUS_PARSE_ERROR;
            goto lost;
        }
        if((c==UMI_VM_CONSOLE_READ&&!out->returnIsString)||(c!=UMI_VM_CONSOLE_READ&&!out->returnIsObject)){
            result=UMI_STATUS_PARSE_ERROR;
            goto lost;
        }
        ++s->snapshot.commands;
        s->snapshot.lastError[0]=0;
        return UMI_STATUS_OK;
    }
    result=UMI_STATUS_CAPACITY_EXCEEDED;
    lost:     s->snapshot.controlAvailable=0;
    snprintf(s->snapshot.lastError,sizeof s->snapshot.lastError,"QMP control lost (%s). No command will be replayed automatically.",UmiSetupStatusText(result));
    return result;
}
static UmiStatus Query(UmiVmSession*s){
    UmiVmQmpMessage m;
    UmiStatus result=Exchange(s,UMI_VM_QUERY,NULL,0,&m);
    if(result==UMI_STATUS_OK){
        if(!m.hasRunning||!m.state[0]){
            s->snapshot.controlAvailable=0;
            return UMI_STATUS_PARSE_ERROR;
        }
        s->snapshot.guestRunning=m.running;
        strcpy(s->snapshot.state,m.state);
    }
    return result;
}
UmiStatus UmiVmQmpAdoptChannel(UmiProcessChannel*c,UmiVmSession**out,UmiVmReport*r){
    if(!c||!out)return UMI_STATUS_INVALID_ARGUMENT;
    *out=NULL;
    UmiVmSession*s=calloc(1,sizeof *s);
    if(!s)return UMI_STATUS_OUT_OF_MEMORY;
    s->channel=c;
    UmiVmQmpMessage m;
    UmiStatus result=Frame(s,&m,VmClock()+5000U);
    if(result==UMI_STATUS_OK&&m.kind!=UMI_VM_QMP_GREETING)result=UMI_STATUS_PARSE_ERROR;
    if(result==UMI_STATUS_OK){
        snprintf(s->snapshot.qemuVersion,sizeof s->snapshot.qemuVersion,"%u.%u.%u",m.versionMajor,m.versionMinor,m.versionMicro);
        result=Exchange(s,UMI_VM_NEGOTIATE,NULL,0,&m);
    }
    if(result==UMI_STATUS_OK){
        s->snapshot.controlAvailable=1;
        result=Query(s);
    }
    if(result==UMI_STATUS_OK){
        *out=s;
        return VmReport(r,UMI_STATUS_OK,"QMP connected and state observed. No guest instruction was resumed by the manager.");
    }
    free(s);
    return VmReport(r,result,"QMP negotiation failed. The caller still owns the process channel.");
}
UmiStatus UmiVmObserve(UmiVmSession*s,UmiVmSnapshot*out){
    if(!s||!out)return UMI_STATUS_INVALID_ARGUMENT;
    UmiProcessChannelSnapshot p;
    UmiStatus result=UmiProcessChannelPoll(s->channel,&p);
    if(result!=UMI_STATUS_OK)return result;
    s->snapshot.processRunning=p.running;
    s->snapshot.processId=p.processId;
    s->snapshot.exitCode=p.exitCode;
    memcpy(s->snapshot.diagnostics,p.diagnostics,sizeof p.diagnostics);
    if(!p.running){
        s->snapshot.controlAvailable=0;
        s->snapshot.guestRunning=0;
        strcpy(s->snapshot.state,"exited");
    }
    *out=s->snapshot;
    return UMI_STATUS_OK;
}
UmiStatus UmiVmControl(UmiVmSession*s,UmiVmCommand c,const void*input,size_t n,void*out,size_t cap,size_t*outSize,UmiVmReport*r){
    if(!s||c<UMI_VM_QUERY||c>UMI_VM_CONSOLE_WRITE||n>UMI_VM_CONSOLE_CHUNK||(!input&&n)||(c!=UMI_VM_CONSOLE_WRITE&&n)||(c==UMI_VM_CONSOLE_READ&&(!out||!outSize||cap<UMI_VM_CONSOLE_CHUNK)))return VmReport(r,UMI_STATUS_INVALID_ARGUMENT,"Invalid VM command or console buffer.");
    if(outSize)*outSize=0;
    UmiVmSnapshot observed;
    UmiStatus result=UmiVmObserve(s,&observed);
    if(result!=UMI_STATUS_OK||!observed.processRunning)return VmReport(r,UMI_STATUS_INVALID_STATE,"The owned QEMU process is no longer running.");
    if(c==UMI_VM_QUERY)result=Query(s);
    else{
        UmiVmQmpMessage m;
        result=Exchange(s,c,input,n,&m);
        if(result==UMI_STATUS_OK&&c==UMI_VM_CONSOLE_READ){
            memcpy(out,m.console,m.consoleLength);
            *outSize=m.consoleLength;
        }
        if(result==UMI_STATUS_OK&&c==UMI_VM_POWERDOWN)s->snapshot.shutdownRequested=1;
        if(result==UMI_STATUS_OK&&(c==UMI_VM_PAUSE||c==UMI_VM_RESUME))result=Query(s);
    }
    const char*message=result==UMI_STATUS_OK?c==UMI_VM_POWERDOWN?"Powerdown request acknowledged. The guest may ignore it; this is not a completed shutdown.":c==UMI_VM_QUIT?"QEMU quit acknowledged. This is a host stop, not proof of guest filesystem shutdown.":"QMP command acknowledged; the reported state is an observation, not guest health.":s->snapshot.lastError;
    return VmReport(r,result,message);
}
UmiStatus UmiVmForceStop(UmiVmSession*s,UmiVmReport*r){
    if(!s)return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status=UmiProcessChannelTerminate(s->channel);
    UmiVmSnapshot ignored;
    (void)UmiVmObserve(s,&ignored);
    return VmReport(r,status,status==UMI_STATUS_OK?"Owned process stop completed. A forced stop is not guest shutdown; data may be inconsistent.":"The owned process could not be stopped. Inspect the reported status before another action.");
}
void UmiVmSessionDestroy(UmiVmSession*s){
    if(!s)return;
    UmiProcessChannelDestroy(s->channel);
    VmDiskUnlease(s->diskLease);
    if(s->runDirectory[0]){
        char text[512];
        int n=snprintf(text,sizeof text,"UMICOM_VM_SESSION_END\t1\ncommands\t%" PRIu64 "\nevents\t%" PRIu64 "\nlast-observed-state\t%s\nboot-qualified\tno\n",s->snapshot.commands,s->snapshot.events,s->snapshot.state);
        if(n>0&&(size_t)n<sizeof text)(void)VmWriteJoined(s->runDirectory,"session-end.umi",text,(size_t)n);
    }
    free(s);
}
static int Contains(const char*a,const char*b){
    size_t n=strlen(a);
    if(strncmp(a,b,n))return 0;
    return !b[n]||b[n]=='/'||b[n]=='\\';
}
static UmiStatus Review(const UmiVmProfile*p,const char*run,VmRuntime*runtime,UmiOsImageBundleInfo*image,void*held,UmiVmReport*r){
    if(r)memset(r,0,sizeof *r);
    memset(runtime,0,sizeof *runtime);
    memset(image,0,sizeof *image);
    if(UmiVmProfileValidate(p)!=UMI_STATUS_OK||!VmPath(run,0)||Contains(p->runtimeDirectory,run)||Contains(p->imageBundle,run)||Contains(run,p->runtimeDirectory)||Contains(run,p->imageBundle)||(p->diskDirectory[0]&&(Contains(p->diskDirectory,run)||Contains(run,p->diskDirectory))))return VmReport(r,UMI_STATUS_INVALID_ARGUMENT,"Use a new run directory separate from runtime, image and disk inputs.");
    UmiStatus status=UmiSetupFileCheck(run,1,NULL);
    if(status!=UMI_STATUS_NOT_FOUND)return VmReport(r,status==UMI_STATUS_OK?UMI_STATUS_ALREADY_EXISTS:status,"Run directory must not exist, including an empty directory.");
    status=VmRuntimeRead(p->runtimeDirectory,runtime,1);
    if(status==UMI_STATUS_OK&&(strcmp(runtime->host,VmHost())||runtime->architecture!=p->architecture))status=UMI_STATUS_UNAVAILABLE;
    UmiOsImageReport report;
    if(status==UMI_STATUS_OK)status=UmiOsImageDescribe(p->imageBundle,image,&report);
    if(status==UMI_STATUS_OK&&((image->architecture==UMI_OS_IMAGE_X86_64)!=(p->architecture==UMI_VM_X86_64)))status=UMI_STATUS_INVALID_ARGUMENT;
    char diskHash[65]="none";
    uint64_t bytes=0;
    void*lease=NULL;
    if(status==UMI_STATUS_OK&&p->diskDirectory[0]){
        if(!held)status=VmDiskLease(p->diskDirectory,&lease);
        uint64_t virtualSize;
        if(status==UMI_STATUS_OK)status=VmDiskValidate(p->diskDirectory,&virtualSize);
        char path[UMI_SETUP_PATH_CAPACITY];
        if(status==UMI_STATUS_OK)status=UmiSetupPathJoin(p->diskDirectory,"disk.qcow2",path);
        if(status==UMI_STATUS_OK)status=UmiSetupFileDigest(path,diskHash,&bytes,NULL);
    }
    VmText plan;
    VmTextInit(&plan);
    if(status==UMI_STATUS_OK)status=VmProfileEncode(p,&plan);
    if(status==UMI_STATUS_OK){
        VmTextPrint(&plan,"run\t%s\nruntime-identity\t%s\nimage-identity\t%s\ndisk-hash\t%s\ndisk-bytes\t%" PRIu64 "\npolicy\tTCG-paused-no-network-no-shares-QMP-stdio\n",run,runtime->identity,image->manifestHash,diskHash,bytes);
        status=plan.status;
        if(status==UMI_STATUS_OK&&r)status=UmiNativeSha256Buffer(plan.data,plan.used,r->fingerprint);
    }
    VmTextFree(&plan);
    VmDiskUnlease(lease);
    if(status!=UMI_STATUS_OK)VmRuntimeFree(runtime);
    return VmReport(r,status,status==UMI_STATUS_OK?"Launch inputs reviewed. TCG; paused start; no network, shared folders or host devices. No process was started.":"Launch input, runtime, image or disk is unavailable, changed or incompatible.");
}
UmiStatus UmiVmReview(const UmiVmProfile*p,const char*run,UmiVmReport*r){
    if(!r)return UMI_STATUS_INVALID_ARGUMENT;
    VmRuntime runtime;
    UmiOsImageBundleInfo *image=calloc(1,sizeof *image);
    if(!image)return UMI_STATUS_OUT_OF_MEMORY;
    UmiStatus s=Review(p,run,&runtime,image,NULL,r);
    VmRuntimeFree(&runtime);
    free(image);
    return s;
}
static void JsonPath(VmText*t,const char*p){
    VmTextPrint(t,"\"");
    for(size_t i=0;p[i];++i){
        if(p[i]=='"'||p[i]=='\\')VmTextPrint(t,"\\");
        VmTextPrint(t,"%c",p[i]);
    }
    VmTextPrint(t,"\"");
}
UmiStatus UmiVmStart(const UmiVmProfile*p,const char*run,const char*expected,UmiVmSession**out,UmiVmReport*r){
    if(!out||!r||!VmHash(expected)||UmiVmProfileValidate(p)!=UMI_STATUS_OK)return UMI_STATUS_INVALID_ARGUMENT;
    *out=NULL;
    void*lease=NULL;
    UmiStatus s=UMI_STATUS_OK;
    if(p->diskDirectory[0])s=VmDiskLease(p->diskDirectory,&lease);
    VmRuntime runtime;
    memset(&runtime,0,sizeof runtime);
    UmiOsImageBundleInfo*image=calloc(1,sizeof *image);
    if(!image){
        VmDiskUnlease(lease);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    if(s==UMI_STATUS_OK)s=Review(p,run,&runtime,image,lease,r);
    if(s==UMI_STATUS_OK&&strcmp(expected,r->fingerprint))s=UMI_STATUS_INVALID_STATE;
    if(s==UMI_STATUS_OK)s=UmiSetupDirectoryCreate(run,NULL);
    if(s==UMI_STATUS_OK)r->outputCreated=1;
    VmText record;
    VmTextInit(&record);
    if(s==UMI_STATUS_OK)s=VmProfileEncode(p,&record);
    if(s==UMI_STATUS_OK){
        VmTextPrint(&record,"review\t%s\nsource\t%s\n",expected,image->sourceId);
        s=record.status;
    }
    if(s==UMI_STATUS_OK)s=VmWriteJoined(run,"session.umi",record.data,record.used);
    VmTextFree(&record);
    char program[UMI_SETUP_PATH_CAPACITY],firmware[UMI_SETUP_PATH_CAPACITY],memory[24],cpus[24];
    if(s==UMI_STATUS_OK)s=UmiSetupPathJoin(p->runtimeDirectory,runtime.emulator,program);
    if(s==UMI_STATUS_OK)s=UmiSetupPathJoin(p->runtimeDirectory,runtime.firmware,firmware);
    if(s==UMI_STATUS_OK)s=UmiSetupNativeProgramCheck(program,NULL);
    snprintf(memory,sizeof memory,"%u",p->memoryMiB);
    snprintf(cpus,sizeof cpus,"%u",p->processors);
    const char*args[64];
    size_t count=0;
#define ARG(x) do { args[count++]=(x); } while(0)
    ARG("-no-user-config");
    ARG("-nodefaults");
    ARG("-machine");
    ARG(p->architecture==UMI_VM_X86_64?"q35":"virt");
    ARG("-accel");
    ARG("tcg");
    ARG("-m");
    ARG(memory);
    ARG("-smp");
    ARG(cpus);
    ARG("-S");
    ARG("-no-reboot");
    ARG("-display");
    ARG("none");
    ARG("-monitor");
    ARG("none");
    ARG("-nic");
    ARG("none");
    ARG("-qmp");
    ARG("stdio");
    ARG("-chardev");
    ARG("ringbuf,id=console,size=65536");
    ARG("-serial");
    ARG("chardev:console");
    ARG("-L");
    ARG(firmware);
    if(p->architecture==UMI_VM_RISCV64){
        ARG("-bios");
        ARG("default");
    }
    ARG("-kernel");
    ARG(image->kernel);
    ARG("-initrd");
    ARG(image->rootfs);
    ARG("-append");
    ARG(p->recovery?"console=ttyS0 rdinit=/init panic=-1 umicom.recovery=1":"console=ttyS0 rdinit=/init panic=-1");
    VmText block;
    VmTextInit(&block);
    if(s==UMI_STATUS_OK&&p->diskDirectory[0]){
        char disk[UMI_SETUP_PATH_CAPACITY];
        s=UmiSetupPathJoin(p->diskDirectory,"disk.qcow2",disk);
        if(s==UMI_STATUS_OK){
            VmTextPrint(&block,"{\"driver\":\"file\",\"node-name\":\"umicom_file\",\"filename\":");
            JsonPath(&block,disk);
            VmTextPrint(&block,"}");
            s=block.status;
            ARG("-blockdev");
            ARG(block.data);
            ARG("-blockdev");
            ARG("{\"driver\":\"qcow2\",\"node-name\":\"umicom_disk\",\"file\":\"umicom_file\"}");
            ARG("-device");
            ARG(p->architecture==UMI_VM_X86_64?"virtio-blk-pci,drive=umicom_disk":"virtio-blk-device,drive=umicom_disk");
        }
    }
#undef ARG
    UmiProcessChannel*channel=NULL;
    if(s==UMI_STATUS_OK){
        UmiProcessChannelRequest request={
            program,args,count,run
        };
        s=UmiProcessChannelOpen(&request,&channel);
        if(s==UMI_STATUS_OK)r->processLaunched=1;
    }
    if(s==UMI_STATUS_OK)s=UmiVmQmpAdoptChannel(channel,out,r);
    if(s==UMI_STATUS_OK&&((*out)->snapshot.guestRunning||         (strcmp((*out)->snapshot.state,"prelaunch")&&strcmp((*out)->snapshot.state,"paused")))){
        /* -S must be observed, not assumed. Do not expose a running child as a
                                                         * reviewed paused machine. Adopt transferred ownership on success. */
        UmiVmSessionDestroy(*out);
        *out=NULL;
        channel=NULL;
        s=UMI_STATUS_INVALID_STATE;
    }
    if(s==UMI_STATUS_OK){
        (*out)->diskLease=lease;
        lease=NULL;
        strcpy((*out)->runDirectory,run);
        channel=NULL;
    }
    UmiProcessChannelDestroy(channel);
    VmDiskUnlease(lease);
    VmRuntimeFree(&runtime);
    free(image);
    VmTextFree(&block);
    return VmReport(r,s,s==UMI_STATUS_OK?"QEMU is connected and paused. Choose Resume to begin guest execution.":"VM start stopped. No successful guest boot is claimed; retain the run directory for inspection.");
}

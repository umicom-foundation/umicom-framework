/*-----------------------------------------------------------------------------
 * Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Ordered guest observations, not substring-based success detection. Offline
 * transcript assessment and actual process execution are separate entry points.
 *---------------------------------------------------------------------------*/
#include "internal.h"
UmiStatus UmiOsImageAssessBoot(const void *data,size_t size,const char *source, UmiOsImageMode mode,int exitCode,UmiOsImageReport *r) {
    OiInit(r);
    if(!data||!size||size>UMI_OS_IMAGE_MAX_LOG||!OiHash(source,64)|| (mode!=UMI_OS_IMAGE_NORMAL&&mode!=UMI_OS_IMAGE_RECOVERY))return UMI_STATUS_INVALID_ARGUMENT;
    if(exitCode!=0)return OiReport(r,UMI_STATUS_INVALID_STATE,"The selected process exit is not zero.");
    char *text=malloc(size+1U);
    if(!text)return UMI_STATUS_OUT_OF_MEMORY;
    const unsigned char *bytes=data;
    size_t used=0;
    for(size_t i=0;i<size;++i) {
        if(!bytes[i]) {
            free(text);
            return UMI_STATUS_PARSE_ERROR;
        }
        if(bytes[i]=='\r'&&i+1U<size&&bytes[i+1U]=='\n')continue;
        text[used++]=(char)bytes[i];
    }
    text[used]=0;
    UmiStatus status=UMI_STATUS_PARSE_ERROR;
    if(strstr(text,"Kernel panic")||!used||text[used-1U]!='\n')goto end;
    unsigned init=0,stage=0,reportSeen=0,shutdown=0;
    int inReport=0;
    OiText report;
    OiTextInit(&report);
    const char *serviceLines[]= {
        "UMICOM_SERVICE start=platform-check","UMICOM_SERVICE end=platform-check outcome=none exit=0 signal=0", "UMICOM_SERVICE start=framework-probe","UMICOM_SERVICE end=framework-probe outcome=none exit=0 signal=0"
    }
    ;
    for(char *at=text;*at;) {
        char *line=at,*nl=strchr(at,'\n');
        if(!nl)break;
        *nl=0;
        at=nl+1;
        if(inReport) {
            if(!strcmp(line,"UMICOM_REPORT_END")) {
                inReport=0;
                reportSeen=1;
            }
            else {
                if(strlen(line)>UMI_BOOT_REPORT_MAX_BYTES) {
                    status=UMI_STATUS_CAPACITY_EXCEEDED;
                    goto reportEnd;
                }
                OiPrint(&report,"%s\n",line);
                if(report.size>UMI_BOOT_REPORT_MAX_BYTES) {
                    status=UMI_STATUS_CAPACITY_EXCEEDED;
                    goto reportEnd;
                }
            }
            continue;
        }
        if(!strncmp(line,"UMICOM_",7)) {
            if(!strcmp(line,"UMICOM_INIT pid=1")) {
                if(init||reportSeen||shutdown)goto reportEnd;
                init=1;
            }
            else if(!strncmp(line,"UMICOM_SERVICE ",15)) {
                if(!init||reportSeen||shutdown||mode!=UMI_OS_IMAGE_NORMAL||stage>=4U||strcmp(line,serviceLines[stage]))goto reportEnd;
                ++stage;
            }
            else if(!strcmp(line,"UMICOM_REPORT_BEGIN")) {
                if(!init||reportSeen||shutdown||stage!=(mode==UMI_OS_IMAGE_NORMAL?4U:0U))goto reportEnd;
                inReport=1;
            }
            else if(!strcmp(line,"UMICOM_SHUTDOWN")) {
                if(!reportSeen||shutdown)goto reportEnd;
                shutdown=1;
            }
            else goto reportEnd;
        }
    }
    if(!init||inReport||!reportSeen||!shutdown||report.status!=UMI_STATUS_OK)goto reportEnd;
    UmiBootReport parsed;
    if(UmiBootReportParse((const char*)report.data,report.size,&parsed)!=UMI_STATUS_OK||strcmp(parsed.sourceId,source)||parsed.planned!=2U)goto reportEnd;
    if(mode==UMI_OS_IMAGE_NORMAL) {
        if(parsed.mode!=UMI_BOOT_REPORT_NORMAL||parsed.state!=UMI_BOOT_REPORT_READY||parsed.completed!=2U||strcmp(parsed.reason,"none"))goto reportEnd;
    }
    else if(parsed.mode!=UMI_BOOT_REPORT_REQUESTED_RECOVERY||parsed.state!=UMI_BOOT_REPORT_RECOVERY||parsed.completed||strcmp(parsed.reason,"requested"))goto reportEnd;
    status=UMI_STATUS_OK;
    if(r) {
        r->completed=1;
        strcpy(r->sourceId,source);
        r->exitCode=0;
    }
    reportEnd:OiTextClear(&report);
    end:free(text);
    return OiReport(r,status,status==UMI_STATUS_OK?"Transcript observations agree. This assessment did not execute QEMU.":"Transcript does not demonstrate the required ordered init-to-poweroff workflow.");
}
UmiStatus UmiOsImageBoot(const char *root,const char *qemu,UmiOsImageMode mode, unsigned timeout,const char *dest,const UmiCancellationToken *cancel,UmiOsImageReport *r) {
    OiInit(r);
    if((mode!=UMI_OS_IMAGE_NORMAL&&mode!=UMI_OS_IMAGE_RECOVERY)||timeout<5U||timeout>600U|| OiAbsolute(qemu,0)!=UMI_STATUS_OK||OiAbsolute(dest,0)!=UMI_STATUS_OK)return UMI_STATUS_INVALID_ARGUMENT;
    UmiOsImageReport inspected;
    UmiStatus s=UmiOsImageVerify(root,&inspected);
    if(s!=UMI_STATUS_OK)return OiReport(r,s,inspected.detail);
    OiBundle b= {
        0
    }
    ;
    s=OiBundleLoad(root,&b);
    if(s!=UMI_STATUS_OK)return s;
    if(OiContains(root,dest)||OiContains(dest,root))return OiReport(r,UMI_STATUS_INVALID_ARGUMENT,"Keep boot results outside the immutable bundle.");
    s=OiNativeProgram(qemu);
    if(s==UMI_STATUS_NOT_FOUND||s==UMI_STATUS_UNAVAILABLE)return OiReport(r,UMI_STATUS_UNAVAILABLE,"NOT RUN: the explicit native QEMU executable is unavailable.");
    if(s!=UMI_STATUS_OK)return OiReport(r,s,"The selected QEMU program is not an accessible native executable.");
    char qemuHash[65];
    uint64_t qemuSize=0;
    s=OiDigest(qemu,qemuHash,&qemuSize);
    if(s!=UMI_STATUS_OK)return s;
    if(umi_cancellation_token_is_requested(cancel))return OiReport(r,UMI_STATUS_CANCELLED,"Cancelled before creating boot results or launching a guest.");
    s=OiDirectory(dest,1);
    if(s!=UMI_STATUS_OK)return OiReport(r,s,"Use a new result directory with an existing parent.");
    if(r)r->outputCreated=1;
    char kernel[UMI_OS_IMAGE_PATH],initrd[UMI_OS_IMAGE_PATH];
    s=OiJoin(root,OiKernelName(b.arch),kernel);
    if(s==UMI_STATUS_OK)s=OiJoin(root,"umicom-rootfs.cpio.gz",initrd);
    char command[192];
    int n=snprintf(command,sizeof command,"console=ttyS0 rdinit=/init panic=-1 umicom.recovery=%u umicom.autopoweroff=1",mode==UMI_OS_IMAGE_RECOVERY?1U:0U);
    if(n<0||(size_t)n>=sizeof command)s=UMI_STATUS_CAPACITY_EXCEEDED;
    const char *args[32];
    size_t count=0;
    args[count++]="-M";
    args[count++]=b.arch==UMI_OS_IMAGE_RISCV64?"virt":"q35";
    if(b.arch==UMI_OS_IMAGE_RISCV64) {
        args[count++]="-bios";
        args[count++]="default";
    }
    const char *common[]= {
        "-accel","tcg","-m","256M","-smp","1","-display","none","-monitor","none","-serial","stdio","-nic","none","-no-reboot","-kernel",kernel,"-initrd",initrd,"-append",command
    }
    ;
    for(size_t i=0;i<sizeof common/sizeof common[0];++i)args[count++]=common[i];
    OiText request;
    OiTextInit(&request);
    OiPrint(&request,"UMICOM_OS_BOOT_REQUEST\t1\nsource\t%s\narch\t%s\nmode\t%s\nqemu-sha256\t%s\nqemu-bytes\t%" PRIu64 "\ntimeout-seconds\t%u\n",b.sourceId,OiArchName(b.arch),mode==UMI_OS_IMAGE_NORMAL?"normal":"recovery",qemuHash,qemuSize,timeout);
    char path[UMI_OS_IMAGE_PATH];
    if(s==UMI_STATUS_OK)s=request.status;
    if(s==UMI_STATUS_OK)s=OiJoin(dest,"request.umi",path);
    if(s==UMI_STATUS_OK)s=OiWrite(path,request.data,request.size);
    OiTextClear(&request);
    OiText transcript;
    OiTextInit(&transcript);
    UmiProcessResult *pr=calloc(1,sizeof *pr);
    if(!pr)s=UMI_STATUS_OUT_OF_MEMORY;
    if(s==UMI_STATUS_OK)s=OiCapture(qemu,args,count,root,timeout*1000U,cancel,&transcript,pr);
    if(r&&pr) {
        r->processLaunched=pr->launched;
        r->exitCode=pr->exit_code;
        r->cancelled=pr->cancelled;
        r->timedOut=pr->timed_out;
        strcpy(r->sourceId,b.sourceId);
    }
    if(s==UMI_STATUS_OK) {
        UmiOsImageReport assessed;
        s=UmiOsImageAssessBoot(transcript.data,transcript.size,b.sourceId,mode,pr->exit_code,&assessed);
    }
    if(s==UMI_STATUS_OK) {
        s=UmiOsImageVerify(root,&inspected);
        if(s==UMI_STATUS_OK&&strcmp(inspected.sourceId,b.sourceId))s=UMI_STATUS_INVALID_STATE;
        char after[65];
        uint64_t length=0;
        if(s==UMI_STATUS_OK)s=OiDigest(qemu,after,&length);
        if(s==UMI_STATUS_OK&&(strcmp(after,qemuHash)||length!=qemuSize))s=UMI_STATUS_INVALID_STATE;
    }
    UmiStatus saved=OiJoin(dest,"console.log",path);
    if(saved==UMI_STATUS_OK)saved=OiWrite(path,transcript.data,transcript.size);
    if(s==UMI_STATUS_OK)s=saved;
    char logHash[65];
    UmiStatus hashed=UmiNativeSha256Buffer(transcript.data,transcript.size,logHash);
    if(s==UMI_STATUS_OK)s=hashed;
    OiText result;
    OiTextInit(&result);
    OiPrint(&result,"UMICOM_OS_BOOT_RESULT\t1\nsource\t%s\nmode\t%s\nstatus\t%s\nlaunched\t%d\nexit\t%d\ntimed-out\t%d\ncancelled\t%d\nlog-sha256\t%s\nlog-bytes\t%zu\nauthenticity-tested\tfalse\nbios-uefi-tested\tfalse\n", b.sourceId,mode==UMI_OS_IMAGE_NORMAL?"normal":"recovery",s==UMI_STATUS_OK?"passed":"failed",pr?pr->launched:0,pr?pr->exit_code:-1,pr?pr->timed_out:0,pr?pr->cancelled:0,hashed==UMI_STATUS_OK?logHash:"unavailable",transcript.size);
    saved=result.status;
    if(saved==UMI_STATUS_OK)saved=OiJoin(dest,"result.umi",path);
    if(saved==UMI_STATUS_OK)saved=OiWrite(path,result.data,result.size);
    if(s==UMI_STATUS_OK)s=saved;
    if(r)r->completed=s==UMI_STATUS_OK;
    free(pr);
    OiTextClear(&result);
    OiTextClear(&transcript);
    return OiReport(r,s,s==UMI_STATUS_OK?"Direct-kernel guest run passed its ordered init, service, report and shutdown checks.":"Boot run did not pass. Keep its new result directory and inspect the transcript.");
}

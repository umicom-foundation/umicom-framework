/*-----------------------------------------------------------------------------
 * Umicom Framework tests | Sammy Hegab, Umicom Foundation | MIT
 * Synthetic boot structures are data, NOT bootable guests. All write tests use
 * fresh regular files. No physical drive, mounted filesystem or optical device
 * is opened by these tests. This test executable never enables device writes.
 *---------------------------------------------------------------------------*/

#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#endif
#include "umicom/boot_media/media.h"
#include "umicom/setup_centre/files.h"
#include "umicom/platform/process.h"
#include "../../src/boot_media/internal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <sys/stat.h>
#endif
#define CHECK(x) do{if(!(x)){fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#x);return 1;}}while(0)
#define OK(x) CHECK((x)==UMI_STATUS_OK)
#define SIZE (2U*1024U*1024U)
static void Put16(unsigned char*p,uint16_t v) {
    p[0]=(unsigned char)v;
    p[1]=(unsigned char)(v>>8U);
}

static void Put32(unsigned char*p,uint32_t v) {
    for (unsigned i=0;i<4U;++i)p[i]=(unsigned char)(v>>(8U*i));
}

static void Put64(unsigned char*p,uint64_t v) {
    for (unsigned i=0;i<8U;++i)p[i]=(unsigned char)(v>>(8U*i));
}

static void Big32(unsigned char*p,uint32_t v) {
    for (unsigned i=0;i<4U;++i)p[3U-i]=(unsigned char)(v>>(8U*i));
}

static void Mbr(unsigned char*p) {
    memset(p,0,SIZE);
    p[0]=0xfa;
    p[510]=0x55;
    p[511]=0xaa;
    p[446]=0x80;
    p[450]=0x0c;
    Put32(p+454,1);
    Put32(p+458,SIZE/512U-1U);
}

static void Gpt(unsigned char*p)
{
    Mbr(p);
    p[446]=0;
    p[450]=0xee;
    unsigned char*entries=p+1024U;
    entries[0]=1;
    entries[16]=2;
    Put64(entries+32,34);
    Put64(entries+40,100);
    uint32_t crc=BmCrc32(entries,512);
    memcpy(p+SIZE-1024U,entries,512);
    for (unsigned i=0;i<2U;++i) {
        unsigned char*h=i?p+SIZE-512U:p+512U;
        memcpy(h,"EFI PART",8);
        Put32(h+8,0x10000U);
        Put32(h+12,92);
        Put64(h+24,i?SIZE/512U-1U:1U);
        Put64(h+32,i?1U:SIZE/512U-1U);
        Put64(h+40,3);
        Put64(h+48,SIZE/512U-3U);
        h[56]=1;
        Put64(h+72,i?SIZE/512U-2U:2U);
        Put32(h+80,4);
        Put32(h+84,128);
        Put32(h+88,crc);
        Put32(h+16,BmCrc32(h,92));
    }
}

static void Iso(unsigned char*p,int efi)
{
    memset(p,0,SIZE);
    unsigned char*primary=p+16U*2048U,*boot=p+17U*2048U,*end=p+18U*2048U,*catalogue=p+20U*2048U;
    unsigned char*descriptors[]= {
        primary,boot,end
    };
    for (size_t i=0;i<3U;++i) {
        memcpy(descriptors[i]+1,"CD001",5);
        descriptors[i][6]=1;
    }
    primary[0]=1;
    Put32(primary+80,SIZE/2048U);
    Big32(primary+84,SIZE/2048U);
    Put16(primary+128,2048);
    primary[130]=8;
    memcpy(boot+7,"EL TORITO SPECIFICATION",23);
    Put32(boot+71,20);
    end[0]=255;
    catalogue[0]=1;
    catalogue[30]=0x55;
    catalogue[31]=0xaa;
    Put16(catalogue+28,(uint16_t)(0U-1U-0xaa55U));
    catalogue[32]=0x88;
    Put16(catalogue+38,4);
    Put32(catalogue+40,22);
    if (efi) {
        catalogue[64]=0x91;
        catalogue[65]=0xef;
        Put16(catalogue+66,1);
        catalogue[96]=0x88;
        Put16(catalogue+102,4);
        Put32(catalogue+104,23);
    }
}

static int Save(const char *path,const void *data,size_t n)
{
    FILE*f=fopen(path,"wb");
    if (!f)return 0;
    size_t put=fwrite(data,1,n,f);
    int close=fclose(f);
    return put==n&&close==0;
}

static int Root(char root[4096])
{
#ifdef _WIN32
    wchar_t temp[2048];
    DWORD n=GetTempPathW(2048,temp);
    CHECK(n&&n<2048);
    char base[4096],leaf[96];
    CHECK(WideCharToMultiByte(CP_UTF8,WC_ERR_INVALID_CHARS,temp,-1,base,sizeof base,NULL,NULL)>0);
    size_t len=strlen(base);
    while (len>3U&&(base[len-1U]=='/'||base[len-1U]=='\\'))base[--len]=0;
    int k=snprintf(leaf,sizeof leaf,"umicom-media-%lu-%llu",(unsigned long)GetCurrentProcessId(),(unsigned long long)GetTickCount64());
    CHECK(k>0&&(size_t)k<sizeof leaf);
    OK(UmiSetupPathJoin(base,leaf,root));
    OK(UmiSetupDirectoryCreate(root,NULL));

#else
    strcpy(root,"/tmp/umicom-media-XXXXXX");
    CHECK(mkdtemp(root));

#endif
    return 0;
}

static int InspectCase(const char *name,const char *file,unsigned char *data)
{
    int valid=0;
    UmiBootMediaImage info;
    UmiBootMediaReport r;
    if (!strncmp(name,"gpt.",4)) {
        Gpt(data);
        valid=!strcmp(name,"gpt.valid");
        if (!strcmp(name,"gpt.header_crc"))data[512+16]^=1;
        if (!strcmp(name,"gpt.table_crc"))data[1024]^=1;
        if (!strcmp(name,"gpt.backup"))data[SIZE-512+24]^=1;
        if (!strcmp(name,"gpt.backup_table"))data[SIZE-1024+10]^=1;
        if (!strcmp(name,"gpt.large_count")) {
            Put32(data+512+80,UINT32_MAX);
            memset(data+512+16,0,4);
            Put32(data+512+16,BmCrc32(data+512,92));
        }
    }
    else if (!strncmp(name,"iso.",4)) {
        Iso(data,1);
        valid=!strcmp(name,"iso.valid")||!strcmp(name,"iso.bios");
        if (!strcmp(name,"iso.bios"))Iso(data,0);
        if (!strcmp(name,"iso.endian"))data[16*2048+84]^=1;
        if (!strcmp(name,"iso.terminator"))data[18*2048]=1;
        if (!strcmp(name,"iso.catalogue"))data[20*2048+30]^=1;
        if (!strcmp(name,"iso.load_range"))Put32(data+20*2048+40,UINT32_MAX);
        if (!strcmp(name,"iso.section_count"))Put16(data+20*2048+66,UINT16_MAX);
        if (!strcmp(name,"iso.version"))data[16*2048+6]=2;
        if (!strcmp(name,"iso.zero_load"))Put16(data+20*2048+38,0);
    }
    else {
        Mbr(data);
        valid=!strcmp(name,"mbr.valid");
        if (!strcmp(name,"mbr.range"))Put32(data+458,UINT32_MAX);
        if (!strcmp(name,"mbr.flag"))data[446]=1;
        if (!strcmp(name,"mbr.signature"))data[510]=0;
        if (!strcmp(name,"mbr.no_loader"))data[0]=0;
        if (!strcmp(name,"mbr.zero_partition"))Put32(data+458,0);
        if (!strcmp(name,"mbr.empty_type"))data[450]=0;
        if (!strcmp(name,"mbr.overlap")) {
            data[466]=0x83;
            Put32(data+470,100);
            Put32(data+474,100);
        }
    }
    CHECK(Save(file,data,SIZE));
    UmiStatus s=UmiBootMediaInspect(file,&info,NULL,NULL,&r);
    CHECK(valid?s==UMI_STATUS_OK:s!=UMI_STATUS_OK);
    if (valid) {
        CHECK(strlen(info.sha256)==64U&&info.bytes==SIZE);
        if (!strncmp(name,"gpt.",4))CHECK(info.gptLayout&&info.partitions==1U);
        if (!strcmp(name,"iso.valid"))CHECK(info.elTorito&&info.biosEntry&&info.efiEntry);
    }
    else CHECK(!info.bytes&&!info.sha256[0]);
    return 0;
}

typedef struct Change {
    const char *source,*target;
    int action,done;
}

Change;
static int Progress(const UmiBootMediaReport *r,void *ctx)
{
    Change*c=ctx;
    if (c->action==1&&r->phase==UMI_BOOT_MEDIA_RECHECK)return 1;
    if (c->action==2&&r->phase==UMI_BOOT_MEDIA_WRITE&&r->bytesDone)return 1;
    if (c->action==3&&r->phase==UMI_BOOT_MEDIA_VERIFY)return 1;
    if (c->action==6&&r->phase==UMI_BOOT_MEDIA_FINISHED)return 1;
    if (!c->done&&c->action==7&&r->phase==UMI_BOOT_MEDIA_INSPECT) {
        FILE *f=fopen(c->source,"r+b");
        if (f) {
            if (fseek(f,510,SEEK_SET)==0)(void)fputc(0,f);
            (void)fclose(f);
        }
        c->done=1;
    }
    if (!c->done&&((c->action==4&&r->phase==UMI_BOOT_MEDIA_WRITE)||(c->action==5&&r->phase==UMI_BOOT_MEDIA_VERIFY))) {
        FILE*f=fopen(c->action==4?c->source:c->target,"r+b");
        if (f) {
            if (fseek(f,(long)(SIZE-1U),SEEK_SET)==0)(void)fputc(123,f);
            (void)fclose(f);
        }
        c->done=1;
    }
    return 0;
}

static int CopyCase(const char *name,const char *source,const char *target,unsigned char *data)
{
    Mbr(data);
    CHECK(Save(source,data,SIZE));
    UmiBootMediaPlan*p=NULL;
    UmiBootMediaReport r;
    if (!strcmp(name,"copy.exists")) {
        CHECK(Save(target,"untouched",9));
        CHECK(UmiBootMediaReviewFile(source,target,&p,NULL,NULL,&r)==UMI_STATUS_ALREADY_EXISTS);
        CHECK(!p);
        return 0;
    }
    if (!strcmp(name,"copy.same")) {
        CHECK(UmiBootMediaReviewFile(source,source,&p,NULL,NULL,&r)!=UMI_STATUS_OK);
        return 0;
    }
    OK(UmiBootMediaReviewFile(source,target,&p,NULL,NULL,&r));
    CHECK(p&&strlen(UmiBootMediaPlanFingerprint(p))==64U);
    CHECK(UmiSetupFileCheck(target,0,NULL)==UMI_STATUS_NOT_FOUND);
    char hash[65],confirm[96];
    strcpy(hash,UmiBootMediaPlanFingerprint(p));
    strcpy(confirm,UmiBootMediaPlanConfirmation(p));
    if (!strcmp(name,"copy.bad_review"))hash[0]=hash[0]=='a'?'b':'a';
    if (!strcmp(name,"copy.bad_confirmation"))confirm[0]='X';
    if (!strcmp(name,"copy.source_changed")) {
        data[SIZE-1U]=7;
        CHECK(Save(source,data,SIZE));
    }
    if (!strcmp(name,"copy.collision"))CHECK(Save(target,"keep",4));
    Change c= {
        source,target,0,0
    };
    if (!strcmp(name,"copy.cancel_before"))c.action=1;
    if (!strcmp(name,"copy.cancel_mid"))c.action=2;
    if (!strcmp(name,"copy.cancel_verify"))c.action=3;

#ifndef _WIN32
    if (!strcmp(name,"copy.source_during"))c.action=4;
    if (!strcmp(name,"copy.target_during"))c.action=5;

#endif
    if (!strcmp(name,"copy.cancel_final"))c.action=6;
    int success=!strcmp(name,"copy.complete")||!strcmp(name,"copy.repeat")||!strcmp(name,"copy.verify_tamper");
    UmiStatus s=UmiBootMediaApply(p,hash,confirm,Progress,&c,&r);
    CHECK(success?s==UMI_STATUS_OK:s!=UMI_STATUS_OK);
    CHECK(r.verified==success);
    if (success) {
        OK(UmiBootMediaVerifyFile(source,target,NULL,NULL,&r));
        if (!strcmp(name,"copy.repeat"))CHECK(UmiBootMediaApply(p,hash,confirm,NULL,NULL,&r)==UMI_STATUS_INVALID_STATE);
        if (!strcmp(name,"copy.verify_tamper")) {
            data[100]=77;
            CHECK(Save(target,data,SIZE));
            CHECK(UmiBootMediaVerifyFile(source,target,NULL,NULL,&r)!=UMI_STATUS_OK);
        }
    }
    if (c.action==2||c.action==3||c.action==6)CHECK(r.writeStarted);
    if (c.action==1||!strcmp(name,"copy.source_changed"))CHECK(UmiSetupFileCheck(target,0,NULL)==UMI_STATUS_NOT_FOUND);
    UmiBootMediaPlanDestroy(p);
    return 0;
}

static int Policy(const char *name)
{
    UmiBootMediaImage im= {
        0
    };
    im.bytes=SIZE;
    im.mbrLayout=1;
    UmiBootMediaDevice d= {
        0
    };
    strcpy(d.path,"practice");
    strcpy(d.identity,"test-observation");
    strcpy(d.model,"test-model");
    strcpy(d.serial,"test-serial");
    d.bytes=2U*SIZE;
    d.sectorBytes=512;
    d.usb=1;
    d.removable=1;
    int valid=!strcmp(name,"policy.valid");
    if (!strcmp(name,"policy.system"))d.protectedDevice=1;
    if (!strcmp(name,"policy.mounted"))d.inUse=1;
    if (!strcmp(name,"policy.fixed"))d.removable=0;
    if (!strcmp(name,"policy.not_usb"))d.usb=0;
    if (!strcmp(name,"policy.readonly"))d.readOnly=1;
    if (!strcmp(name,"policy.unknown"))d.serial[0]=0;
    if (!strcmp(name,"policy.small"))d.bytes=512;
    if (!strcmp(name,"policy.large"))d.bytes=UMI_BOOT_MEDIA_MAX_DEVICE+512U;
    if (!strcmp(name,"policy.sector"))d.sectorBytes=3;
    if (!strcmp(name,"policy.4kn"))d.sectorBytes=4096;
    if (!strcmp(name,"policy.optical_only")) {
        im.mbrLayout=0;
        im.elTorito=1;
    }
    if (!strcmp(name,"policy.gpt_resize")) {
        im.gptLayout=1;
        im.mbrLayout=0;
    }
    if (!strcmp(name,"policy.unterminated_model"))memset(d.model,'x',sizeof d.model);
    if (!strcmp(name,"policy.unterminated_serial"))memset(d.serial,'x',sizeof d.serial);
    if (!strcmp(name,"policy.unterminated_identity"))memset(d.identity,'x',sizeof d.identity);
    if (strstr(name,"unterminated")) {
        UmiBootMediaPlan *plan=NULL;
        CHECK(UmiBootMediaReviewDevice("not-an-image",&d,&plan,NULL,NULL,NULL)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(!plan);
    }
    UmiStatus s=UmiBootMediaCheckDevicePolicy(&im,&d,NULL);
    CHECK(valid?s==UMI_STATUS_OK:s!=UMI_STATUS_OK);
    return 0;
}

int main(int argc,char **argv)
{
    CHECK(argc==2||(argc==3&&!strcmp(argv[1],"example.file_workflow")));
    const char*name=argv[1];
    if(!strcmp(name,"path.drive_root")){
        char out[4096];OK(UmiSetupPathJoin("C:\\","umicom-setup-1",out));
        CHECK(!strcmp(out,"C:\\umicom-setup-1"));return 0;
    }
    if(!strcmp(name,"device.write_gate")){
        UmiBootMediaPlan *p=calloc(1,sizeof *p);CHECK(p);
        p->physical=1;strcpy(p->source,"deliberately-not-absolute");
        memset(p->fingerprint,'a',64);strcpy(p->confirmation,"ERASE TEST ONLY");
        UmiBootMediaReport report={0};
        UmiStatus status=UmiBootMediaApply(p,p->fingerprint,p->confirmation,NULL,NULL,&report);
        CHECK(status==(UmiBootMediaDeviceWritesEnabled()?UMI_STATUS_INVALID_ARGUMENT:UMI_STATUS_UNAVAILABLE));
        CHECK(!report.outputCreated&&!report.writeStarted&&!report.verified&&p->consumed);
        UmiBootMediaPlanDestroy(p);return 0;
    }
    if (!strncmp(name,"policy.",7))return Policy(name);
    if (!strcmp(name,"crc.vector")) {
        CHECK(BmCrc32("123456789",9)==UINT32_C(0xcbf43926));
        return 0;
    }
    if (!strcmp(name,"path.capacity")) {
        char base[4096],out[4096];
        memset(base,'x',sizeof base);
        base[0]='/';
        for (size_t i=200;i<4095;i+=200)base[i]='/';
        base[4095]=0;
        CHECK(UmiSetupPathJoin(base,"umicom-setup-123-456",out)==UMI_STATUS_CAPACITY_EXCEEDED);
        return 0;
    }
    char root[4096],source[4096],target[4096];
    CHECK(!Root(root));
    OK(UmiSetupPathJoin(root,"source image.img",source));
    OK(UmiSetupPathJoin(root,"copy image.img",target));
    unsigned char *data=malloc(SIZE);
    CHECK(data);
    int result=1;
    if (!strncmp(name,"mbr.",4)||!strncmp(name,"gpt.",4)||!strncmp(name,"iso.",4))result=InspectCase(name,source,
        data);
    else if (!strncmp(name,"copy.",5))result=CopyCase(name,source,target,data);
    else if(!strcmp(name,"example.file_workflow")){
        const char *args[]={"--output",source};
        UmiProcessRequest request={0};UmiProcessResult *process=calloc(1,sizeof *process);CHECK(process);
        request.program=argv[2];request.arguments=args;request.argument_count=2;
        request.working_directory=root;request.capture_stdout=1;request.capture_stderr=1;
        request.timeout_ms=10000;request.window_mode=UMI_PROCESS_WINDOW_HIDDEN;
        OK(UmiProcessExecuteWithLifetime(&request,UMI_PROCESS_LIFETIME_TREE,NULL,NULL,NULL,process));
        CHECK(process->launched&&process->exit_code==0&&!process->timed_out);
        CHECK(strstr(process->output,"Not bootable"));
        UmiBootMediaPlan *plan=NULL;OK(UmiBootMediaReviewFile(source,target,&plan,NULL,NULL,NULL));
        char fingerprint[65],confirmation[96];
        strcpy(fingerprint,UmiBootMediaPlanFingerprint(plan));strcpy(confirmation,UmiBootMediaPlanConfirmation(plan));
        OK(UmiBootMediaApply(plan,fingerprint,confirmation,NULL,NULL,NULL));
        UmiBootMediaPlanDestroy(plan);
        UmiStatus again=UmiProcessExecuteWithLifetime(&request,UMI_PROCESS_LIFETIME_TREE,NULL,NULL,NULL,process);
        CHECK(again!=UMI_STATUS_OK&&process->launched&&process->exit_code!=0);
        OK(UmiBootMediaVerifyFile(source,target,NULL,NULL,NULL));free(process);result=0;
    }
    else if (!strcmp(name,"image.short")) {
        CHECK(Save(source,"x",1));
        UmiBootMediaImage im;
        CHECK(UmiBootMediaInspect(source,&im,NULL,NULL,NULL)!=UMI_STATUS_OK);
        result=0;
    }
    else if (!strcmp(name,"image.non_sector")) {
        Mbr(data);
        CHECK(Save(source,data,SIZE-1U));
        UmiBootMediaImage im;
        CHECK(UmiBootMediaInspect(source,&im,NULL,NULL,NULL)!=UMI_STATUS_OK);
        result=0;
    }

#ifndef _WIN32
    else if (!strcmp(name,"source.symlink")) {
        Mbr(data);
        CHECK(Save(source,data,SIZE));
        CHECK(!symlink(source,target));
        UmiBootMediaImage im;
        CHECK(UmiBootMediaInspect(target,&im,NULL,NULL,NULL)!=UMI_STATUS_OK);
        result=0;
    }
    else if (!strcmp(name,"source.hardlink")) {
        Mbr(data);
        CHECK(Save(source,data,SIZE));
        CHECK(!link(source,target));
        UmiBootMediaImage im;
        CHECK(UmiBootMediaInspect(source,&im,NULL,NULL,NULL)!=UMI_STATUS_OK);
        result=0;
    }
    else if (!strcmp(name,"source.fifo")) {
        CHECK(!mkfifo(source,0600));
        UmiBootMediaImage im;
        CHECK(UmiBootMediaInspect(source,&im,NULL,NULL,NULL)!=UMI_STATUS_OK);
        result=0;
    }

#endif
    else if (!strcmp(name,"image.changed_during_inspection")) {
        Mbr(data);
        CHECK(Save(source,data,SIZE));
        Change c= {
            source,target,7,0
        };
        UmiBootMediaImage im;
        CHECK(UmiBootMediaInspect(source,&im,Progress,&c,NULL)!=UMI_STATUS_OK);
        CHECK(!im.bytes&&!im.sha256[0]);
        result=0;
    }
    else if (!strcmp(name,"image.mutations")) {
        Mbr(data);
        CHECK(Save(source,data,SIZE));
        BmFile*f=NULL;
        uint64_t bytes=0;
        OK(BmSourceOpen(source,&f,&bytes));
        unsigned state=1427;
        for (unsigned i=0;i<1500U;++i) {
            state=state*1664525U+1013904223U;
            unsigned at=446U+state%66U;
            unsigned char old=data[at];
            data[at]^=(unsigned char)(state>>24U);
            CHECK(Save(source,data,SIZE));
            UmiBootMediaImage im;
            UmiStatus s=BmInspect(f,bytes,&im);
            CHECK(s>=UMI_STATUS_OK&&s<=UMI_STATUS_BUSY);
            data[at]=old;
        }
        BmClose(f);
        result=0;
    }
    free(data);
    return result;
}

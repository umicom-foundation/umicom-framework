/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/vm_manager/runtime.c
 * PURPOSE:
 *   Explicit offline QEMU runtime inventory. A publisher supplies the selected binaries,
 *   firmware, licence and corresponding-source notice. Hash agreement does not assert legal
 *   sufficiency, provenance or Windows compatibility.
 * ORGANISATION: Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
 * Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Explicit offline QEMU runtime inventory. A publisher supplies the selected
 * binaries, firmware, licence and corresponding-source notice. Hash agreement
 * does not assert legal sufficiency, provenance or Windows compatibility.
 *---------------------------------------------------------------------------*/
#include "internal.h"
static char *Field(char **c){
    if(!*c)return NULL;
    char*s=*c,*end=strchr(s,'\t');
    if(end){
        *end=0;
        *c=end+1;
    }
    else *c=NULL;
    return s;
}
static int PathField(const char*s){
    return s&&*s&&VmUtf8(s,768)&&UmiSetupValidateRelative(s)==UMI_STATUS_OK&&strcmp(s,"runtime.umi")&& !strstr(s,".umicom-vm-lock");
}
static int FoldEqual(const char*a,const char*b){
    for(;;){
        unsigned x=(unsigned char)*a++,y=(unsigned char)*b++;
        if(x>='A'&&x<='Z')x+=32;
        if(y>='A'&&y<='Z')y+=32;
        if(x!=y)return 0;
        if(!x)return 1;
    }
}
static int IsAncestor(const char*a,const char*b){
    size_t n=strlen(a);
    return !strncmp(a,b,n)&&(b[n]=='/'||b[n]=='\\'||b[n]==0);
}
static UmiStatus Decode(char*data,size_t length,VmRuntime*r,int sealed){
    const char *header=sealed?VM_RUNTIME_HEADER:"UMICOM_QEMU_INPUT\t1\n";
    size_t h=strlen(header);
    if(length<h||memcmp(data,header,h)||memchr(data,0,length)||data[length-1U]!='\n')return UMI_STATUS_PARSE_ERROR;
    r->files=calloc(UMI_VM_MAX_RUNTIME_FILES,sizeof *r->files);
    if(!r->files)return UMI_STATUS_OUT_OF_MEMORY;
    const char*keys[]={
        "host","architecture","version","emulator","image-tool","firmware","licence","source-notice"
    };
    char *cursor=data+h;
    unsigned field=0;
    uint64_t total=0;
    while(*cursor){
        char*line=cursor,*end=strchr(cursor,'\n');
        if(!end)return UMI_STATUS_PARSE_ERROR;
        *end=0;
        cursor=end+1;
        char*p=line,*kind=Field(&p);
        if(field<8U){
            char*v=Field(&p);
            if(!v||p||strcmp(kind,keys[field]))return UMI_STATUS_PARSE_ERROR;
            if(field==0){
                if(strcmp(v,"windows-amd64")&&strcmp(v,"linux-amd64")&&strcmp(v,"linux-arm64"))return UMI_STATUS_PARSE_ERROR;
                strcpy(r->host,v);
            }
            else if(field==1){
                uint64_t n;
                if(!VmNumber(v,&n)||(n!=1&&n!=2))return UMI_STATUS_PARSE_ERROR;
                r->architecture=(UmiVmArchitecture)n;
            }
            else if(field==2){
                if(!*v||!VmUtf8(v,sizeof r->version))return UMI_STATUS_PARSE_ERROR;
                strcpy(r->version,v);
            }
            else{
                if(!PathField(v))return UMI_STATUS_PARSE_ERROR;
                char*dest=field==3?r->emulator:field==4?r->imageTool:field==5?r->firmware:field==6?r->licence:r->sourceNotice;
                strcpy(dest,v);
            }
            ++field;
            continue;
        }
        if(strcmp(kind,"file")||r->count==UMI_VM_MAX_RUNTIME_FILES)return UMI_STATUS_PARSE_ERROR;
        VmRuntimeFile*f=&r->files[r->count];
        if(sealed){
            char*size=Field(&p),*hash=Field(&p);
            if(!size||!hash||!VmNumber(size,&f->size)||!VmHash(hash)||f->size>UMI_SETUP_MAX_FILE_BYTES)return UMI_STATUS_PARSE_ERROR;
            strcpy(f->hash,hash);
        }
        char*name=Field(&p);
        if(!PathField(name)||p)return UMI_STATUS_PARSE_ERROR;
        strcpy(f->relative,name);
        for(size_t i=0;i<r->count;++i)if(FoldEqual(r->files[i].relative,name)||IsAncestor(r->files[i].relative,name)||IsAncestor(name,r->files[i].relative))return UMI_STATUS_PARSE_ERROR;
        if(f->size>UINT64_C(4294967296)-total)return UMI_STATUS_CAPACITY_EXCEEDED;
        total+=f->size;
        ++r->count;
    }
    /* The initial redistributable profile has one private loader directory.
                             * Do not accidentally resolve QEMU DLLs against Umicom's main bin folder. */
    if(strncmp(r->emulator,"bin/",4)||strchr(r->emulator+4,'/')||strncmp(r->imageTool,"bin/",4)||strchr(r->imageTool+4,'/'))return UMI_STATUS_PARSE_ERROR;
    int emulator=0,tool=0,licence=0,notice=0,firmware=0;
    for(size_t i=0;i<r->count;++i){
        const char*n=r->files[i].relative;
        emulator|=!strcmp(n,r->emulator);
        tool|=!strcmp(n,r->imageTool);
        licence|=!strcmp(n,r->licence);
        notice|=!strcmp(n,r->sourceNotice);
        firmware|=IsAncestor(r->firmware,n)&&strcmp(r->firmware,n);
    }
    return field==8U&&emulator&&tool&&licence&&notice&&firmware&&strcmp(r->emulator,r->imageTool)&&strcmp(r->licence,r->sourceNotice)?UMI_STATUS_OK:UMI_STATUS_PARSE_ERROR;
}
static UmiStatus Encode(const VmRuntime*r,VmText*t){
    VmTextPrint(t,"%shost\t%s\narchitecture\t%u\nversion\t%s\nemulator\t%s\nimage-tool\t%s\nfirmware\t%s\nlicence\t%s\nsource-notice\t%s\n",VM_RUNTIME_HEADER,r->host,(unsigned)r->architecture,r->version,r->emulator,r->imageTool,r->firmware,r->licence,r->sourceNotice);
    for(size_t i=0;i<r->count;++i)VmTextPrint(t,"file\t%" PRIu64 "\t%s\t%s\n",r->files[i].size,r->files[i].hash,r->files[i].relative);
    return t->status;
}
void VmRuntimeFree(VmRuntime*r){
    if(!r)return;
    free(r->files);
    memset(r,0,sizeof *r);
}
UmiStatus VmRuntimeRead(const char*root,VmRuntime*r,int verify){
    if(!r)return UMI_STATUS_INVALID_ARGUMENT;
    memset(r,0,sizeof *r);
    if(!VmPath(root,0))return UMI_STATUS_INVALID_ARGUMENT;
    char*data=NULL;
    size_t n=0;
    UmiStatus s=VmReadJoined(root,"runtime.umi",VM_META_LIMIT,&data,&n);
    if(s==UMI_STATUS_OK)s=UmiNativeSha256Buffer(data,n,r->identity);
    if(s==UMI_STATUS_OK)s=Decode(data,n,r,1);
    free(data);
    for(size_t i=0;verify&&i<r->count&&s==UMI_STATUS_OK;++i){
        char p[UMI_SETUP_PATH_CAPACITY],hash[65];
        uint64_t size=0;
        s=UmiSetupPathJoin(root,r->files[i].relative,p);
        if(s==UMI_STATUS_OK)s=UmiSetupFileSingleLink(p,NULL);
        if(s==UMI_STATUS_OK)s=UmiSetupFileDigest(p,hash,&size,NULL);
        if(s==UMI_STATUS_OK&&(size!=r->files[i].size||strcmp(hash,r->files[i].hash)))s=UMI_STATUS_INVALID_STATE;
    }
    if(s==UMI_STATUS_OK&&verify)s=VmRuntimeInventory(root,r);
    if(s!=UMI_STATUS_OK)VmRuntimeFree(r);
    return s;
}
UmiStatus UmiVmRuntimeVerify(const char*root,UmiVmReport*r){
    if(r)memset(r,0,sizeof *r);
    VmRuntime runtime;
    UmiStatus s=VmRuntimeRead(root,&runtime,1);
    if(s==UMI_STATUS_OK&&r)strcpy(r->fingerprint,runtime.identity);
    VmRuntimeFree(&runtime);
    return VmReport(r,s,s==UMI_STATUS_OK?"Runtime inventory agrees. No executable, licence obligation or guest boot was qualified.":"Runtime inventory or a recorded file did not agree.");
}
UmiStatus UmiVmRuntimePack(const char*source,const char*inventory,const char*destination,UmiVmReport*r){
    if(r)memset(r,0,sizeof *r);
    if(!VmPath(source,0)||!VmPath(inventory,0)||!VmPath(destination,0)||IsAncestor(source,destination)||IsAncestor(destination,source))return VmReport(r,UMI_STATUS_INVALID_ARGUMENT,"Choose distinct, non-overlapping source and new output directories.");
    char*data=NULL;
    size_t size=0;
    VmRuntime runtime;
    memset(&runtime,0,sizeof runtime);
    UmiStatus s=UmiSetupFileRead(inventory,VM_META_LIMIT,&data,&size,NULL);
    if(s==UMI_STATUS_OK)s=Decode(data,size,&runtime,0);
    free(data);
    uint64_t total=0;
    for(size_t i=0;i<runtime.count&&s==UMI_STATUS_OK;++i){
        char path[UMI_SETUP_PATH_CAPACITY];
        s=UmiSetupPathJoin(source,runtime.files[i].relative,path);
        if(s==UMI_STATUS_OK)s=UmiSetupFileDigest(path,runtime.files[i].hash,&runtime.files[i].size,NULL);
        if(s==UMI_STATUS_OK){
            if(runtime.files[i].size>UINT64_C(4294967296)-total)s=UMI_STATUS_CAPACITY_EXCEEDED;
            else total+=runtime.files[i].size;
        }
    }
    for(size_t i=0;i<runtime.count&&s==UMI_STATUS_OK;++i){
        const VmRuntimeFile*f=&runtime.files[i];
        if(!f->size&&(!strcmp(f->relative,runtime.emulator)||!strcmp(f->relative,runtime.imageTool)||!strcmp(f->relative,runtime.licence)||!strcmp(f->relative,runtime.sourceNotice)))s=UMI_STATUS_INVALID_STATE;
    }
    if(s==UMI_STATUS_OK)s=UmiSetupDirectoryCreate(destination,NULL);
    if(s==UMI_STATUS_OK&&r)r->outputCreated=1;
    for(size_t i=0;i<runtime.count&&s==UMI_STATUS_OK;++i){
        char from[UMI_SETUP_PATH_CAPACITY],to[UMI_SETUP_PATH_CAPACITY];
        s=UmiSetupPathJoin(source,runtime.files[i].relative,from);
        if(s==UMI_STATUS_OK)s=UmiSetupFileParents(destination,runtime.files[i].relative,NULL);
        if(s==UMI_STATUS_OK)s=UmiSetupPathJoin(destination,runtime.files[i].relative,to);
        if(s==UMI_STATUS_OK)s=UmiSetupFileCopyChecked(from,to,runtime.files[i].hash,runtime.files[i].size,NULL);
        if(s==UMI_STATUS_OK&&!strncmp(runtime.host,"linux-",6)&&(!strcmp(runtime.files[i].relative,runtime.emulator)||!strcmp(runtime.files[i].relative,runtime.imageTool)))s=UmiSetupFileGrantOwnerExecute(to,NULL);
    }
    VmText text;
    VmTextInit(&text);
    if(s==UMI_STATUS_OK)s=Encode(&runtime,&text);
    if(s==UMI_STATUS_OK)s=VmWriteJoined(destination,"runtime.umi",text.data,text.used);
    if(s==UMI_STATUS_OK&&r)UmiNativeSha256Buffer(text.data,text.used,r->fingerprint);
    VmTextFree(&text);
    VmRuntimeFree(&runtime);
    return VmReport(r,s,s==UMI_STATUS_OK?"Offline runtime copied and sealed. No binary was executed; qualify this exact host/guest combination before release.":"Runtime preparation stopped. Any new partial directory was retained.");
}
UmiStatus UmiVmRuntimeComponent(const char*root,const char*output,UmiVmReport*r){
    if(r)memset(r,0,sizeof *r);
    if(!VmPath(output,0))return UMI_STATUS_INVALID_ARGUMENT;
    VmRuntime runtime;
    UmiStatus s=VmRuntimeRead(root,&runtime,1);
    VmText text;
    VmTextInit(&text);
    if(s==UMI_STATUS_OK&&strcmp(runtime.host,"windows-amd64"))s=UMI_STATUS_UNAVAILABLE;
    for(size_t i=0;s==UMI_STATUS_OK&&i<=runtime.count;++i){
        const char*name=i==runtime.count?"runtime.umi":runtime.files[i].relative;
        char source[UMI_SETUP_PATH_CAPACITY];
        if(strlen(name)+strlen("share/umicom/qemu/")>=UMI_SETUP_RELATIVE_CAPACITY){s=UMI_STATUS_CAPACITY_EXCEEDED;break;}
        s=UmiSetupPathJoin(root,name,source);
        if(s==UMI_STATUS_OK)VmTextPrint(&text,"owned\tumicom-vm-manager\tshare/umicom/qemu/%s\t%s\n",name,source);
    }
    if(s==UMI_STATUS_OK)s=text.status;
    if(s==UMI_STATUS_OK)s=UmiSetupFileWriteNew(output,text.data,text.used,NULL);
    if(s==UMI_STATUS_OK&&r)r->outputCreated=1;
    VmTextFree(&text);
    VmRuntimeFree(&runtime);
    return VmReport(r,s,s==UMI_STATUS_OK?"Optional runtime component records written. Their presence is not binary qualification or publisher authentication.":"Component preparation stopped; an existing output is never replaced.");
}

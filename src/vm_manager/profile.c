/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/vm_manager/profile.c
 * PURPOSE:
 *   Profiles are authoritative Data Server records. No module calls SQLite or writes a
 *   parallel profile file. Stored intentions never resurrect processes.
 * ORGANISATION: Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
 * Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Profiles are authoritative Data Server records. No module calls SQLite or
 * writes a parallel profile file. Stored intentions never resurrect processes.
 *---------------------------------------------------------------------------*/
#include "internal.h"
#include <limits.h>
static const char Prefix[]="vm.profile.";
void UmiVmProfileInit(UmiVmProfile*p){
    if(!p)return;
    memset(p,0,sizeof *p);
    p->architecture=UMI_VM_X86_64;
    p->memoryMiB=1024;
    p->processors=2;
}
UmiStatus UmiVmProfileValidate(const UmiVmProfile*p){
    if(!p||!memchr(p->id,0,sizeof p->id)||!VmId(p->id)||!memchr(p->name,0,sizeof p->name)||!p->name[0]||!VmUtf8(p->name,sizeof p->name)||        !memchr(p->runtimeDirectory,0,sizeof p->runtimeDirectory)||!VmPath(p->runtimeDirectory,0)||        !memchr(p->imageBundle,0,sizeof p->imageBundle)||!VmPath(p->imageBundle,0)||        !memchr(p->diskDirectory,0,sizeof p->diskDirectory)||!VmPath(p->diskDirectory,1)||        (p->architecture!=UMI_VM_X86_64&&p->architecture!=UMI_VM_RISCV64)||p->memoryMiB<256U||p->memoryMiB>32768U||        !p->processors||p->processors>16U||(p->recovery!=0&&p->recovery!=1))return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}
static void Hex(VmText*t,const char*s){
    const char*h="0123456789abcdef";
    if(!*s){
        VmTextPrint(t,"-");
        return;
    }
    for(size_t i=0;s[i];++i){
        unsigned c=(unsigned char)s[i];
        VmTextPrint(t,"%c%c",h[c>>4],h[c&15U]);
    }
}
UmiStatus VmProfileEncode(const UmiVmProfile*p,VmText*t){
    UmiStatus s=UmiVmProfileValidate(p);
    if(s!=UMI_STATUS_OK)return s;
    VmTextPrint(t,"UMICOM_VM_PROFILE\t1\nid\t%s\nrevision\t%" PRIu64 "\nname\t",p->id,p->revision);
    Hex(t,p->name);
    VmTextPrint(t,"\nruntime\t");
    Hex(t,p->runtimeDirectory);
    VmTextPrint(t,"\nimage\t");
    Hex(t,p->imageBundle);
    VmTextPrint(t,"\ndisk\t");
    Hex(t,p->diskDirectory);
    VmTextPrint(t,"\narchitecture\t%u\nmemory\t%u\nprocessors\t%u\nrecovery\t%d\n",(unsigned)p->architecture,p->memoryMiB,p->processors,p->recovery);
    return t->status;
}
static int H(char c){
    if(c>='0'&&c<='9')return c-'0';
    if(c>='a'&&c<='f')return c-'a'+10;
    return -1;
}
static int Unhex(const char*s,char*out,size_t cap){
    if(!strcmp(s,"-")){
        out[0]=0;
        return 1;
    }
    size_t n=strlen(s);
    if(n%2U||n/2U>=cap)return 0;
    for(size_t i=0;i<n;i+=2U){
        int a=H(s[i]),b=H(s[i+1U]);
        if(a<0||b<0||(!a&&!b))return 0;
        out[i/2U]=(char)((a<<4)|b);
    }
    out[n/2U]=0;
    return 1;
}
static char *Line(char **cursor,const char *key){
    if(!*cursor)return NULL;
    char*p=*cursor,*end=strchr(p,'\n');
    if(!end)return NULL;
    *end=0;
    *cursor=end+1;
    size_t n=strlen(key);
    if(strlen(p)<=n||strncmp(p,key,n)||p[n]!='\t')return NULL;
    return p+n+1U;
}
UmiStatus VmProfileDecode(const char*text,UmiVmProfile*p){
    const char*h="UMICOM_VM_PROFILE\t1\n";
    if(!text||!p||strlen(text)>=VM_PROFILE_BYTES||strncmp(text,h,strlen(h)))return UMI_STATUS_PARSE_ERROR;
    char *copy=malloc(strlen(text)+1U);
    if(!copy)return UMI_STATUS_OUT_OF_MEMORY;
    strcpy(copy,text);
    char*cursor=copy+strlen(h),*v;
    uint64_t n;
    UmiVmProfileInit(p);
    UmiStatus s=UMI_STATUS_PARSE_ERROR;
    v=Line(&cursor,"id");
    if(!v||!VmId(v))goto done;
    strcpy(p->id,v);
    v=Line(&cursor,"revision");
    if(!v||!VmNumber(v,&p->revision)||!p->revision)goto done;
    v=Line(&cursor,"name");
    if(!v||!Unhex(v,p->name,sizeof p->name))goto done;
    v=Line(&cursor,"runtime");
    if(!v||!Unhex(v,p->runtimeDirectory,sizeof p->runtimeDirectory))goto done;
    v=Line(&cursor,"image");
    if(!v||!Unhex(v,p->imageBundle,sizeof p->imageBundle))goto done;
    v=Line(&cursor,"disk");
    if(!v||!Unhex(v,p->diskDirectory,sizeof p->diskDirectory))goto done;
    v=Line(&cursor,"architecture");
    if(!v||!VmNumber(v,&n)||n>2U)goto done;
    p->architecture=(UmiVmArchitecture)n;
    v=Line(&cursor,"memory");
    if(!v||!VmNumber(v,&n)||n>UINT_MAX)goto done;
    p->memoryMiB=(unsigned)n;
    v=Line(&cursor,"processors");
    if(!v||!VmNumber(v,&n)||n>UINT_MAX)goto done;
    p->processors=(unsigned)n;
    v=Line(&cursor,"recovery");
    if(!v||!VmNumber(v,&n)||n>1U||*cursor)goto done;
    p->recovery=(int)n;
    if(UmiVmProfileValidate(p)==UMI_STATUS_OK)s=UMI_STATUS_OK;
    done:free(copy);
    if(s!=UMI_STATUS_OK)memset(p,0,sizeof *p);
    return s;
}
static void Key(const char*id,char out[96]){
    snprintf(out,96,"%s%s",Prefix,id);
}
UmiStatus UmiVmProfileLoad(const UmiDataServer*server,const char*id,UmiVmProfile*out){
    if(!server||!VmId(id)||!out)return UMI_STATUS_INVALID_ARGUMENT;
    char key[96];
    Key(id,key);
    char *text=malloc(VM_PROFILE_BYTES);
    if(!text)return UMI_STATUS_OUT_OF_MEMORY;
    UmiStatus s=umi_data_server_get(server,key,text,VM_PROFILE_BYTES);
    if(s==UMI_STATUS_OK)s=VmProfileDecode(text,out);
    if(s==UMI_STATUS_OK&&strcmp(id,out->id))s=UMI_STATUS_INVALID_STATE;
    free(text);
    return s;
}
typedef struct Visitor {
    UmiVmProfileVisitor callback;
    void*context;
    size_t count;
}
Visitor;
static UmiStatus Visit(const char*k,const char*v,void*c){
    Visitor *x=c;
    if(strncmp(k,Prefix,sizeof Prefix-1U))return UMI_STATUS_OK;
    if(++x->count>UMI_VM_MAX_PROFILES)return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiVmProfile p;
    UmiStatus s=VmProfileDecode(v,&p);
    if(s==UMI_STATUS_OK&&strcmp(k+sizeof Prefix-1U,p.id))s=UMI_STATUS_INVALID_STATE;
    if(s==UMI_STATUS_OK&&x->callback)s=x->callback(&p,x->context);
    return s;
}
UmiStatus UmiVmProfileVisit(const UmiDataServer*s,UmiVmProfileVisitor v,void*c){
    if(!s||!v)return UMI_STATUS_INVALID_ARGUMENT;
    Visitor x={
        v,c,0
    };
    return umi_data_server_visit(s,Visit,&x);
}
static UmiStatus End(UmiDataServer*s,UmiStatus status){
    if(status==UMI_STATUS_OK)status=umi_data_server_commit(s);
    if(status!=UMI_STATUS_OK&&umi_data_server_rollback(s)!=UMI_STATUS_OK)return UMI_STATUS_INVALID_STATE;
    return status;
}
UmiStatus UmiVmProfileSave(UmiDataServer*server,const UmiVmProfile*p,uint64_t expected,uint64_t*out){
    if(!server||!out||expected==UINT64_MAX||UmiVmProfileValidate(p)!=UMI_STATUS_OK)return UMI_STATUS_INVALID_ARGUMENT;
    *out=0;
    if(umi_data_server_in_transaction(server))return UMI_STATUS_BUSY;
    UmiStatus s=umi_data_server_begin(server);
    if(s!=UMI_STATUS_OK)return s;
    UmiVmProfile old;
    s=UmiVmProfileLoad(server,p->id,&old);
    if(s==UMI_STATUS_NOT_FOUND){
        s=expected?UMI_STATUS_INVALID_STATE:UMI_STATUS_OK;
        if(s==UMI_STATUS_OK){
            Visitor x={
                NULL,NULL,0
            };
            s=umi_data_server_visit(server,Visit,&x);
            if(s==UMI_STATUS_OK&&x.count==UMI_VM_MAX_PROFILES)s=UMI_STATUS_CAPACITY_EXCEEDED;
        }
    }
    else if(s==UMI_STATUS_OK&&old.revision!=expected)s=UMI_STATUS_INVALID_STATE;
    if(s==UMI_STATUS_OK){
        UmiVmProfile next=*p;
        next.revision=expected+1U;
        VmText t;
        VmTextInit(&t);
        s=VmProfileEncode(&next,&t);
        char key[96];
        Key(p->id,key);
        if(s==UMI_STATUS_OK)s=umi_data_server_set(server,key,t.data);
        VmTextFree(&t);
    }
    s=End(server,s);
    if(s==UMI_STATUS_OK)*out=expected+1U;
    return s;
}
UmiStatus UmiVmProfileRemove(UmiDataServer*server,const char*id,uint64_t expected){
    if(!server||!VmId(id)||!expected)return UMI_STATUS_INVALID_ARGUMENT;
    if(umi_data_server_in_transaction(server))return UMI_STATUS_BUSY;
    UmiStatus s=umi_data_server_begin(server);
    if(s!=UMI_STATUS_OK)return s;
    UmiVmProfile p;
    s=UmiVmProfileLoad(server,id,&p);
    if(s==UMI_STATUS_OK&&p.revision!=expected)s=UMI_STATUS_INVALID_STATE;
    if(s==UMI_STATUS_OK){
        char key[96];
        Key(id,key);
        s=umi_data_server_delete(server,key);
    }
    return End(server,s);
}

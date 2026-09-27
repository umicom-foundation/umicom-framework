/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Bounded native image text and integer operations. */
#include "internal.h"
#include <limits.h>
void OiInit(UmiOsImageReport *r) {
    if(r) {
        memset(r,0,sizeof *r);
        r->exitCode=-1;
    }
}
UmiStatus OiReport(UmiOsImageReport *r,UmiStatus s,const char *message) {
    if(r) {
        r->status=s;
        size_t n=strlen(message);
        if(n>=sizeof r->detail)n=sizeof r->detail-1U;
        memcpy(r->detail,message,n);
        r->detail[n]=0;
    }
    return s;
}
void OiTextInit(OiText *t) {
    memset(t,0,sizeof *t);
}
void OiTextClear(OiText *t) {
    free(t->data);
    memset(t,0,sizeof *t);
}
void OiAppend(OiText *t,const void *data,size_t size) {
    if(t->status!=UMI_STATUS_OK)return;
    if((size && !data)||size>UMI_OS_IMAGE_MAX_ARCHIVE+16384U-t->size) {
        t->status=UMI_STATUS_CAPACITY_EXCEEDED;
        return;
    }
    size_t need=t->size+size+1U;
    if(need>t->capacity) {
        size_t cap=need+(need/4U);
        if(cap<need)cap=need;
        unsigned char *p=realloc(t->data,cap);
        if(!p) {
            t->status=UMI_STATUS_OUT_OF_MEMORY;
            return;
        }
        t->data=p;
        t->capacity=cap;
    }
    if(size)memcpy(t->data+t->size,data,size);
    t->size+=size;
    t->data[t->size]=0;
}
void OiPrint(OiText *t,const char *format,...) {
    if(t->status!=UMI_STATUS_OK)return;
    va_list args,copy;
    va_start(args,format);
    va_copy(copy,args);
    int n=vsnprintf(NULL,0,format,copy);
    va_end(copy);
    if(n<0||(unsigned)n>OI_META_LIMIT) {
        t->status=UMI_STATUS_CAPACITY_EXCEEDED;
        va_end(args);
        return;
    }
    char *buf=malloc((size_t)n+1U);
    if(!buf) {
        t->status=UMI_STATUS_OUT_OF_MEMORY;
        va_end(args);
        return;
    }
    int written=vsnprintf(buf,(size_t)n+1U,format,args);
    va_end(args);
    if(written!=n)t->status=UMI_STATUS_INTERNAL_ERROR;
    else OiAppend(t,buf,(size_t)n);
    free(buf);
}
int OiHash(const char *s,size_t digits) {
    if(!s||strlen(s)!=digits)return 0;
    for(size_t i=0;i<digits;++i)if(!((s[i]>='0'&&s[i]<='9')||(s[i]>='a'&&s[i]<='f')))return 0;
    return 1;
}
int OiNumber(const char *s,uint64_t *out) {
    if(!s||!*s||!out||(s[0]=='0'&&s[1]))return 0;
    uint64_t n=0;
    for(;*s;++s) {
        if(*s<'0'||*s>'9'||n>(UINT64_MAX-(unsigned)(*s-'0'))/10U)return 0;
        n=n*10U+(unsigned)(*s-'0');
    }
    *out=n;
    return 1;
}
int OiArchValid(UmiOsImageArch a) {
    return a==UMI_OS_IMAGE_RISCV64||a==UMI_OS_IMAGE_X86_64;
}
const char *OiArchName(UmiOsImageArch a) {
    return a==UMI_OS_IMAGE_RISCV64?"riscv64":a==UMI_OS_IMAGE_X86_64?"x86_64":"invalid";
}
const char *OiKernelName(UmiOsImageArch a) {
    return a==UMI_OS_IMAGE_RISCV64?"Image":"bzImage";
}
UmiStatus UmiOsImageValidateName(const char *s) {
    if(!s||!*s||strlen(s)>=UMI_OS_IMAGE_NAME||!strcmp(s,"TRAILER!!!")||s[0]=='/')return UMI_STATUS_INVALID_ARGUMENT;
    const char *start=s;
    for(const char *p=s;;++p) {
        unsigned char c=(unsigned char)*p;
        if(c && (c<33U||c>126U||c=='\\'||c==':'||c=='\t'))return UMI_STATUS_INVALID_ARGUMENT;
        if(c=='/'||!c) {
            size_t n=(size_t)(p-start);
            if(!n||(n==1U&&start[0]=='.')||(n==2U&&start[0]=='.'&&start[1]=='.'))return UMI_STATUS_INVALID_ARGUMENT;
            start=p+1;
            if(!c)break;
        }
    }
    return UMI_STATUS_OK;
}
UmiStatus OiAbsolute(const char *s,int buildPath) {
    if(!s||!*s||strlen(s)>=UMI_OS_IMAGE_PATH)return UMI_STATUS_INVALID_ARGUMENT;
    size_t start=1;
#ifdef _WIN32
    if(!((s[0]>='A'&&s[0]<='Z')||(s[0]>='a'&&s[0]<='z'))||s[1]!=':'||(s[2]!='/'&&s[2]!='\\'))return UMI_STATUS_INVALID_ARGUMENT;
    start=3;
#else
    if(s[0]!='/')return UMI_STATUS_INVALID_ARGUMENT;
#endif
    size_t length=strlen(s);
    if(length<=start)return UMI_STATUS_INVALID_ARGUMENT;
    for(size_t i=0;i<length;++i) {
        unsigned char c=(unsigned char)s[i];
        if(c<32U||c==127U||(buildPath && (c<=32U||c>126U||strchr("$#;`\"'\\",c))))return UMI_STATUS_INVALID_ARGUMENT;
    }
    size_t component=start;
    for(size_t i=start;i<=length;++i) {
        if(s[i]=='/'||s[i]=='\\'||!s[i]) {
            size_t n=i-component;
            if(!n||(n==1U&&s[component]=='.')||(n==2U&&s[component]=='.'&&s[component+1U]=='.'))return UMI_STATUS_INVALID_ARGUMENT;
            component=i+1U;
        }
    }
    return UMI_STATUS_OK;
}
UmiStatus OiJoin(const char *root,const char *name,char out[UMI_OS_IMAGE_PATH]) {
    if(!root||!name||!out||!root[0])return UMI_STATUS_INVALID_ARGUMENT;
    size_t a=strlen(root),b=strlen(name);
    if(a+b+2U>UMI_OS_IMAGE_PATH)return UMI_STATUS_CAPACITY_EXCEEDED;
    memcpy(out,root,a);
    out[a]='/';
    memcpy(out+a+1U,name,b+1U);
    return UMI_STATUS_OK;
}
int OiContains(const char *parent,const char *child) {
    size_t n=strlen(parent);
    return !strncmp(parent,child,n)&&(child[n]==0||child[n]=='/');
}
uint16_t OiLe16(const unsigned char *p) {
    return (uint16_t)((unsigned)p[0]|((unsigned)p[1]<<8));
}
uint32_t OiLe32(const unsigned char *p) {
    return (uint32_t)p[0]|((uint32_t)p[1]<<8)|((uint32_t)p[2]<<16)|((uint32_t)p[3]<<24);
}
uint64_t OiLe64(const unsigned char *p) {
    return (uint64_t)OiLe32(p)|((uint64_t)OiLe32(p+4)<<32);
}
void OiPut32(unsigned char *p,uint32_t v) {
    for(unsigned i=0;i<4;++i)p[i]=(unsigned char)(v>>(8U*i));
}
uint32_t OiCrc32(const void *data,size_t size) {
    const unsigned char *p=data;
    uint32_t c=UINT32_MAX;
    for(size_t i=0;i<size;++i) {
        c^=p[i];
        for(unsigned b=0;b<8;++b)c=(c>>1)^((0U-(c&1U))&UINT32_C(0xedb88320));
    }
    return ~c;
}
UmiStatus OiReadJoined(const char *root,const char *name,size_t limit,unsigned char **data,size_t *size) {
    char path[UMI_OS_IMAGE_PATH];
    UmiStatus s=OiJoin(root,name,path);
    return s==UMI_STATUS_OK?OiRead(path,limit,data,size):s;
}
UmiStatus OiDigest(const char *path,char out[65],uint64_t *bytes) {
    unsigned char *data=NULL;
    size_t size=0;
    UmiStatus s=OiRead(path,UMI_OS_IMAGE_MAX_ARCHIVE,&data,&size);
    if(s==UMI_STATUS_OK)s=UmiNativeSha256Buffer(data,size,out);
    if(s==UMI_STATUS_OK&&bytes)*bytes=(uint64_t)size;
    free(data);
    return s;
}

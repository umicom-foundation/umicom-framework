/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT */
#include "internal.h"
#include <stdarg.h>
#include <limits.h>
void VmTextInit(VmText*t){
    memset(t,0,sizeof *t);
}
void VmTextPrint(VmText*t,const char *format,...){
    if(t->status!=UMI_STATUS_OK)return;
    va_list a,b;
    va_start(a,format);
    va_copy(b,a);
    int n=vsnprintf(NULL,0,format,a);
    va_end(a);
    if(n<0||(size_t)n>VM_META_LIMIT-t->used){
        t->status=UMI_STATUS_CAPACITY_EXCEEDED;
        va_end(b);
        return;
    }
    size_t needed=t->used+(size_t)n+1U;
    if(needed>t->capacity){
        size_t cap=needed+1024U;
        char*p=realloc(t->data,cap);
        if(!p){
            t->status=UMI_STATUS_OUT_OF_MEMORY;
            va_end(b);
            return;
        }
        t->data=p;
        t->capacity=cap;
    }
    vsnprintf(t->data+t->used,t->capacity-t->used,format,b);
    va_end(b);
    t->used+=(size_t)n;
}
void VmTextFree(VmText*t){
    free(t->data);
    memset(t,0,sizeof *t);
}
UmiStatus VmReport(UmiVmReport*r,UmiStatus s,const char*m){
    if(r){
        r->status=s;
        snprintf(r->detail,sizeof r->detail,"%s",m?m:UmiSetupStatusText(s));
    }
    return s;
}
int VmId(const char*s){
    if(!s||!*s||strlen(s)>=64U)return 0;
    for(size_t i=0;s[i];++i)if(!((s[i]>='a'&&s[i]<='z')||(s[i]>='0'&&s[i]<='9')||s[i]=='-'||s[i]=='_'))return 0;
    return 1;
}
int VmHash(const char*s){
    if(!s||strlen(s)!=64U)return 0;
    for(size_t i=0;i<64U;++i)if(!((s[i]>='0'&&s[i]<='9')||(s[i]>='a'&&s[i]<='f')))return 0;
    return 1;
}
int VmNumber(const char*s,uint64_t*n){
    if(!s||!*s||!n||(s[0]=='0'&&s[1]))return 0;
    uint64_t v=0;
    for(size_t i=0;s[i];++i){
        unsigned c=(unsigned char)s[i];
        if(c<'0'||c>'9'||v>(UINT64_MAX-(c-'0'))/10U)return 0;
        v=v*10U+c-'0';
    }
    *n=v;
    return 1;
}
int VmPath(const char*p,int optional){
    if(!p)return 0;
    if(!*p)return optional;
    return VmUtf8(p,UMI_VM_PATH)&&UmiSetupValidateAbsolute(p)==UMI_STATUS_OK;
}
const char *VmHost(void){
#ifdef _WIN32
    return "windows-amd64";
#elif defined(__linux__) && defined(__x86_64__)
    return "linux-amd64";
#elif defined(__linux__) && defined(__aarch64__)
    return "linux-arm64";
#else
    return "unsupported";
#endif
}
UmiStatus VmReadJoined(const char*r,const char*n,size_t limit,char **b,size_t*z){
    char p[UMI_SETUP_PATH_CAPACITY];
    UmiStatus s=UmiSetupPathJoin(r,n,p);
    return s==UMI_STATUS_OK?UmiSetupFileRead(p,limit,b,z,NULL):s;
}
UmiStatus VmWriteJoined(const char*r,const char*n,const void*b,size_t z){
    char p[UMI_SETUP_PATH_CAPACITY];
    UmiStatus s=UmiSetupPathJoin(r,n,p);
    return s==UMI_STATUS_OK?UmiSetupFileWriteNew(p,b,z,NULL):s;
}

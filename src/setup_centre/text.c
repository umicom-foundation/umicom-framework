/*-----------------------------------------------------------------------------
 * Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * File: src/setup_centre/text.c
 * Native bootstrap encoders and portable path validation. Paths in a release
 * are data, not commands. Reject ambiguous Windows names even on Linux.
 *---------------------------------------------------------------------------*/

#include "internal.h"
#include <ctype.h>
#include <inttypes.h>
void ScReport(UmiSetupReport *r,UmiStatus status,const char *format,...)
{

    if(r==NULL)return;

    r->status=status;

    if(format!=NULL) {
        va_list args;
        va_start(args,format);
        (void)vsnprintf(r->detail,sizeof r->detail,format,args);
        va_end(args);
    }

}

const char *UmiSetupStatusText(UmiStatus s)
{

    switch(s) {
        case UMI_STATUS_OK:return "Completed";
        case UMI_STATUS_INVALID_ARGUMENT:return "Invalid input";

        case UMI_STATUS_INVALID_STATE:return "Inputs changed or state invalid";
        case UMI_STATUS_CAPACITY_EXCEEDED:return "Capacity exceeded";

        case UMI_STATUS_NOT_FOUND:return "Not found";
        case UMI_STATUS_ALREADY_EXISTS:return "Destination already exists";

        case UMI_STATUS_OUT_OF_MEMORY:return "Not enough memory";
        case UMI_STATUS_PERMISSION_DENIED:return "Permission denied";

        case UMI_STATUS_IO_ERROR:return "File operation failed";
        case UMI_STATUS_PARSE_ERROR:return "Invalid manifest";

        case UMI_STATUS_UNAVAILABLE:return "Not available on this host";
        case UMI_STATUS_CANCELLED:return "Cancelled";

        case UMI_STATUS_TIMEOUT:return "Timed out";
        default:return "Not completed";
    }

}

void ScTextInit(ScText *t) {
    memset(t,0,sizeof *t);
}

void ScTextFree(ScText *t) {
    free(t->data);
    memset(t,0,sizeof *t);
}

void ScPrint(ScText *t,const char *format,...)
{

    if(t->status!=UMI_STATUS_OK)return;

    va_list a,b;
    va_start(a,format);
    va_copy(b,a);
    int n=vsnprintf(NULL,0,format,a);
    va_end(a);

    if(n<0||(size_t)n>UMI_SETUP_MANIFEST_LIMIT-t->size) {
        va_end(b);
        t->status=UMI_STATUS_CAPACITY_EXCEEDED;
        return;
    }

    size_t need=t->size+(size_t)n+1U;

    if(need>t->capacity) {
        size_t cap=t->capacity?t->capacity:1024U;
        while(cap<need)cap*=2U;

        char *p=realloc(t->data,cap);
        if(p==NULL) {
            va_end(b);
            t->status=UMI_STATUS_OUT_OF_MEMORY;
            return;
        }
        t->data=p;
        t->capacity=cap;
    }

    (void)vsnprintf(t->data+t->size,t->capacity-t->size,format,b);
    va_end(b);
    t->size+=(size_t)n;

}

int ScUtf8(const char *text)
{

    if(text==NULL)return 0;
    const unsigned char *p=(const unsigned char *)text;

    while(*p) {
        uint32_t point;
        unsigned more;

        if(*p<0x80U) {
            if(*p<32U||*p==127U)return 0;
            ++p;
            continue;
        }

        if(*p>=0xc2U&&*p<=0xdfU) {
            point=*p&31U;
            more=1U;
        }

        else if(*p>=0xe0U&&*p<=0xefU) {
            point=*p&15U;
            more=2U;
        }

        else if(*p>=0xf0U&&*p<=0xf4U) {
            point=*p&7U;
            more=3U;
        }
        else return 0;

        ++p;
        for(unsigned i=0; i<more; ++i) {
            if((*p&0xc0U)!=0x80U)return 0;
            point=(point<<6U)|(*p&63U);
            ++p;
        }

        if((more==1U&&point<0x80U)||(more==2U&&point<0x800U)||(more==3U&&point<0x10000U)||point>0x10ffffU||(point>=0xd800U&&point<=0xdfffU))return 0;

    }
    return 1;

}

static unsigned char Fold(unsigned char c) {
    return c>='A'&&c<='Z'?(unsigned char)(c+('a'-'A')):c;
}

int ScEqualFold(const char *a,const char *b)
{
    while(*a&&*b) {
        unsigned char x=Fold((unsigned char)*a++),y=Fold((unsigned char)*b++);
        if(x!=y)return x<y?-1:1;
    }
    return *a?1:*b?-1:0;
}

int ScToken(const char *s)
{
    if(s==NULL||!*s||strlen(s)>=80U)return 0;
    for(; *s; ++s)if(!((*s>='a'&&*s<='z')||(*s>='0'&&*s<='9')||*s=='-'||*s=='_'))return 0;
    return 1;
}

int ScHashValid(const char *s)
{
    if(s==NULL||strlen(s)!=64U)return 0;
    for(size_t i=0; i<64U; ++i)if(!((s[i]>='0'&&s[i]<='9')||(s[i]>='a'&&s[i]<='f')))return 0;
    return 1;
}

int ScNumber(const char *s,uint64_t *out)
{
    if(s==NULL||!*s||(s[0]=='0'&&s[1]))return 0;
    uint64_t n=0;
    for(; *s; ++s) {
        if(*s<'0'||*s>'9'||n>(UINT64_MAX-(uint64_t)(*s-'0'))/10U)return 0;
        n=n*10U+(uint64_t)(*s-'0');
    }
    *out=n;
    return 1;
}

static int Component(const char *s,size_t n,int portable)
{

    if(n==0U||n>255U||(n==1U&&s[0]=='.')||(n==2U&&s[0]=='.'&&s[1]=='.'))return 0;

    if(!portable)return 1;

    if(s[n-1U]=='.'||s[n-1U]==' ')return 0;

    char base[8]= {
        0
    };
    size_t b=0;
    for(size_t i=0; i<n; ++i) {
        if(strchr("<>:\"|?*\\",s[i]))return 0;
        if(s[i]=='.')break;
        if(b<7U)base[b++]=(char)Fold((unsigned char)s[i]);
    }

    if(!strcmp(base,"con")||!strcmp(base,"prn")||!strcmp(base,"aux")||!strcmp(base,"nul"))return 0;

    if(b==4U&&(!strncmp(base,"com",3)||!strncmp(base,"lpt",3))&&base[3]>='1'&&base[3]<='9')return 0;

    return 1;

}

UmiStatus UmiSetupValidateRelative(const char *s)
{

    if(s==NULL||!*s||strlen(s)>=UMI_SETUP_RELATIVE_CAPACITY||!ScUtf8(s))return UMI_STATUS_INVALID_ARGUMENT;

    const char *begin=s;
    for(const char *p=s; ; ++p)if(*p=='/'||*p==0) {
        if(!Component(begin,(size_t)(p-begin),1))return UMI_STATUS_INVALID_ARGUMENT;
        if(!*p)break;
        begin=p+1;
    }

    return UMI_STATUS_OK;

}

UmiStatus UmiSetupValidateAbsolute(const char *s)
{

    if(s==NULL||strlen(s)>=UMI_SETUP_PATH_CAPACITY||!ScUtf8(s))return UMI_STATUS_INVALID_ARGUMENT;

    int windows=0;
    const char *p=s;

    if(s[0]=='/'&&s[1]!='/')++p;

    else if(strlen(s)>=3U&&isalpha((unsigned char)s[0])&&s[1]==':'&&(s[2]=='/'||s[2]=='\\')) {
        windows=1;
        p+=3;
    }

    else return UMI_STATUS_INVALID_ARGUMENT;

    if(!*p)return UMI_STATUS_OK;

    const char *begin=p;

    for(; ; ++p)if(*p=='/'||(windows&&*p=='\\')||!*p) {
        if(!Component(begin,(size_t)(p-begin),windows))return UMI_STATUS_INVALID_ARGUMENT;
        if(!*p)break;
        begin=p+1;
    }

    return UMI_STATUS_OK;

}

UmiStatus ScJoin(const char *root,const char *rel,char out[UMI_SETUP_PATH_CAPACITY])
{

    if(UmiSetupValidateAbsolute(root)!=UMI_STATUS_OK||UmiSetupValidateRelative(rel)!=UMI_STATUS_OK)return UMI_STATUS_INVALID_ARGUMENT;

    size_t n=strlen(root);
    int slash=n&&root[n-1U]!='/'&&root[n-1U]!='\\';

    int r=snprintf(out,UMI_SETUP_PATH_CAPACITY,"%s%s%s",root,slash?"/":"",rel);

    return r<0||(size_t)r>=UMI_SETUP_PATH_CAPACITY?UMI_STATUS_CAPACITY_EXCEEDED:UMI_STATUS_OK;

}

int ScSelected(const ScFile *f,uint64_t selected) {
    return f->owner==SC_SHARED||((selected>>f->owner)&UINT64_C(1))!=0U;
}

static int Prefix(const char *a,const char *b)
{

    size_t n=strlen(a);

    if(strlen(b)<=n || b[n]!='/')return 0;

    for(size_t i=0; i<n; ++i)if(Fold((unsigned char)a[i])!=Fold((unsigned char)b[i]))return 0;

    return 1;

}

static size_t LowerBound(const UmiSetupBundle *b,const char *path)
{

    size_t lo=0,hi=b->fileCount;

    while(lo<hi) {
        size_t mid=lo+(hi-lo)/2U;

        if(ScEqualFold(b->files[mid].relative,path)<0)lo=mid+1U;
        else hi=mid;
    }

    return lo;

}

UmiStatus ScAddFile(UmiSetupBundle *b,const ScFile *f)
{

    if(UmiSetupValidateRelative(f->relative)!=UMI_STATUS_OK||!ScHashValid(f->hash)||f->bytes>UMI_SETUP_MAX_FILE_BYTES)return UMI_STATUS_PARSE_ERROR;

    if(ScEqualFold(f->relative,UMI_SETUP_RECEIPT)==0 ||
        ScEqualFold(f->relative,UMI_SETUP_CATALOGUE)==0)
        return UMI_STATUS_PARSE_ERROR;
    size_t lo=0,hi=b->fileCount;
    while(lo<hi) {
        size_t m=lo+(hi-lo)/2U;
        if(ScEqualFold(b->files[m].relative,f->relative)<0)lo=m+1U;
        else hi=m;
    }

    if(lo<b->fileCount&&ScEqualFold(b->files[lo].relative,f->relative)==0) {
        const ScFile *old=&b->files[lo];
        return !strcmp(old->relative,f->relative)&&old->bytes==f->bytes&&!strcmp(old->hash,f->hash)&&old->owner==f->owner?UMI_STATUS_ALREADY_EXISTS:UMI_STATUS_INVALID_STATE;
    }

    /* A sibling such as a-file sorts between a and a/b. Check every exact
     * ancestor and the first possible descendant, not just adjacent records. */

    char path[UMI_SETUP_RELATIVE_CAPACITY+1U];
    strcpy(path,f->relative);

    for(size_t i=0; path[i]; ++i)if(path[i]=='/') {

        path[i]=0;
        size_t at=LowerBound(b,path);

        int collision=at<b->fileCount&&ScEqualFold(b->files[at].relative,path)==0;

        path[i]='/';
        if(collision)return UMI_STATUS_INVALID_STATE;
    }

    size_t length=strlen(path);
    path[length]='/';
    path[length+1U]=0;

    size_t descendant=LowerBound(b,path);

    if(descendant<b->fileCount&&Prefix(f->relative,b->files[descendant].relative))return UMI_STATUS_INVALID_STATE;

    if(b->fileCount==UMI_SETUP_MAX_FILES)return UMI_STATUS_CAPACITY_EXCEEDED;

    if(b->fileCount==b->capacity) {
        size_t cap=b->capacity?b->capacity*2U:128U;
        if(cap>UMI_SETUP_MAX_FILES)cap=UMI_SETUP_MAX_FILES;
        ScFile *p=realloc(b->files,cap*sizeof *p);
        if(!p)return UMI_STATUS_OUT_OF_MEMORY;
        b->files=p;
        b->capacity=cap;
    }

    memmove(b->files+lo+1U,b->files+lo,(b->fileCount-lo)*sizeof *b->files);
    b->files[lo]=*f;
    ++b->fileCount;
    return UMI_STATUS_OK;

}

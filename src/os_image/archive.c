/*-----------------------------------------------------------------------------
 * Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Linux newc archives and deterministic gzip stored blocks, implemented in C.
 * Ownership: this module describes device nodes as archive records. It never
 * creates a device on the host. The decoder is deliberately narrower than a
 * general gzip extractor and never executes or extracts untrusted content.
 *---------------------------------------------------------------------------*/
#include "internal.h"
struct UmiOsImageArchive {
    unsigned char *raw;
    size_t size, count;
    UmiOsImageEntry entries[UMI_OS_IMAGE_MAX_ENTRIES];
}
;
static UmiStatus EntryValid(const UmiOsImageEntry *e) {
    if(!e||UmiOsImageValidateName(e->name)!=UMI_STATUS_OK||e->size>UMI_OS_IMAGE_MAX_FILE|| (e->size&&!e->data)||(e->mode&~0177777U)||(e->mode&06000U))return UMI_STATUS_INVALID_ARGUMENT;
    uint32_t kind=e->mode&OI_TYPE_MASK;
    if(kind!=OI_REG&&kind!=OI_DIR&&kind!=OI_CHR)return UMI_STATUS_INVALID_ARGUMENT;
    if(kind!=OI_REG&&e->size)return UMI_STATUS_INVALID_ARGUMENT;
    if(kind==OI_CHR) {
        if(!((!strcmp(e->name,"dev/console")&&e->major==5U&&e->minor==1U)|| (!strcmp(e->name,"dev/null")&&e->major==1U&&e->minor==3U)))return UMI_STATUS_INVALID_ARGUMENT;
    }
    else if(e->major||e->minor)return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}
static int Compare(const void *left,const void *right) {
    const UmiOsImageEntry *const *a=left,*const *b=right;
    return strcmp((*a)->name,(*b)->name);
}
static void Pad(OiText *t,size_t alignment) {
    static const unsigned char zero[512]= {
        0
    }
    ;
    size_t n=(alignment-t->size%alignment)%alignment;
    OiAppend(t,zero,n);
}
static void Header(OiText *t,uint32_t inode,const UmiOsImageEntry *e,int trailer) {
    uint32_t f[13]= {
        inode,e->mode,0,0,(e->mode&OI_TYPE_MASK)==OI_DIR?2U:1U,0,(uint32_t)e->size,0,0,e->major,e->minor,(uint32_t)strlen(e->name)+1U,0
    }
    ;
    if(trailer) {
        memset(f,0,sizeof f);
        f[4]=1;
        f[11]=11;
    }
    OiAppend(t,"070701",6);
    for(size_t i=0;i<13U;++i)OiPrint(t,"%08" PRIx32,f[i]);
    OiAppend(t,e->name,strlen(e->name)+1U);
    Pad(t,4);
    OiAppend(t,e->data,e->size);
    Pad(t,4);
}
static UmiStatus Gzip(const OiText *raw,unsigned char **out,size_t *size) {
    const unsigned char header[10]= {
        31,139,8,0,0,0,0,0,0,3
    }
    ;
    OiText t;
    OiTextInit(&t);
    OiAppend(&t,header,sizeof header);
    size_t at=0;
    while(at<raw->size) {
        size_t n=raw->size-at;
        if(n>65535U)n=65535U;
        unsigned char block[5]= {
            (unsigned char)(at+n==raw->size?1:0),0,0,0,0
        }
        ;
        uint16_t len=(uint16_t)n,complement=(uint16_t)~len;
        block[1]=(unsigned char)len;
        block[2]=(unsigned char)(len>>8);
        block[3]=(unsigned char)complement;
        block[4]=(unsigned char)(complement>>8);
        OiAppend(&t,block,5);
        OiAppend(&t,raw->data+at,n);
        at+=n;
    }
    unsigned char tail[8];
    OiPut32(tail,OiCrc32(raw->data,raw->size));
    OiPut32(tail+4,(uint32_t)raw->size);
    OiAppend(&t,tail,8);
    UmiStatus s=t.status;
    if(s==UMI_STATUS_OK) {
        *out=t.data;
        *size=t.size;
    }
    else OiTextClear(&t);
    return s;
}
UmiStatus UmiOsImageArchiveBuild(const UmiOsImageEntry *entries,size_t count,unsigned char **out,size_t *size) {
    if(!out||!size)return UMI_STATUS_INVALID_ARGUMENT;
    *out=NULL;
    *size=0;
    if(!entries||!count||count>UMI_OS_IMAGE_MAX_ENTRIES)return UMI_STATUS_INVALID_ARGUMENT;
    const UmiOsImageEntry *order[UMI_OS_IMAGE_MAX_ENTRIES];
    for(size_t i=0;i<count;++i) {
        if(EntryValid(&entries[i])!=UMI_STATUS_OK)return UMI_STATUS_INVALID_ARGUMENT;
        order[i]=&entries[i];
    }
    qsort(order,count,sizeof order[0],Compare);
    OiText raw;
    OiTextInit(&raw);
    UmiStatus s=UMI_STATUS_OK;
    for(size_t i=0;i<count&&s==UMI_STATUS_OK;++i) {
        const UmiOsImageEntry *e=order[i];
        if(i&&!strcmp(e->name,order[i-1U]->name)) {
            s=UMI_STATUS_INVALID_ARGUMENT;
            break;
        }
        const char *slash=strrchr(e->name,'/');
        if(slash) {
            size_t n=(size_t)(slash-e->name);
            int found=0;
            for(size_t j=0;j<i;++j)if(strlen(order[j]->name)==n&&!strncmp(order[j]->name,e->name,n)&& (order[j]->mode&OI_TYPE_MASK)==OI_DIR)found=1;
            if(!found) {
                s=UMI_STATUS_INVALID_ARGUMENT;
                break;
            }
        }
        if(raw.size+strlen(e->name)+e->size+1024U>UMI_OS_IMAGE_MAX_ARCHIVE) {
            s=UMI_STATUS_CAPACITY_EXCEEDED;
            break;
        }
        Header(&raw,(uint32_t)i+1U,e,0);
        s=raw.status;
    }
    UmiOsImageEntry trailer= {
        "TRAILER!!!",0,0,0,NULL,0
    }
    ;
    if(s==UMI_STATUS_OK) {
        Header(&raw,0,&trailer,1);
        Pad(&raw,512);
        s=raw.status;
    }
    if(s==UMI_STATUS_OK)s=Gzip(&raw,out,size);
    OiTextClear(&raw);
    return s;
}
static int Hex(const unsigned char *p,uint32_t *out) {
    uint32_t n=0;
    for(size_t i=0;i<8;++i) {
        unsigned c=p[i],v;
        if(c>='0'&&c<='9')v=c-'0';
        else if(c>='a'&&c<='f')v=c-'a'+10U;
        else if(c>='A'&&c<='F')v=c-'A'+10U;
        else return 0;
        n=(n<<4)|v;
    }
    *out=n;
    return 1;
}
static UmiStatus InflateStored(const unsigned char *p,size_t n,unsigned char **out,size_t *size) {
    if(n<18U||memcmp(p,"\x1f\x8b\x08",3))return UMI_STATUS_PARSE_ERROR;
    /* Only the documented no-option, no-time, stored-block profile is accepted.
     * Unsupported compression is not misreported as a successfully read image. */
    if(p[3]||p[4]||p[5]||p[6]||p[7]||p[8]||p[9]!=3U)return UMI_STATUS_UNAVAILABLE;
    size_t at=10;
    OiText t;
    OiTextInit(&t);
    int last=0;
    UmiStatus s=UMI_STATUS_OK;
    while(!last&&s==UMI_STATUS_OK) {
        if(at>n-8U||n-8U-at<5U) {
            s=UMI_STATUS_PARSE_ERROR;
            break;
        }
        unsigned flag=p[at++];
        if(flag>1U) {
            s=UMI_STATUS_UNAVAILABLE;
            break;
        }
        last=flag==1U;
        uint16_t length=OiLe16(p+at),inverse=OiLe16(p+at+2U);
        at+=4U;
        if(((uint32_t)length^(uint32_t)inverse)!=65535U||!length||length>n-8U-at) {
            s=UMI_STATUS_PARSE_ERROR;
            break;
        }
        if(t.size+(size_t)length>UMI_OS_IMAGE_MAX_ARCHIVE) {
            s=UMI_STATUS_CAPACITY_EXCEEDED;
            break;
        }
        OiAppend(&t,p+at,length);
        at+=length;
        s=t.status;
    }
    if(s==UMI_STATUS_OK&&(at!=n-8U||OiLe32(p+at+4U)!=(uint32_t)t.size||OiLe32(p+at)!=OiCrc32(t.data,t.size)))s=UMI_STATUS_PARSE_ERROR;
    if(s==UMI_STATUS_OK) {
        *out=t.data;
        *size=t.size;
    }
    else OiTextClear(&t);
    return s;
}
static int Zeros(const unsigned char *p,size_t n) {
    for(size_t i=0;i<n;++i)if(p[i])return 0;
    return 1;
}
static UmiStatus ParseRaw(UmiOsImageArchive *a) {
    size_t at=0;
    const size_t n=a->size;
    unsigned char *p=a->raw;
    if(n%512U)return UMI_STATUS_PARSE_ERROR;
    while(at<n) {
        if(n-at<110U||memcmp(p+at,"070701",6))return UMI_STATUS_PARSE_ERROR;
        uint32_t f[13];
        for(size_t i=0;i<13;++i)if(!Hex(p+at+6U+8U*i,&f[i]))return UMI_STATUS_PARSE_ERROR;
        at+=110U;
        if(f[2]||f[3]||f[5]||f[7]||f[8]||f[12]||f[11]<2U||f[11]>UMI_OS_IMAGE_NAME||f[11]>n-at)return UMI_STATUS_PARSE_ERROR;
        char *name=(char *)(p+at);
        if(name[f[11]-1U]||memchr(name,0,f[11]-1U))return UMI_STATUS_PARSE_ERROR;
        at+=f[11];
        size_t padding=(4U-at%4U)%4U;
        if(padding>n-at||!Zeros(p+at,padding))return UMI_STATUS_PARSE_ERROR;
        at+=padding;
        if(f[6]>UMI_OS_IMAGE_MAX_FILE||f[6]>n-at)return UMI_STATUS_PARSE_ERROR;
        const unsigned char *content=p+at;
        at+=f[6];
        padding=(4U-at%4U)%4U;
        if(padding>n-at||!Zeros(p+at,padding))return UMI_STATUS_PARSE_ERROR;
        at+=padding;
        if(!strcmp(name,"TRAILER!!!")) {
            if(f[0]||f[1]||f[6]||f[9]||f[10]||f[4]!=1U||!a->count||n-at>=512U||!Zeros(p+at,n-at))return UMI_STATUS_PARSE_ERROR;
            return UMI_STATUS_OK;
        }
        if(a->count==UMI_OS_IMAGE_MAX_ENTRIES||f[0]!=a->count+1U)return UMI_STATUS_PARSE_ERROR;
        UmiOsImageEntry e= {
            name,f[1],f[9],f[10],content,f[6]
        }
        ;
        if(EntryValid(&e)!=UMI_STATUS_OK||f[4]!=((f[1]&OI_TYPE_MASK)==OI_DIR?2U:1U))return UMI_STATUS_PARSE_ERROR;
        if(a->count&&strcmp(a->entries[a->count-1U].name,name)>=0)return UMI_STATUS_PARSE_ERROR;
        const char *slash=strrchr(name,'/');
        if(slash) {
            size_t len=(size_t)(slash-name);
            int found=0;
            for(size_t i=0;i<a->count;++i) if(strlen(a->entries[i].name)==len&&!strncmp(a->entries[i].name,name,len)&&(a->entries[i].mode&OI_TYPE_MASK)==OI_DIR)found=1;
            if(!found)return UMI_STATUS_PARSE_ERROR;
        }
        a->entries[a->count++]=e;
    }
    return UMI_STATUS_PARSE_ERROR;
}
UmiStatus UmiOsImageArchiveOpen(const void *data,size_t size,UmiOsImageArchive **out) {
    if(!out)return UMI_STATUS_INVALID_ARGUMENT;
    *out=NULL;
    if(!data||!size||size>UMI_OS_IMAGE_MAX_ARCHIVE+16384U)return UMI_STATUS_INVALID_ARGUMENT;
    UmiOsImageArchive *a=calloc(1,sizeof *a);
    if(!a)return UMI_STATUS_OUT_OF_MEMORY;
    const unsigned char *p=data;
    UmiStatus s=UMI_STATUS_OK;
    if(size>=3U&&!memcmp(p,"\x1f\x8b\x08",3))s=InflateStored(p,size,&a->raw,&a->size);
    else if(size<=UMI_OS_IMAGE_MAX_ARCHIVE) {
        a->raw=malloc(size);
        if(!a->raw)s=UMI_STATUS_OUT_OF_MEMORY;
        else {
            memcpy(a->raw,data,size);
            a->size=size;
        }
    }
    else s=UMI_STATUS_CAPACITY_EXCEEDED;
    if(s==UMI_STATUS_OK)s=ParseRaw(a);
    if(s==UMI_STATUS_OK)*out=a;
    else UmiOsImageArchiveDestroy(a);
    return s;
}
size_t UmiOsImageArchiveCount(const UmiOsImageArchive *a) {
    return a?a->count:0;
}
const UmiOsImageEntry *UmiOsImageArchiveAt(const UmiOsImageArchive *a,size_t i) {
    return a&&i<a->count?&a->entries[i]:NULL;
}
void UmiOsImageArchiveDestroy(UmiOsImageArchive *a) {
    if(a) {
        free(a->raw);
        free(a);
    }
}

/*-----------------------------------------------------------------------------
 * Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * File: src/boot_media/image.c
 * Checked structural observations of raw disk and ISO images. A valid table is
 * not proof that its boot loader executes or that its publisher is trusted.
 *---------------------------------------------------------------------------*/

#include "internal.h"
static uint16_t Le16(const unsigned char *p) {
    return (uint16_t)((unsigned)p[0]|((unsigned)p[1]<<8U));
}

static uint32_t Le32(const unsigned char *p) {
    return (uint32_t)p[0]|((uint32_t)p[1]<<8U)|((uint32_t)p[2]<<16U)|((uint32_t)p[3]<<24U);
}

static uint32_t Be32(const unsigned char *p) {
    return (uint32_t)p[3]|((uint32_t)p[2]<<8U)|((uint32_t)p[1]<<16U)|((uint32_t)p[0]<<24U);
}

static uint64_t Le64(const unsigned char *p) {
    return (uint64_t)Le32(p)|((uint64_t)Le32(p+4)<<32U);
}

static int Zero(const unsigned char *p,size_t n) {
    for (size_t i=0;i<n;++i)if (p[i])return 0;
    return 1;
}

uint32_t BmCrc32(const void *data,size_t n)
{
    const unsigned char *p=data;
    uint32_t c=UINT32_MAX;
    for (size_t i=0;i<n;++i) {
        c^=p[i];
        for (unsigned j=0;j<8U;++j)c=(c>>1U)^(UINT32_C(0xedb88320)&(0U-(c&1U)));
    }
    return ~c;
}

static UmiStatus BootEntry(const unsigned char *p,unsigned platform,uint64_t bytes,UmiBootMediaImage *out)
{
    if (p[0]==0)return UMI_STATUS_OK;
    if (p[0]!=0x88U||p[1]!=0U)return UMI_STATUS_UNAVAILABLE;

    /* no-emulation profile */
    uint64_t start=(uint64_t)Le32(p+8)*2048U,load=(uint64_t)Le16(p+6)*512U;
    if (!load||start>=bytes||load>bytes-start)return UMI_STATUS_PARSE_ERROR;
    if (platform==0U)out->biosEntry=1;
    else if (platform==0xefU)out->efiEntry=1;
    else return UMI_STATUS_UNAVAILABLE;
    return UMI_STATUS_OK;
}

static UmiStatus Iso(BmFile *f,uint64_t bytes,UmiBootMediaImage *out)
{
    if (bytes<18U*2048U)return UMI_STATUS_OK;
    unsigned char sector[2048];
    UmiStatus s=BmRead(f,16U*2048U,sector,sizeof sector);
    if (s!=UMI_STATUS_OK)return s;
    if (memcmp(sector+1,"CD001",5))return UMI_STATUS_OK;
    uint32_t catalogue=0;
    int primary=0,terminated=0;
    for (unsigned i=16U;i<80U;++i) {
        if ((uint64_t)(i+1U)*2048U>bytes)return UMI_STATUS_PARSE_ERROR;
        s=BmRead(f,(uint64_t)i*2048U,sector,sizeof sector);
        if (s!=UMI_STATUS_OK)return s;
        if (memcmp(sector+1,"CD001",5)||sector[6]!=1U)return UMI_STATUS_PARSE_ERROR;
        if (sector[0]==1U) {
            uint32_t blocks=Le32(sector+80);
            if (primary||!blocks||blocks!=Be32(sector+84)||(uint64_t)blocks*2048U>bytes||
                Le16(sector+128)!=2048U||sector[130]!=8U||sector[131]!=0U)return UMI_STATUS_PARSE_ERROR;
            primary=1;
        }
        else if (sector[0]==0U&&!memcmp(sector+7,"EL TORITO SPECIFICATION",23)) {
            if (catalogue)return UMI_STATUS_PARSE_ERROR;
            catalogue=Le32(sector+71);
            if (!catalogue)return UMI_STATUS_PARSE_ERROR;
        }
        else if (sector[0]==255U) {
            terminated=1;
            break;
        }
        else if (sector[0]!=0U&&sector[0]!=2U&&sector[0]!=3U)return UMI_STATUS_PARSE_ERROR;
    }
    if (!primary||!terminated)return UMI_STATUS_PARSE_ERROR;
    out->iso9660=1;
    if (!catalogue)return UMI_STATUS_OK;
    if ((uint64_t)catalogue*2048U>bytes-2048U)return UMI_STATUS_PARSE_ERROR;
    s=BmRead(f,(uint64_t)catalogue*2048U,sector,sizeof sector);
    if (s!=UMI_STATUS_OK)return s;
    unsigned sum=0;
    for (unsigned i=0;i<32U;i+=2U)sum+=Le16(sector+i);
    if (sector[0]!=1U||sector[30]!=0x55U||sector[31]!=0xaaU||(sum&65535U))return UMI_STATUS_PARSE_ERROR;
    s=BootEntry(sector+32,sector[1],bytes,out);
    if (s!=UMI_STATUS_OK)return s;
    size_t at=64U;
    while (at<sizeof sector&&!Zero(sector+at,32U)) {
        unsigned type=sector[at],platform=sector[at+1U],count=Le16(sector+at+2U);
        if ((type!=0x90U&&type!=0x91U)||!count||count>(sizeof sector-at-32U)/32U)return UMI_STATUS_PARSE_ERROR;
        at+=32U;
        for (unsigned i=0;i<count;++i,at+=32U) {
            s=BootEntry(sector+at,platform,bytes,out);
            if (s!=UMI_STATUS_OK)return s;
        }
        if (type==0x91U) {
            if (!Zero(sector+at,sizeof sector-at))return UMI_STATUS_UNAVAILABLE;
            break;
        }
        if (at==sizeof sector||Zero(sector+at,32U))return UMI_STATUS_PARSE_ERROR;
    }
    if (!out->biosEntry&&!out->efiEntry)return UMI_STATUS_PARSE_ERROR;
    out->elTorito=1;
    return UMI_STATUS_OK;
}

static UmiStatus GptHeader(BmFile *f,uint64_t lba,uint64_t sectors,unsigned char header[512])
{
    if (lba>=sectors)return UMI_STATUS_PARSE_ERROR;
    UmiStatus s=BmRead(f,lba*512U,header,512U);
    if (s!=UMI_STATUS_OK)return s;
    uint32_t n=Le32(header+12),crc=Le32(header+16);
    if (memcmp(header,"EFI PART",8)||Le32(header+8)!=0x10000U||n<92U||n>512U||Le32(header+20)||Le64(header+24)!=lba)return UMI_STATUS_PARSE_ERROR;
    memset(header+16,0,4);
    uint32_t actual=BmCrc32(header,n);
    header[16]=(unsigned char)crc;
    header[17]=(unsigned char)(crc>>8U);
    header[18]=(unsigned char)(crc>>16U);
    header[19]=(unsigned char)(crc>>24U);
    return actual==crc?UMI_STATUS_OK:UMI_STATUS_PARSE_ERROR;
}

static UmiStatus Gpt(BmFile *f,uint64_t bytes,UmiBootMediaImage *out)
{
    unsigned char h[512],b[512];
    uint64_t sectors=bytes/512U;
    UmiStatus s=GptHeader(f,1U,sectors,h);
    if (s!=UMI_STATUS_OK)return s;
    uint64_t backup=Le64(h+32),first=Le64(h+40),last=Le64(h+48),table=Le64(h+72);
    uint32_t count=Le32(h+80),width=Le32(h+84),crc=Le32(h+88);
    if (backup!=sectors-1U||first<3U||first>last||last>=backup||table<2U||table>=first||
        !count||count>256U||width<128U||width>512U||width%128U||Zero(h+56,16))return UMI_STATUS_PARSE_ERROR;
    size_t n=(size_t)count*width;
    uint64_t tableSectors=((uint64_t)n+511U)/512U;
    if (tableSectors>first-table)return UMI_STATUS_PARSE_ERROR;
    s=GptHeader(f,backup,sectors,b);
    if (s!=UMI_STATUS_OK)return s;
    uint64_t other=Le64(b+72);
    if (Le64(b+32)!=1U||Le64(b+40)!=first||Le64(b+48)!=last||memcmp(h+56,b+56,16)||
        Le32(b+80)!=count||Le32(b+84)!=width||Le32(b+88)!=crc||other<=last||other>=backup||tableSectors>backup-other)return UMI_STATUS_PARSE_ERROR;
    unsigned char *entries=malloc(n),*copy=malloc(n);
    if (!entries||!copy) {
        free(entries);
        free(copy);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    s=BmRead(f,table*512U,entries,n);
    if (s==UMI_STATUS_OK)s=BmRead(f,other*512U,copy,n);
    if (s==UMI_STATUS_OK&&(BmCrc32(entries,n)!=crc||memcmp(entries,copy,n)))s=UMI_STATUS_PARSE_ERROR;
    for (uint32_t i=0;s==UMI_STATUS_OK&&i<count;++i) {
        const unsigned char *p=entries+(size_t)i*width;
        if (Zero(p,16))continue;
        uint64_t a=Le64(p+32),z=Le64(p+40);
        if (a<first||z>last||a>z||Zero(p+16,16)) {
            s=UMI_STATUS_PARSE_ERROR;
            break;
        }
        for (uint32_t j=0;j<i;++j) {
            const unsigned char*q=entries+(size_t)j*width;
            if (!Zero(q,16)&&((a<=Le64(q+40)&&z>=Le64(q+32))||!memcmp(p+16,q+16,16))) {
                s=UMI_STATUS_PARSE_ERROR;
                break;
            }
        }
        ++out->partitions;
    }
    free(entries);
    free(copy);
    if (s==UMI_STATUS_OK&&!out->partitions)s=UMI_STATUS_PARSE_ERROR;
    if (s==UMI_STATUS_OK)out->gptLayout=1;
    return s;
}

UmiStatus BmInspect(BmFile *f,uint64_t bytes,UmiBootMediaImage *out)
{
    memset(out,0,sizeof *out);
    out->bytes=bytes;
    if (bytes<512U||bytes>UMI_BOOT_MEDIA_MAX_IMAGE||bytes%512U)return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus s=Iso(f,bytes,out);
    if (s!=UMI_STATUS_OK)return s;
    unsigned char mbr[512];
    s=BmRead(f,0,mbr,sizeof mbr);
    if (s!=UMI_STATUS_OK)return s;
    if (mbr[510]==0x55U&&mbr[511]==0xaaU) {
        int protective=0;
        unsigned normal=0;
        uint64_t starts[4]= {
            0
        }, ends[4]= {
            0
        };
        for (unsigned i=0;i<4U;++i) {
            const unsigned char*p=mbr+446U+i*16U;
            uint32_t start=Le32(p+8),length=Le32(p+12);
            if (p[0]!=0U&&p[0]!=0x80U)return UMI_STATUS_PARSE_ERROR;
            if (!p[4]) {
                if (start||length||p[0])return UMI_STATUS_PARSE_ERROR;
                continue;
            }
            if (!length||(uint64_t)start+length>bytes/512U)return UMI_STATUS_PARSE_ERROR;
            if (p[4]==0xeeU) {
                if (start!=1U||protective)return UMI_STATUS_PARSE_ERROR;
                protective=1;
            }
            else {
                if (!start&&!out->iso9660)return UMI_STATUS_PARSE_ERROR;

                /* Hybrid optical images can deliberately have overlapping
                 * descriptions. Plain disk images cannot overlap partitions. */
                if (!out->iso9660)for (unsigned j=0;j<normal;++j)
                if ((uint64_t)start<ends[j]&&starts[j]<(uint64_t)start+length)return UMI_STATUS_PARSE_ERROR;
                starts[normal]=start;
                ends[normal]=(uint64_t)start+length;
                ++normal;
            }
        }
        if (protective) {
            s=Gpt(f,bytes,out);
            if (s!=UMI_STATUS_OK)return s;
        }
        else if (normal) {
            if (Zero(mbr,440U))return UMI_STATUS_PARSE_ERROR;
            out->mbrLayout=1;
            out->partitions=normal;
        }
    }
    if (!out->mbrLayout&&!out->gptLayout&&!out->elTorito)return UMI_STATUS_PARSE_ERROR;
    return UMI_STATUS_OK;
}

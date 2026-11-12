/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/image_inspection/iso.c
 *
 * PURPOSE:
 *   Inspect bounded ISO9660 and El Torito records without filesystem or device authority.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/image_inspection/iso.h"
#include <string.h>

static uint16_t Le16(const unsigned char *p) {
    return (uint16_t)((unsigned)p[0]|((unsigned)p[1]<<8U));
}

static uint32_t Le32(const unsigned char *p) {
    return (uint32_t)p[0]|((uint32_t)p[1]<<8U)|((uint32_t)p[2]<<16U)|((uint32_t)p[3]<<24U);
}

static uint32_t Be32(const unsigned char *p) {
    return (uint32_t)p[3]|((uint32_t)p[2]<<8U)|((uint32_t)p[1]<<16U)|((uint32_t)p[0]<<24U);
}

static int Zero(const unsigned char *p,size_t n) {
    for (size_t i=0;i<n;++i)if (p[i])return 0;
    return 1;
}

static UmiStatus BootEntry(const unsigned char *p,unsigned platform,uint64_t bytes,UmiIsoImageInfo *out)
{
    if (p[0]==0)return UMI_STATUS_OK;
    if (p[0]!=0x88U||p[1]!=0U)return UMI_STATUS_UNAVAILABLE;

    /* no-emulation profile */
    uint64_t start=(uint64_t)Le32(p+8)*2048U;
    unsigned sectors=Le16(p+6);
    if (start>=bytes)return UMI_STATUS_PARSE_ERROR;
    /* UEFI reserves counts zero and one for an image extending to the end
     * of the optical medium. A BIOS zero count remains malformed. */
    uint64_t load=(platform==0xefU&&sectors<=1U)?
        bytes-start:(uint64_t)sectors*512U;
    if (!load||load>bytes-start)return UMI_STATUS_PARSE_ERROR;
    if (platform==0U)out->biosEntry=1;
    else if (platform==0xefU)out->efiEntry=1;
    else return UMI_STATUS_UNAVAILABLE;
    return UMI_STATUS_OK;
}

static UmiStatus Inspect(UmiIsoReadAt readAt,void *context,uint64_t bytes,UmiIsoImageInfo *out)
{
/* The former minimum-length shortcut could hide a recognised truncated descriptor. Format recognition now uses only available bytes before the existing full-descriptor parser. The previous implementation is retained for review. */
#if 0
    if (bytes<18U*2048U)return UMI_STATUS_OK;
#endif
    /* A complete CD001 prefix identifies the format even when the remainder
     * of descriptor 16 is missing. Non-ISO input may still return zero flags;
     * recognised truncation must not fall through as an unrelated disk image. */
    if (bytes<17U*2048U) {
        const uint64_t descriptorOffset=16U*2048U;
        unsigned char prefix[6];
        if (bytes<descriptorOffset+sizeof(prefix)) return UMI_STATUS_OK;
        UmiStatus prefixStatus=readAt(context,descriptorOffset,prefix,sizeof(prefix));
        if (prefixStatus!=UMI_STATUS_OK) return prefixStatus;
        return memcmp(prefix+1,"CD001",5)==0 ? UMI_STATUS_PARSE_ERROR : UMI_STATUS_OK;
    }
    unsigned char sector[2048];
    UmiStatus s=readAt(context,16U*2048U,sector,sizeof sector);
    if (s!=UMI_STATUS_OK)return s;
    if (memcmp(sector+1,"CD001",5))return UMI_STATUS_OK;
    uint32_t catalogue=0;
    int primary=0,terminated=0;
    for (unsigned i=16U;i<80U;++i) {
        if ((uint64_t)(i+1U)*2048U>bytes)return UMI_STATUS_PARSE_ERROR;
        /* Sector 16 is already available from format detection. */
        if (i!=16U) {
            s=readAt(context,(uint64_t)i*2048U,sector,sizeof sector);
            if (s!=UMI_STATUS_OK)return s;
        }
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
    s=readAt(context,(uint64_t)catalogue*2048U,sector,sizeof sector);
    if (s!=UMI_STATUS_OK)return s;
    unsigned sum=0;
    for (unsigned i=0;i<32U;i+=2U)sum+=Le16(sector+i);
    if (sector[0]!=1U||sector[30]!=0x55U||sector[31]!=0xaaU||(sum&65535U))return UMI_STATUS_PARSE_ERROR;
    s=BootEntry(sector+32,sector[1],bytes,out);
    if (s!=UMI_STATUS_OK)return s;
    size_t at=64U;
    while (at<sizeof sector) {
        if (Zero(sector+at,32U)) {
            /* A zero header ends the supported catalogue. Nonzero data after
             * it may be an extension; do not silently accept unparsed entries. */
            if (!Zero(sector+at,sizeof sector-at))return UMI_STATUS_UNAVAILABLE;
            break;
        }
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

UmiStatus UmiIsoImageInspect(UmiIsoReadAt readAt,void *context,
    uint64_t bytes,UmiIsoImageInfo *out)
{
    if (!readAt||!out)return UMI_STATUS_INVALID_ARGUMENT;
    UmiIsoImageInfo candidate={0};
    UmiStatus status=Inspect(readAt,context,bytes,&candidate);
    /* Failed observations must never leak a partial set of acceptance flags. */
    if (status==UMI_STATUS_OK)*out=candidate;
    return status;
}

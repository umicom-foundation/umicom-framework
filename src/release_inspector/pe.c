/*-----------------------------------------------------------------------------
 * Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * File: src/release_inspector/pe.c
 * Purpose: Bounded PE inspection without casting untrusted bytes into structs.
 * The portable byte reader can inspect Windows files without executing them.
 * Format reference: Microsoft PE/COFF specification (see developer guide).
 *---------------------------------------------------------------------------*/
#include "umicom/release_inspector/inspection.h"
#include <stdlib.h>
#include <string.h>
#define PE_MAX_SECTIONS 96U

typedef struct Section {
    uint32_t rva, span, raw, rawBytes;
} Section;
typedef struct Image {
    const unsigned char *bytes;
    size_t length;
    uint32_t headers, imageBytes;
    uint32_t directoryRva[16], directoryBytes[16];
    Section sections[PE_MAX_SECTIONS];
    size_t count;
    unsigned thunkBytes;
} Image;
static uint16_t Word(const unsigned char *p)
{
    return (uint16_t)((uint16_t)p[0] | ((uint16_t)p[1] << 8U));
}
static uint32_t Dword(const unsigned char *p)
{
    return (uint32_t)p[0] | ((uint32_t)p[1]<<8U) |
        ((uint32_t)p[2]<<16U) | ((uint32_t)p[3]<<24U);
}
static int Within(size_t at, size_t count, size_t length)
{
    return at <= length && count <= length-at;
}
static int PowerOfTwo(uint32_t value)
{
    return value != 0U && (value & (value-1U)) == 0U;
}
/* A virtual-only tail is not present in the file. Never reinterpret it as an
 * offset, and reject overlapping mappings instead of choosing the first one. */
static const unsigned char *Map(const Image *image, uint32_t rva, size_t count)
{
    if (rva < image->headers) {
        if (!Within((size_t)rva,count,(size_t)image->headers) ||
            !Within((size_t)rva,count,image->length)) return NULL;
        return image->bytes+rva;
    }
    for (size_t i=0U;i<image->count;++i) {
        const Section *s=&image->sections[i];
        if (rva >= s->rva && (uint64_t)rva < (uint64_t)s->rva+s->span) {
            size_t delta=(size_t)(rva-s->rva);
            if (!Within(delta,count,s->rawBytes) ||
                !Within((size_t)s->raw+delta,count,image->length)) return NULL;
            return image->bytes+s->raw+delta;
        }
    }
    return NULL;
}
static UmiStatus Header(Image *image, UmiReleasePeInfo *info)
{
    const unsigned char *b=image->bytes;
    if (image->length<64U || b[0]!='M' || b[1]!='Z') return UMI_STATUS_PARSE_ERROR;
    size_t pe=Dword(b+60U);
    if (pe<64U || !Within(pe,24U,image->length) || memcmp(b+pe,"PE\0\0",4U))
        return UMI_STATUS_PARSE_ERROR;
    const unsigned char *coff=b+pe+4U;
    info->machine=Word(coff);
    info->sectionCount=Word(coff+2U);
    info->characteristics=Word(coff+18U);
    size_t optionalBytes=Word(coff+16U), optional=pe+24U;
    image->count=info->sectionCount;
    if (!image->count || image->count>PE_MAX_SECTIONS ||
        !Within(optional,optionalBytes,image->length) || optionalBytes<2U)
        return UMI_STATUS_PARSE_ERROR;
    const unsigned char *o=b+optional;
    info->optionalMagic=Word(o);
    size_t dirs;
    if (info->optionalMagic==0x20bU) { dirs=112U; image->thunkBytes=8U; }
    else if (info->optionalMagic==0x10bU) { dirs=96U; image->thunkBytes=4U; }
    else return UMI_STATUS_PARSE_ERROR;
    if (optionalBytes<dirs) return UMI_STATUS_PARSE_ERROR;
    uint32_t dirCount=Dword(o+dirs-4U);
    if (dirCount>16U || (size_t)dirCount>(optionalBytes-dirs)/8U)
        return UMI_STATUS_PARSE_ERROR;
    image->headers=Dword(o+60U);
    image->imageBytes=Dword(o+56U);
    info->entryRva=Dword(o+16U);
    info->subsystem=Word(o+68U);
    info->imageBytes=image->imageBytes;
    uint32_t sectionAlignment=Dword(o+32U), fileAlignment=Dword(o+36U);
    if (!PowerOfTwo(sectionAlignment) || !PowerOfTwo(fileAlignment) ||
        sectionAlignment<fileAlignment || fileAlignment>65536U ||
        (sectionAlignment<4096U ? sectionAlignment!=fileAlignment : fileAlignment<512U))
        return UMI_STATUS_PARSE_ERROR;
    size_t sectionTable=optional+optionalBytes;
    if (!Within(sectionTable,image->count*40U,image->length) ||
        image->headers>image->length || image->headers<sectionTable+image->count*40U ||
        !image->imageBytes || image->imageBytes<image->headers ||
        info->entryRva>=image->imageBytes || !(info->characteristics&2U))
        return UMI_STATUS_PARSE_ERROR;
    for (size_t i=0U;i<image->count;++i) {
        const unsigned char *h=b+sectionTable+i*40U;
        Section *s=&image->sections[i];
        uint32_t virtualBytes=Dword(h+8U);
        s->rva=Dword(h+12U); s->rawBytes=Dword(h+16U); s->raw=Dword(h+20U);
        s->span=virtualBytes>s->rawBytes?virtualBytes:s->rawBytes;
        if (s->rva<image->headers || s->rva%sectionAlignment!=0U ||
            (uint64_t)s->rva+s->span>image->imageBytes ||
            (s->rawBytes && (s->raw<image->headers || s->raw%fileAlignment!=0U ||
                !Within(s->raw,s->rawBytes,image->length)))) return UMI_STATUS_PARSE_ERROR;
        for (size_t j=0U;j<i;++j) {
            const Section *a=&image->sections[j];
            if ((s->span && a->span &&
                (uint64_t)s->rva < (uint64_t)a->rva+a->span &&
                (uint64_t)a->rva < (uint64_t)s->rva+s->span) ||
                (s->rawBytes && a->rawBytes &&
                (uint64_t)s->raw < (uint64_t)a->raw+a->rawBytes &&
                (uint64_t)a->raw < (uint64_t)s->raw+s->rawBytes)) return UMI_STATUS_PARSE_ERROR;
        }
    }
    for (size_t i=0U;i<(size_t)dirCount;++i) {
        image->directoryRva[i]=Dword(o+dirs+i*8U);
        image->directoryBytes[i]=Dword(o+dirs+i*8U+4U);
    }
    if (info->entryRva && !Map(image,info->entryRva,1U)) return UMI_STATUS_PARSE_ERROR;
    return UMI_STATUS_OK;
}
static UmiStatus Name(const Image *image, uint32_t rva, char out[UMI_RELEASE_IMPORT_CAPACITY])
{
    if (!rva) return UMI_STATUS_PARSE_ERROR;
    for (size_t i=0U;i<UMI_RELEASE_IMPORT_CAPACITY;++i) {
        if (i>UINT32_MAX-rva) return UMI_STATUS_PARSE_ERROR;
        const unsigned char *p=Map(image,rva+(uint32_t)i,1U);
        if (!p) return UMI_STATUS_PARSE_ERROR;
        unsigned char c=*p;
        if (!c) {
            if (i<5U) return UMI_STATUS_PARSE_ERROR;
            out[i]=0;
            const char *extension=out+i-4U;
            if (strcmp(extension,".dll") && strcmp(extension,".drv")) return UMI_STATUS_PARSE_ERROR;
            return UMI_STATUS_OK;
        }
        if (!((c>='a'&&c<='z')||(c>='A'&&c<='Z')||(c>='0'&&c<='9')||c=='_'||c=='-'||c=='.'))
            return UMI_STATUS_PARSE_ERROR;
        out[i]=(char)(c>='A'&&c<='Z'?c+('a'-'A'):c);
    }
    return UMI_STATUS_CAPACITY_EXCEEDED;
}
static UmiStatus Imports(const Image *image, UmiReleasePeInfo *info, size_t index, int delay)
{
    uint32_t rva=image->directoryRva[index], bytes=image->directoryBytes[index];
    if (!rva && !bytes) return UMI_STATUS_OK;
    size_t stride=delay?32U:20U;
    if (!rva || bytes<stride || (uint64_t)rva+bytes>image->imageBytes)
        return UMI_STATUS_PARSE_ERROR;
    for (size_t offset=0U;Within(offset,stride,bytes);offset+=stride) {
        if (offset>UINT32_MAX-rva) return UMI_STATUS_PARSE_ERROR;
        const unsigned char *d=Map(image,rva+(uint32_t)offset,stride);
        if (!d) return UMI_STATUS_PARSE_ERROR;
        int zero=1;
        for(size_t j=0U;j<stride;++j) if(d[j])zero=0;
        if(zero)return UMI_STATUS_OK;
        if(info->importCount==UMI_RELEASE_MAX_IMPORTS)return UMI_STATUS_CAPACITY_EXCEEDED;
        if(delay && Dword(d)!=1U)return UMI_STATUS_UNAVAILABLE;
        uint32_t name=Dword(d+(delay?4U:12U)), iat=Dword(d+(delay?12U:16U));
        uint32_t names=Dword(d+(delay?16U:0U));
        if(!iat || !Map(image,iat,image->thunkBytes) ||
            (names && !Map(image,names,image->thunkBytes)))return UMI_STATUS_PARSE_ERROR;
        UmiStatus status=Name(image,name,info->imports[info->importCount]);
        if(status!=UMI_STATUS_OK)return status;
        info->delayed[info->importCount]=(unsigned char)delay;
        ++info->importCount;
    }
    return UMI_STATUS_PARSE_ERROR; /* A missing null descriptor is not success. */
}
UmiStatus UmiReleasePeInspect(const void *bytes, size_t length, UmiReleasePeInfo *out)
{
    if(!out)return UMI_STATUS_INVALID_ARGUMENT;
    memset(out,0,sizeof *out);
    if(!bytes || !length)return UMI_STATUS_INVALID_ARGUMENT;
    if(length>UMI_RELEASE_MAX_PE_BYTES)return UMI_STATUS_CAPACITY_EXCEEDED;
    Image image={0}; image.bytes=bytes; image.length=length;
    UmiStatus status=Header(&image,out);
    if(status==UMI_STATUS_OK)status=Imports(&image,out,1U,0);
    if(status==UMI_STATUS_OK)status=Imports(&image,out,13U,1);
    if(status!=UMI_STATUS_OK)memset(out,0,sizeof *out);
    return status;
}

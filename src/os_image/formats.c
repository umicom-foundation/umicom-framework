/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/os_image/formats.c
 * PURPOSE:
 *   Inspect image structure without loading or executing it. Header agreement does not
 *   establish kernel correctness, publisher identity or guest boot.
 * ORGANISATION: Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Inspect image structure without loading or executing it. Header agreement
 * does not establish kernel correctness, publisher identity or guest boot. */
#include "internal.h"
UmiStatus UmiOsImageValidateElf(const void *data,size_t size,UmiOsImageArch arch) {
    if(!data||!OiArchValid(arch))return UMI_STATUS_INVALID_ARGUMENT;
    const unsigned char *p=data;
    if(size<64U||memcmp(p,"\177ELF\2\1\1",7)||OiLe16(p+16)!=2U||OiLe16(p+18)!=(arch==UMI_OS_IMAGE_RISCV64?243U:62U)|| OiLe32(p+20)!=1U||OiLe16(p+52)!=64U||OiLe16(p+54)!=56U)return UMI_STATUS_PARSE_ERROR;
    uint64_t offset=OiLe64(p+32),entry=OiLe64(p+24);
    uint16_t count=OiLe16(p+56);
    if(!count||count>128U||offset<64U||offset>size||(uint64_t)count*56U>size-offset)return UMI_STATUS_PARSE_ERROR;
    int executableEntry=0;
    for(size_t i=0;i<count;++i) {
        const unsigned char *h=p+(size_t)offset+i*56U;
        uint32_t kind=OiLe32(h),flags=OiLe32(h+4);
        uint64_t begin=OiLe64(h+8),address=OiLe64(h+16),bytes=OiLe64(h+32),memory=OiLe64(h+40),alignment=OiLe64(h+48);
        if(kind==2U||kind==3U)return UMI_STATUS_INVALID_STATE;
        if(kind==1U) {
            if(bytes>memory||begin>size||bytes>size-begin||memory>UINT64_MAX-address)return UMI_STATUS_PARSE_ERROR;
            if(alignment>1U&&((alignment&(alignment-1U))||begin%alignment!=address%alignment))return UMI_STATUS_PARSE_ERROR;
            if((flags&1U)&&entry>=address&&entry-address<bytes)executableEntry=1;
        }
        if(kind==UINT32_C(0x6474e551)&&(flags&1U))return UMI_STATUS_INVALID_STATE;
    }
    return executableEntry?UMI_STATUS_OK:UMI_STATUS_PARSE_ERROR;
}
UmiStatus UmiOsImageValidateKernel(const void *data,size_t size,UmiOsImageArch arch) {
    if(!data||!OiArchValid(arch))return UMI_STATUS_INVALID_ARGUMENT;
    const unsigned char *p=data;
    if(size<4096U||size>UMI_OS_IMAGE_MAX_FILE)return UMI_STATUS_PARSE_ERROR;
    if(arch==UMI_OS_IMAGE_X86_64) {
        if(p[0x1fe]!=0x55U||p[0x1ff]!=0xaaU||memcmp(p+0x202,"HdrS",4)||OiLe16(p+0x206)<0x20cU||!(OiLe16(p+0x236)&1U))return UMI_STATUS_PARSE_ERROR;
        uint32_t setup=p[0x1f1]?p[0x1f1]:4U;
        if(((uint64_t)setup+1U)*512U>=size)return UMI_STATUS_PARSE_ERROR;
    }
    else {
        if(memcmp(p+48,"RISCV\0\0\0",8)||memcmp(p+56,"RSC\x05",4)||OiLe64(p+24)&1U||OiLe64(p+16)<4096U)return UMI_STATUS_PARSE_ERROR;
    }
    return UMI_STATUS_OK;
}

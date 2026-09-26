/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Bounded ELF-header and PNG-envelope validation. Neither executes a program
 * nor decompresses pixel data. Passing is not full loader/image certification. */
#include "internal.h"
#include <string.h>
static uint32_t Big32(const unsigned char *p)
{
    return ((uint32_t)p[0]<<24U)|((uint32_t)p[1]<<16U)|((uint32_t)p[2]<<8U)|p[3];
}
static uint16_t Little16(const unsigned char *p) { return (uint16_t)((uint16_t)p[0]|((uint16_t)p[1]<<8U)); }
static uint64_t Little64(const unsigned char *p)
{
    uint64_t n=0U; for (size_t i=0U;i<8U;++i) n|=(uint64_t)p[i]<<(8U*i); return n;
}
UmiStatus UmiNativeStageElfCheck(const unsigned char *h, size_t length, uint64_t fileLength, uint16_t *outMachine)
{
    if (h==NULL || outMachine==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *outMachine=0U;
    if (length<64U || fileLength<64U || fileLength>UMI_NATIVE_STAGE_BINARY_LIMIT) return UMI_STATUS_PARSE_ERROR;
    if (memcmp(h,"\177ELF\2\1\1",7U)!=0 || (Little16(h+16U)!=2U && Little16(h+16U)!=3U)
        || h[20]!=1U || h[21]!=0U || h[22]!=0U || h[23]!=0U || Little16(h+52U)!=64U) return UMI_STATUS_PARSE_ERROR;
    uint16_t machine=Little16(h+18U), count=Little16(h+56U);
    uint64_t offset=Little64(h+32U);
    if ((machine!=62U && machine!=183U && machine!=243U) || count==0U || count>1024U
        || Little16(h+54U)!=56U || offset<64U || offset>fileLength
        || (uint64_t)count*56U>fileLength-offset) return UMI_STATUS_PARSE_ERROR;
    *outMachine=machine; return UMI_STATUS_OK;
}
static uint32_t Crc(const unsigned char *p, size_t length)
{
    uint32_t crc=0xffffffffU;
    for (size_t i=0U; i<length; ++i) {
        crc^=p[i];
        for (unsigned bit=0U; bit<8U; ++bit) crc=(crc>>1U)^((0U-(crc&1U))&0xedb88320U);
    }
    return ~crc;
}
UmiStatus UmiNativeStagePngCheck(const unsigned char *data, size_t length)
{
    static const unsigned char SIGNATURE[8]={137U,80U,78U,71U,13U,10U,26U,10U};
    if (data==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (length<45U || length>UMI_NATIVE_STAGE_ICON_LIMIT || memcmp(data,SIGNATURE,8U)!=0) return UMI_STATUS_PARSE_ERROR;
    size_t offset=8U; unsigned chunks=0U; int seenHeader=0,seenData=0,seenPalette=0,dataEnded=0; unsigned colour=0U,depth=0U;
    while (offset<length) {
        if (++chunks>4096U || length-offset<12U) return UMI_STATUS_PARSE_ERROR;
        uint32_t size=Big32(data+offset); const unsigned char *type=data+offset+4U;
        if ((size_t)size>length-offset-12U || Crc(type,(size_t)size+4U)!=Big32(type+4U+size)) return UMI_STATUS_PARSE_ERROR;
        for (size_t i=0U;i<4U;++i) if (!((type[i]>='A'&&type[i]<='Z')||(type[i]>='a'&&type[i]<='z'))) return UMI_STATUS_PARSE_ERROR;
        if ((type[2]&32U)!=0U) return UMI_STATUS_PARSE_ERROR;
        if (memcmp(type,"IHDR",4U)==0) {
            if (seenHeader || offset!=8U || size!=13U) return UMI_STATUS_PARSE_ERROR;
            uint32_t width=Big32(type+4U),height=Big32(type+8U);
            depth=type[12]; colour=type[13];
            int depthOk=(colour==0U && (depth==1U||depth==2U||depth==4U||depth==8U||depth==16U))
                || (colour==3U && (depth==1U||depth==2U||depth==4U||depth==8U))
                || ((colour==2U||colour==4U||colour==6U) && (depth==8U||depth==16U));
            if (!width||!height||width>4096U||height>4096U||!depthOk||type[14]!=0U||type[15]!=0U||type[16]>1U)
                return UMI_STATUS_PARSE_ERROR;
            seenHeader=1;
        } else if (memcmp(type,"PLTE",4U)==0) {
            if (!seenHeader||seenData||seenPalette||size==0U||size>768U||size%3U!=0U||colour==0U||colour==4U
                || (colour==3U && size/3U>(1U<<depth))) return UMI_STATUS_PARSE_ERROR;
            seenPalette=1;
        } else if (memcmp(type,"IDAT",4U)==0) {
            if (!seenHeader||dataEnded||(colour==3U&&!seenPalette)) return UMI_STATUS_PARSE_ERROR;
            seenData=1;
        } else if (memcmp(type,"IEND",4U)==0) {
            return seenHeader&&seenData&&size==0U&&offset+12U==length ? UMI_STATUS_OK : UMI_STATUS_PARSE_ERROR;
        } else {
            if (!seenHeader||(type[0]&32U)==0U) return UMI_STATUS_PARSE_ERROR;
            if (seenData) dataEnded=1;
        }
        offset+=(size_t)size+12U;
    }
    return UMI_STATUS_PARSE_ERROR;
}

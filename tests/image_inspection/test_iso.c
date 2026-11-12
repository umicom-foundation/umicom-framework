/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/image_inspection/test_iso.c
 *
 * PURPOSE:
 *   Exercise shared ISO admission, UEFI count semantics and unchanged results on refusal.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/image_inspection/iso.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BLOCK 2048U
#define CAPACITY (96U*BLOCK)
#define CHECK(condition) do { if (!(condition)) { \
    fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#condition); return 1; \
} } while (0)

typedef struct Image {
    unsigned char *data;
    uint64_t bytes;
    unsigned reads,failAt;
} Image;

static void Little16(unsigned char *p,unsigned value)
{
    p[0]=(unsigned char)value;
    p[1]=(unsigned char)(value>>8U);
}

static void Little32(unsigned char *p,uint32_t value)
{
    for (unsigned i=0;i<4U;++i)p[i]=(unsigned char)(value>>(i*8U));
}

static void Big32(unsigned char *p,uint32_t value)
{
    for (unsigned i=0;i<4U;++i)p[3U-i]=(unsigned char)(value>>(i*8U));
}

static void Descriptor(unsigned char *p,unsigned type)
{
    memset(p,0,BLOCK);
    p[0]=(unsigned char)type;
    memcpy(p+1,"CD001",5);
    p[6]=1;
}

static void Checksum(unsigned char *p)
{
    p[28]=p[29]=0;
    unsigned sum=0;
    for (unsigned i=0;i<32U;i+=2U)sum+=(unsigned)p[i]|((unsigned)p[i+1U]<<8U);
    Little16(p+28,(0U-sum)&65535U);
}

static void Entry(unsigned char *p,unsigned count,unsigned block)
{
    p[0]=0x88;
    Little16(p+6,count);
    Little32(p+8,block);
}

static void Base(Image *image)
{
    memset(image->data,0,CAPACITY);
    image->bytes=24U*BLOCK;
    unsigned char *p=image->data+16U*BLOCK;
    Descriptor(p,1);
    Little32(p+80,24);
    Big32(p+84,24);
    Little16(p+128,BLOCK);
    p[130]=8;
    p=image->data+17U*BLOCK;
    Descriptor(p,0);
    memcpy(p+7,"EL TORITO SPECIFICATION",23);
    Little32(p+71,20);
    Descriptor(image->data+18U*BLOCK,255);
    p=image->data+20U*BLOCK;
    p[0]=1;
    p[30]=0x55;
    p[31]=0xaa;
    Checksum(p);
    Entry(p+32,4,22);
}

static UmiStatus Read(void *context,uint64_t offset,void *destination,size_t length)
{
    Image *image=context;
    ++image->reads;
    if (image->reads==image->failAt)return UMI_STATUS_IO_ERROR;
    /* The fake port independently enforces exact bounded reads. No physical
     * device, boot loader or external image-production program is involved. */
    if (offset>image->bytes||(uint64_t)length>image->bytes-offset)
        return UMI_STATUS_IO_ERROR;
    memcpy(destination,image->data+(size_t)offset,length);
    return UMI_STATUS_OK;
}

static int Scenario(const char *name,Image *image)
{
    Base(image);
    unsigned char *p=image->data+16U*BLOCK;
    unsigned char *boot=image->data+17U*BLOCK;
    unsigned char *end=image->data+18U*BLOCK;
    unsigned char *cat=image->data+20U*BLOCK;
    UmiStatus expected=UMI_STATUS_OK;
    UmiIsoImageInfo wanted={1,1,1,0};
    if (!strcmp(name,"bios")) { /* Base fixture exercises the BIOS default. */ }
    else if (!strcmp(name,"efi")||!strcmp(name,"efi_zero")||!strcmp(name,"efi_one")) {
        cat[1]=0xef;
        Checksum(cat);
        if (!strcmp(name,"efi_zero"))Little16(cat+38,0);
        if (!strcmp(name,"efi_one"))Little16(cat+38,1);
        wanted.biosEntry=0;
        wanted.efiEntry=1;
    }
    else if (!strcmp(name,"hybrid")||!strncmp(name,"efi_section_",12)) {
        cat[64]=0x91;
        cat[65]=0xef;
        Little16(cat+66,1);
        Entry(cat+96,4,23);
        if (!strcmp(name,"efi_section_zero"))Little16(cat+102,0);
        if (!strcmp(name,"efi_section_one"))Little16(cat+102,1);
        if (!strcmp(name,"efi_section_only")) { cat[32]=0; wanted.biosEntry=0; }
        wanted.efiEntry=1;
    }
    else if (!strcmp(name,"plain_iso")) {
        boot[7]=0;
        wanted.elTorito=wanted.biosEntry=0;
    }
    else if (!strcmp(name,"non_iso")||!strcmp(name,"short")) {
        memset(image->data,0,CAPACITY);
        if (!strcmp(name,"short"))image->bytes=511;
        wanted=(UmiIsoImageInfo){0};
    }
    else if (!strncmp(name,"prefix_",7)) {
        wanted=(UmiIsoImageInfo){0};
        expected=UMI_STATUS_PARSE_ERROR;
        image->bytes=16U*BLOCK+6U;
        /* Keep the volume extent consistent where a full first descriptor is
         * available, so these cases reach the missing-descriptor boundary. */
        Little32(p+80,17);Big32(p+84,17);
        if(!strcmp(name,"prefix_absent")){image->bytes=16U*BLOCK;expected=UMI_STATUS_OK;}
        else if(!strcmp(name,"prefix_incomplete")){image->bytes=16U*BLOCK+5U;expected=UMI_STATUS_OK;}
        else if(!strcmp(name,"prefix_complete")){ /* exactly the identifying bytes */ }
        else if(!strcmp(name,"prefix_version"))image->bytes=16U*BLOCK+7U;
        else if(!strcmp(name,"prefix_sector_minus_one"))image->bytes=17U*BLOCK-1U;
        else if(!strcmp(name,"prefix_full_sector"))image->bytes=17U*BLOCK;
        else if(!strcmp(name,"prefix_next_partial"))image->bytes=17U*BLOCK+1U;
        else if(!strcmp(name,"prefix_two_minus_one"))image->bytes=18U*BLOCK-1U;
        else if(!strcmp(name,"prefix_non_iso")){p[1]='X';expected=UMI_STATUS_OK;}
        else if(!strcmp(name,"prefix_read_failure")){image->failAt=1;expected=UMI_STATUS_IO_ERROR;}
        else if(!strcmp(name,"prefix_full_non_iso")){image->bytes=17U*BLOCK;p[1]='X';expected=UMI_STATUS_OK;}
        else {fprintf(stderr,"Unknown ISO prefix case: %s\n",name);return 1;}
    }
    else if (!strcmp(name,"arguments")) {
        UmiIsoImageInfo out={7,7,7,7};
        CHECK(UmiIsoImageInspect(NULL,image,image->bytes,&out)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiIsoImageInspect(Read,image,image->bytes,NULL)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(out.iso9660==7&&out.elTorito==7&&out.biosEntry==7&&out.efiEntry==7);
        CHECK(image->reads==0);
        return 0;
    }
    else if (!strcmp(name,"descriptor_boundary")||!strcmp(name,"descriptor_limit")) {
        image->bytes=CAPACITY;
        Little32(p+80,96);
        Big32(p+84,96);
        memcpy(image->data+81U*BLOCK,cat,BLOCK);
        Little32(boot+71,81);
        Little32(image->data+81U*BLOCK+40,82);
        unsigned last=!strcmp(name,"descriptor_boundary")?79U:80U;
        for (unsigned i=18;i<last;++i)Descriptor(image->data+i*BLOCK,2);
        Descriptor(image->data+last*BLOCK,255);
        if (last==80U)expected=UMI_STATUS_PARSE_ERROR;
    }
    else {
        expected=UMI_STATUS_PARSE_ERROR;
        if (!strcmp(name,"bios_zero"))Little16(cat+38,0);
        else if (!strcmp(name,"load_range"))Little32(cat+40,UINT32_MAX);
        else if (!strcmp(name,"load_count"))Little16(cat+38,65535);
        else if (!strcmp(name,"efi_rest_range")) {
            cat[1]=0xef; Checksum(cat); Little16(cat+38,0); Little32(cat+40,24);
        }
        else if (!strcmp(name,"endian"))p[87]^=1;
        else if (!strcmp(name,"volume_zero")) { Little32(p+80,0); Big32(p+84,0); }
        else if (!strcmp(name,"volume_range")) { Little32(p+80,25); Big32(p+84,25); }
        else if (!strcmp(name,"block_size"))p[130]=4;
        else if (!strcmp(name,"duplicate_primary")) {
            memcpy(end,p,BLOCK); Descriptor(image->data+19U*BLOCK,255);
        }
        else if (!strcmp(name,"duplicate_boot")) {
            memcpy(end,boot,BLOCK); Descriptor(image->data+19U*BLOCK,255);
        }
        else if (!strcmp(name,"missing_primary"))p[0]=2;
        else if (!strcmp(name,"missing_terminator"))end[0]=2;
        else if (!strcmp(name,"version"))p[6]=2;
        else if (!strcmp(name,"catalogue_zero"))Little32(boot+71,0);
        else if (!strcmp(name,"catalogue_range"))Little32(boot+71,24);
        else if (!strcmp(name,"checksum"))cat[28]^=1;
        else if (!strcmp(name,"validation_signature"))cat[30]=0;
        else if (!strcmp(name,"section_count")) {
            cat[64]=0x91; cat[65]=0xef; Little16(cat+66,65535);
        }
        else if (!strcmp(name,"section_empty"))cat[64]=0x91;
        else if (!strcmp(name,"section_continuation")) {
            cat[64]=0x90; cat[65]=0xef; Little16(cat+66,1); Entry(cat+96,4,23);
        }
        else if (!strcmp(name,"trailer")) {
            cat[64]=0x91; cat[65]=0xef; Little16(cat+66,1);
            Entry(cat+96,4,23); cat[128]=1; expected=UMI_STATUS_UNAVAILABLE;
        }
        else if (!strcmp(name,"hidden_trailer")) {
            cat[96]=1; expected=UMI_STATUS_UNAVAILABLE;
        }
        else if (!strcmp(name,"no_boot_entry"))cat[32]=0;
        else if (!strcmp(name,"emulation")) { cat[33]=2; expected=UMI_STATUS_UNAVAILABLE; }
        else if (!strcmp(name,"platform")) { cat[1]=1; Checksum(cat); expected=UMI_STATUS_UNAVAILABLE; }
        else if (!strcmp(name,"boot_indicator")) { cat[32]=1; expected=UMI_STATUS_UNAVAILABLE; }
        else if (!strcmp(name,"read_failure")) { image->failAt=2; expected=UMI_STATUS_IO_ERROR; }
        else if (!strcmp(name,"catalogue_read_failure")) { image->failAt=4; expected=UMI_STATUS_IO_ERROR; }
        else { fprintf(stderr,"Unknown ISO case: %s\n",name); return 1; }
    }
    UmiIsoImageInfo out;
    memset(&out,0xa5,sizeof out);
    unsigned char before[sizeof out];
    memcpy(before,&out,sizeof out);
    UmiStatus actual=UmiIsoImageInspect(Read,image,image->bytes,&out);
    CHECK(actual==expected);
    CHECK(image->reads<=65);
    if (actual!=UMI_STATUS_OK)CHECK(!memcmp(before,&out,sizeof out));
    else {
        CHECK(out.iso9660==wanted.iso9660&&out.elTorito==wanted.elTorito);
        CHECK(out.biosEntry==wanted.biosEntry&&out.efiEntry==wanted.efiEntry);
    }
    if (!strcmp(name,"short"))CHECK(image->reads==0);
    if (!strcmp(name,"prefix_absent")||!strcmp(name,"prefix_incomplete"))CHECK(image->reads==0);
    else if (!strncmp(name,"prefix_",7))CHECK(image->reads==1);
    if (!strcmp(name,"descriptor_boundary"))CHECK(image->reads==65);
    return 0;
}

int main(int argc,char **argv)
{
    if (argc!=2)return 2;
    Image image={0};
    image.data=malloc(CAPACITY);
    if (!image.data)return 1;
    int result=Scenario(argv[1],&image);
    free(image.data);
    return result;
}

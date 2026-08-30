/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/compiler/native/target_profile.c
 *
 * PURPOSE:
 *   Define native code-generation profiles for x86-64 and RISC-V without replacing canonical compiler triples.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/target_profile.h"
#include <string.h>
UmiStatus umi_nc_target_profile_init(UmiNativeTargetProfile *p,const char *triple){if(p==NULL||triple==NULL)return UMI_STATUS_INVALID_ARGUMENT;memset(p,0,sizeof(*p));if(umi_nc_copy_text(p->triple,sizeof(p->triple),triple)!=UMI_STATUS_OK)return UMI_STATUS_CAPACITY_EXCEEDED;p->little_endian=true;if(strncmp(triple,"x86_64",6U)==0){p->architecture=UMI_NC_ARCH_X86_64;p->pointer_bits=64U;p->object_format=strstr(triple,"windows")!=NULL?UMI_NC_OBJECT_COFF:UMI_NC_OBJECT_ELF;p->feature_mask=1U;}else if(strncmp(triple,"riscv64",7U)==0){p->architecture=UMI_NC_ARCH_RISCV64;p->pointer_bits=64U;p->object_format=UMI_NC_OBJECT_ELF;p->feature_mask=0x1FU;}else return UMI_STATUS_NOT_FOUND;return UMI_STATUS_OK;}
bool umi_nc_target_profile_compatible(const UmiNativeTargetProfile *a,const UmiNativeTargetProfile *b){return a!=NULL&&b!=NULL&&a->architecture==b->architecture&&a->object_format==b->object_format&&a->pointer_bits==b->pointer_bits&&a->little_endian==b->little_endian;}

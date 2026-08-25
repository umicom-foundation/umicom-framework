/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/compiler/native/object_section.c
 *
 * PURPOSE:
 *   Describe object-file sections, flags, alignments and planned byte sizes before backend encoding.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/object_section.h"
#include <string.h>
UmiStatus umi_nc_object_section_init(UmiNativeObjectSection *s,const char *name,UmiNativeSectionKind kind,size_t align){if(s==NULL||name==NULL||align==0U)return UMI_STATUS_INVALID_ARGUMENT;memset(s,0,sizeof(*s));if(umi_nc_copy_text(s->name,sizeof(s->name),name)!=UMI_STATUS_OK)return UMI_STATUS_CAPACITY_EXCEEDED;s->kind=kind;s->alignment=align;s->readable=true;s->writable=kind==UMI_NC_SECTION_DATA||kind==UMI_NC_SECTION_BSS;s->executable=kind==UMI_NC_SECTION_TEXT;return UMI_STATUS_OK;}
UmiStatus umi_nc_object_section_reserve(UmiNativeObjectSection *s,size_t bytes){if(s==NULL)return UMI_STATUS_INVALID_ARGUMENT;if(bytes>SIZE_MAX-s->size)return UMI_STATUS_CAPACITY_EXCEEDED;s->size+=bytes;return UMI_STATUS_OK;}
size_t umi_nc_object_section_aligned_size(const UmiNativeObjectSection *s){if(s==NULL||s->alignment==0U)return 0U;return ((s->size+s->alignment-1U)/s->alignment)*s->alignment;}

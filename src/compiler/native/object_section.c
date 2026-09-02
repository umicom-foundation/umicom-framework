/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/compiler/native/object_section.c
 *
 * PURPOSE:
 *   Describe object-file sections, flags, alignments and planned byte sizes before backend encoding.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/object_section.h"
#include <string.h>
/*
 * Initialise nc object section from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_nc_object_section_init(UmiNativeObjectSection *s,const char *name,UmiNativeSectionKind kind,size_t align){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL||name==NULL||align==0U)return UMI_STATUS_INVALID_ARGUMENT;memset(s,0,sizeof(*s));/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_nc_copy_text(s->name,sizeof(s->name),name)!=UMI_STATUS_OK)return UMI_STATUS_CAPACITY_EXCEEDED;s->kind=kind;s->alignment=align;s->readable=true;s->writable=kind==UMI_NC_SECTION_DATA||kind==UMI_NC_SECTION_BSS;s->executable=kind==UMI_NC_SECTION_TEXT;return UMI_STATUS_OK;}
/*
 * Provide the nc object section reserve operation used by this module and its client
 * applications.
 */
UmiStatus umi_nc_object_section_reserve(UmiNativeObjectSection *s,size_t bytes){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL)return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(bytes>SIZE_MAX-s->size)return UMI_STATUS_CAPACITY_EXCEEDED;s->size+=bytes;return UMI_STATUS_OK;}
/*
 * Return the number of records represented by nc object section aligned without changing
 * their state.
 */
size_t umi_nc_object_section_aligned_size(const UmiNativeObjectSection *s){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL||s->alignment==0U)return 0U;return ((s->size+s->alignment-1U)/s->alignment)*s->alignment;}

/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/compiler/native/frame_layout.c
 *
 * PURPOSE:
 *   Allocate aligned stack-frame slots and compute deterministic final frame sizes.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/frame_layout.h"
#include <string.h>
static size_t align_up(size_t v,size_t a){return a==0U?v:((v+a-1U)/a)*a;}
void umi_nc_frame_layout_init(UmiNativeFrameLayout *l,size_t align){if(l!=NULL){memset(l,0,sizeof(*l));l->alignment=align==0U?16U:align;}}
UmiStatus umi_nc_frame_layout_add(UmiNativeFrameLayout *l,uint32_t id,size_t size,size_t align,size_t *out){if(l==NULL||id==0U||size==0U||align==0U||out==NULL)return UMI_STATUS_INVALID_ARGUMENT;if(l->count>=UMI_NC_MAX_FRAME_SLOTS)return UMI_STATUS_CAPACITY_EXCEEDED;size_t off=align_up(l->size,align);if(off>SIZE_MAX-size)return UMI_STATUS_CAPACITY_EXCEEDED;UmiNativeFrameSlot *s=&l->slots[l->count++];s->id=id;s->size=size;s->alignment=align;s->offset=off;l->size=off+size;if(align>l->alignment)l->alignment=align;*out=off;return UMI_STATUS_OK;}
size_t umi_nc_frame_layout_finalize(UmiNativeFrameLayout *l){if(l==NULL)return 0U;l->size=align_up(l->size,l->alignment);return l->size;}

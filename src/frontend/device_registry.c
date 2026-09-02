/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/device_registry.c
 *
 * PURPOSE:
 *   Implement the device registry behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/frontend/device_registry.h"
#include <string.h>
/* Provide the find index operation used by this module and its client applications. */
static size_t find_index(const UmiFrontendDeviceRegistry *r,const char *id){size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(r==NULL||id==NULL)return SIZE_MAX;/* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<r->count;++i)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(r->items[i].id,id)==0)return i;return SIZE_MAX;}
/*
 * Initialise frontend device registry from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_frontend_device_registry_init(UmiFrontendDeviceRegistry *r){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(r==NULL)return UMI_STATUS_INVALID_ARGUMENT;(void)memset(r,0,sizeof(*r));r->revision=1U;return UMI_STATUS_OK;}
/*
 * Add frontend device registry only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_frontend_device_registry_add(UmiFrontendDeviceRegistry *r,const UmiFrontendDevicePreset *p){size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(r==NULL||p==NULL||p->id[0]=='\0'||p->width==0U||p->height==0U)return UMI_STATUS_INVALID_ARGUMENT;i=find_index(r,p->id);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(i==SIZE_MAX){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(r->count>=UMI_FRONTEND_DEV_MAX_DEVICES)return UMI_STATUS_CAPACITY_EXCEEDED;i=r->count++;}r->items[i]=*p;r->revision+=1U;return UMI_STATUS_OK;}
/*
 * Find frontend device registry while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_frontend_device_registry_find(const UmiFrontendDeviceRegistry *r,const char *id,UmiFrontendDevicePreset *out){size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(r==NULL||id==NULL||out==NULL)return UMI_STATUS_INVALID_ARGUMENT;i=find_index(r,id);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(i==SIZE_MAX)return UMI_STATUS_NOT_FOUND;*out=r->items[i];return UMI_STATUS_OK;}
/* Provide the add operation used by this module and its client applications. */
static UmiStatus add(UmiFrontendDeviceRegistry *r,const char *id,const char *label,UmiFrontendDeviceKind kind,uint32_t w,uint32_t h,int touch){UmiFrontendDevicePreset p={0};UmiStatus s=umi_frontend_dev_copy_text(p.id,sizeof(p.id),id);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s==UMI_STATUS_OK)s=umi_frontend_dev_copy_text(p.label,sizeof(p.label),label);p.kind=kind;p.orientation=UMI_FRONTEND_ORIENTATION_PORTRAIT;p.width=w;p.height=h;p.scale_percent=100U;p.touch=touch;return s==UMI_STATUS_OK?umi_frontend_device_registry_add(r,&p):s;}
/*
 * Provide the frontend device registry add defaults operation used by this module and its
 * client applications.
 */
UmiStatus umi_frontend_device_registry_add_defaults(UmiFrontendDeviceRegistry *r){UmiStatus s;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(r==NULL)return UMI_STATUS_INVALID_ARGUMENT;s=add(r,"desktop-1440","Desktop 1440",UMI_FRONTEND_DEVICE_DESKTOP,1440U,900U,0);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==UMI_STATUS_OK)s=add(r,"tablet-768","Tablet 768",UMI_FRONTEND_DEVICE_TABLET,768U,1024U,1);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==UMI_STATUS_OK)s=add(r,"phone-390","Phone 390",UMI_FRONTEND_DEVICE_PHONE,390U,844U,1);return s;}

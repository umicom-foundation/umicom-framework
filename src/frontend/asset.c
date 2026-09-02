/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/asset.c
 *
 * PURPOSE:
 *   Describe one frontend resource without embedding toolkit details.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * The implementation turns semantic frontend information into portable output while keeping product state outside any specific UI toolkit.
 */

#include "umicom/frontend/asset.h"
#include "umicom/web/types.h"
#include <string.h>
/*
 * Initialise frontend asset from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_frontend_asset_init(UmiFrontendAsset *a,const char *name,const char *path,const char *type){UmiStatus s;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(a==NULL)return UMI_STATUS_INVALID_ARGUMENT;(void)memset(a,0,sizeof(*a));s=umi_web_copy_text(a->logical_name,sizeof(a->logical_name),name);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==UMI_STATUS_OK)s=umi_web_copy_text(a->path,sizeof(a->path),path);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==UMI_STATUS_OK)s=umi_web_copy_text(a->media_type,sizeof(a->media_type),type);return s;}

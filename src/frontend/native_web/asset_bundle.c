/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/native_web/asset_bundle.c
 *
 * PURPOSE:
 *   Catalogue Framework-generated browser assets such as runtime JavaScript and semantic CSS.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/frontend/native_web/asset_bundle.h"

#include <string.h>
void umi_native_web_asset_bundle_init(UmiNativeWebAssetBundle *bundle){if(bundle!=NULL)(void)memset(bundle,0,sizeof(*bundle));}
UmiStatus umi_native_web_asset_bundle_add(UmiNativeWebAssetBundle *bundle,const UmiNativeWebAsset *asset){size_t i;if(bundle==NULL||asset==NULL||asset->path[0]=='\0'||asset->content_type[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;for(i=0;i<bundle->count;++i)if(strcmp(bundle->items[i].path,asset->path)==0)return UMI_STATUS_ALREADY_EXISTS;if(bundle->count>=UMI_NATIVE_WEB_MAX_ASSETS)return UMI_STATUS_CAPACITY_EXCEEDED;bundle->items[bundle->count++]=*asset;++bundle->revision;return UMI_STATUS_OK;}


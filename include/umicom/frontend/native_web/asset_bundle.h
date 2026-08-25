/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/native_web/asset_bundle.h
 *
 * PURPOSE:
 *   Catalogue Framework-generated browser assets such as runtime JavaScript and semantic CSS.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_INCLUDE_UMICOM_FRONTEND_NATIVE_WEB_ASSET_BUNDLE_H
#define UMICOM_INCLUDE_UMICOM_FRONTEND_NATIVE_WEB_ASSET_BUNDLE_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/frontend/native_web/types.h"

typedef struct UmiNativeWebAsset { char path[UMI_NATIVE_WEB_TEXT_CAPACITY]; char content_type[64]; uint64_t checksum; bool immutable; } UmiNativeWebAsset;
typedef struct UmiNativeWebAssetBundle { UmiNativeWebAsset items[UMI_NATIVE_WEB_MAX_ASSETS]; size_t count; uint64_t revision; } UmiNativeWebAssetBundle;
/* Initialise an empty browser asset bundle. */
void umi_native_web_asset_bundle_init(UmiNativeWebAssetBundle *bundle);
/* Add one unique generated/static browser asset descriptor. */
UmiStatus umi_native_web_asset_bundle_add(UmiNativeWebAssetBundle *bundle, const UmiNativeWebAsset *asset);

#ifdef __cplusplus
}
#endif
#endif

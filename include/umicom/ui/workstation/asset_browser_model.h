/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/workstation/asset_browser_model.h
 *
 * PURPOSE:
 *   Define a reusable asset-browser model for media, AI creator, kitchen design, IDE resources and other content-heavy applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_WORKSTATION_ASSET_BROWSER_MODEL_H
#define UMICOM_UI_WORKSTATION_ASSET_BROWSER_MODEL_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/ui/workstation/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * List the named ws asset kind values accepted by this public contract.
 */
typedef enum UmiWsAssetKind {
    UMI_WS_ASSET_FILE = 1,
    UMI_WS_ASSET_IMAGE = 2,
    UMI_WS_ASSET_VIDEO = 3,
    UMI_WS_ASSET_AUDIO = 4,
    UMI_WS_ASSET_MODEL = 5,
    UMI_WS_ASSET_MATERIAL = 6,
    UMI_WS_ASSET_PRESET = 7
} UmiWsAssetKind;

/**
 * Represent the ws asset entry data shared with callers of this public contract.
 */
typedef struct UmiWsAssetEntry {
    char asset_id[UMI_UI_ID_CAPACITY];
    char label[UMI_UI_TEXT_CAPACITY];
    char uri[UMI_UI_DESCRIPTION_CAPACITY];
    UmiWsAssetKind kind;
    uint64_t size_bytes;
} UmiWsAssetEntry;

/**
 * Represent the ws asset browser model data shared with callers of this public contract.
 */
typedef struct UmiWsAssetBrowserModel {
    UmiWsAssetEntry assets[UMI_WS_MAX_ASSETS];
    size_t count;
} UmiWsAssetBrowserModel;

/**
 * Initialise ws asset browser model from caller-provided values so later operations
 * receive a known state.
 */
void umi_ws_asset_browser_model_init(UmiWsAssetBrowserModel *model);
/**
 * Add ws asset browser model only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_ws_asset_browser_model_add(UmiWsAssetBrowserModel *model,
                                         const char *asset_id,
                                         const char *label,
                                         const char *uri,
                                         UmiWsAssetKind kind,
                                         uint64_t size_bytes);
/**
 * Provide the ws asset browser model count kind operation used by this module and its
 * client applications.
 */
size_t umi_ws_asset_browser_model_count_kind(const UmiWsAssetBrowserModel *model, UmiWsAssetKind kind);

#ifdef __cplusplus
}
#endif

#endif

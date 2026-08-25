/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/workstation/asset_browser_model.c
 *
 * PURPOSE:
 *   Implement a reusable asset-browser model for media, AI creator, kitchen design, IDE resources and other content-heavy applications.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/workstation/asset_browser_model.h"

void umi_ws_asset_browser_model_init(UmiWsAssetBrowserModel *model) {
    if (model != NULL) *model = (UmiWsAssetBrowserModel){0};
}

UmiStatus umi_ws_asset_browser_model_add(UmiWsAssetBrowserModel *model,
                                         const char *asset_id,
                                         const char *label,
                                         const char *uri,
                                         UmiWsAssetKind kind,
                                         uint64_t size_bytes) {
    UmiWsAssetEntry *asset;
    if (model == NULL || !umi_ws_id_valid(asset_id) || label == NULL || uri == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (kind < UMI_WS_ASSET_FILE || kind > UMI_WS_ASSET_PRESET) return UMI_STATUS_INVALID_ARGUMENT;
    if (model->count >= UMI_WS_MAX_ASSETS) return UMI_STATUS_CAPACITY_EXCEEDED;
    asset = &model->assets[model->count++];
    *asset = (UmiWsAssetEntry){0};
    if (umi_ws_copy_text(asset->asset_id, sizeof(asset->asset_id), asset_id) != UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (umi_ws_copy_text(asset->label, sizeof(asset->label), label) != UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (umi_ws_copy_text(asset->uri, sizeof(asset->uri), uri) != UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED;
    asset->kind = kind;
    asset->size_bytes = size_bytes;
    return UMI_STATUS_OK;
}

size_t umi_ws_asset_browser_model_count_kind(const UmiWsAssetBrowserModel *model, UmiWsAssetKind kind) {
    size_t index;
    size_t count = 0U;
    if (model == NULL) return 0U;
    for (index = 0U; index < model->count; ++index) if (model->assets[index].kind == kind) ++count;
    return count;
}

/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/workstation/asset_browser_model.c
 *
 * PURPOSE:
 *   Implement a reusable asset-browser model for media, AI creator, kitchen design, IDE resources and other content-heavy applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/workstation/asset_browser_model.h"

/*
 * Initialise ws asset browser model from caller-provided values so later operations
 * receive a known state.
 */
void umi_ws_asset_browser_model_init(UmiWsAssetBrowserModel *model) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model != NULL) *model = (UmiWsAssetBrowserModel){0};
}

/*
 * Add ws asset browser model only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_ws_asset_browser_model_add(UmiWsAssetBrowserModel *model,
                                         const char *asset_id,
                                         const char *label,
                                         const char *uri,
                                         UmiWsAssetKind kind,
                                         uint64_t size_bytes) {
    UmiWsAssetEntry *asset;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || !umi_ws_id_valid(asset_id) || label == NULL || uri == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (kind < UMI_WS_ASSET_FILE || kind > UMI_WS_ASSET_PRESET) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (model->count >= UMI_WS_MAX_ASSETS) return UMI_STATUS_CAPACITY_EXCEEDED;
    asset = &model->assets[model->count++];
    *asset = (UmiWsAssetEntry){0};
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ws_copy_text(asset->asset_id, sizeof(asset->asset_id), asset_id) != UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ws_copy_text(asset->label, sizeof(asset->label), label) != UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ws_copy_text(asset->uri, sizeof(asset->uri), uri) != UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED;
    asset->kind = kind;
    asset->size_bytes = size_bytes;
    return UMI_STATUS_OK;
}

/*
 * Provide the ws asset browser model count kind operation used by this module and its
 * client applications.
 */
size_t umi_ws_asset_browser_model_count_kind(const UmiWsAssetBrowserModel *model, UmiWsAssetKind kind) {
    size_t index;
    size_t count = 0U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL) return 0U;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < model->count; ++index) /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (model->assets[index].kind == kind) ++count;
    return count;
}

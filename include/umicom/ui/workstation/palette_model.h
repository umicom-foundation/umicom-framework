/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/workstation/palette_model.h
 *
 * PURPOSE:
 *   Define searchable component/tool palette metadata for visual designers, media tools and application builders.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_WORKSTATION_PALETTE_MODEL_H
#define UMICOM_UI_WORKSTATION_PALETTE_MODEL_H

#include <stddef.h>
#include "umicom/ui/workstation/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the ws palette item data shared with callers of this public contract.
 */
typedef struct UmiWsPaletteItem {
    char item_id[UMI_UI_ID_CAPACITY];
    char label[UMI_UI_TEXT_CAPACITY];
    char category[UMI_UI_TEXT_CAPACITY];
    char icon_name[UMI_UI_ICON_CAPACITY];
} UmiWsPaletteItem;

/**
 * Represent the ws palette model data shared with callers of this public contract.
 */
typedef struct UmiWsPaletteModel {
    UmiWsPaletteItem items[UMI_WS_MAX_PALETTE_ITEMS];
    size_t count;
} UmiWsPaletteModel;

/**
 * Initialise ws palette model from caller-provided values so later operations receive a
 * known state.
 */
void umi_ws_palette_model_init(UmiWsPaletteModel *model);
/**
 * Add ws palette model only after its inputs and available capacity have been checked.
 */
UmiStatus umi_ws_palette_model_add(UmiWsPaletteModel *model,
                                   const char *item_id,
                                   const char *label,
                                   const char *category);
/**
 * Provide the ws palette model count matching operation used by this module and its client
 * applications.
 */
size_t umi_ws_palette_model_count_matching(const UmiWsPaletteModel *model, const char *query);

#ifdef __cplusplus
}
#endif

#endif

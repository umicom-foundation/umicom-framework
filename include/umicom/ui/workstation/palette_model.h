/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/workstation/palette_model.h
 *
 * PURPOSE:
 *   Define searchable component/tool palette metadata for visual designers, media tools and application builders.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_WORKSTATION_PALETTE_MODEL_H
#define UMICOM_UI_WORKSTATION_PALETTE_MODEL_H

#include <stddef.h>
#include "umicom/ui/workstation/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWsPaletteItem {
    char item_id[UMI_UI_ID_CAPACITY];
    char label[UMI_UI_TEXT_CAPACITY];
    char category[UMI_UI_TEXT_CAPACITY];
    char icon_name[UMI_UI_ICON_CAPACITY];
} UmiWsPaletteItem;

typedef struct UmiWsPaletteModel {
    UmiWsPaletteItem items[UMI_WS_MAX_PALETTE_ITEMS];
    size_t count;
} UmiWsPaletteModel;

void umi_ws_palette_model_init(UmiWsPaletteModel *model);
UmiStatus umi_ws_palette_model_add(UmiWsPaletteModel *model,
                                   const char *item_id,
                                   const char *label,
                                   const char *category);
size_t umi_ws_palette_model_count_matching(const UmiWsPaletteModel *model, const char *query);

#ifdef __cplusplus
}
#endif

#endif

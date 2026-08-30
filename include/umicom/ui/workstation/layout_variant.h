/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/workstation/layout_variant.h
 *
 * PURPOSE:
 *   Define automatic layout-variant selection for laptops, ultrawide displays and multi-monitor workstations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_WORKSTATION_LAYOUT_VARIANT_H
#define UMICOM_UI_WORKSTATION_LAYOUT_VARIANT_H

#include <stddef.h>
#include "umicom/ui/workstation/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWsLayoutEnvironment {
    int32_t primary_width;
    int32_t primary_height;
    size_t monitor_count;
    int32_t scale_percent;
} UmiWsLayoutEnvironment;

UmiWsLayoutVariantKind umi_ws_layout_variant_select(const UmiWsLayoutEnvironment *environment);
const char *umi_ws_layout_variant_text(UmiWsLayoutVariantKind variant);

#ifdef __cplusplus
}
#endif

#endif

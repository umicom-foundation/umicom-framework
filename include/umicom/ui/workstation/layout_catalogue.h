/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/workstation/layout_catalogue.h
 *
 * PURPOSE:
 *   Define a bounded catalogue of reusable and user-customisable workstation layout templates.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_WORKSTATION_LAYOUT_CATALOGUE_H
#define UMICOM_UI_WORKSTATION_LAYOUT_CATALOGUE_H

#include <stddef.h>
#include "umicom/ui/workstation/layout_template.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWsLayoutCatalogue {
    UmiWsLayoutTemplate layouts[UMI_WS_MAX_LAYOUTS];
    size_t count;
    uint64_t revision;
} UmiWsLayoutCatalogue;

void umi_ws_layout_catalogue_init(UmiWsLayoutCatalogue *catalogue);
UmiStatus umi_ws_layout_catalogue_upsert(UmiWsLayoutCatalogue *catalogue, const UmiWsLayoutTemplate *layout);
const UmiWsLayoutTemplate *umi_ws_layout_catalogue_find(const UmiWsLayoutCatalogue *catalogue, const char *layout_id);
size_t umi_ws_layout_catalogue_count_domain(const UmiWsLayoutCatalogue *catalogue, UmiWsApplicationDomain domain);

#ifdef __cplusplus
}
#endif

#endif

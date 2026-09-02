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

/**
 * Represent the ws layout catalogue data shared with callers of this public contract.
 */
typedef struct UmiWsLayoutCatalogue {
    UmiWsLayoutTemplate layouts[UMI_WS_MAX_LAYOUTS];
    size_t count;
    uint64_t revision;
} UmiWsLayoutCatalogue;

/**
 * Initialise ws layout catalogue from caller-provided values so later operations receive a
 * known state.
 */
void umi_ws_layout_catalogue_init(UmiWsLayoutCatalogue *catalogue);
/**
 * Provide the ws layout catalogue upsert operation used by this module and its client
 * applications.
 */
UmiStatus umi_ws_layout_catalogue_upsert(UmiWsLayoutCatalogue *catalogue, const UmiWsLayoutTemplate *layout);
/**
 * Find ws layout catalogue while leaving the underlying catalogue or model owned by this
 * module.
 */
const UmiWsLayoutTemplate *umi_ws_layout_catalogue_find(const UmiWsLayoutCatalogue *catalogue, const char *layout_id);
/**
 * Provide the ws layout catalogue count domain operation used by this module and its
 * client applications.
 */
size_t umi_ws_layout_catalogue_count_domain(const UmiWsLayoutCatalogue *catalogue, UmiWsApplicationDomain domain);

#ifdef __cplusplus
}
#endif

#endif

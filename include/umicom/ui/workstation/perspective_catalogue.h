/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/workstation/perspective_catalogue.h
 *
 * PURPOSE:
 *   Define a bounded perspective catalogue so products can contribute task semantics without owning layout infrastructure.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_WORKSTATION_PERSPECTIVE_CATALOGUE_H
#define UMICOM_UI_WORKSTATION_PERSPECTIVE_CATALOGUE_H

#include <stddef.h>
#include "umicom/ui/workstation/perspective_template.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the ws perspective catalogue data shared with callers of this public contract.
 */
typedef struct UmiWsPerspectiveCatalogue {
    UmiWsPerspectiveTemplate items[UMI_WS_MAX_PERSPECTIVES];
    size_t count;
} UmiWsPerspectiveCatalogue;

/**
 * Initialise ws perspective catalogue from caller-provided values so later operations
 * receive a known state.
 */
void umi_ws_perspective_catalogue_init(UmiWsPerspectiveCatalogue *catalogue);
/**
 * Provide the ws perspective catalogue upsert operation used by this module and its client
 * applications.
 */
UmiStatus umi_ws_perspective_catalogue_upsert(UmiWsPerspectiveCatalogue *catalogue,
                                               const UmiWsPerspectiveTemplate *perspective);
/**
 * Find ws perspective catalogue while leaving the underlying catalogue or model owned by
 * this module.
 */
const UmiWsPerspectiveTemplate *umi_ws_perspective_catalogue_find(const UmiWsPerspectiveCatalogue *catalogue,
                                                                  const char *perspective_id);

#ifdef __cplusplus
}
#endif

#endif

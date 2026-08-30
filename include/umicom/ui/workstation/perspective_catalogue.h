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

typedef struct UmiWsPerspectiveCatalogue {
    UmiWsPerspectiveTemplate items[UMI_WS_MAX_PERSPECTIVES];
    size_t count;
} UmiWsPerspectiveCatalogue;

void umi_ws_perspective_catalogue_init(UmiWsPerspectiveCatalogue *catalogue);
UmiStatus umi_ws_perspective_catalogue_upsert(UmiWsPerspectiveCatalogue *catalogue,
                                               const UmiWsPerspectiveTemplate *perspective);
const UmiWsPerspectiveTemplate *umi_ws_perspective_catalogue_find(const UmiWsPerspectiveCatalogue *catalogue,
                                                                  const char *perspective_id);

#ifdef __cplusplus
}
#endif

#endif

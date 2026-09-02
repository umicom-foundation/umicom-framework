/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/workstation/perspective_template.h
 *
 * PURPOSE:
 *   Define task-oriented workstation perspective metadata separately from concrete layout geometry.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_WORKSTATION_PERSPECTIVE_TEMPLATE_H
#define UMICOM_UI_WORKSTATION_PERSPECTIVE_TEMPLATE_H

#include <stddef.h>
#include "umicom/ui/workstation/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the ws perspective template data shared with callers of this public contract.
 */
typedef struct UmiWsPerspectiveTemplate {
    char perspective_id[UMI_UI_ID_CAPACITY];
    char label[UMI_UI_TEXT_CAPACITY];
    char default_layout_id[UMI_UI_ID_CAPACITY];
    char preferred_surface_ids[UMI_WS_MAX_SURFACES][UMI_UI_ID_CAPACITY];
    size_t preferred_surface_count;
    UmiWsApplicationDomain domain;
} UmiWsPerspectiveTemplate;

/**
 * Initialise ws perspective template from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_ws_perspective_template_init(UmiWsPerspectiveTemplate *perspective,
                                           const char *perspective_id,
                                           const char *label,
                                           UmiWsApplicationDomain domain);
/**
 * Provide the ws perspective template set default layout operation used by this module and
 * its client applications.
 */
UmiStatus umi_ws_perspective_template_set_default_layout(UmiWsPerspectiveTemplate *perspective,
                                                         const char *layout_id);
/**
 * Provide the ws perspective template add surface operation used by this module and its
 * client applications.
 */
UmiStatus umi_ws_perspective_template_add_surface(UmiWsPerspectiveTemplate *perspective,
                                                  const char *surface_id);

#ifdef __cplusplus
}
#endif

#endif

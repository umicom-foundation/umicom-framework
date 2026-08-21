/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/cross_application_panel/types.h
 *
 * PURPOSE:
 *   Define stable C23 contracts for reusable panels hosted across application workbenches.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CROSS_APPLICATION_PANEL_TYPES_H
#define UMICOM_CROSS_APPLICATION_PANEL_TYPES_H
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/context_channel/context_channel.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_PANEL_TEXT_CAPACITY 160U
#define UMI_PANEL_DESCRIPTION_CAPACITY 512U
#define UMI_PANEL_MAX_ITEMS 256U
#define UMI_PANEL_MAX_COMMANDS 64U
#define UMI_PANEL_MAX_VIEWS 32U
#define UMI_PANEL_MAX_CONTEXTS 16U
#define UMI_PANEL_MAX_EVENTS 256U
typedef enum UmiPanelLifecycleState { UMI_PANEL_CREATED=1, UMI_PANEL_REGISTERED=2, UMI_PANEL_MOUNTED=3, UMI_PANEL_VISIBLE=4, UMI_PANEL_HIDDEN=5, UMI_PANEL_SUSPENDED=6, UMI_PANEL_DESTROYED=7, UMI_PANEL_FAILED=8 } UmiPanelLifecycleState;
typedef enum UmiPanelPlacement { UMI_PANEL_PLACE_LEFT=1, UMI_PANEL_PLACE_RIGHT=2, UMI_PANEL_PLACE_TOP=3, UMI_PANEL_PLACE_BOTTOM=4, UMI_PANEL_PLACE_DOCUMENT=5, UMI_PANEL_PLACE_FLOATING=6 } UmiPanelPlacement;
typedef enum UmiPanelActivationReason { UMI_PANEL_ACTIVATION_USER=1, UMI_PANEL_ACTIVATION_CONTEXT=2, UMI_PANEL_ACTIVATION_COMMAND=3, UMI_PANEL_ACTIVATION_RESTORE=4 } UmiPanelActivationReason;
typedef struct UmiPanelIdentity { char panel_id[UMI_PANEL_TEXT_CAPACITY]; char application_id[UMI_PANEL_TEXT_CAPACITY]; char instance_id[UMI_PANEL_TEXT_CAPACITY]; char component_id[UMI_PANEL_TEXT_CAPACITY]; } UmiPanelIdentity;
typedef struct UmiPanelBounds { double x; double y; double width; double height; } UmiPanelBounds;
const char *umi_panel_lifecycle_state_text(UmiPanelLifecycleState state);
const char *umi_panel_placement_text(UmiPanelPlacement placement);
bool umi_panel_identity_valid(const UmiPanelIdentity *identity);
#ifdef __cplusplus
}
#endif
#endif

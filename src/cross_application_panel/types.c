/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/cross_application_panel/types.c
 *
 * PURPOSE:
 *   Implement panel lifecycle text and identity validation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/cross_application_panel/types.h"
/*
 * Provide the panel lifecycle state text operation used by this module and its client
 * applications.
 */
const char *umi_panel_lifecycle_state_text(UmiPanelLifecycleState state)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch(state){
case UMI_PANEL_CREATED:return "created";
case UMI_PANEL_REGISTERED:return "registered";
case UMI_PANEL_MOUNTED:return "mounted";
case UMI_PANEL_VISIBLE:return "visible";
case UMI_PANEL_HIDDEN:return "hidden";
case UMI_PANEL_SUSPENDED:return "suspended";
case UMI_PANEL_DESTROYED:return "destroyed";
case UMI_PANEL_FAILED:return "failed";
default:return "unknown";
}
}
/*
 * Provide the panel placement text operation used by this module and its client
 * applications.
 */
const char *umi_panel_placement_text(UmiPanelPlacement placement)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch(placement){
case UMI_PANEL_PLACE_LEFT:return "left";
case UMI_PANEL_PLACE_RIGHT:return "right";
case UMI_PANEL_PLACE_TOP:return "top";
case UMI_PANEL_PLACE_BOTTOM:return "bottom";
case UMI_PANEL_PLACE_DOCUMENT:return "document";
case UMI_PANEL_PLACE_FLOATING:return "floating";
default:return "unknown";
}
}
/* Check that panel identity satisfies its contract before another service relies on it. */
bool umi_panel_identity_valid(const UmiPanelIdentity *identity)
{
    return identity!=NULL&&identity->panel_id[0]!='\0'&&identity->application_id[0]!='\0'&&umi_context_text_is_valid(identity->panel_id,sizeof(identity->panel_id))&&umi_context_text_is_valid(identity->application_id,sizeof(identity->application_id))&&umi_context_text_is_valid(identity->instance_id,sizeof(identity->instance_id))&&umi_context_text_is_valid(identity->component_id,sizeof(identity->component_id));
}

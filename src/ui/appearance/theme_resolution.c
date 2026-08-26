/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/appearance/theme_resolution.c
 *
 * PURPOSE:
 *   Record deterministic system/application/workspace/component theme resolution evidence.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/theme_resolution.h"
#include <string.h>
/* Initialise bounded state without allocating renderer-specific resources. */
UmiStatus umi_appearance_theme_resolution_init(UmiAppearanceThemeResolution *item) {
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(item, 0, sizeof *item);
    (void)umi_appearance_copy_text(item->requested_pack_id,sizeof item->requested_pack_id,"theme.default.dark");
    (void)umi_appearance_copy_text(item->resolved_pack_id,sizeof item->resolved_pack_id,"theme.default.dark");
    item->winning_scope = UMI_APPEARANCE_SCOPE_SYSTEM;
    return UMI_STATUS_OK;
}

/* Validate semantic invariants before the record is published to a renderer. */
int umi_appearance_theme_resolution_is_valid(const UmiAppearanceThemeResolution *item) {
    if (item == NULL) return 0;
    return (umi_appearance_id_valid(item->resolved_pack_id) && item->winning_scope >= UMI_APPEARANCE_SCOPE_SYSTEM && item->winning_scope <= UMI_APPEARANCE_SCOPE_COMPONENT);
}
UmiStatus umi_appearance_theme_resolution_choose(UmiAppearanceThemeResolution *item,const char *system_id,const char *application_id,const char *workspace_id,const char *component_id){ const char *selected=system_id; UmiAppearanceScope scope=UMI_APPEARANCE_SCOPE_SYSTEM; if(item==NULL||!umi_appearance_id_valid(system_id)) return UMI_STATUS_INVALID_ARGUMENT; if(application_id&&application_id[0]){selected=application_id;scope=UMI_APPEARANCE_SCOPE_APPLICATION;} if(workspace_id&&workspace_id[0]){selected=workspace_id;scope=UMI_APPEARANCE_SCOPE_WORKSPACE;} if(component_id&&component_id[0]){selected=component_id;scope=UMI_APPEARANCE_SCOPE_COMPONENT;} if(umi_appearance_copy_text(item->resolved_pack_id,sizeof item->resolved_pack_id,selected)!=UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED; item->winning_scope=scope; return UMI_STATUS_OK;}

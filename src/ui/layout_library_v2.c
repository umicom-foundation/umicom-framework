/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/layout_library_v2.c
 *
 * PURPOSE:
 *   Implement the layout library v2 behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework | Workspace layout library v2 | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/ui/layout_library_v2.h"
#include <string.h>
UmiStatus umi_ui_layout_library_v2_add(UmiUiLayoutLibraryV2 *library,const UmiUiLayoutLibraryItemV2 *item)
{ size_t index; if (library == NULL || item == NULL || item->preset_id[0] == '\0' || item->layout.layout_id[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT; for (index = 0U; index < library->count; ++index) if (strcmp(library->items[index].preset_id,item->preset_id) == 0) return UMI_STATUS_ALREADY_EXISTS; if (library->count >= UMI_UI_LAYOUT_LIBRARY_MAX) return UMI_STATUS_CAPACITY_EXCEEDED; library->items[library->count++] = *item; library->revision += 1U; return UMI_STATUS_OK; }
const UmiUiLayoutLibraryItemV2 *umi_ui_layout_library_v2_find(const UmiUiLayoutLibraryV2 *library,const char *preset_id)
{ size_t index; if (library == NULL || preset_id == NULL) return NULL; for (index = 0U; index < library->count; ++index) if (strcmp(library->items[index].preset_id,preset_id) == 0) return &library->items[index]; return NULL; }
UmiStatus umi_ui_layout_library_v2_instantiate(const UmiUiLayoutLibraryV2 *library,const char *preset_id,const char *layout_id,const char *name,UmiUiWorkspaceLayoutV3 *out_layout)
{ const UmiUiLayoutLibraryItemV2 *item = umi_ui_layout_library_v2_find(library,preset_id); if (item == NULL) return UMI_STATUS_NOT_FOUND; return umi_ui_workspace_layout_v3_clone(&item->layout,layout_id,name,out_layout); }

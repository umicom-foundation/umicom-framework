/* Umicom Framework | Workspace layout library v2 | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/ui/layout_library.h"
#include <string.h>
UmiStatus umi_ui_layout_library_add(UmiUiLayoutLibrary *library,const UmiUiLayoutLibraryItem *item)
{ size_t index; if (library == NULL || item == NULL || item->preset_id[0] == '\0' || item->layout.layout_id[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT; for (index = 0U; index < library->count; ++index) if (strcmp(library->items[index].preset_id,item->preset_id) == 0) return UMI_STATUS_ALREADY_EXISTS; if (library->count >= UMI_UI_LAYOUT_LIBRARY_MAX) return UMI_STATUS_CAPACITY_EXCEEDED; library->items[library->count++] = *item; library->revision += 1U; return UMI_STATUS_OK; }
const UmiUiLayoutLibraryItem *umi_ui_layout_library_find(const UmiUiLayoutLibrary *library,const char *preset_id)
{ size_t index; if (library == NULL || preset_id == NULL) return NULL; for (index = 0U; index < library->count; ++index) if (strcmp(library->items[index].preset_id,preset_id) == 0) return &library->items[index]; return NULL; }
UmiStatus umi_ui_layout_library_instantiate(const UmiUiLayoutLibrary *library,const char *preset_id,const char *layout_id,const char *name,UmiUiWorkspaceLayout *out_layout)
{ const UmiUiLayoutLibraryItem *item = umi_ui_layout_library_find(library,preset_id); if (item == NULL) return UMI_STATUS_NOT_FOUND; return umi_ui_workspace_layout_clone(&item->layout,layout_id,name,out_layout); }

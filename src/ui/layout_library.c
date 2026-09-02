/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/layout_library.c
 *
 * PURPOSE:
 *   Implement the layout library behavior for
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
#include "umicom/ui/layout_library.h"
#include <stdlib.h>
#include <string.h>
/*
 * Initialise ui layout library from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_ui_layout_library_create(UmiUiLayoutLibrary **out_library)
{ UmiUiLayoutLibrary *library; /* Protect caller-owned memory by checking that required state is available before it is used. */ if (out_library == NULL) return UMI_STATUS_INVALID_ARGUMENT; *out_library = NULL; library = (UmiUiLayoutLibrary *)calloc(1U,sizeof(*library)); /* Protect caller-owned memory by checking that required state is available before it is used. */ if (library == NULL) return UMI_STATUS_OUT_OF_MEMORY; *out_library = library; return UMI_STATUS_OK; }
/*
 * Release or reset state held by ui layout library so the same storage can be reused
 * safely.
 */
void umi_ui_layout_library_destroy(UmiUiLayoutLibrary *library)
{ free(library); }
/* Add ui layout library only after its inputs and available capacity have been checked. */
UmiStatus umi_ui_layout_library_add(UmiUiLayoutLibrary *library,const UmiUiLayoutLibraryItem *item)
{ size_t index; /* Protect caller-owned memory by checking that required state is available before it is used. */ if (library == NULL || item == NULL || item->preset_id[0] == '\0' || item->layout.layout_id[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT; /* Visit each bounded item once so every record receives the same rule. */ for (index = 0U; index < library->count; ++index) /* Protect caller-owned memory by checking that required state is available before it is used. */ if (strcmp(library->items[index].preset_id,item->preset_id) == 0) return UMI_STATUS_ALREADY_EXISTS; /* Protect caller-owned memory by checking that required state is available before it is used. */ if (library->count >= UMI_UI_LAYOUT_LIBRARY_MAX) return UMI_STATUS_CAPACITY_EXCEEDED; library->items[library->count++] = *item; library->revision += 1U; return UMI_STATUS_OK; }
/*
 * Find ui layout library while leaving the underlying catalogue or model owned by this
 * module.
 */
const UmiUiLayoutLibraryItem *umi_ui_layout_library_find(const UmiUiLayoutLibrary *library,const char *preset_id)
{ size_t index; /* Protect caller-owned memory by checking that required state is available before it is used. */ if (library == NULL || preset_id == NULL) return NULL; /* Visit each bounded item once so every record receives the same rule. */ for (index = 0U; index < library->count; ++index) /* Protect caller-owned memory by checking that required state is available before it is used. */ if (strcmp(library->items[index].preset_id,preset_id) == 0) return &library->items[index]; return NULL; }
/*
 * Provide the ui layout library instantiate operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_layout_library_instantiate(const UmiUiLayoutLibrary *library,const char *preset_id,const char *layout_id,const char *name,UmiUiWorkspaceLayout *out_layout)
{ const UmiUiLayoutLibraryItem *item = umi_ui_layout_library_find(library,preset_id); /* Protect caller-owned memory by checking that required state is available before it is used. */ if (item == NULL) return UMI_STATUS_NOT_FOUND; return umi_ui_workspace_layout_clone(&item->layout,layout_id,name,out_layout); }

/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/layout_library.h
 *
 * PURPOSE:
 *   Publish the public layout library contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_LAYOUT_LIBRARY_H
#define UMICOM_UI_LAYOUT_LIBRARY_H
#include "umicom/ui/workspace_layout.h"
#define UMI_UI_LAYOUT_LIBRARY_MAX 32U
/**
 * Represent the ui layout library item data shared with callers of this public contract.
 */
typedef struct UmiUiLayoutLibraryItem { char preset_id[UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY]; char category[UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY]; char description[384U]; UmiUiWorkspaceLayout layout; } UmiUiLayoutLibraryItem;
/**
 * Represent the ui layout library data shared with callers of this public contract.
 */
typedef struct UmiUiLayoutLibrary { UmiUiLayoutLibraryItem items[UMI_UI_LAYOUT_LIBRARY_MAX]; size_t count; uint64_t revision; } UmiUiLayoutLibrary;
/**
 * Initialise ui layout library from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_ui_layout_library_create(UmiUiLayoutLibrary **out_library);
/**
 * Release or reset state held by ui layout library so the same storage can be reused
 * safely.
 */
void umi_ui_layout_library_destroy(UmiUiLayoutLibrary *library);
/**
 * Add ui layout library only after its inputs and available capacity have been checked.
 */
UmiStatus umi_ui_layout_library_add(UmiUiLayoutLibrary *library,const UmiUiLayoutLibraryItem *item);
/**
 * Find ui layout library while leaving the underlying catalogue or model owned by this
 * module.
 */
const UmiUiLayoutLibraryItem *umi_ui_layout_library_find(const UmiUiLayoutLibrary *library,const char *preset_id);
/**
 * Provide the ui layout library instantiate operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_layout_library_instantiate(const UmiUiLayoutLibrary *library,const char *preset_id,const char *layout_id,const char *name,UmiUiWorkspaceLayout *out_layout);
#endif

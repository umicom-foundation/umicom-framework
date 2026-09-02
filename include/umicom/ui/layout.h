/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/layout.h
 *
 * PURPOSE:
 *   Define a serialisable toolkit-neutral workbench layout tree containing split,
 *   tab, pane and document-area nodes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_LAYOUT_H
#define UMICOM_UI_LAYOUT_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/ui/types.h"

#ifdef __cplusplus
extern "C" {
#endif


#define UMI_UI_LAYOUT_MAX 512U
/**
 * List the named ui layout kind values accepted by this public contract.
 */
typedef enum UmiUiLayoutKind { UMI_UI_LAYOUT_ROOT=1, UMI_UI_LAYOUT_SPLIT=2, UMI_UI_LAYOUT_TABS=3, UMI_UI_LAYOUT_PANE=4, UMI_UI_LAYOUT_DOCUMENTS=5 } UmiUiLayoutKind;
/**
 * Represent the ui layout node data shared with callers of this public contract.
 */
typedef struct UmiUiLayoutNode { char node_id[UMI_UI_ID_CAPACITY]; char parent_id[UMI_UI_ID_CAPACITY]; char target_id[UMI_UI_ID_CAPACITY]; UmiUiLayoutKind kind; UmiUiOrientation orientation; double ratio; int32_t order; } UmiUiLayoutNode;
/**
 * Represent the ui layout data shared with callers of this public contract.
 */
typedef struct UmiUiLayout UmiUiLayout;
/**
 * Initialise ui layout from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_ui_layout_create(UmiUiLayout **out_layout);
/**
 * Release or reset state held by ui layout so the same storage can be reused safely.
 */
void umi_ui_layout_destroy(UmiUiLayout *layout);
/**
 * Provide the ui layout upsert operation used by this module and its client applications.
 */
UmiStatus umi_ui_layout_upsert(UmiUiLayout *layout, const UmiUiLayoutNode *node);
/**
 * Remove ui layout while keeping the remaining records in a valid and discoverable state.
 */
UmiStatus umi_ui_layout_remove(UmiUiLayout *layout, const char *node_id);
/**
 * Find ui layout while leaving the underlying catalogue or model owned by this module.
 */
UmiStatus umi_ui_layout_find(const UmiUiLayout *layout, const char *node_id, UmiUiLayoutNode *out_node);
/**
 * Find ui layout while leaving the underlying catalogue or model owned by this module.
 */
UmiStatus umi_ui_layout_at(const UmiUiLayout *layout, size_t index, UmiUiLayoutNode *out_node);
/**
 * Return the number of records represented by ui layout without changing their state.
 */
size_t umi_ui_layout_count(const UmiUiLayout *layout);
/**
 * Provide the ui layout revision operation used by this module and its client
 * applications.
 */
uint64_t umi_ui_layout_revision(const UmiUiLayout *layout);
/**
 * Check that ui layout satisfies its contract before another service relies on it.
 */
UmiStatus umi_ui_layout_validate(const UmiUiLayout *layout, char *out_message, size_t capacity);

#ifdef __cplusplus
}
#endif

#endif

/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/layout.h
 *
 * PURPOSE:
 *   Define a serialisable toolkit-neutral workbench layout tree containing split,
 *   tab, pane and document-area nodes.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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
typedef enum UmiUiLayoutKind { UMI_UI_LAYOUT_ROOT=1, UMI_UI_LAYOUT_SPLIT=2, UMI_UI_LAYOUT_TABS=3, UMI_UI_LAYOUT_PANE=4, UMI_UI_LAYOUT_DOCUMENTS=5 } UmiUiLayoutKind;
typedef struct UmiUiLayoutNode { char node_id[UMI_UI_ID_CAPACITY]; char parent_id[UMI_UI_ID_CAPACITY]; char target_id[UMI_UI_ID_CAPACITY]; UmiUiLayoutKind kind; UmiUiOrientation orientation; double ratio; int32_t order; } UmiUiLayoutNode;
typedef struct UmiUiLayout UmiUiLayout;
UmiStatus umi_ui_layout_create(UmiUiLayout **out_layout);
void umi_ui_layout_destroy(UmiUiLayout *layout);
UmiStatus umi_ui_layout_upsert(UmiUiLayout *layout, const UmiUiLayoutNode *node);
UmiStatus umi_ui_layout_remove(UmiUiLayout *layout, const char *node_id);
UmiStatus umi_ui_layout_find(const UmiUiLayout *layout, const char *node_id, UmiUiLayoutNode *out_node);
UmiStatus umi_ui_layout_at(const UmiUiLayout *layout, size_t index, UmiUiLayoutNode *out_node);
size_t umi_ui_layout_count(const UmiUiLayout *layout);
uint64_t umi_ui_layout_revision(const UmiUiLayout *layout);
UmiStatus umi_ui_layout_validate(const UmiUiLayout *layout, char *out_message, size_t capacity);

#ifdef __cplusplus
}
#endif

#endif

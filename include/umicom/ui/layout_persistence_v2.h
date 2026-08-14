/* Umicom Framework | Layout persistence boundary v2 | Sammy Hegab | Umicom Foundation | MIT */
#ifndef UMICOM_UI_LAYOUT_PERSISTENCE_V2_H
#define UMICOM_UI_LAYOUT_PERSISTENCE_V2_H
#include "umicom/ui/workspace_layout_v3.h"
#define UMI_UI_LAYOUT_ENCODED_CAPACITY 32768U
typedef struct UmiUiLayoutPersistenceRecordV2 { uint32_t schema_version; uint64_t saved_at_ns; UmiUiWorkspaceLayoutV3 layout; } UmiUiLayoutPersistenceRecordV2;
UmiStatus umi_ui_layout_persistence_v2_encode(const UmiUiLayoutPersistenceRecordV2 *record,char *out_text,size_t capacity);
UmiStatus umi_ui_layout_persistence_v2_decode(const char *text,UmiUiLayoutPersistenceRecordV2 *out_record);
#endif

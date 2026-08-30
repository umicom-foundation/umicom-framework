/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/layout_persistence.h
 *
 * PURPOSE:
 *   Publish the public layout persistence contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_LAYOUT_PERSISTENCE_H
#define UMICOM_UI_LAYOUT_PERSISTENCE_H
#include "umicom/ui/workspace_layout.h"
#define UMI_UI_LAYOUT_ENCODED_CAPACITY 32768U
typedef struct UmiUiLayoutPersistenceRecord { uint32_t schema_version; uint64_t saved_at_ns; UmiUiWorkspaceLayout layout; } UmiUiLayoutPersistenceRecord;
UmiStatus umi_ui_layout_persistence_encode(const UmiUiLayoutPersistenceRecord *record,char *out_text,size_t capacity);
UmiStatus umi_ui_layout_persistence_decode(const char *text,UmiUiLayoutPersistenceRecord *out_record);
#endif

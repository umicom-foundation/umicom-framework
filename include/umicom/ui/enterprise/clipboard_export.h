/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/enterprise/clipboard_export.h
 *
 * PURPOSE:
 *   Build bounded tab-separated text for selected enterprise cells.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_ENTERPRISE_CLIPBOARD_EXPORT_H
#define UMICOM_UI_ENTERPRISE_CLIPBOARD_EXPORT_H

#include "umicom/ui/enterprise/types.h"
/**
 * Represent the ui ent clipboard export data shared with callers of this public contract.
 */
typedef struct UmiUiEntClipboardExport { char *buffer; size_t capacity; size_t length; } UmiUiEntClipboardExport;
/**
 * Initialise ui ent clipboard export from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_ui_ent_clipboard_export_init(UmiUiEntClipboardExport *e,char *buffer,size_t capacity);
/**
 * Add ui ent clipboard export only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_ui_ent_clipboard_export_append(UmiUiEntClipboardExport *e,const char *text,int end_of_row);

#endif

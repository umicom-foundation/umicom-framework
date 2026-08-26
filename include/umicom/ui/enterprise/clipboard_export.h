/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/enterprise/clipboard_export.h
 *
 * PURPOSE:
 *   Build bounded tab-separated text for selected enterprise cells.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_ENTERPRISE_CLIPBOARD_EXPORT_H
#define UMICOM_UI_ENTERPRISE_CLIPBOARD_EXPORT_H

#include "umicom/ui/enterprise/types.h"
typedef struct UmiUiEntClipboardExport { char *buffer; size_t capacity; size_t length; } UmiUiEntClipboardExport;
UmiStatus umi_ui_ent_clipboard_export_init(UmiUiEntClipboardExport *e,char *buffer,size_t capacity);
UmiStatus umi_ui_ent_clipboard_export_append(UmiUiEntClipboardExport *e,const char *text,int end_of_row);

#endif

/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/clipboard.h
 *
 * PURPOSE:
 *   Define a frontend-neutral clipboard contract and an in-memory implementation
 *   used by headless tests and non-native hosts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_CLIPBOARD_H
#define UMICOM_UI_CLIPBOARD_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif


#define UMI_UI_CLIPBOARD_TEXT_CAPACITY 65536U
typedef struct UmiUiClipboard UmiUiClipboard;
UmiStatus umi_ui_clipboard_create(UmiUiClipboard **out_clipboard);
void umi_ui_clipboard_destroy(UmiUiClipboard *clipboard);
UmiStatus umi_ui_clipboard_set_text(UmiUiClipboard *clipboard, const char *text);
UmiStatus umi_ui_clipboard_get_text(const UmiUiClipboard *clipboard, char *out_text, size_t capacity);
void umi_ui_clipboard_clear(UmiUiClipboard *clipboard);
uint64_t umi_ui_clipboard_revision(const UmiUiClipboard *clipboard);

#ifdef __cplusplus
}
#endif

#endif

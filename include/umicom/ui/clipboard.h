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
/**
 * Represent the ui clipboard data shared with callers of this public contract.
 */
typedef struct UmiUiClipboard UmiUiClipboard;
/**
 * Initialise ui clipboard from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_ui_clipboard_create(UmiUiClipboard **out_clipboard);
/**
 * Release or reset state held by ui clipboard so the same storage can be reused safely.
 */
void umi_ui_clipboard_destroy(UmiUiClipboard *clipboard);
/**
 * Provide the ui clipboard set text operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_clipboard_set_text(UmiUiClipboard *clipboard, const char *text);
/**
 * Provide the ui clipboard get text operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_clipboard_get_text(const UmiUiClipboard *clipboard, char *out_text, size_t capacity);
/**
 * Release or reset state held by ui clipboard so the same storage can be reused safely.
 */
void umi_ui_clipboard_clear(UmiUiClipboard *clipboard);
/**
 * Provide the ui clipboard revision operation used by this module and its client
 * applications.
 */
uint64_t umi_ui_clipboard_revision(const UmiUiClipboard *clipboard);

#ifdef __cplusplus
}
#endif

#endif

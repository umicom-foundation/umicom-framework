/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/enterprise/streaming_window.h
 *
 * PURPOSE:
 *   Track ordered streaming windows and reject stale sequence updates.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_ENTERPRISE_STREAMING_WINDOW_H
#define UMICOM_UI_ENTERPRISE_STREAMING_WINDOW_H

#include "umicom/ui/enterprise/types.h"
/**
 * Represent the ui ent streaming window data shared with callers of this public contract.
 */
typedef struct UmiUiEntStreamingWindow { uint64_t generation; uint64_t last_sequence; size_t first_row; size_t row_count; size_t capacity; } UmiUiEntStreamingWindow;
/**
 * Initialise ui ent streaming window from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_ui_ent_streaming_window_init(UmiUiEntStreamingWindow *w,size_t capacity);
/**
 * Provide the ui ent streaming window accept operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_ent_streaming_window_accept(UmiUiEntStreamingWindow *w,uint64_t sequence,size_t first_row,size_t row_count);

#endif

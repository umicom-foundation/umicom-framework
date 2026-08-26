/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/enterprise/streaming_window.h
 *
 * PURPOSE:
 *   Track ordered streaming windows and reject stale sequence updates.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_ENTERPRISE_STREAMING_WINDOW_H
#define UMICOM_UI_ENTERPRISE_STREAMING_WINDOW_H

#include "umicom/ui/enterprise/types.h"
typedef struct UmiUiEntStreamingWindow { uint64_t generation; uint64_t last_sequence; size_t first_row; size_t row_count; size_t capacity; } UmiUiEntStreamingWindow;
UmiStatus umi_ui_ent_streaming_window_init(UmiUiEntStreamingWindow *w,size_t capacity);
UmiStatus umi_ui_ent_streaming_window_accept(UmiUiEntStreamingWindow *w,uint64_t sequence,size_t first_row,size_t row_count);

#endif

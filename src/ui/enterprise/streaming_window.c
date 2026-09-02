/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/enterprise/streaming_window.c
 *
 * PURPOSE:
 *   Implement monotonic streaming-window state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/streaming_window.h"
/*
 * Initialise ui ent streaming window from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_ui_ent_streaming_window_init(UmiUiEntStreamingWindow *w,size_t cap){/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(!w||cap==0U)return UMI_STATUS_INVALID_ARGUMENT;w->generation=1U;w->last_sequence=0U;w->first_row=0U;w->row_count=0U;w->capacity=cap;return UMI_STATUS_OK;}
/*
 * Provide the ui ent streaming window accept operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_ent_streaming_window_accept(UmiUiEntStreamingWindow *w,uint64_t seq,size_t first,size_t count){/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(!w||seq==0U||count>w->capacity)return UMI_STATUS_INVALID_ARGUMENT;/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(seq<=w->last_sequence)return UMI_STATUS_INVALID_STATE;w->last_sequence=seq;w->first_row=first;w->row_count=count;w->generation++;return UMI_STATUS_OK;}

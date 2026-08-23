/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language_runtime/types.c
 *
 * PURPOSE:
 *   Implement stable runtime state text and range validation.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language_runtime/types.h"
const char *umi_language_runtime_message_kind_text(UmiLanguageRuntimeMessageKind kind){switch(kind){case UMI_LANGUAGE_RUNTIME_MESSAGE_REQUEST:return "request";case UMI_LANGUAGE_RUNTIME_MESSAGE_RESPONSE:return "response";case UMI_LANGUAGE_RUNTIME_MESSAGE_NOTIFICATION:return "notification";case UMI_LANGUAGE_RUNTIME_MESSAGE_ERROR:return "error";default:return "unknown";}}
const char *umi_language_runtime_server_state_text(UmiLanguageRuntimeServerState state){switch(state){case UMI_LANGUAGE_RUNTIME_SERVER_STOPPED:return "stopped";case UMI_LANGUAGE_RUNTIME_SERVER_STARTING:return "starting";case UMI_LANGUAGE_RUNTIME_SERVER_INITIALIZING:return "initializing";case UMI_LANGUAGE_RUNTIME_SERVER_READY:return "ready";case UMI_LANGUAGE_RUNTIME_SERVER_STOPPING:return "stopping";case UMI_LANGUAGE_RUNTIME_SERVER_FAILED:return "failed";default:return "unknown";}}
UmiStatus umi_language_runtime_range_validate(const UmiLanguageRuntimeRange *range){if(range==NULL)return UMI_STATUS_INVALID_ARGUMENT;if(range->end.line<range->start.line||(range->end.line==range->start.line&&range->end.character<range->start.character))return UMI_STATUS_INVALID_ARGUMENT;return UMI_STATUS_OK;}

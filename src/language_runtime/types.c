/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language_runtime/types.c
 *
 * PURPOSE:
 *   Implement stable runtime state text and range validation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language_runtime/types.h"
/*
 * Provide the language runtime message kind text operation used by this module and its
 * client applications.
 */
const char *umi_language_runtime_message_kind_text(UmiLanguageRuntimeMessageKind kind){/* Select the behaviour associated with the requested command or state value. */ switch(kind){case UMI_LANGUAGE_RUNTIME_MESSAGE_REQUEST:return "request";case UMI_LANGUAGE_RUNTIME_MESSAGE_RESPONSE:return "response";case UMI_LANGUAGE_RUNTIME_MESSAGE_NOTIFICATION:return "notification";case UMI_LANGUAGE_RUNTIME_MESSAGE_ERROR:return "error";default:return "unknown";}}
/*
 * Provide the language runtime server state text operation used by this module and its
 * client applications.
 */
const char *umi_language_runtime_server_state_text(UmiLanguageRuntimeServerState state){/* Select the behaviour associated with the requested command or state value. */ switch(state){case UMI_LANGUAGE_RUNTIME_SERVER_STOPPED:return "stopped";case UMI_LANGUAGE_RUNTIME_SERVER_STARTING:return "starting";case UMI_LANGUAGE_RUNTIME_SERVER_INITIALIZING:return "initializing";case UMI_LANGUAGE_RUNTIME_SERVER_READY:return "ready";case UMI_LANGUAGE_RUNTIME_SERVER_STOPPING:return "stopping";case UMI_LANGUAGE_RUNTIME_SERVER_FAILED:return "failed";default:return "unknown";}}
/*
 * Check that language runtime range satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_language_runtime_range_validate(const UmiLanguageRuntimeRange *range){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(range==NULL)return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(range->end.line<range->start.line||(range->end.line==range->start.line&&range->end.character<range->start.character))return UMI_STATUS_INVALID_ARGUMENT;return UMI_STATUS_OK;}

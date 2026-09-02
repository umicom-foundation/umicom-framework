/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language_runtime/types.h
 *
 * PURPOSE:
 *   Define bounded primitives shared by the persistent LSP/IntelliSense runtime.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_RUNTIME_TYPES_H
#define UMICOM_LANGUAGE_RUNTIME_TYPES_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_LANGUAGE_RUNTIME_API_VERSION 1U
#define UMI_LANGUAGE_RUNTIME_ID_CAPACITY 192U
#define UMI_LANGUAGE_RUNTIME_NAME_CAPACITY 256U
#define UMI_LANGUAGE_RUNTIME_PATH_CAPACITY 2048U
#define UMI_LANGUAGE_RUNTIME_TEXT_CAPACITY 4096U
#define UMI_LANGUAGE_RUNTIME_JSON_CAPACITY 65536U
#define UMI_LANGUAGE_RUNTIME_FRAME_CAPACITY 131072U
#define UMI_LANGUAGE_RUNTIME_MAX_ARGUMENTS 32U
#define UMI_LANGUAGE_RUNTIME_MAX_SERVERS 16U
#define UMI_LANGUAGE_RUNTIME_MAX_DOCUMENTS 256U
#define UMI_LANGUAGE_RUNTIME_MAX_PENDING_REQUESTS 256U
#define UMI_LANGUAGE_RUNTIME_MAX_TOKENS 4096U
#define UMI_LANGUAGE_RUNTIME_MAX_EDITS 256U

/**
 * Represent the language runtime position data shared with callers of this public
 * contract.
 */
typedef struct UmiLanguageRuntimePosition { uint32_t line; uint32_t character; } UmiLanguageRuntimePosition;
/**
 * Represent the language runtime range data shared with callers of this public contract.
 */
typedef struct UmiLanguageRuntimeRange { UmiLanguageRuntimePosition start; UmiLanguageRuntimePosition end; } UmiLanguageRuntimeRange;
/**
 * Represent the language runtime location data shared with callers of this public
 * contract.
 */
typedef struct UmiLanguageRuntimeLocation { char uri[UMI_LANGUAGE_RUNTIME_PATH_CAPACITY]; UmiLanguageRuntimeRange range; } UmiLanguageRuntimeLocation;
/**
 * Represent the language runtime text edit data shared with callers of this public
 * contract.
 */
typedef struct UmiLanguageRuntimeTextEdit { UmiLanguageRuntimeRange range; char new_text[UMI_LANGUAGE_RUNTIME_TEXT_CAPACITY]; } UmiLanguageRuntimeTextEdit;
/**
 * List the named language runtime message kind values accepted by this public contract.
 */
typedef enum UmiLanguageRuntimeMessageKind { UMI_LANGUAGE_RUNTIME_MESSAGE_UNKNOWN=0, UMI_LANGUAGE_RUNTIME_MESSAGE_REQUEST=1, UMI_LANGUAGE_RUNTIME_MESSAGE_RESPONSE=2, UMI_LANGUAGE_RUNTIME_MESSAGE_NOTIFICATION=3, UMI_LANGUAGE_RUNTIME_MESSAGE_ERROR=4 } UmiLanguageRuntimeMessageKind;
/**
 * List the named language runtime server state values accepted by this public contract.
 */
typedef enum UmiLanguageRuntimeServerState { UMI_LANGUAGE_RUNTIME_SERVER_STOPPED=0, UMI_LANGUAGE_RUNTIME_SERVER_STARTING=1, UMI_LANGUAGE_RUNTIME_SERVER_INITIALIZING=2, UMI_LANGUAGE_RUNTIME_SERVER_READY=3, UMI_LANGUAGE_RUNTIME_SERVER_STOPPING=4, UMI_LANGUAGE_RUNTIME_SERVER_FAILED=5 } UmiLanguageRuntimeServerState;
/**
 * Represent the language runtime envelope data shared with callers of this public
 * contract.
 */
typedef struct UmiLanguageRuntimeEnvelope { UmiLanguageRuntimeMessageKind kind; uint64_t request_id; char method[UMI_LANGUAGE_RUNTIME_NAME_CAPACITY]; char json[UMI_LANGUAGE_RUNTIME_JSON_CAPACITY]; int error_code; char error_message[UMI_LANGUAGE_RUNTIME_TEXT_CAPACITY]; } UmiLanguageRuntimeEnvelope;
/**
 * Provide the language runtime message kind text operation used by this module and its
 * client applications.
 */
const char *umi_language_runtime_message_kind_text(UmiLanguageRuntimeMessageKind kind);
/**
 * Provide the language runtime server state text operation used by this module and its
 * client applications.
 */
const char *umi_language_runtime_server_state_text(UmiLanguageRuntimeServerState state);
/**
 * Check that language runtime range satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_language_runtime_range_validate(const UmiLanguageRuntimeRange *range);
#ifdef __cplusplus
}
#endif
#endif

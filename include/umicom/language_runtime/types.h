/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language_runtime/types.h
 *
 * PURPOSE:
 *   Define bounded primitives shared by the persistent LSP/IntelliSense runtime.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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

typedef struct UmiLanguageRuntimePosition { uint32_t line; uint32_t character; } UmiLanguageRuntimePosition;
typedef struct UmiLanguageRuntimeRange { UmiLanguageRuntimePosition start; UmiLanguageRuntimePosition end; } UmiLanguageRuntimeRange;
typedef struct UmiLanguageRuntimeLocation { char uri[UMI_LANGUAGE_RUNTIME_PATH_CAPACITY]; UmiLanguageRuntimeRange range; } UmiLanguageRuntimeLocation;
typedef struct UmiLanguageRuntimeTextEdit { UmiLanguageRuntimeRange range; char new_text[UMI_LANGUAGE_RUNTIME_TEXT_CAPACITY]; } UmiLanguageRuntimeTextEdit;
typedef enum UmiLanguageRuntimeMessageKind { UMI_LANGUAGE_RUNTIME_MESSAGE_UNKNOWN=0, UMI_LANGUAGE_RUNTIME_MESSAGE_REQUEST=1, UMI_LANGUAGE_RUNTIME_MESSAGE_RESPONSE=2, UMI_LANGUAGE_RUNTIME_MESSAGE_NOTIFICATION=3, UMI_LANGUAGE_RUNTIME_MESSAGE_ERROR=4 } UmiLanguageRuntimeMessageKind;
typedef enum UmiLanguageRuntimeServerState { UMI_LANGUAGE_RUNTIME_SERVER_STOPPED=0, UMI_LANGUAGE_RUNTIME_SERVER_STARTING=1, UMI_LANGUAGE_RUNTIME_SERVER_INITIALIZING=2, UMI_LANGUAGE_RUNTIME_SERVER_READY=3, UMI_LANGUAGE_RUNTIME_SERVER_STOPPING=4, UMI_LANGUAGE_RUNTIME_SERVER_FAILED=5 } UmiLanguageRuntimeServerState;
typedef struct UmiLanguageRuntimeEnvelope { UmiLanguageRuntimeMessageKind kind; uint64_t request_id; char method[UMI_LANGUAGE_RUNTIME_NAME_CAPACITY]; char json[UMI_LANGUAGE_RUNTIME_JSON_CAPACITY]; int error_code; char error_message[UMI_LANGUAGE_RUNTIME_TEXT_CAPACITY]; } UmiLanguageRuntimeEnvelope;
const char *umi_language_runtime_message_kind_text(UmiLanguageRuntimeMessageKind kind);
const char *umi_language_runtime_server_state_text(UmiLanguageRuntimeServerState state);
UmiStatus umi_language_runtime_range_validate(const UmiLanguageRuntimeRange *range);
#ifdef __cplusplus
}
#endif
#endif

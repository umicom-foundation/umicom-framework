/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language_runtime/language_runtime.h
 *
 * PURPOSE:
 *   Aggregate the real persistent Language Server Protocol runtime and Framework IntelliSense integration.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_RUNTIME_LANGUAGE_RUNTIME_H
#define UMICOM_LANGUAGE_RUNTIME_LANGUAGE_RUNTIME_H
#include "umicom/language_runtime/types.h"
#include "umicom/language_runtime/json.h"
#include "umicom/language_runtime/json_writer.h"
#include "umicom/language_runtime/framing.h"
#include "umicom/language_runtime/message.h"
#include "umicom/language_runtime/process_stream.h"
#include "umicom/language_runtime/transport.h"
#include "umicom/language_runtime/memory_transport.h"
#include "umicom/language_runtime/arguments.h"
#include "umicom/language_runtime/pending.h"
#include "umicom/language_runtime/request_context.h"
#include "umicom/language_runtime/language_id.h"
#include "umicom/language_runtime/server.h"
#include "umicom/language_runtime/request_support.h"
#include "umicom/language_runtime/requests/requests.h"
#include "umicom/language_runtime/profiles/profiles.h"
#include "umicom/language_runtime/builtin_profiles.h"
#include "umicom/language_runtime/profile_health.h"
#include "umicom/language_runtime/results.h"
#include "umicom/language_runtime/decoder_support.h"
#include "umicom/language_runtime/decoders/decoders.h"
#include "umicom/language_runtime/service_bridge.h"
#include "umicom/language_runtime/editor_bridge.h"
#include "umicom/language_runtime/workspace_edit_bridge.h"
#include "umicom/language_runtime/document_sync.h"
#include "umicom/language_runtime/server_manager.h"
#include "umicom/language_runtime/dispatcher.h"
#include "umicom/language_runtime/platform.h"
#include "umicom/language_runtime/workbench_bridge.h"
#endif

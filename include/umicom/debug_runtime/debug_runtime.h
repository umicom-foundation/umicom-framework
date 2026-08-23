/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug_runtime/debug_runtime.h
 *
 * PURPOSE:
 *   Aggregate the real persistent Debug Adapter Protocol runtime and its
 *   integration with existing Debug Service, Advanced Debugging and Developer
 *   Workbench contracts.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEBUG_RUNTIME_DEBUG_RUNTIME_H
#define UMICOM_DEBUG_RUNTIME_DEBUG_RUNTIME_H

#include "umicom/debug_runtime/types.h"
#include "umicom/debug_runtime/results.h"
#include "umicom/debug_runtime/message.h"
#include "umicom/debug_runtime/pending.h"
#include "umicom/debug_runtime/event_queue.h"
#include "umicom/debug_runtime/response_queue.h"
#include "umicom/debug_runtime/transport.h"
#include "umicom/debug_runtime/adapter.h"
#include "umicom/debug_runtime/request_support.h"
#include "umicom/debug_runtime/function_breakpoint.h"
#include "umicom/debug_runtime/data_breakpoint.h"
#include "umicom/debug_runtime/instruction_breakpoint.h"
#include "umicom/debug_runtime/requests/requests.h"
#include "umicom/debug_runtime/profiles/profiles.h"
#include "umicom/debug_runtime/builtin_profiles.h"
#include "umicom/debug_runtime/profile_health.h"
#include "umicom/debug_runtime/base64.h"
#include "umicom/debug_runtime/decoder_support.h"
#include "umicom/debug_runtime/decoders/decoders.h"
#include "umicom/debug_runtime/capabilities.h"
#include "umicom/debug_runtime/contract_adapter.h"
#include "umicom/debug_runtime/service_bridge.h"
#include "umicom/debug_runtime/platform.h"
#include "umicom/debug_runtime/command_router.h"
#include "umicom/debug_runtime/workbench_bridge.h"

#endif

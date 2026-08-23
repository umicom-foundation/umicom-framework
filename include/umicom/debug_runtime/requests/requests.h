/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug_runtime/requests/requests.h
 *
 * PURPOSE:
 *   Aggregate built-in Debug Adapter Protocol request builders.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEBUG_RUNTIME_REQUESTS_H
#define UMICOM_DEBUG_RUNTIME_REQUESTS_H

#include "umicom/debug_runtime/requests/attach.h"
#include "umicom/debug_runtime/requests/breakpoint_locations.h"
#include "umicom/debug_runtime/requests/cancel.h"
#include "umicom/debug_runtime/requests/completions.h"
#include "umicom/debug_runtime/requests/configuration_done.h"
#include "umicom/debug_runtime/requests/continue.h"
#include "umicom/debug_runtime/requests/data_breakpoint_info.h"
#include "umicom/debug_runtime/requests/disassemble.h"
#include "umicom/debug_runtime/requests/disconnect.h"
#include "umicom/debug_runtime/requests/evaluate.h"
#include "umicom/debug_runtime/requests/exception_info.h"
#include "umicom/debug_runtime/requests/goto.h"
#include "umicom/debug_runtime/requests/goto_targets.h"
#include "umicom/debug_runtime/requests/initialize.h"
#include "umicom/debug_runtime/requests/launch.h"
#include "umicom/debug_runtime/requests/loaded_sources.h"
#include "umicom/debug_runtime/requests/modules.h"
#include "umicom/debug_runtime/requests/next.h"
#include "umicom/debug_runtime/requests/pause.h"
#include "umicom/debug_runtime/requests/read_memory.h"
#include "umicom/debug_runtime/requests/restart.h"
#include "umicom/debug_runtime/requests/restart_frame.h"
#include "umicom/debug_runtime/requests/reverse_continue.h"
#include "umicom/debug_runtime/requests/scopes.h"
#include "umicom/debug_runtime/requests/set_breakpoints.h"
#include "umicom/debug_runtime/requests/set_data_breakpoints.h"
#include "umicom/debug_runtime/requests/set_exception_breakpoints.h"
#include "umicom/debug_runtime/requests/set_expression.h"
#include "umicom/debug_runtime/requests/set_function_breakpoints.h"
#include "umicom/debug_runtime/requests/set_instruction_breakpoints.h"
#include "umicom/debug_runtime/requests/set_variable.h"
#include "umicom/debug_runtime/requests/source.h"
#include "umicom/debug_runtime/requests/stack_trace.h"
#include "umicom/debug_runtime/requests/step_back.h"
#include "umicom/debug_runtime/requests/step_in.h"
#include "umicom/debug_runtime/requests/step_in_targets.h"
#include "umicom/debug_runtime/requests/step_out.h"
#include "umicom/debug_runtime/requests/terminate.h"
#include "umicom/debug_runtime/requests/terminate_threads.h"
#include "umicom/debug_runtime/requests/threads.h"
#include "umicom/debug_runtime/requests/variables.h"
#include "umicom/debug_runtime/requests/write_memory.h"

#endif

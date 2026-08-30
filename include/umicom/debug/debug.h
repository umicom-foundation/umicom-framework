/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug/debug.h
 *
 * PURPOSE:
 *   Aggregate the reusable debugger platform contracts for applications,
 *   adapters, tests and frontend implementations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEBUG_DEBUG_H
#define UMICOM_DEBUG_DEBUG_H

#include "umicom/debug/launch_configuration.h"
#include "umicom/debug/breakpoint.h"
#include "umicom/debug/session.h"
#include "umicom/debug/thread.h"
#include "umicom/debug/stack_frame.h"
#include "umicom/debug/scope.h"
#include "umicom/debug/variable.h"
#include "umicom/debug/watch.h"
#include "umicom/debug/console_entry.h"
#include "umicom/debug/module.h"
#include "umicom/debug/source.h"
#include "umicom/debug/exception.h"
#include "umicom/debug/event.h"
#include "umicom/debug/adapter_profile.h"
#include "umicom/debug/capabilities.h"
#include "umicom/debug/controller.h"
#include "umicom/debug/breakpoint_plan.h"
#include "umicom/debug/timeline.h"
#include "umicom/debug/view_state.h"
#include "umicom/debug/watch_expression.h"
#include "umicom/debug/workspace.h"
#include "umicom/debug/service.h"
#include "umicom/debug/configuration_resolver.h"
#include "umicom/debug/compound_configuration.h"
#include "umicom/debug/breakpoint_query.h"
#include "umicom/debug/watch_query.h"
#include "umicom/debug/console_query.h"
#include "umicom/debug/command.h"
#include "umicom/debug/orchestration.h"

/* Advanced debugger inspection and adapter platform. */
#include "umicom/debug/advanced_breakpoint.h"
#include "umicom/debug/adapter_contract.h"
#include "umicom/debug/thread_inspector.h"
#include "umicom/debug/register_bank.h"
#include "umicom/debug/memory_view.h"
#include "umicom/debug/disassembly_view.h"
#include "umicom/debug/inspection_session.h"
#include "umicom/debug/advanced_debugging.h"

#endif /* UMICOM_DEBUG_DEBUG_H */

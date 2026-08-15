/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug/debug.h
 *
 * PURPOSE:
 *   Aggregate the complete Umicom debugger service API.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This module uses a small, explicit C API and bounded storage.  The public
 * contract does not expose toolkit objects, C++ types, or private structures.
 */
#ifndef FRAMEWORK_INCLUDE_UMICOM_DEBUG_DEBUG_H
#define FRAMEWORK_INCLUDE_UMICOM_DEBUG_DEBUG_H

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

#endif

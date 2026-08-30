/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: templates/workspace/framework/include/umicom/umicom.h
 *
 * PURPOSE:
 *   Provide the aggregate public Framework include for applications that use
 *   the complete C23 foundation through the Umicom::Framework target.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UMICOM_H
#define UMICOM_UMICOM_H

#include "umicom/base/version.h"
#include "umicom/base/status.h"
#include "umicom/base/result.h"
#include "umicom/base/memory.h"

#include "umicom/diagnostics/diagnostic.h"
#include "umicom/diagnostics/log.h"
#include "umicom/diagnostics/hub.h"
#include "umicom/diagnostics/store.h"

#include "umicom/messaging/message.h"
#include "umicom/messaging/event_bus.h"
#include "umicom/messaging/command_bus.h"
#include "umicom/messaging/query_bus.h"
#include "umicom/messaging/journal.h"

#include "umicom/data/data_server.h"

#include "umicom/platform/config.h"
#include "umicom/platform/settings.h"
#include "umicom/platform/filesystem.h"
#include "umicom/platform/process.h"
#include "umicom/platform/workspace.h"
#include "umicom/platform/document.h"
#include "umicom/platform/clock.h"

#include "umicom/ui/contracts.h"

#include "umicom/runtime/module.h"
#include "umicom/runtime/module_registry.h"
#include "umicom/runtime/master_controller.h"
#include "umicom/runtime/scheduler.h"

#include "umicom/scaffold/scaffold.h"
#include "umicom/plugin/plugin.h"

#endif

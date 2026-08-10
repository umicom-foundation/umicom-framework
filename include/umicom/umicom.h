/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/umicom.h
 *
 * PURPOSE:
 *   Provide the aggregate public Framework include for applications that use
 *   the complete C23 foundation through the Umicom::Framework target.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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
#include "umicom/messaging/payload.h"
#include "umicom/messaging/schema.h"
#include "umicom/messaging/channel.h"
#include "umicom/messaging/subscription.h"
#include "umicom/messaging/dispatcher.h"
#include "umicom/messaging/reliability.h"
#include "umicom/messaging/replay.h"
#include "umicom/messaging/inbox.h"
#include "umicom/messaging/outbox.h"
#include "umicom/messaging/dead_letter.h"
#include "umicom/messaging/idempotency.h"
#include "umicom/messaging/delivery_lease.h"
#include "umicom/messaging/transport.h"
#include "umicom/messaging/router.h"
#include "umicom/messaging/filter.h"
#include "umicom/messaging/transformer.h"
#include "umicom/messaging/aggregator.h"
#include "umicom/messaging/resequencer.h"
#include "umicom/messaging/flow.h"
#include "umicom/messaging/workflow.h"
#include "umicom/messaging/saga.h"
#include "umicom/messaging/topic.h"
#include "umicom/messaging/metrics.h"

#include "umicom/data/data_server.h"
#include "umicom/data/store.h"
#include "umicom/data/repository.h"
#include "umicom/data/transaction.h"
#include "umicom/data/schema_version.h"
#include "umicom/data/migration.h"
#include "umicom/data/sqlite.h"
#include "umicom/data/statement.h"
#include "umicom/data/row.h"
#include "umicom/data/blob_store.h"
#include "umicom/data/cache.h"
#include "umicom/data/metadata.h"
#include "umicom/data/lineage.h"
#include "umicom/data/backup.h"
#include "umicom/data/integrity.h"
#include "umicom/data/message_store.h"
#include "umicom/data/journal_store.h"

#include "umicom/platform/config.h"
#include "umicom/platform/settings.h"
#include "umicom/platform/path.h"
#include "umicom/platform/filesystem.h"
#include "umicom/platform/directory.h"
#include "umicom/platform/cancellation.h"
#include "umicom/platform/atomic_file.h"
#include "umicom/platform/threading.h"
#include "umicom/platform/task.h"
#include "umicom/platform/task_queue.h"
#include "umicom/platform/document_store.h"
#include "umicom/platform/workspace_graph.h"
#include "umicom/platform/file_index.h"
#include "umicom/platform/search.h"
#include "umicom/platform/session_store.h"
#include "umicom/platform/recovery.h"
#include "umicom/platform/process.h"
#include "umicom/platform/process_supervisor.h"
#include "umicom/platform/watcher.h"
#include "umicom/platform/workspace.h"
#include "umicom/platform/document.h"
#include "umicom/platform/clock.h"

#include "umicom/security/policy.h"
#include "umicom/security/secrets.h"

#include "umicom/ui/contracts.h"

#include "umicom/runtime/capability_registry.h"
#include "umicom/runtime/service_registry.h"
#include "umicom/runtime/command_registry.h"
#include "umicom/runtime/health.h"
#include "umicom/runtime/capability_catalogue.h"
#include "umicom/runtime/application_manifest.h"
#include "umicom/runtime/suite.h"
#include "umicom/runtime/module.h"
#include "umicom/runtime/module_registry.h"
#include "umicom/runtime/master_controller.h"
#include "umicom/runtime/scheduler.h"

#include "umicom/toolchain/tool.h"
#include "umicom/toolchain/profile.h"
#include "umicom/toolchain/discovery.h"
#include "umicom/toolchain/environment.h"
#include "umicom/toolchain/dependency.h"
#include "umicom/toolchain/build.h"

#include "umicom/repository/repository.h"
#include "umicom/scaffold/scaffold.h"
#include "umicom/scaffold/repository.h"
#include "umicom/plugin/plugin.h"

#endif

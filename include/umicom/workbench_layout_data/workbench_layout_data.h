/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_layout_data/workbench_layout_data.h
 *
 * PURPOSE:
 *   Provide the aggregate public include for authoritative layout persistence and synchronisation.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_LAYOUT_DATA_WORKBENCH_LAYOUT_DATA_H
#define UMICOM_WORKBENCH_LAYOUT_DATA_WORKBENCH_LAYOUT_DATA_H

#include "umicom/workbench_layout_data/types.h"
#include "umicom/workbench_layout_data/key_codec.h"
#include "umicom/workbench_layout_data/value_codec.h"
#include "umicom/workbench_layout_data/record_repository.h"
#include "umicom/workbench_layout_data/chunk_store.h"
#include "umicom/workbench_layout_data/document_store.h"
#include "umicom/workbench_layout_data/session_store.h"
#include "umicom/workbench_layout_data/data_server_store.h"
#include "umicom/workbench_layout_data/revision_store.h"
#include "umicom/workbench_layout_data/grant_store.h"
#include "umicom/workbench_layout_data/lease_store.h"
#include "umicom/workbench_layout_data/change_feed.h"
#include "umicom/workbench_layout_data/outbox.h"
#include "umicom/workbench_layout_data/conflict_store.h"
#include "umicom/workbench_layout_data/sync_cursor.h"
#include "umicom/workbench_layout_data/sync_plan.h"
#include "umicom/workbench_layout_data/sync_engine.h"
#include "umicom/workbench_layout_data/offline_queue.h"
#include "umicom/workbench_layout_data/presence_store.h"
#include "umicom/workbench_layout_data/migration_store.h"
#include "umicom/workbench_layout_data/schema_catalogue.h"
#include "umicom/workbench_layout_data/migration_runner.h"
#include "umicom/workbench_layout_data/metrics.h"
#include "umicom/workbench_layout_data/health.h"
#include "umicom/workbench_layout_data/backup.h"
#include "umicom/workbench_layout_data/restore.h"
#include "umicom/workbench_layout_data/reconciliation.h"
#include "umicom/workbench_layout_data/retention.h"
#include "umicom/workbench_layout_data/service.h"
#include "umicom/workbench_layout_data/controller.h"

#endif

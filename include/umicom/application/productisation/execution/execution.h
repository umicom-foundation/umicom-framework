/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/productisation/execution/execution.h
 *
 * PURPOSE:
 *   Aggregate Framework productisation execution control-plane contracts.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_PRODUCTISATION_EXECUTION_EXECUTION_H
#define UMICOM_APPLICATION_PRODUCTISATION_EXECUTION_EXECUTION_H
#include "umicom/application/productisation/execution/types.h"
#include "umicom/application/productisation/execution/work_item.h"
#include "umicom/application/productisation/execution/work_queue.h"
#include "umicom/application/productisation/execution/dependency_graph.h"
#include "umicom/application/productisation/execution/cancellation.h"
#include "umicom/application/productisation/execution/policy.h"
#include "umicom/application/productisation/execution/adapter.h"
#include "umicom/application/productisation/execution/history.h"
#include "umicom/application/productisation/execution/checkpoint.h"
#include "umicom/application/productisation/execution/scheduler.h"
#include "umicom/application/productisation/execution/evidence_bridge.h"
#include "umicom/application/productisation/execution/metrics.h"
#include "umicom/application/productisation/execution/rollback.h"
#include "umicom/application/productisation/execution/acceptance.h"
#include "umicom/application/productisation/execution/snapshot.h"
#include "umicom/application/productisation/execution/helix_bridge.h"
#include "umicom/application/productisation/execution/executor.h"
#include "umicom/application/productisation/execution/runtime.h"
#endif

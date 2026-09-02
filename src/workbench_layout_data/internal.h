/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_layout_data/internal.h
 *
 * PURPOSE:
 *   Provide private helpers shared by the Data Server-backed layout persistence implementation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_LAYOUT_DATA_INTERNAL_H
#define UMICOM_WORKBENCH_LAYOUT_DATA_INTERNAL_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "umicom/data/data_server.h"
#include "umicom/workbench_layout_data/key_codec.h"
#include "umicom/workbench_layout_data/value_codec.h"

/**
 * Represent the workbench layout data visit context data shared with callers of this
 * public contract.
 */
typedef struct UmiWorkbenchLayoutDataVisitContext {
    const char *prefix;
    UmiStatus (*accept)(const char *key, const char *value, void *context);
    void *context;
    size_t visited;
    size_t accepted;
    UmiStatus status;
} UmiWorkbenchLayoutDataVisitContext;

/**
 * Copy workbench layout data store into module-owned storage so callers keep ownership of
 * their input values.
 */
UmiStatus umi_workbench_layout_data_store_set(
    UmiDataServer *server,
    const char *key,
    const char *value);

/**
 * Provide the workbench layout data store get allocated operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_layout_data_store_get_allocated(
    const UmiDataServer *server,
    const char *key,
    char **out_value,
    size_t *out_size);

/**
 * Provide the workbench layout data store delete if present operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_layout_data_store_delete_if_present(
    UmiDataServer *server,
    const char *key);

/**
 * Provide the workbench layout data store visit prefix operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_layout_data_store_visit_prefix(
    const UmiDataServer *server,
    const char *prefix,
    UmiStatus (*accept)(const char *key, const char *value, void *context),
    void *context,
    size_t *out_accepted);

/**
 * Provide the workbench layout data transaction begin operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_layout_data_transaction_begin(
    UmiDataServer *server,
    bool *out_started);

/**
 * Provide the workbench layout data transaction finish operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_layout_data_transaction_finish(
    UmiDataServer *server,
    bool started,
    UmiStatus operation_status);

/**
 * Provide the workbench layout data make record id operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_layout_data_make_record_id(
    const char *prefix,
    const char *aggregate_id,
    uint64_t sequence,
    char *buffer,
    size_t capacity);

#endif

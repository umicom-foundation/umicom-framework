/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_layout_data/internal.h
 *
 * PURPOSE:
 *   Provide private helpers shared by the Data Server-backed layout persistence implementation.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_LAYOUT_DATA_INTERNAL_H
#define UMICOM_WORKBENCH_LAYOUT_DATA_INTERNAL_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "umicom/data/data_server.h"
#include "umicom/workbench_layout_data/key_codec.h"
#include "umicom/workbench_layout_data/value_codec.h"

typedef struct UmiWorkbenchLayoutDataVisitContext {
    const char *prefix;
    UmiStatus (*accept)(const char *key, const char *value, void *context);
    void *context;
    size_t visited;
    size_t accepted;
    UmiStatus status;
} UmiWorkbenchLayoutDataVisitContext;

UmiStatus umi_workbench_layout_data_store_set(
    UmiDataServer *server,
    const char *key,
    const char *value);

UmiStatus umi_workbench_layout_data_store_get_allocated(
    const UmiDataServer *server,
    const char *key,
    char **out_value,
    size_t *out_size);

UmiStatus umi_workbench_layout_data_store_delete_if_present(
    UmiDataServer *server,
    const char *key);

UmiStatus umi_workbench_layout_data_store_visit_prefix(
    const UmiDataServer *server,
    const char *prefix,
    UmiStatus (*accept)(const char *key, const char *value, void *context),
    void *context,
    size_t *out_accepted);

UmiStatus umi_workbench_layout_data_transaction_begin(
    UmiDataServer *server,
    bool *out_started);

UmiStatus umi_workbench_layout_data_transaction_finish(
    UmiDataServer *server,
    bool started,
    UmiStatus operation_status);

UmiStatus umi_workbench_layout_data_make_record_id(
    const char *prefix,
    const char *aggregate_id,
    uint64_t sequence,
    char *buffer,
    size_t capacity);

#endif

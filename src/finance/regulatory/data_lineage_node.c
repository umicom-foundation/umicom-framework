/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/regulatory/data_lineage_node.c
 *
 * PURPOSE:
 *   Represent a versioned node in a regulatory data-lineage graph.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; financial applications remain thin compositions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/regulatory/data_lineage_node.h"

#include <string.h>

/*
 * Initialise reg data lineage node from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_reg_data_lineage_node_init(UmiDataLineageNode *record, const char *node_id, const char *source_id, const char *object_name, uint64_t schema_hash)
{
    UmiStatus status = UMI_STATUS_OK;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (record == NULL || !(schema_hash != 0U)) return UMI_STATUS_INVALID_ARGUMENT;
    memset(record, 0, sizeof *record);
    status = umi_reg_copy_text(record->node_id, sizeof record->node_id, node_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_reg_copy_text(record->source_id, sizeof record->source_id, source_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_reg_copy_text(record->object_name, sizeof record->object_name, object_name);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    record->schema_hash = schema_hash;
    return UMI_STATUS_OK;
}

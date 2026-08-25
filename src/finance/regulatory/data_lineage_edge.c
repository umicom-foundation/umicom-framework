/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/regulatory/data_lineage_edge.c
 *
 * PURPOSE:
 *   Represent a directed transformation edge in a data-lineage graph.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; financial applications remain thin compositions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/regulatory/data_lineage_edge.h"

#include <string.h>

UmiStatus umi_reg_data_lineage_edge_init(UmiDataLineageEdge *record, const char *from_node, const char *to_node, const char *transform, uint64_t logic_hash)
{
    UmiStatus status = UMI_STATUS_OK;
    if (record == NULL || !(logic_hash != 0U)) return UMI_STATUS_INVALID_ARGUMENT;
    memset(record, 0, sizeof *record);
    status = umi_reg_copy_text(record->from_node, sizeof record->from_node, from_node);
    if (status != UMI_STATUS_OK) return status;
    status = umi_reg_copy_text(record->to_node, sizeof record->to_node, to_node);
    if (status != UMI_STATUS_OK) return status;
    status = umi_reg_copy_text(record->transform, sizeof record->transform, transform);
    if (status != UMI_STATUS_OK) return status;
    record->logic_hash = logic_hash;
    return UMI_STATUS_OK;
}

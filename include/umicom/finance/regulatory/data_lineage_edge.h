/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/regulatory/data_lineage_edge.h
 *
 * PURPOSE:
 *   Represent a directed transformation edge in a data-lineage graph.
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

#ifndef INCLUDE_UMICOM_FINANCE_REGULATORY_DATA_LINEAGE_EDGE_H
#define INCLUDE_UMICOM_FINANCE_REGULATORY_DATA_LINEAGE_EDGE_H

#include "umicom/finance/regulatory/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the data lineage edge data shared with callers of this public contract.
 */
typedef struct UmiDataLineageEdge {
    char from_node[UMI_REG_ID_CAPACITY];
    char to_node[UMI_REG_ID_CAPACITY];
    char transform[UMI_REG_NAME_CAPACITY];
    uint64_t logic_hash;
} UmiDataLineageEdge;

/* Initialise one validated represent a directed transformation edge in a data-lineage graph. record. */
UmiStatus umi_reg_data_lineage_edge_init(UmiDataLineageEdge *record, const char *from_node, const char *to_node, const char *transform, uint64_t logic_hash);

#ifdef __cplusplus
}
#endif

#endif

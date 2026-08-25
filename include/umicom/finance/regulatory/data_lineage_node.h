/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/regulatory/data_lineage_node.h
 *
 * PURPOSE:
 *   Represent a versioned node in a regulatory data-lineage graph.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; financial applications remain thin compositions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_FINANCE_REGULATORY_DATA_LINEAGE_NODE_H
#define INCLUDE_UMICOM_FINANCE_REGULATORY_DATA_LINEAGE_NODE_H

#include "umicom/finance/regulatory/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDataLineageNode {
    char node_id[UMI_REG_ID_CAPACITY];
    char source_id[UMI_REG_ID_CAPACITY];
    char object_name[UMI_REG_NAME_CAPACITY];
    uint64_t schema_hash;
} UmiDataLineageNode;

/* Initialise one validated represent a versioned node in a regulatory data-lineage graph. record. */
UmiStatus umi_reg_data_lineage_node_init(UmiDataLineageNode *record, const char *node_id, const char *source_id, const char *object_name, uint64_t schema_hash);

#ifdef __cplusplus
}
#endif

#endif

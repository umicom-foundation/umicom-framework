/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/data/enterprise/schema_index.h
 *
 * PURPOSE:
 *   Describe an index and its uniqueness/coverage characteristics for compatibility and planning.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DATA_ENTERPRISE_SCHEMA_INDEX_H
#define UMICOM_DATA_ENTERPRISE_SCHEMA_INDEX_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"
#include "umicom/data/enterprise/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the data schema index data shared with callers of this public contract.
 */
typedef struct UmiDataSchemaIndex {
    char index_id[UMI_DATA_ENTERPRISE_ID_CAPACITY];
    char table_id[UMI_DATA_ENTERPRISE_ID_CAPACITY];
    char key_expression[UMI_DATA_ENTERPRISE_TEXT_CAPACITY];
    bool unique;
    bool covering;
} UmiDataSchemaIndex;

/* Initialise a validated schema index descriptor. */
UmiStatus umi_data_schema_index_init(UmiDataSchemaIndex *item, const char *index_id, const char *table_id, const char *key_expression, bool unique);
/* Validate invariants before the descriptor is admitted to a catalogue or plan. */
UmiStatus umi_data_schema_index_validate(const UmiDataSchemaIndex *item);

#ifdef __cplusplus
}
#endif
#endif

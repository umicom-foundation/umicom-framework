/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/data/enterprise/schema_diff.h
 *
 * PURPOSE:
 *   Compare two schema snapshots and classify table additions, removals and structural changes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DATA_ENTERPRISE_SCHEMA_DIFF_H
#define UMICOM_DATA_ENTERPRISE_SCHEMA_DIFF_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"
#include "umicom/data/enterprise/types.h"
#include "umicom/data/enterprise/schema_snapshot.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDataSchemaDiff {
    size_t added_tables;
    size_t removed_tables;
    size_t changed_tables;
    size_t unchanged_tables;
    UmiDataCompatibility compatibility;
} UmiDataSchemaDiff;
/* Compare schema snapshots using stable table/column identities. */
UmiStatus umi_data_schema_diff_compute(const UmiDataSchemaSnapshot *before, const UmiDataSchemaSnapshot *after, UmiDataSchemaDiff *out_diff);

#ifdef __cplusplus
}
#endif
#endif

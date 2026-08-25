/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/data/enterprise/schema_snapshot.h
 *
 * PURPOSE:
 *   Capture an immutable-by-value schema snapshot for comparison, migration and compatibility checks.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DATA_ENTERPRISE_SCHEMA_SNAPSHOT_H
#define UMICOM_DATA_ENTERPRISE_SCHEMA_SNAPSHOT_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"
#include "umicom/data/enterprise/types.h"
#include "umicom/data/enterprise/schema_table.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDataSchemaSnapshot {
    UmiDataSchemaTable items[UMI_DATA_ENTERPRISE_MAX_ITEMS];
    size_t count;
    uint64_t revision;
} UmiDataSchemaSnapshot;

/* Reset the bounded schema snapshot registry. */
void umi_data_schema_snapshot_init(UmiDataSchemaSnapshot *registry);
/* Add a unique item by stable identifier. */
UmiStatus umi_data_schema_snapshot_add(UmiDataSchemaSnapshot *registry, const UmiDataSchemaTable *item);
/* Find a registry item by stable identifier. */
UmiStatus umi_data_schema_snapshot_find(const UmiDataSchemaSnapshot *registry, const char *id, UmiDataSchemaTable *out_item);
/* Return the number of registered items. */
size_t umi_data_schema_snapshot_count(const UmiDataSchemaSnapshot *registry);

#ifdef __cplusplus
}
#endif
#endif

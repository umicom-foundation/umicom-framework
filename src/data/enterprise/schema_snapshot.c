/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/data/enterprise/schema_snapshot.c
 *
 * PURPOSE:
 *   Capture an immutable-by-value schema snapshot for comparison, migration and compatibility checks.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/enterprise/schema_snapshot.h"
#include <string.h>

/* Registry initialisation is deterministic and allocation-free. */
void umi_data_schema_snapshot_init(UmiDataSchemaSnapshot *registry) { if (registry != NULL) (void)memset(registry, 0, sizeof(*registry)); }

/* Duplicate identifiers are rejected to keep mapping semantics unambiguous. */
UmiStatus umi_data_schema_snapshot_add(UmiDataSchemaSnapshot *registry, const UmiDataSchemaTable *item) {
    size_t i;
    if (registry == NULL || item == NULL || item->table_id[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    for (i = 0U; i < registry->count; ++i) if (strcmp(registry->items[i].table_id, item->table_id) == 0) return UMI_STATUS_ALREADY_EXISTS;
    if (registry->count >= UMI_DATA_ENTERPRISE_MAX_ITEMS) return UMI_STATUS_CAPACITY_EXCEEDED;
    registry->items[registry->count++] = *item; registry->revision++;
    return UMI_STATUS_OK;
}

/* Lookup copies a snapshot so callers never borrow internal registry storage. */
UmiStatus umi_data_schema_snapshot_find(const UmiDataSchemaSnapshot *registry, const char *id, UmiDataSchemaTable *out_item) {
    size_t i;
    if (registry == NULL || id == NULL || out_item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    for (i = 0U; i < registry->count; ++i) if (strcmp(registry->items[i].table_id, id) == 0) { *out_item = registry->items[i]; return UMI_STATUS_OK; }
    return UMI_STATUS_NOT_FOUND;
}

/* Count exposes registry size without exposing mutable storage. */
size_t umi_data_schema_snapshot_count(const UmiDataSchemaSnapshot *registry) { return registry != NULL ? registry->count : 0U; }

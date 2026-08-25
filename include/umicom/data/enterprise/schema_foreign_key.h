/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/data/enterprise/schema_foreign_key.h
 *
 * PURPOSE:
 *   Describe referential constraints in a backend-neutral form for migration ordering and ORM relations.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DATA_ENTERPRISE_SCHEMA_FOREIGN_KEY_H
#define UMICOM_DATA_ENTERPRISE_SCHEMA_FOREIGN_KEY_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"
#include "umicom/data/enterprise/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDataSchemaForeignKey {
    char constraint_id[UMI_DATA_ENTERPRISE_ID_CAPACITY];
    char source_table[UMI_DATA_ENTERPRISE_ID_CAPACITY];
    char target_table[UMI_DATA_ENTERPRISE_ID_CAPACITY];
    char source_column[UMI_DATA_ENTERPRISE_ID_CAPACITY];
    char target_column[UMI_DATA_ENTERPRISE_ID_CAPACITY];
    bool cascade_delete;
} UmiDataSchemaForeignKey;

/* Initialise a validated schema foreign key descriptor. */
UmiStatus umi_data_schema_foreign_key_init(UmiDataSchemaForeignKey *item, const char *constraint_id, const char *source_table, const char *source_column, const char *target_table, const char *target_column);
/* Validate invariants before the descriptor is admitted to a catalogue or plan. */
UmiStatus umi_data_schema_foreign_key_validate(const UmiDataSchemaForeignKey *item);

#ifdef __cplusplus
}
#endif
#endif

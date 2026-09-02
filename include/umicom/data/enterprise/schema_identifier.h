/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/data/enterprise/schema_identifier.h
 *
 * PURPOSE:
 *   Represent a qualified schema object identifier without binding to a specific SQL engine.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DATA_ENTERPRISE_SCHEMA_IDENTIFIER_H
#define UMICOM_DATA_ENTERPRISE_SCHEMA_IDENTIFIER_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"
#include "umicom/data/enterprise/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the data schema identifier data shared with callers of this public contract.
 */
typedef struct UmiDataSchemaIdentifier {
    char catalog[UMI_DATA_ENTERPRISE_ID_CAPACITY];
    char schema[UMI_DATA_ENTERPRISE_ID_CAPACITY];
    char name[UMI_DATA_ENTERPRISE_ID_CAPACITY];
} UmiDataSchemaIdentifier;

/* Initialise a validated schema identifier descriptor. */
UmiStatus umi_data_schema_identifier_init(UmiDataSchemaIdentifier *item, const char *catalog, const char *schema, const char *name);
/* Validate invariants before the descriptor is admitted to a catalogue or plan. */
UmiStatus umi_data_schema_identifier_validate(const UmiDataSchemaIdentifier *item);

#ifdef __cplusplus
}
#endif
#endif

/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/data/enterprise/field_mapping.h
 *
 * PURPOSE:
 *   Map one logical entity field to portable column metadata and conversion semantics.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DATA_ENTERPRISE_FIELD_MAPPING_H
#define UMICOM_DATA_ENTERPRISE_FIELD_MAPPING_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"
#include "umicom/data/enterprise/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the data field mapping data shared with callers of this public contract.
 */
typedef struct UmiDataFieldMapping {
    char mapping_id[UMI_DATA_ENTERPRISE_ID_CAPACITY];
    char entity_id[UMI_DATA_ENTERPRISE_ID_CAPACITY];
    char field_name[UMI_DATA_ENTERPRISE_ID_CAPACITY];
    char column_name[UMI_DATA_ENTERPRISE_ID_CAPACITY];
    UmiDataValueKind kind;
    bool nullable;
} UmiDataFieldMapping;

/* Initialise a validated field mapping descriptor. */
UmiStatus umi_data_field_mapping_init(UmiDataFieldMapping *item, const char *mapping_id, const char *entity_id, const char *field_name, const char *column_name, UmiDataValueKind kind);
/* Validate invariants before the descriptor is admitted to a catalogue or plan. */
UmiStatus umi_data_field_mapping_validate(const UmiDataFieldMapping *item);

#ifdef __cplusplus
}
#endif
#endif

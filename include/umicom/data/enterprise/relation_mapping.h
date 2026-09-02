/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/data/enterprise/relation_mapping.h
 *
 * PURPOSE:
 *   Describe entity relations independently from SQL foreign-key syntax.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DATA_ENTERPRISE_RELATION_MAPPING_H
#define UMICOM_DATA_ENTERPRISE_RELATION_MAPPING_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"
#include "umicom/data/enterprise/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the data relation mapping data shared with callers of this public contract.
 */
typedef struct UmiDataRelationMapping {
    char relation_id[UMI_DATA_ENTERPRISE_ID_CAPACITY];
    char source_entity[UMI_DATA_ENTERPRISE_ID_CAPACITY];
    char target_entity[UMI_DATA_ENTERPRISE_ID_CAPACITY];
    char source_field[UMI_DATA_ENTERPRISE_ID_CAPACITY];
    bool collection;
    bool required;
} UmiDataRelationMapping;

/* Initialise a validated relation mapping descriptor. */
UmiStatus umi_data_relation_mapping_init(UmiDataRelationMapping *item, const char *relation_id, const char *source_entity, const char *target_entity, const char *source_field, bool collection);
/* Validate invariants before the descriptor is admitted to a catalogue or plan. */
UmiStatus umi_data_relation_mapping_validate(const UmiDataRelationMapping *item);

#ifdef __cplusplus
}
#endif
#endif

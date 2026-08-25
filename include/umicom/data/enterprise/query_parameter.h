/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/data/enterprise/query_parameter.h
 *
 * PURPOSE:
 *   Represent a typed bound query parameter without embedding values in generated SQL.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DATA_ENTERPRISE_QUERY_PARAMETER_H
#define UMICOM_DATA_ENTERPRISE_QUERY_PARAMETER_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"
#include "umicom/data/enterprise/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDataQueryParameter {
    char parameter_id[UMI_DATA_ENTERPRISE_ID_CAPACITY];
    char name[UMI_DATA_ENTERPRISE_ID_CAPACITY];
    UmiDataValueKind kind;
    char value[UMI_DATA_ENTERPRISE_TEXT_CAPACITY];
    bool sensitive;
} UmiDataQueryParameter;

/* Initialise a validated query parameter descriptor. */
UmiStatus umi_data_query_parameter_init(UmiDataQueryParameter *item, const char *parameter_id, const char *name, UmiDataValueKind kind, const char *value, bool sensitive);
/* Validate invariants before the descriptor is admitted to a catalogue or plan. */
UmiStatus umi_data_query_parameter_validate(const UmiDataQueryParameter *item);

#ifdef __cplusplus
}
#endif
#endif

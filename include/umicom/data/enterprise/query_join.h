/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/data/enterprise/query_join.h
 *
 * PURPOSE:
 *   Describe backend-neutral joins for cost analysis and SQL generation.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DATA_ENTERPRISE_QUERY_JOIN_H
#define UMICOM_DATA_ENTERPRISE_QUERY_JOIN_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"
#include "umicom/data/enterprise/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDataQueryJoin {
    char join_id[UMI_DATA_ENTERPRISE_ID_CAPACITY];
    char left_table[UMI_DATA_ENTERPRISE_ID_CAPACITY];
    char right_table[UMI_DATA_ENTERPRISE_ID_CAPACITY];
    char condition[UMI_DATA_ENTERPRISE_TEXT_CAPACITY];
    bool outer_join;
} UmiDataQueryJoin;

/* Initialise a validated query join descriptor. */
UmiStatus umi_data_query_join_init(UmiDataQueryJoin *item, const char *join_id, const char *left_table, const char *right_table, const char *condition, bool outer_join);
/* Validate invariants before the descriptor is admitted to a catalogue or plan. */
UmiStatus umi_data_query_join_validate(const UmiDataQueryJoin *item);

#ifdef __cplusplus
}
#endif
#endif

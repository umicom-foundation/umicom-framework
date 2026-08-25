/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/data/enterprise/statement_plan.h
 *
 * PURPOSE:
 *   Describe a prepared statement contract and its query/schema fingerprints.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DATA_ENTERPRISE_STATEMENT_PLAN_H
#define UMICOM_DATA_ENTERPRISE_STATEMENT_PLAN_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"
#include "umicom/data/enterprise/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDataStatementPlan {
    char statement_id[UMI_DATA_ENTERPRISE_ID_CAPACITY];
    uint64_t query_fingerprint;
    uint64_t schema_fingerprint;
    size_t parameter_count;
    bool read_only;
} UmiDataStatementPlan;

/* Initialise a validated statement plan descriptor. */
UmiStatus umi_data_statement_plan_init(UmiDataStatementPlan *item, const char *statement_id, uint64_t query_fingerprint, uint64_t schema_fingerprint, size_t parameter_count, bool read_only);
/* Validate invariants before the descriptor is admitted to a catalogue or plan. */
UmiStatus umi_data_statement_plan_validate(const UmiDataStatementPlan *item);

#ifdef __cplusplus
}
#endif
#endif

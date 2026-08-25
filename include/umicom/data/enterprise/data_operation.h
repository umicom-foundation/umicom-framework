/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/data/enterprise/data_operation.h
 *
 * PURPOSE:
 *   Describe one reviewable Data Server operation for queueing, audit and cancellation.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DATA_ENTERPRISE_DATA_OPERATION_H
#define UMICOM_DATA_ENTERPRISE_DATA_OPERATION_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"
#include "umicom/data/enterprise/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDataOperation {
    char operation_id[UMI_DATA_ENTERPRISE_ID_CAPACITY];
    char session_id[UMI_DATA_ENTERPRISE_ID_CAPACITY];
    char operation_kind[64];
    uint64_t submitted_at;
    uint32_t priority;
    bool cancellable;
} UmiDataOperation;

/* Initialise a validated data operation descriptor. */
UmiStatus umi_data_data_operation_init(UmiDataOperation *item, const char *operation_id, const char *session_id, const char *operation_kind, uint64_t submitted_at, uint32_t priority);
/* Validate invariants before the descriptor is admitted to a catalogue or plan. */
UmiStatus umi_data_data_operation_validate(const UmiDataOperation *item);

#ifdef __cplusplus
}
#endif
#endif

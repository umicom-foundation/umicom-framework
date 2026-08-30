/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/data/enterprise/connection_slot.h
 *
 * PURPOSE:
 *   Describe one pooled backend connection lease slot without owning the backend handle.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DATA_ENTERPRISE_CONNECTION_SLOT_H
#define UMICOM_DATA_ENTERPRISE_CONNECTION_SLOT_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"
#include "umicom/data/enterprise/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDataConnectionSlot {
    char slot_id[UMI_DATA_ENTERPRISE_ID_CAPACITY];
    uint64_t connection_token;
    uint64_t last_used_at;
    uint32_t lease_count;
    bool healthy;
    bool leased;
} UmiDataConnectionSlot;

/* Initialise a validated connection slot descriptor. */
UmiStatus umi_data_connection_slot_init(UmiDataConnectionSlot *item, const char *slot_id, uint64_t connection_token);
/* Validate invariants before the descriptor is admitted to a catalogue or plan. */
UmiStatus umi_data_connection_slot_validate(const UmiDataConnectionSlot *item);

#ifdef __cplusplus
}
#endif
#endif

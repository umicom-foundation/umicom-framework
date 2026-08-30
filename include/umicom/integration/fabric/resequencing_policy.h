/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/integration/fabric/resequencing_policy.h
 *
 * PURPOSE:
 *   Describe ordering window and gap handling for out-of-order integration messages.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_INTEGRATION_FABRIC_RESEQUENCING_POLICY_H
#define UMICOM_INTEGRATION_FABRIC_RESEQUENCING_POLICY_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"
#include "umicom/integration/fabric/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiFabricResequencingPolicy {
    char policy_id[UMI_FABRIC_ID_CAPACITY];
    size_t maximum_buffered;
    uint64_t gap_timeout_ms;
    bool release_on_timeout;
} UmiFabricResequencingPolicy;

UmiStatus umi_fabric_resequencing_policy_init(UmiFabricResequencingPolicy *item, const char *policy_id, size_t maximum_buffered, uint64_t gap_timeout_ms, bool release_on_timeout);
UmiStatus umi_fabric_resequencing_policy_validate(const UmiFabricResequencingPolicy *item);

#ifdef __cplusplus
}
#endif
#endif

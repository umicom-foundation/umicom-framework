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

/**
 * Represent the fabric resequencing policy data shared with callers of this public
 * contract.
 */
typedef struct UmiFabricResequencingPolicy {
    char policy_id[UMI_FABRIC_ID_CAPACITY];
    size_t maximum_buffered;
    uint64_t gap_timeout_ms;
    bool release_on_timeout;
} UmiFabricResequencingPolicy;

/**
 * Initialise fabric resequencing policy from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_fabric_resequencing_policy_init(UmiFabricResequencingPolicy *item, const char *policy_id, size_t maximum_buffered, uint64_t gap_timeout_ms, bool release_on_timeout);
/**
 * Check that fabric resequencing policy satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_fabric_resequencing_policy_validate(const UmiFabricResequencingPolicy *item);

#ifdef __cplusplus
}
#endif
#endif

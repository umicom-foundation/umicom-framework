/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_host/delivery_policy.h
 *
 * PURPOSE:
 *   Define bounded delivery pressure and replacement policy independent of frontend toolkits.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_HOST_DELIVERY_POLICY_H
#define UMICOM_WORKBENCH_CONTEXT_HOST_DELIVERY_POLICY_H
#include "umicom/workbench_context_host/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * List the named workbench context host overflow mode values accepted by this public
 * contract.
 */
typedef enum UmiWorkbenchContextHostOverflowMode {
    UMI_WORKBENCH_CONTEXT_HOST_OVERFLOW_DROP_OLDEST=1,
    UMI_WORKBENCH_CONTEXT_HOST_OVERFLOW_DROP_NEWEST=2,
    UMI_WORKBENCH_CONTEXT_HOST_OVERFLOW_REJECT=3
} UmiWorkbenchContextHostOverflowMode;
/**
 * Represent the workbench context host delivery policy data shared with callers of this
 * public contract.
 */
typedef struct UmiWorkbenchContextHostDeliveryPolicy {
    size_t max_pending_per_endpoint;
    UmiWorkbenchContextHostOverflowMode overflow_mode;
    bool coalesce_same_kind;
    bool coalesce_same_context;
    bool reject_expired;
    uint64_t revision;
} UmiWorkbenchContextHostDeliveryPolicy;
/**
 * Provide the workbench context host delivery policy default operation used by this module
 * and its client applications.
 */
UmiWorkbenchContextHostDeliveryPolicy umi_workbench_context_host_delivery_policy_default(void);
/**
 * Check that workbench context host delivery policy satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_workbench_context_host_delivery_policy_validate(
    const UmiWorkbenchContextHostDeliveryPolicy *policy);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_workbench_context_host_delivery_policy_archive_encode(const UmiWorkbenchContextHostDeliveryPolicy *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_workbench_context_host_delivery_policy_archive_decode(const void *bytes, size_t byte_count,
    UmiWorkbenchContextHostDeliveryPolicy *value);

#ifdef __cplusplus
}
#endif
#endif

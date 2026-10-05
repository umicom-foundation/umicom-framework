/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/integration/fabric/dead_letter_policy.h
 *
 * PURPOSE:
 *   Describe bounded dead-letter escalation and retention rules.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_INTEGRATION_FABRIC_DEAD_LETTER_POLICY_H
#define UMICOM_INTEGRATION_FABRIC_DEAD_LETTER_POLICY_H

#include <stddef.h>
#include "umicom/base/value_archive.h"
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"
#include "umicom/integration/fabric/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the fabric dead letter policy data shared with callers of this public
 * contract.
 */
typedef struct UmiFabricDeadLetterPolicy {
    char policy_id[UMI_FABRIC_ID_CAPACITY];
    char destination[UMI_FABRIC_URI_CAPACITY];
    uint32_t after_attempts;
    uint64_t retention_ms;
    bool include_payload;
} UmiFabricDeadLetterPolicy;

/**
 * Initialise fabric dead letter policy from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_fabric_dead_letter_policy_init(UmiFabricDeadLetterPolicy *item, const char *policy_id, const char *destination, uint32_t after_attempts, uint64_t retention_ms, bool include_payload);
/**
 * Check that fabric dead letter policy satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_fabric_dead_letter_policy_validate(const UmiFabricDeadLetterPolicy *item);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_fabric_dead_letter_policy_archive_encode(const UmiFabricDeadLetterPolicy *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_fabric_dead_letter_policy_archive_decode(const void *bytes, size_t byte_count,
    UmiFabricDeadLetterPolicy *value);

#ifdef __cplusplus
}
#endif
#endif

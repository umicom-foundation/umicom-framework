/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/conformance/event_contract.h
 *
 * PURPOSE:
 *   semantic user-event support requirements independent of native toolkit event classes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FRONTEND_CONFORMANCE_EVENT_CONTRACT_H
#define UMICOM_FRONTEND_CONFORMANCE_EVENT_CONTRACT_H

#include <stddef.h>
#include "umicom/base/value_archive.h"
#include <stdint.h>
#include <stdbool.h>
#include "umicom/frontend/conformance/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the fc event contract data shared with callers of this public contract.
 */
typedef struct UmiFcEventContract { uint64_t required_families; bool ordered; bool cancellable; } UmiFcEventContract;
/**
 * Check that fc event contract satisfies its contract before another service relies on it.
 */
bool umi_fc_event_contract_validate(const UmiFcEventContract *item);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_fc_event_contract_archive_encode(const UmiFcEventContract *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_fc_event_contract_archive_decode(const void *bytes, size_t byte_count,
    UmiFcEventContract *value);

#ifdef __cplusplus
}
#endif
#endif

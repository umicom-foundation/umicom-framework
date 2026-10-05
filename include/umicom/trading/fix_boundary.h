/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/fix_boundary.h
 *
 * PURPOSE:
 *   Validate minimal FIX-style sequence and session identifiers without binding to a specific FIX engine.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This public contract isolates fix boundary behaviour so Studio, Trader, TMS, tests and broker adapters can reuse the same rule.
 */

#ifndef INCLUDE_UMICOM_TRADING_FIX_BOUNDARY_H
#define INCLUDE_UMICOM_TRADING_FIX_BOUNDARY_H
#include <stddef.h>
#include "umicom/base/value_archive.h"
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/trading/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the fix session info data shared with callers of this public contract.
 */
typedef struct UmiFixSessionInfo { char sender_comp_id[32]; char target_comp_id[32]; uint64_t next_out_sequence; uint64_t next_in_sequence; } UmiFixSessionInfo;
/**
 * Check that fix session info satisfies its contract before another service relies on it.
 */
int umi_fix_session_info_valid(const UmiFixSessionInfo *session);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_fix_session_info_archive_encode(const UmiFixSessionInfo *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_fix_session_info_archive_decode(const void *bytes, size_t byte_count,
    UmiFixSessionInfo *value);

#ifdef __cplusplus
}
#endif
#endif

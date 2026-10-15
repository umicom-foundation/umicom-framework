/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/ibkr_boundary.h
 *
 * PURPOSE:
 *   Validate Interactive Brokers connection settings without exposing vendor SDK types.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This public contract isolates ibkr boundary behaviour so Studio, Trader, TMS, tests and broker adapters can reuse the same rule.
 */

#ifndef INCLUDE_UMICOM_TRADING_IBKR_BOUNDARY_H
#define INCLUDE_UMICOM_TRADING_IBKR_BOUNDARY_H
#include <stddef.h>
#include "umicom/base/value_archive.h"
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/trading/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the ibkr connection settings data shared with callers of this public contract.
 */
typedef struct UmiIbkrConnectionSettings { char host[64]; uint16_t port; int32_t client_id; UmiTradingEnvironment environment; } UmiIbkrConnectionSettings;
/**
 * Check that ibkr settings satisfies its contract before another service relies on it.
 */
int umi_ibkr_settings_valid(const UmiIbkrConnectionSettings *settings);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_ibkr_settings_archive_encode(const UmiIbkrConnectionSettings *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_ibkr_settings_archive_decode(const void *bytes, size_t byte_count,
    UmiIbkrConnectionSettings *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_ibkr_settings_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiIbkrConnectionSettings *expected, UmiIbkrConnectionSettings *value);


#ifdef __cplusplus
}
#endif
#endif

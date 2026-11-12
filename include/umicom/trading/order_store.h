/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/order_store.h
 *
 * PURPOSE:
 *   Store a bounded set of orders for deterministic simulations and tests.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This public contract isolates order store behaviour so Studio, Trader, TMS, tests and broker adapters can reuse the same rule.
 */

#ifndef INCLUDE_UMICOM_TRADING_ORDER_STORE_H
#define INCLUDE_UMICOM_TRADING_ORDER_STORE_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/trading/types.h"
#include "umicom/trading/order_state.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the order store data shared with callers of this public contract.
 */
typedef struct UmiOrderStore { UmiOrder orders[UMI_TRADING_MAX_ORDERS]; size_t count; } UmiOrderStore;
/**
 * Initialise order store from caller-provided values so later operations receive a known
 * state.
 */
void umi_order_store_init(UmiOrderStore *store);

/** Admit count, terminated nonempty active IDs and uniqueness. Inactive rows are
 * ignored. This validates storage identity, not economic or lifecycle policy.
 * NULL returns INVALID_ARGUMENT; malformed active storage is INVALID_STATE. */
UmiStatus umi_order_store_validate(const UmiOrderStore *store);

/** Store calls require complete aligned records and serialized owner access.
 * Add leaves the store unchanged on refusal, including borrowed input.
 * A valid full store reports an existing ID before capacity exhaustion.
 * Find scans no more than sizeof(UmiFinancialId.value) query bytes; the caller
 * supplies readable storage through a terminator or that full capacity.
 * Empty queries are INVALID_ARGUMENT; missing termination is CAPACITY_EXCEEDED.
 * For non-NULL arguments and separate output storage, lookup failure clears
 * the output pointer. Missing arguments or output overlapping the store are
 * INVALID_ARGUMENT and leave output untouched. Queries may borrow separate
 * output storage because query capture precedes pointer publication.
 * Returned pointers borrow the store lifetime. Do not change IDs through the
 * mutable pointer; subsequent calls reject duplicate or malformed identities. */
UmiStatus umi_order_store_find_const(const UmiOrderStore *store,
    const char *client_order_id, const UmiOrder **out_order);
/**
 * Add order store only after its inputs and available capacity have been checked.
 */
UmiStatus umi_order_store_add(UmiOrderStore *store, const UmiOrder *order);
/**
 * Find order store while leaving the underlying catalogue or model owned by this module.
 */
UmiStatus umi_order_store_find(UmiOrderStore *store, const char *client_order_id, UmiOrder **out_order);
#ifdef __cplusplus
}
#endif
#endif

/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_delivery_lease_ticket.c
 *
 * PURPOSE:
 *   Verify stale delivery tickets cannot mutate a later acquisition, including when an owner name is reused.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "support/assertions.h"
#include "umicom/messaging/delivery_lease.h"

#include <assert.h>
#include <stdint.h>
#include <string.h>

/* Compare defined fields, never padding bytes in a public value type. */
static void expect_ticket(UmiDeliveryLeaseTicket actual,
                          UmiDeliveryLeaseTicket expected)
{
    assert(actual.message_id == expected.message_id);
    assert(actual.generation == expected.generation);
}

static UmiDeliveryLeaseStore *create_store(size_t capacity)
{
    UmiDeliveryLeaseStore *store = NULL;
    assert(umi_delivery_lease_store_create(capacity, &store) == UMI_STATUS_OK);
    return store;
}

static void test_restarted_owner(void)
{
    UmiDeliveryLeaseStore *store = create_store(1U);
    UmiDeliveryLeaseTicket first = {0U, 0U};
    UmiDeliveryLeaseTicket second = {0U, 0U};
    assert(umi_delivery_lease_acquire_ticket(store, 42U, "worker", 10U, 5U,
                                            &first) == UMI_STATUS_OK);
    assert(umi_delivery_lease_ticket_is_owned(store, first, "worker", 14U));
    assert(!umi_delivery_lease_ticket_is_owned(store, first, "worker", 15U));
    assert(umi_delivery_lease_acquire_ticket(store, 42U, "worker", 15U, 20U,
                                            &second) == UMI_STATUS_OK);
    assert(second.generation > first.generation);
    assert(second.message_id == first.message_id);
    assert(umi_delivery_lease_renew_ticket(store, first, "worker", 16U, 100U) ==
           UMI_STATUS_PERMISSION_DENIED);
    assert(umi_delivery_lease_release_ticket(store, first, "worker") ==
           UMI_STATUS_PERMISSION_DENIED);
    assert(!umi_delivery_lease_ticket_is_owned(store, first, "worker", 16U));
    assert(umi_delivery_lease_ticket_is_owned(store, second, "worker", 34U));
    assert(!umi_delivery_lease_ticket_is_owned(store, second, "worker", 35U));
    umi_delivery_lease_store_destroy(store);
}

static void test_release_and_reacquire(void)
{
    UmiDeliveryLeaseStore *store = create_store(1U);
    UmiDeliveryLeaseTicket first = {0U, 0U}, second = {0U, 0U};
    assert(umi_delivery_lease_acquire_ticket(store, 1U, "a", 0U, 50U, &first) ==
           UMI_STATUS_OK);
    assert(umi_delivery_lease_release_ticket(store, first, "a") == UMI_STATUS_OK);
    assert(umi_delivery_lease_release_ticket(store, first, "a") == UMI_STATUS_NOT_FOUND);
    assert(umi_delivery_lease_acquire_ticket(store, 1U, "a", 1U, 50U, &second) ==
           UMI_STATUS_OK);
    assert(second.generation > first.generation);
    assert(umi_delivery_lease_release_ticket(store, first, "a") ==
           UMI_STATUS_PERMISSION_DENIED);
    assert(umi_delivery_lease_ticket_is_owned(store, second, "a", 2U));
    umi_delivery_lease_store_destroy(store);
}

static void test_slot_recycling(void)
{
    UmiDeliveryLeaseStore *store = create_store(1U);
    UmiDeliveryLeaseTicket first = {0U, 0U}, other = {0U, 0U}, again = {0U, 0U};
    assert(umi_delivery_lease_acquire_ticket(store, 1U, "a", 0U, 1U, &first) ==
           UMI_STATUS_OK);
    assert(umi_delivery_lease_acquire_ticket(store, 2U, "b", 1U, 1U, &other) ==
           UMI_STATUS_OK);
    assert(umi_delivery_lease_release_ticket(store, first, "a") == UMI_STATUS_NOT_FOUND);
    assert(umi_delivery_lease_acquire_ticket(store, 1U, "a", 2U, 10U, &again) ==
           UMI_STATUS_OK);
    assert(again.generation > other.generation);
    assert(other.generation > first.generation);
    assert(umi_delivery_lease_release_ticket(store, first, "a") ==
           UMI_STATUS_PERMISSION_DENIED);
    assert(umi_delivery_lease_ticket_is_owned(store, again, "a", 3U));
    umi_delivery_lease_store_destroy(store);
}

static void test_busy_preserves_output(void)
{
    UmiDeliveryLeaseStore *store = create_store(1U);
    UmiDeliveryLeaseTicket live = {0U, 0U};
    const UmiDeliveryLeaseTicket sentinel = {999U, 888U};
    UmiDeliveryLeaseTicket output = sentinel;
    assert(umi_delivery_lease_acquire_ticket(store, 1U, "a", 10U, 10U, &live) ==
           UMI_STATUS_OK);
    assert(umi_delivery_lease_acquire_ticket(store, 1U, "a", 11U, 100U, &output) ==
           UMI_STATUS_BUSY);
    expect_ticket(output, sentinel);
    assert(umi_delivery_lease_acquire_ticket(store, 1U, "b", 11U, 100U, &output) ==
           UMI_STATUS_BUSY);
    expect_ticket(output, sentinel);
    assert(umi_delivery_lease_acquire_ticket(store, 2U, "a", 11U, 100U, &output) ==
           UMI_STATUS_CAPACITY_EXCEEDED);
    expect_ticket(output, sentinel);
    assert(umi_delivery_lease_ticket_is_owned(store, live, "a", 19U));
    assert(!umi_delivery_lease_ticket_is_owned(store, live, "a", 20U));
    umi_delivery_lease_store_destroy(store);
}

static void test_legacy_interoperation(void)
{
    UmiDeliveryLeaseStore *store = create_store(1U);
    UmiDeliveryLeaseTicket ticket = {0U, 0U}, later = {0U, 0U};
    assert(umi_delivery_lease_acquire(store, 1U, "a", 0U, 5U) == UMI_STATUS_OK);
    assert(umi_delivery_lease_acquire_ticket(store, 1U, "a", 1U, 10U, &ticket) ==
           UMI_STATUS_BUSY);
    assert(umi_delivery_lease_acquire_ticket(store, 1U, "a", 5U, 10U, &ticket) ==
           UMI_STATUS_OK);
    assert(umi_delivery_lease_acquire(store, 1U, "a", 6U, 100U) == UMI_STATUS_BUSY);
    assert(umi_delivery_lease_renew(store, 1U, "a", 6U, 100U) == UMI_STATUS_PERMISSION_DENIED);
    assert(umi_delivery_lease_release(store, 1U, "a") == UMI_STATUS_PERMISSION_DENIED);
    assert(umi_delivery_lease_is_owned(store, 1U, "a", 6U));
    assert(umi_delivery_lease_acquire(store, 1U, "a", 15U, 10U) == UMI_STATUS_OK);
    assert(!umi_delivery_lease_ticket_is_owned(store, ticket, "a", 16U));
    assert(umi_delivery_lease_renew_ticket(store, ticket, "a", 16U, 100U) ==
           UMI_STATUS_PERMISSION_DENIED);
    assert(umi_delivery_lease_release_ticket(store, ticket, "a") == UMI_STATUS_PERMISSION_DENIED);
    assert(umi_delivery_lease_renew(store, 1U, "a", 16U, 10U) == UMI_STATUS_OK);
    assert(umi_delivery_lease_release(store, 1U, "a") == UMI_STATUS_OK);
    assert(umi_delivery_lease_acquire_ticket(store, 1U, "a", 17U, 10U, &later) ==
           UMI_STATUS_OK);
    assert(later.generation > ticket.generation);
    umi_delivery_lease_store_destroy(store);
}

static void test_renewal_and_expiry(void)
{
    UmiDeliveryLeaseStore *store = create_store(1U);
    UmiDeliveryLeaseTicket ticket = {0U, 0U};
    assert(umi_delivery_lease_acquire_ticket(store, 1U, "a", 10U, 5U, &ticket) ==
           UMI_STATUS_OK);
    assert(umi_delivery_lease_renew_ticket(store, ticket, "a", 14U, 10U) == UMI_STATUS_OK);
    assert(umi_delivery_lease_ticket_is_owned(store, ticket, "a", 23U));
    assert(umi_delivery_lease_renew_ticket(store, ticket, "a", 23U, 0U) ==
           UMI_STATUS_INVALID_ARGUMENT);
    assert(umi_delivery_lease_renew_ticket(store, ticket, "a", 23U, UINT64_MAX) ==
           UMI_STATUS_INVALID_ARGUMENT);
    assert(umi_delivery_lease_ticket_is_owned(store, ticket, "a", 23U));
    assert(umi_delivery_lease_renew_ticket(store, ticket, "a", 24U, 10U) ==
           UMI_STATUS_PERMISSION_DENIED);
    /* Expired release is permitted only while this exact record still exists. */
    assert(umi_delivery_lease_release_ticket(store, ticket, "a") == UMI_STATUS_OK);
    umi_delivery_lease_store_destroy(store);
}

static void test_wrong_identity(void)
{
    UmiDeliveryLeaseStore *store = create_store(2U);
    UmiDeliveryLeaseTicket first = {0U, 0U}, second = {0U, 0U}, wrong;
    assert(umi_delivery_lease_acquire_ticket(store, 1U, "a", 0U, 10U, &first) == UMI_STATUS_OK);
    assert(umi_delivery_lease_acquire_ticket(store, 2U, "a", 0U, 10U, &second) == UMI_STATUS_OK);
    wrong = first;
    wrong.message_id = second.message_id;
    assert(umi_delivery_lease_release_ticket(store, wrong, "a") == UMI_STATUS_PERMISSION_DENIED);
    assert(umi_delivery_lease_renew_ticket(store, first, "b", 1U, 100U) ==
           UMI_STATUS_PERMISSION_DENIED);
    assert(umi_delivery_lease_release_ticket(store, first, "b") == UMI_STATUS_PERMISSION_DENIED);
    assert(!umi_delivery_lease_ticket_is_owned(store, first, "b", 1U));
    assert(umi_delivery_lease_ticket_is_owned(store, first, "a", 1U));
    assert(umi_delivery_lease_ticket_is_owned(store, second, "a", 1U));
    umi_delivery_lease_store_destroy(store);
}

static void test_invalid_requests(void)
{
    UmiDeliveryLeaseStore *store = create_store(1U);
    const UmiDeliveryLeaseTicket sentinel = {9U, 9U};
    UmiDeliveryLeaseTicket output = sentinel;
    UmiDeliveryLeaseTicket invalid = {0U, 1U};
    char owner[UMI_DELIVERY_LEASE_OWNER_CAPACITY + 1U];
    memset(owner, 'a', sizeof(owner));
    owner[sizeof(owner) - 1U] = ' ';
    assert(umi_delivery_lease_acquire_ticket(NULL, 1U, "a", 0U, 1U, &output) ==
           UMI_STATUS_INVALID_ARGUMENT);
    assert(umi_delivery_lease_acquire_ticket(store, 0U, "a", 0U, 1U, &output) ==
           UMI_STATUS_INVALID_ARGUMENT);
    assert(umi_delivery_lease_acquire_ticket(store, 1U, NULL, 0U, 1U, &output) ==
           UMI_STATUS_INVALID_ARGUMENT);
    assert(umi_delivery_lease_acquire_ticket(store, 1U, "", 0U, 1U, &output) ==
           UMI_STATUS_INVALID_ARGUMENT);
    assert(umi_delivery_lease_acquire_ticket(store, 1U, owner, 0U, 1U, &output) ==
           UMI_STATUS_CAPACITY_EXCEEDED);
    assert(umi_delivery_lease_acquire_ticket(store, 1U, "a", 0U, 1U, NULL) ==
           UMI_STATUS_INVALID_ARGUMENT);
    assert(umi_delivery_lease_acquire_ticket(store, 1U, "a", UINT64_MAX, 1U, &output) ==
           UMI_STATUS_INVALID_ARGUMENT);
    expect_ticket(output, sentinel);
    assert(umi_delivery_lease_release_ticket(store, invalid, "a") == UMI_STATUS_INVALID_ARGUMENT);
    invalid.message_id = 1U;
    invalid.generation = 0U;
    assert(umi_delivery_lease_release_ticket(store, invalid, "a") == UMI_STATUS_INVALID_ARGUMENT);
    assert(!umi_delivery_lease_ticket_is_owned(store, invalid, "a", 0U));
    /* Invalid requests did not consume the only slot or the first generation. */
    assert(umi_delivery_lease_acquire_ticket(store, 1U, "a", 0U, 1U, &output) == UMI_STATUS_OK);
    assert(output.generation == 1U);
    assert(umi_delivery_lease_release_ticket(store, output, owner) == UMI_STATUS_CAPACITY_EXCEEDED);
    assert(umi_delivery_lease_ticket_is_owned(store, output, "a", 0U));
    umi_delivery_lease_store_destroy(store);
}

int main(void)
{
    test_restarted_owner();
    test_release_and_reacquire();
    test_slot_recycling();
    test_busy_preserves_output();
    test_legacy_interoperation();
    test_renewal_and_expiry();
    test_wrong_identity();
    test_invalid_requests();
    return 0;
}

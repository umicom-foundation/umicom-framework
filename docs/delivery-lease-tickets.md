# Delivery lease tickets

A delivery lease temporarily assigns one message to a worker. A worker is a piece of application code that performs a job. A ticket identifies one acquisition of that lease, so a delayed callback from an earlier acquisition cannot renew or release the current one.

This service belongs to the Framework messaging component. It coordinates serialized calls in one process. It does not persist ownership, authenticate workers, stop a running thread, or establish ownership in another process.

## Choose the contract

Use **umi_delivery_lease_acquire_ticket** when callbacks, retries or restarted workers may reuse an owner name. Keep the returned ticket inside that particular work item. Pass that same ticket to renewal, ownership checks and release.

The name-only functions remain available for existing callers. They cannot renew or release a ticket-protected lease. A live protected lease also refuses name-only acquisition. A new acquisition after expiry may use either API.

## Acquisition and renewal

1. Create a store with a positive number of slots using **umi_delivery_lease_store_create**.
2. Choose a nonzero message identifier and a nonempty owner name of at most 127 bytes.
3. Read time from one monotonic clock. Every participant using this store must use that clock domain.
4. Acquire with a positive duration whose addition to the current time fits in **uint64_t**.
5. Use the output ticket only when the function returns **UMI_STATUS_OK**.
6. Renew with that ticket before its exclusive deadline. At the deadline, ownership has already expired.
7. Release with that ticket when the work item no longer needs local ownership.

Ticket acquisition returns BUSY for every unexpired lease on that message, even if the owner name matches. Renewal is a separate operation. A successful renewal keeps the generation and sets the deadline to the supplied time plus duration; a shorter valid duration can shorten the remaining lease.

The ticket output is unchanged on acquisition failure. The store is unchanged on any rejected acquisition, renewal or release. Store creation has its existing separate output contract: a valid output pointer is set to null before allocation.

## Example

~~~c
UmiDeliveryLeaseStore *store = NULL;
UmiDeliveryLeaseTicket ticket = {0};
UmiStatus status = umi_delivery_lease_store_create(4U, &store);
if (status == UMI_STATUS_OK) {
    status = umi_delivery_lease_acquire_ticket(
        store, 42U, "preview-worker", 100U, 20U, &ticket);
    if (status == UMI_STATUS_OK) {
        /* Perform bounded local work. Check every operation's result. */
        status = umi_delivery_lease_release_ticket(store, ticket, "preview-worker");
    }
    umi_delivery_lease_store_destroy(store);
}
/* Report status to the caller. Do not reuse ticket after store destruction. */
~~~

The numbers above are illustrative clock readings. Production code supplies a real monotonic timestamp in nanoseconds. Do not interpret wall-clock dates as interchangeable with monotonic elapsed time.

## What a generation prevents

Suppose message 42 belongs to generation 1 until time 120. At time 120, the same owner acquires generation 2. A delayed callback carrying generation 1 is rejected, even though its message and owner still match. The generation also changes after release and reacquisition, or after a slot is recycled for another message.

The sequence is shared across the store. Releasing a record does not reset it. Zero is reserved for legacy records. If the sequence reaches its maximum representable value, further ticket acquisitions return CAPACITY_EXCEEDED without wrapping. Existing ticket renewals and releases remain usable.

A ticket contains public integers, not a cryptographic proof. It must only be used with its issuing store, during that store's lifetime. A freshly created store can issue the same numeric ticket as an old store; application ownership must prevent carrying tickets across that boundary.

## Stale work and external effects

A successful ownership observation is only a snapshot. Time may advance before a later action. The service cannot prevent an expired worker from sending a request to a payment provider, broker or database.

For an external effect, enforce ownership at the authority that commits that effect. This may require a durable transaction, an idempotency record and a persisted generation checked by the destination. An in-memory ticket alone does not provide those guarantees. Acknowledging a broker delivery is also a separate action.

Studio background jobs, Trader report jobs and Bank statement jobs can use this common service for local scheduling. Their domain-specific commit and cancellation rules remain necessary. Adopting the API in those workflows requires an explicit integration and its own tests.

## Diagnostics

| Result | Meaning | Response |
| --- | --- | --- |
| OK | The requested operation succeeded | Continue with the returned or retained ticket |
| BUSY | The message has an unexpired acquisition | Keep the current work item; retry later according to policy |
| NOT_FOUND | No record with this message identifier is stored | Discard stale work and reconcile the job |
| PERMISSION_DENIED | Owner or generation differs, or renewal is expired | Stop using this acquisition |
| INVALID_ARGUMENT | Null store, zero identity, empty owner or invalid deadline | Correct the caller's inputs |
| CAPACITY_EXCEEDED | Owner is too long, no slot is available, or sequence is exhausted | Report the exact context; do not silently truncate or reset ownership |

An expired ticket can release its exact stored record, provided no replacement has taken that record. No time argument is required for release. A released or recycled record cannot be renewed through an old ticket.

## Source and qualification

- Public API: [delivery_lease.h](../include/umicom/messaging/delivery_lease.h).
- Implementation: [delivery_lease.c](../src/messaging/delivery_lease.c).
- Legacy regression: [test_delivery_lease.c](../tests/test_delivery_lease.c).
- Ticket regression: [test_delivery_lease_ticket.c](../tests/test_delivery_lease_ticket.c).

The ticket regression covers restarted owners, early release, recycled slots, rejected output preservation, legacy interoperability, renewal deadlines, wrong identities and invalid inputs. It does not claim distributed-system or external-provider qualification.

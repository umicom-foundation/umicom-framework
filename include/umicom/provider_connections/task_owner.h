/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/provider_connections/task_owner.h
 *
 * PURPOSE:
 *   Retain one cooperative local task owner while callers perform reviewed provider work.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#ifndef UMICOM_PROVIDER_CONNECTIONS_TASK_OWNER_H
#define UMICOM_PROVIDER_CONNECTIONS_TASK_OWNER_H
#include "umicom/provider_connections/task_store.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiProviderTaskOwner UmiProviderTaskOwner;
/* Derive a stable guard filename from application, profile and local task ID.
 * Receipt revision, phase, provider settings and request contents intentionally
 * do not alter the guard: competing versions of the same task must contend.
 * The directory is an absolute ordinary path under trusted, stable ancestors;
 * it must already exist before acquisition. One trailing separator is allowed.
 * No filesystem operation occurs here. Output includes a terminating NUL and
 * stays byte-for-byte unchanged on every refusal. Input/output must not overlap.
 * This digest names a cooperative resource; it is not a credential or signature. */
UmiStatus UmiProviderTaskOwnerPath(const char *directory,
    const UmiProviderTaskReceipt *receipt,char *out_path,size_t capacity);
/* out_owner must address NULL. Validate the expected receipt, take the existing
 * nonblocking native file lease, then reread and compare the complete canonical
 * persisted receipt. Any failure preserves the output pointer and releases any
 * newly acquired handle. BUSY means contention or a different current receipt;
 * it does not authorize retrying a provider creation request.
 *
 * Borrow store until Release; join workers before destroying either object.
 * Serialize calls on each owner. Distinct owners coordinate through the native
 * guard only when ALL participants use this same persistent directory namespace.
 * Use one guard for original submission AND later polling/download recovery.
 * Never remove, replace or truncate guard files, including after a crash.
 *
 * All valid receipt phases can be owned. Ownership is not provider permission,
 * a recovery plan or evidence that an operation occurred. Existing receipt and
 * adapter policy still govern permitted transitions. This is cooperative local
 * ownership, not a distributed fence, filesystem sandbox or protection against
 * arbitrary writers. External database edits bypass the protocol. Native handle
 * inheritance and supported-platform limits are those of UmiFileLease. */
UmiStatus UmiProviderTaskOwnerAcquire(UmiProviderTasks *store,const char *directory,
    const UmiProviderTaskReceipt *expected,UmiProviderTaskOwner **out_owner);
/* Recheck this owner's bound scope/local ID and the caller's current expected
 * receipt against persisted canonical content. Use the returned receipt from
 * each successful store transition for the next check. An old revision or any
 * changed content returns BUSY. A failed check retains ownership for cleanup;
 * it never silently changes the expected value or releases a worker's handle.
 * Recheck before effects, but remember that noncooperating writers can race it.
 * No network operation or user callback runs while checking storage. */
UmiStatus UmiProviderTaskOwnerCheck(UmiProviderTaskOwner *owner,
    const UmiProviderTaskReceipt *expected);
/* Join protected work first. Release consumes *owner, including when native
 * close reports an error; do not retry a consumed descriptor. NULL *owner is an
 * OK no-op. The persistent guard filename remains usable by future owners. */
UmiStatus UmiProviderTaskOwnerRelease(UmiProviderTaskOwner **owner);
#ifdef __cplusplus
}
#endif
#endif

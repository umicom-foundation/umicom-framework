/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/security/session.h
 *
 * PURPOSE:
 *   Define revocable authenticated sessions with explicit expiry, principal
 *   ownership and bounded storage.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_SECURITY_SESSION_H
#define UMICOM_SECURITY_SESSION_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/security/identity.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_SESSION_ID_CAPACITY 128U
#define UMI_SESSION_REGISTRY_MAX 1024U
/**
 * Represent the session record data shared with callers of this public contract.
 */
typedef struct UmiSessionRecord {
    char session_id[UMI_SESSION_ID_CAPACITY];
    char principal_id[UMI_IDENTITY_ID_CAPACITY];
    uint64_t issued_at_ns;
    uint64_t expires_at_ns;
    uint64_t last_seen_ns;
    int active;
} UmiSessionRecord;
/**
 * Represent the session registry data shared with callers of this public contract.
 */
typedef struct UmiSessionRegistry UmiSessionRegistry;
/**
 * Initialise session registry from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_session_registry_create(UmiSessionRegistry **out_registry);
/**
 * Release or reset state held by session registry so the same storage can be reused
 * safely.
 */
void umi_session_registry_destroy(UmiSessionRegistry *registry);
/**
 * Provide the session registry issue operation used by this module and its client
 * applications.
 */
/* The host authenticates the principal before issuance. This registry does
 * not verify credentials or identity enablement. Handles contain 256 bits from
 * the platform random source; treat them as opaque private values. Failure to
 * obtain entropy or a unique handle does not create a session or alter output.
 * Reject zero lifetime and expiry overflow. now_ns is a trusted clock value.
 * Existing APIs and records remain available; do not parse handle formatting. */
UmiStatus umi_session_registry_issue(UmiSessionRegistry *registry,
                                     const char *principal_id,
                                     uint64_t now_ns,
                                     uint64_t lifetime_ns,
                                     UmiSessionRecord *out_session);
/**
 * Check that session registry satisfies its contract before another service relies on it.
 */
/* Refuse inactive/expired sessions and observations before issued_at or the
 * previous last_seen. Expiry permanently deactivates a session. Other clock
 * refusals retain its state. Failure leaves output unchanged. Registry lookup
 * is not authorization: separately check current identity and resource policy. */
UmiStatus umi_session_registry_validate(UmiSessionRegistry *registry,
                                        const char *session_id,
                                        uint64_t now_ns,
                                        UmiSessionRecord *out_session);
/**
 * Provide the session registry revoke operation used by this module and its client
 * applications.
 */
UmiStatus umi_session_registry_revoke(UmiSessionRegistry *registry,
                                      const char *session_id);
/**
 * Provide the session registry cleanup operation used by this module and its client
 * applications.
 */
size_t umi_session_registry_cleanup(UmiSessionRegistry *registry,
                                    uint64_t now_ns);
/**
 * Return the number of records represented by session registry without changing their
 * state.
 */
size_t umi_session_registry_count(const UmiSessionRegistry *registry);
/**
 * Find session registry while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_session_registry_at(const UmiSessionRegistry *registry,
                                  size_t index,
                                  UmiSessionRecord *out_session);
#ifdef __cplusplus
}
#endif

#endif

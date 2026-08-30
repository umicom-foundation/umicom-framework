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
typedef struct UmiSessionRecord {
    char session_id[UMI_SESSION_ID_CAPACITY];
    char principal_id[UMI_IDENTITY_ID_CAPACITY];
    uint64_t issued_at_ns;
    uint64_t expires_at_ns;
    uint64_t last_seen_ns;
    int active;
} UmiSessionRecord;
typedef struct UmiSessionRegistry UmiSessionRegistry;
UmiStatus umi_session_registry_create(UmiSessionRegistry **out_registry);
void umi_session_registry_destroy(UmiSessionRegistry *registry);
UmiStatus umi_session_registry_issue(UmiSessionRegistry *registry,
                                     const char *principal_id,
                                     uint64_t now_ns,
                                     uint64_t lifetime_ns,
                                     UmiSessionRecord *out_session);
UmiStatus umi_session_registry_validate(UmiSessionRegistry *registry,
                                        const char *session_id,
                                        uint64_t now_ns,
                                        UmiSessionRecord *out_session);
UmiStatus umi_session_registry_revoke(UmiSessionRegistry *registry,
                                      const char *session_id);
size_t umi_session_registry_cleanup(UmiSessionRegistry *registry,
                                    uint64_t now_ns);
size_t umi_session_registry_count(const UmiSessionRegistry *registry);
UmiStatus umi_session_registry_at(const UmiSessionRegistry *registry,
                                  size_t index,
                                  UmiSessionRecord *out_session);
#ifdef __cplusplus
}
#endif

#endif

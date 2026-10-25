/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/security/record_access.h
 *
 * PURPOSE:
 *   Bind record permissions to current session identity, namespace and purpose.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Bind record permissions to current session identity, namespace and purpose. */
#ifndef UMICOM_SECURITY_RECORD_ACCESS_H
#define UMICOM_SECURITY_RECORD_ACCESS_H
#include "umicom/security/identity.h"
#include "umicom/security/policy.h"
#include "umicom/security/session.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef enum UmiRecordAccessOperation {
  UMI_RECORD_ACCESS_READ_HEAD = 1,
  UMI_RECORD_ACCESS_READ_HISTORY = 2,
  UMI_RECORD_ACCESS_CREATE = 3,
  UMI_RECORD_ACCESS_AMEND = 4
} UmiRecordAccessOperation;
typedef struct UmiRecordAccessRequest {
  char session_id[UMI_SESSION_ID_CAPACITY];
  char entity_id[64];
  char purpose[64];
  UmiRecordAccessOperation operation;
  uint64_t now_nanoseconds;
} UmiRecordAccessRequest;
/* A copied result of one current check, not a reusable authorization token.
 * No session handle is included. It is not signed or durably recorded. */
typedef struct UmiRecordAccessReceipt {
  char principal_id[UMI_IDENTITY_ID_CAPACITY];
  char source[160];
  char entity_id[64];
  char purpose[64];
  UmiRecordAccessOperation operation;
  uint64_t checked_at_nanoseconds, identity_revision, session_expires_at_nanoseconds;
} UmiRecordAccessReceipt;
typedef struct UmiRecordAccessAuthority UmiRecordAccessAuthority;
/* Registries and policy are borrowed; source and namespace are copied.
 * The prefix ends in '.', and record/purpose identifiers use ASCII letters,
 * digits, '.', '_' or '-'. All callers and policy/identity/session mutations
 * must be serialized by one trusted owner through the protected operation.
 * No callbacks or background work are invoked. Retain dependencies until use
 * ends. This does not authenticate credentials or protect raw storage access. */
UmiStatus umi_record_access_authority_create(UmiIdentityRegistry *identities,
                                             UmiSessionRegistry *sessions, UmiPolicyEngine *policy,
                                             const char *source, const char *entity_prefix,
                                             UmiRecordAccessAuthority **out);
void umi_record_access_authority_destroy(UmiRecordAccessAuthority *authority);
/* Each check validates the live session and current identity enablement, then
 * requires explicit policy allows for BOTH:
 *   record.read / record.history / record.create / record.amend on entity_id
 *   record.purpose on "purpose/<purpose>/record/<entity_id>"
 * Existing policy last-matching-rule semantics apply. No caller-supplied role
 * can grant access. The host supplies trusted current time, never a client
 * timestamp. Policy denials and missing/disabled identities return
 * PERMISSION_DENIED. Failures leave output unchanged, although live session
 * validation may advance last_seen or deactivate an expired session. */
UmiStatus umi_record_access_authorize(UmiRecordAccessAuthority *authority,
                                      const UmiRecordAccessRequest *request,
                                      UmiRecordAccessReceipt *out);
/* Validate a complete copied request without touching registries or outputs. */
UmiStatus umi_record_access_request_validate(const UmiRecordAccessRequest *request);
#ifdef __cplusplus
}
#endif
#endif

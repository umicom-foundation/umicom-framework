/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/data/record_host.h
 *
 * PURPOSE:
 *   Own and serialize record storage with its identity/session/policy boundary.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Own and serialize record storage with its identity/session/policy boundary. */
#ifndef UMICOM_DATA_RECORD_HOST_H
#define UMICOM_DATA_RECORD_HOST_H
#include "umicom/data/authorized_form_records.h"
#include <stdbool.h>
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiRecordHost UmiRecordHost;
typedef enum UmiRecordHostBackend {
  UMI_RECORD_HOST_MEMORY = 1,
  UMI_RECORD_HOST_SQLITE = 2
} UmiRecordHostBackend;
typedef struct UmiRecordHostOptions {
  UmiRecordHostBackend backend;
  const char *database_path; /* SQLite only; consumed during creation. */
  const char *source;
  const char *entity_prefix;
} UmiRecordHostOptions;
typedef struct UmiRecordHostState {
  bool retired;
  size_t identities, sessions, policy_rules;
  uint64_t policy_revision;
} UmiRecordHostState;
/* Creation owns storage and all security services; none are exposed as raw
 * handles. Empty policy denies all. SQLite persists records, not identities,
 * sessions or policy. A trusted host must bootstrap those on every open.
 * Output is unchanged on failure. Options and their text are copied/consumed. */
UmiStatus umi_record_host_create(const UmiRecordHostOptions *options, UmiRecordHost **out);
/* Each worker owns a retained reference acquired while another reference is
 * alive. Every call requires a live owned reference. Retain cannot resurrect
 * freed memory. Final release destroys storage; join workers or give each its
 * own reference before releasing the controller's ownership. */
UmiStatus umi_record_host_retain(UmiRecordHost *host);
void umi_record_host_release(UmiRecordHost *host);
/* Retire waits for an admitted operation to finish, then refuses subsequent
 * operations with CANCELLED. It is permanent and idempotent. It does not undo
 * completed writes or interrupt an in-progress storage call. State remains
 * readable until the last reference is released. */
UmiStatus umi_record_host_retire(UmiRecordHost *host);
UmiStatus umi_record_host_state(UmiRecordHost *host, UmiRecordHostState *out);
/* Trusted administrative APIs: never dispatch them from an untrusted client
 * merely because it can access a record. The host authenticates before issuing
 * a session. Issuance requires a registered, currently enabled identity.
 * Security changes and document operations share one mutex. No user callback
 * runs while it is held. Work that may block belongs on a worker thread. */
/* The no-callback statement above describes these explicit-time APIs.
 * record_host_clock.h adds current-time operations whose trusted clock provider
 * runs under admission ownership; that provider must never reenter this host. */
UmiStatus umi_record_host_register_identity(UmiRecordHost *host, const UmiIdentityRecord *identity);
UmiStatus umi_record_host_set_identity_enabled(UmiRecordHost *host, const char *identity_id,
                                               int enabled);
UmiStatus umi_record_host_issue_session(UmiRecordHost *host, const char *identity_id,
                                        uint64_t now_nanoseconds, uint64_t lifetime_nanoseconds,
                                        UmiSessionRecord *out);
UmiStatus umi_record_host_revoke_session(UmiRecordHost *host, const char *session_id);
/* Replace the whole policy atomically after every rule has been validated and
 * copied. Zero rules clears policy and denies all. Last matching rule wins.
 * Failure retains the previous policy and leaves out_revision unchanged. */
UmiStatus umi_record_host_replace_policy(UmiRecordHost *host, const UmiPolicyRule *rules,
                                         size_t count, uint64_t *out_revision);
/* Requests and schema buffers remain stable until return. The trusted host
 * supplies the clock and session binding; a client must not choose either to
 * impersonate another principal or extend a session. Every operation checks
 * current access while holding the same mutex as administrative mutations. */
UmiStatus umi_record_host_latest(UmiRecordHost *host, const UmiRecordAccessRequest *request,
                                 UmiAuthorizedRecordDocument *out);
UmiStatus umi_record_host_load(UmiRecordHost *host, const UmiRecordAccessRequest *request,
                               uint64_t revision, UmiAuthorizedRecordDocument *out);
UmiStatus umi_record_host_open_reviewed(UmiRecordHost *host, const UmiRecordAccessRequest *request,
                                        const UmiRecordRevision *reviewed,
                                        UmiAuthorizedRecordDocument *out);
UmiStatus umi_record_host_append(UmiRecordHost *host, const UmiRecordAccessRequest *request,
                                 const char *reason, uint64_t observed_at_nanoseconds,
                                 const void *content, size_t length, uint64_t expected_revision,
                                 UmiAuthorizedRecordRevision *out);
UmiStatus umi_record_host_form_load(UmiRecordHost *host, const UmiRecordAccessRequest *request,
                                    uint64_t revision, const UmiStructuredFormSchema *schema,
                                    UmiAuthorizedFormRecord *out);
UmiStatus umi_record_host_form_open_reviewed(UmiRecordHost *host,
                                             const UmiRecordAccessRequest *request,
                                             const UmiRecordRevision *reviewed,
                                             const UmiStructuredFormSchema *schema,
                                             UmiAuthorizedFormRecord *out);
UmiStatus umi_record_host_form_save(UmiRecordHost *host, const UmiRecordAccessRequest *request,
                                    const UmiStructuredFormSchema *schema,
                                    const UmiStructuredForm *form, const char *reason,
                                    uint64_t observed_at_nanoseconds, uint64_t expected_revision,
                                    UmiAuthorizedRecordRevision *out);
#ifdef __cplusplus
}
#endif
#endif

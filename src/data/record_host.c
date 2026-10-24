/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * One owned lock spans current security decisions and storage completion. */
#include "umicom/data/record_host.h"
#include "umicom/data/record_host_clock.h"
#include "umicom/platform/threading.h"
#include <limits.h>
#include <stdatomic.h>
#include <stdlib.h>
#include <string.h>
struct UmiRecordHost {
  atomic_uint references;
  UmiClock clock;
  UmiMutex *mutex;
  UmiIdentityRegistry *identities;
  UmiSessionRegistry *sessions;
  UmiPolicyEngine *policy;
  UmiRecordAccessAuthority *authority;
  UmiDataServer *server;
  UmiAuthorizedRecords *records;
  char source[160], prefix[64];
  bool retired;
  uint64_t policy_revision;
};
static void destroy_host(UmiRecordHost *host) {
  if (!host)
    return;
  umi_authorized_records_destroy(host->records);
  umi_record_access_authority_destroy(host->authority);
  umi_data_server_destroy(host->server);
  umi_policy_engine_destroy(host->policy);
  umi_session_registry_destroy(host->sessions);
  umi_identity_registry_destroy(host->identities);
  umi_mutex_destroy(host->mutex);
  free(host);
}
UmiStatus umi_record_host_create(const UmiRecordHostOptions *options, UmiRecordHost **out) {
  if (!options || !out || !options->source || !options->entity_prefix ||
      (options->backend != UMI_RECORD_HOST_MEMORY && options->backend != UMI_RECORD_HOST_SQLITE) ||
      (options->backend == UMI_RECORD_HOST_SQLITE &&
       (!options->database_path || !options->database_path[0])) ||
      (options->backend == UMI_RECORD_HOST_MEMORY && options->database_path))
    return UMI_STATUS_INVALID_ARGUMENT;
  UmiRecordHost *host = calloc(1, sizeof *host);
  if (!host)
    return UMI_STATUS_OUT_OF_MEMORY;
  atomic_init(&host->references, 1);
  host->clock = umi_clock_system();
  UmiStatus s = umi_mutex_create(&host->mutex);
  if (s == UMI_STATUS_OK)
    s = umi_identity_registry_create(&host->identities);
  if (s == UMI_STATUS_OK)
    s = umi_session_registry_create(&host->sessions);
  if (s == UMI_STATUS_OK)
    s = umi_policy_engine_create(&host->policy);
  /* Validate bounded source/prefix before opening a persistent database. */
  if (s == UMI_STATUS_OK)
    s = umi_record_access_authority_create(host->identities, host->sessions, host->policy,
                                           options->source, options->entity_prefix,
                                           &host->authority);
  if (s == UMI_STATUS_OK) {
    strcpy(host->source, options->source);
    strcpy(host->prefix, options->entity_prefix);
    s = options->backend == UMI_RECORD_HOST_MEMORY
            ? umi_data_server_create_memory(&host->server)
            : umi_data_server_create_sqlite(options->database_path, &host->server);
  }
  if (s == UMI_STATUS_OK)
    s = umi_authorized_records_create(host->server, host->authority, &host->records);
  if (s != UMI_STATUS_OK) {
    destroy_host(host);
    return s;
  }
  host->policy_revision = 1;
  *out = host;
  return UMI_STATUS_OK;
}
UmiStatus umi_record_host_retain(UmiRecordHost *host) {
  if (!host)
    return UMI_STATUS_INVALID_ARGUMENT;
  unsigned current = atomic_load_explicit(&host->references, memory_order_relaxed);
  for (;;) {
    if (!current)
      return UMI_STATUS_INVALID_STATE;
    if (current == UINT_MAX)
      return UMI_STATUS_CAPACITY_EXCEEDED;
    if (atomic_compare_exchange_weak_explicit(&host->references, &current, current + 1,
                                              memory_order_relaxed, memory_order_relaxed))
      return UMI_STATUS_OK;
  }
}
void umi_record_host_release(UmiRecordHost *host) {
  if (host && atomic_fetch_sub_explicit(&host->references, 1, memory_order_acq_rel) == 1)
    destroy_host(host);
}
static UmiStatus enter_host(UmiRecordHost *host, bool allow_retired) {
  if (!host)
    return UMI_STATUS_INVALID_ARGUMENT;
  UmiStatus s = umi_mutex_lock(host->mutex);
  if (s != UMI_STATUS_OK)
    return s;
  if (host->retired && !allow_retired) {
    (void)umi_mutex_unlock(host->mutex);
    return UMI_STATUS_CANCELLED;
  }
  return UMI_STATUS_OK;
}
static UmiStatus leave_host(UmiRecordHost *host, UmiStatus status) {
  /* This thread owns a valid mutex. Preserve the operation's committed result;
   * mutex misuse is outside the host's live-reference contract. */
  (void)umi_mutex_unlock(host->mutex);
  return status;
}
UmiStatus umi_record_host_retire(UmiRecordHost *host) {
  UmiStatus s = enter_host(host, true);
  if (s != UMI_STATUS_OK)
    return s;
  host->retired = true;
  return leave_host(host, UMI_STATUS_OK);
}
UmiStatus umi_record_host_state(UmiRecordHost *host, UmiRecordHostState *out) {
  if (!out)
    return UMI_STATUS_INVALID_ARGUMENT;
  UmiStatus s = enter_host(host, true);
  if (s != UMI_STATUS_OK)
    return s;
  UmiRecordHostState state = {.retired = host->retired,
                              .identities = umi_identity_registry_count(host->identities),
                              .sessions = umi_session_registry_count(host->sessions),
                              .policy_rules = umi_policy_engine_count(host->policy),
                              .policy_revision = host->policy_revision};
  *out = state;
  return leave_host(host, UMI_STATUS_OK);
}
UmiStatus umi_record_host_register_identity(UmiRecordHost *host,
                                            const UmiIdentityRecord *identity) {
  UmiStatus s = enter_host(host, false);
  if (s != UMI_STATUS_OK)
    return s;
  return leave_host(host, umi_identity_registry_register(host->identities, identity));
}
UmiStatus umi_record_host_set_identity_enabled(UmiRecordHost *host, const char *identity_id,
                                               int enabled) {
  UmiStatus s = enter_host(host, false);
  if (s != UMI_STATUS_OK)
    return s;
  return leave_host(host,
                    umi_identity_registry_set_enabled(host->identities, identity_id, enabled));
}
UmiStatus umi_record_host_issue_session(UmiRecordHost *host, const char *identity_id,
                                        uint64_t now_nanoseconds, uint64_t lifetime_nanoseconds,
                                        UmiSessionRecord *out) {
  if (!out || !now_nanoseconds)
    return UMI_STATUS_INVALID_ARGUMENT;
  UmiStatus s = enter_host(host, false);
  if (s != UMI_STATUS_OK)
    return s;
  UmiIdentityRecord identity;
  s = umi_identity_registry_get(host->identities, identity_id, &identity);
  if (s == UMI_STATUS_NOT_FOUND || (s == UMI_STATUS_OK && !identity.enabled))
    s = UMI_STATUS_PERMISSION_DENIED;
  if (s == UMI_STATUS_OK)
    s = umi_session_registry_issue(host->sessions, identity.identity_id, now_nanoseconds,
                                   lifetime_nanoseconds, out);
  return leave_host(host, s);
}
UmiStatus umi_record_host_revoke_session(UmiRecordHost *host, const char *session_id) {
  UmiStatus s = enter_host(host, false);
  if (s != UMI_STATUS_OK)
    return s;
  return leave_host(host, umi_session_registry_revoke(host->sessions, session_id));
}
UmiStatus umi_record_host_replace_policy(UmiRecordHost *host, const UmiPolicyRule *rules,
                                         size_t count, uint64_t *out_revision) {
  if (!host || !out_revision || (!rules && count))
    return UMI_STATUS_INVALID_ARGUMENT;
  if (count > UMI_POLICY_MAX_RULES)
    return UMI_STATUS_CAPACITY_EXCEEDED;
  /* Build replacements before taking the shared lock. Failed rules cannot
   * partially clear or grant access, and no client can observe a half policy. */
  UmiPolicyEngine *policy = NULL;
  UmiRecordAccessAuthority *authority = NULL;
  UmiAuthorizedRecords *records = NULL;
  UmiStatus s = umi_policy_engine_create(&policy);
  for (size_t i = 0; i < count && s == UMI_STATUS_OK; ++i) {
    if (rules[i].effect != UMI_POLICY_ALLOW && rules[i].effect != UMI_POLICY_DENY)
      s = UMI_STATUS_INVALID_ARGUMENT;
    else
      s = umi_policy_engine_add(policy, &rules[i]);
  }
  if (s == UMI_STATUS_OK)
    s = umi_record_access_authority_create(host->identities, host->sessions, policy, host->source,
                                           host->prefix, &authority);
  if (s == UMI_STATUS_OK)
    s = umi_authorized_records_create(host->server, authority, &records);
  if (s == UMI_STATUS_OK) {
    s = enter_host(host, false);
    if (s == UMI_STATUS_OK) {
      if (host->policy_revision == UINT64_MAX)
        s = UMI_STATUS_CAPACITY_EXCEEDED;
      else {
        UmiPolicyEngine *old_policy = host->policy;
        UmiRecordAccessAuthority *old_authority = host->authority;
        UmiAuthorizedRecords *old_records = host->records;
        host->policy = policy;
        host->authority = authority;
        host->records = records;
        policy = old_policy;
        authority = old_authority;
        records = old_records;
        *out_revision = ++host->policy_revision;
      }
      s = leave_host(host, s);
    }
  }
  umi_authorized_records_destroy(records);
  umi_record_access_authority_destroy(authority);
  umi_policy_engine_destroy(policy);
  return s;
}
UmiStatus umi_record_host_latest(UmiRecordHost *host, const UmiRecordAccessRequest *request,
                                 UmiAuthorizedRecordDocument *out) {
  UmiStatus s = enter_host(host, false);
  if (s != UMI_STATUS_OK)
    return s;
  return leave_host(host, umi_authorized_record_latest(host->records, request, out));
}
UmiStatus umi_record_host_load(UmiRecordHost *host, const UmiRecordAccessRequest *request,
                               uint64_t revision, UmiAuthorizedRecordDocument *out) {
  UmiStatus s = enter_host(host, false);
  if (s != UMI_STATUS_OK)
    return s;
  return leave_host(host, umi_authorized_record_load(host->records, request, revision, out));
}
UmiStatus umi_record_host_open_reviewed(UmiRecordHost *host, const UmiRecordAccessRequest *request,
                                        const UmiRecordRevision *reviewed,
                                        UmiAuthorizedRecordDocument *out) {
  UmiStatus s = enter_host(host, false);
  if (s != UMI_STATUS_OK)
    return s;
  return leave_host(host,
                    umi_authorized_record_open_reviewed(host->records, request, reviewed, out));
}
UmiStatus umi_record_host_append(UmiRecordHost *host, const UmiRecordAccessRequest *request,
                                 const char *reason, uint64_t observed_at_nanoseconds,
                                 const void *content, size_t length, uint64_t expected_revision,
                                 UmiAuthorizedRecordRevision *out) {
  UmiStatus s = enter_host(host, false);
  if (s != UMI_STATUS_OK)
    return s;
  return leave_host(host, umi_authorized_record_append(host->records, request, reason,
                                                       observed_at_nanoseconds, content, length,
                                                       expected_revision, out));
}
UmiStatus umi_record_host_form_load(UmiRecordHost *host, const UmiRecordAccessRequest *request,
                                    uint64_t revision, const UmiStructuredFormSchema *schema,
                                    UmiAuthorizedFormRecord *out) {
  UmiStatus s = enter_host(host, false);
  if (s != UMI_STATUS_OK)
    return s;
  return leave_host(host,
                    umi_authorized_form_record_load(host->records, request, revision, schema, out));
}
UmiStatus umi_record_host_form_open_reviewed(UmiRecordHost *host,
                                             const UmiRecordAccessRequest *request,
                                             const UmiRecordRevision *reviewed,
                                             const UmiStructuredFormSchema *schema,
                                             UmiAuthorizedFormRecord *out) {
  UmiStatus s = enter_host(host, false);
  if (s != UMI_STATUS_OK)
    return s;
  return leave_host(host, umi_authorized_form_record_open_reviewed(host->records, request, reviewed,
                                                                   schema, out));
}
UmiStatus umi_record_host_form_save(UmiRecordHost *host, const UmiRecordAccessRequest *request,
                                    const UmiStructuredFormSchema *schema,
                                    const UmiStructuredForm *form, const char *reason,
                                    uint64_t observed_at_nanoseconds, uint64_t expected_revision,
                                    UmiAuthorizedRecordRevision *out) {
  UmiStatus s = enter_host(host, false);
  if (s != UMI_STATUS_OK)
    return s;
  return leave_host(host, umi_authorized_form_record_save(host->records, request, schema, form,
                                                          reason, observed_at_nanoseconds,
                                                          expected_revision, out));
}

/* Queued jobs must not extend a session by retaining a pre-queue clock value.
 * Read the host's trusted provider only after owning the shared admission lock. */
UmiStatus umi_record_host_create_with_clock(const UmiRecordHostOptions *options,
                                            const UmiClock *clock, UmiRecordHost **out) {
  if (!clock || !clock->monotonic_nanoseconds || !out)
    return UMI_STATUS_INVALID_ARGUMENT;
  UmiRecordHost *candidate = NULL;
  UmiStatus s = umi_record_host_create(options, &candidate);
  if (s == UMI_STATUS_OK) {
    candidate->clock = *clock;
    *out = candidate;
  }
  return s;
}
static UmiStatus current_request(UmiRecordHost *host, const UmiRecordAccessRequest *input,
                                 UmiRecordAccessRequest *out) {
  if (!input)
    return UMI_STATUS_INVALID_ARGUMENT;
  uint64_t now = host->clock.monotonic_nanoseconds(&host->clock);
  if (!now)
    return UMI_STATUS_UNAVAILABLE;
  *out = *input;
  out->now_nanoseconds = now;
  return UMI_STATUS_OK;
}
UmiStatus umi_record_host_issue_current_session(UmiRecordHost *host, const char *identity_id,
                                                uint64_t lifetime_nanoseconds,
                                                UmiSessionRecord *out) {
  if (!out)
    return UMI_STATUS_INVALID_ARGUMENT;
  UmiStatus s = enter_host(host, false);
  if (s != UMI_STATUS_OK)
    return s;
  uint64_t now = host->clock.monotonic_nanoseconds(&host->clock);
  if (!now)
    return leave_host(host, UMI_STATUS_UNAVAILABLE);
  UmiIdentityRecord identity;
  s = umi_identity_registry_get(host->identities, identity_id, &identity);
  if (s == UMI_STATUS_NOT_FOUND || (s == UMI_STATUS_OK && !identity.enabled))
    s = UMI_STATUS_PERMISSION_DENIED;
  if (s == UMI_STATUS_OK)
    s = umi_session_registry_issue(host->sessions, identity.identity_id, now, lifetime_nanoseconds,
                                   out);
  return leave_host(host, s);
}

UmiStatus umi_record_host_latest_current(UmiRecordHost *host, const UmiRecordAccessRequest *request,
                                         UmiAuthorizedRecordDocument *out) {
  UmiStatus s = enter_host(host, false);
  if (s != UMI_STATUS_OK)
    return s;
  UmiRecordAccessRequest current;
  s = current_request(host, request, &current);
  if (s == UMI_STATUS_OK)
    s = umi_authorized_record_latest(host->records, &current, out);
  return leave_host(host, s);
}

UmiStatus umi_record_host_load_current(UmiRecordHost *host, const UmiRecordAccessRequest *request,
                                       uint64_t revision, UmiAuthorizedRecordDocument *out) {
  UmiStatus s = enter_host(host, false);
  if (s != UMI_STATUS_OK)
    return s;
  UmiRecordAccessRequest current;
  s = current_request(host, request, &current);
  if (s == UMI_STATUS_OK)
    s = umi_authorized_record_load(host->records, &current, revision, out);
  return leave_host(host, s);
}

UmiStatus umi_record_host_open_reviewed_current(UmiRecordHost *host,
                                                const UmiRecordAccessRequest *request,
                                                const UmiRecordRevision *reviewed,
                                                UmiAuthorizedRecordDocument *out) {
  UmiStatus s = enter_host(host, false);
  if (s != UMI_STATUS_OK)
    return s;
  UmiRecordAccessRequest current;
  s = current_request(host, request, &current);
  if (s == UMI_STATUS_OK)
    s = umi_authorized_record_open_reviewed(host->records, &current, reviewed, out);
  return leave_host(host, s);
}

UmiStatus umi_record_host_append_current(UmiRecordHost *host, const UmiRecordAccessRequest *request,
                                         const char *reason, uint64_t observed_at_nanoseconds,
                                         const void *content, size_t length,
                                         uint64_t expected_revision,
                                         UmiAuthorizedRecordRevision *out) {
  UmiStatus s = enter_host(host, false);
  if (s != UMI_STATUS_OK)
    return s;
  UmiRecordAccessRequest current;
  s = current_request(host, request, &current);
  if (s == UMI_STATUS_OK)
    s = umi_authorized_record_append(host->records, &current, reason, observed_at_nanoseconds,
                                     content, length, expected_revision, out);
  return leave_host(host, s);
}

UmiStatus umi_record_host_form_load_current(UmiRecordHost *host,
                                            const UmiRecordAccessRequest *request,
                                            uint64_t revision,
                                            const UmiStructuredFormSchema *schema,
                                            UmiAuthorizedFormRecord *out) {
  UmiStatus s = enter_host(host, false);
  if (s != UMI_STATUS_OK)
    return s;
  UmiRecordAccessRequest current;
  s = current_request(host, request, &current);
  if (s == UMI_STATUS_OK)
    s = umi_authorized_form_record_load(host->records, &current, revision, schema, out);
  return leave_host(host, s);
}

UmiStatus umi_record_host_form_open_reviewed_current(UmiRecordHost *host,
                                                     const UmiRecordAccessRequest *request,
                                                     const UmiRecordRevision *reviewed,
                                                     const UmiStructuredFormSchema *schema,
                                                     UmiAuthorizedFormRecord *out) {
  UmiStatus s = enter_host(host, false);
  if (s != UMI_STATUS_OK)
    return s;
  UmiRecordAccessRequest current;
  s = current_request(host, request, &current);
  if (s == UMI_STATUS_OK)
    s = umi_authorized_form_record_open_reviewed(host->records, &current, reviewed, schema, out);
  return leave_host(host, s);
}

UmiStatus umi_record_host_form_save_current(UmiRecordHost *host,
                                            const UmiRecordAccessRequest *request,
                                            const UmiStructuredFormSchema *schema,
                                            const UmiStructuredForm *form, const char *reason,
                                            uint64_t observed_at_nanoseconds,
                                            uint64_t expected_revision,
                                            UmiAuthorizedRecordRevision *out) {
  UmiStatus s = enter_host(host, false);
  if (s != UMI_STATUS_OK)
    return s;
  UmiRecordAccessRequest current;
  s = current_request(host, request, &current);
  if (s == UMI_STATUS_OK)
    s = umi_authorized_form_record_save(host->records, &current, schema, form, reason,
                                        observed_at_nanoseconds, expected_revision, out);
  return leave_host(host, s);
}

/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/security/record_access.c
 *
 * PURPOSE:
 *   Compose existing identity/session/policy services; never trust an actor label.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Compose existing identity/session/policy services; never trust an actor label. */
#include "umicom/security/record_access.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
struct UmiRecordAccessAuthority {
  UmiIdentityRegistry *identities;
  UmiSessionRegistry *sessions;
  UmiPolicyEngine *policy;
  char source[160], prefix[64];
};
static bool access_text(const char *text, size_t capacity, bool identifier) {
  if (!text)
    return false;
  size_t length = 0;
  while (length < capacity && text[length]) {
    unsigned char c = (unsigned char)text[length];
    if (identifier && !((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
                        (c >= '0' && c <= '9') || c == '.' || c == '_' || c == '-'))
      return false;
    if (!identifier && (c < 32 || c == 127))
      return false;
    ++length;
  }
  return length > 0 && length < capacity;
}
static const char *capability(UmiRecordAccessOperation operation) {
  switch (operation) {
  case UMI_RECORD_ACCESS_READ_HEAD:
    return "record.read";
  case UMI_RECORD_ACCESS_READ_HISTORY:
    return "record.history";
  case UMI_RECORD_ACCESS_CREATE:
    return "record.create";
  case UMI_RECORD_ACCESS_AMEND:
    return "record.amend";
  default:
    return NULL;
  }
}
UmiStatus umi_record_access_request_validate(const UmiRecordAccessRequest *request) {
  if (!request || !capability(request->operation) || !request->now_nanoseconds ||
      !access_text(request->session_id, sizeof request->session_id, false) ||
      !access_text(request->entity_id, sizeof request->entity_id, true) ||
      !access_text(request->purpose, sizeof request->purpose, true))
    return UMI_STATUS_INVALID_ARGUMENT;
  return UMI_STATUS_OK;
}
UmiStatus umi_record_access_authority_create(UmiIdentityRegistry *identities,
                                             UmiSessionRegistry *sessions, UmiPolicyEngine *policy,
                                             const char *source, const char *entity_prefix,
                                             UmiRecordAccessAuthority **out) {
  if (!identities || !sessions || !policy || !out || !access_text(source, 160, false) ||
      !access_text(entity_prefix, 64, true))
    return UMI_STATUS_INVALID_ARGUMENT;
  size_t prefix_length = strlen(entity_prefix);
  if (entity_prefix[prefix_length - 1] != '.')
    return UMI_STATUS_INVALID_ARGUMENT;
  UmiRecordAccessAuthority *candidate = calloc(1, sizeof *candidate);
  if (!candidate)
    return UMI_STATUS_OUT_OF_MEMORY;
  candidate->identities = identities;
  candidate->sessions = sessions;
  candidate->policy = policy;
  strcpy(candidate->source, source);
  strcpy(candidate->prefix, entity_prefix);
  *out = candidate;
  return UMI_STATUS_OK;
}
void umi_record_access_authority_destroy(UmiRecordAccessAuthority *authority) { free(authority); }
UmiStatus umi_record_access_authorize(UmiRecordAccessAuthority *authority,
                                      const UmiRecordAccessRequest *request,
                                      UmiRecordAccessReceipt *out) {
  if (!authority || !out)
    return UMI_STATUS_INVALID_ARGUMENT;
  UmiStatus status = umi_record_access_request_validate(request);
  if (status != UMI_STATUS_OK)
    return status;
  size_t prefix_length = strlen(authority->prefix);
  if (strncmp(request->entity_id, authority->prefix, prefix_length) ||
      strlen(request->entity_id) <= prefix_length)
    return UMI_STATUS_PERMISSION_DENIED;
  UmiSessionRecord session;
  status = umi_session_registry_validate(authority->sessions, request->session_id,
                                         request->now_nanoseconds, &session);
  if (status == UMI_STATUS_NOT_FOUND || status == UMI_STATUS_PERMISSION_DENIED)
    return UMI_STATUS_PERMISSION_DENIED;
  if (status != UMI_STATUS_OK)
    return status;
  UmiIdentityRecord identity;
  status = umi_identity_registry_get(authority->identities, session.principal_id, &identity);
  if (status == UMI_STATUS_NOT_FOUND || (status == UMI_STATUS_OK && !identity.enabled))
    return UMI_STATUS_PERMISSION_DENIED;
  if (status != UMI_STATUS_OK)
    return status;
  UmiPolicyDecision operation = umi_policy_engine_authorize(
      authority->policy, identity.identity_id, capability(request->operation), request->entity_id);
  if (operation.effect != UMI_POLICY_ALLOW)
    return UMI_STATUS_PERMISSION_DENIED;
  char purpose_resource[160];
  int n = snprintf(purpose_resource, sizeof purpose_resource, "purpose/%s/record/%s",
                   request->purpose, request->entity_id);
  if (n < 0 || (size_t)n >= sizeof purpose_resource)
    return UMI_STATUS_CAPACITY_EXCEEDED;
  UmiPolicyDecision purpose = umi_policy_engine_authorize(authority->policy, identity.identity_id,
                                                          "record.purpose", purpose_resource);
  if (purpose.effect != UMI_POLICY_ALLOW)
    return UMI_STATUS_PERMISSION_DENIED;
  UmiRecordAccessReceipt receipt = {0};
  strcpy(receipt.principal_id, identity.identity_id);
  strcpy(receipt.source, authority->source);
  strcpy(receipt.entity_id, request->entity_id);
  strcpy(receipt.purpose, request->purpose);
  receipt.operation = request->operation;
  receipt.checked_at_nanoseconds = request->now_nanoseconds;
  receipt.identity_revision = identity.revision;
  receipt.session_expires_at_nanoseconds = session.expires_at_ns;
  *out = receipt;
  return UMI_STATUS_OK;
}

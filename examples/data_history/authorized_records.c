/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * A synthetic, single-owner host demonstrates explicit record and purpose policy.
 * Real applications authenticate their user before issuing a session. */
#include "umicom/data/authorized_records.h"
#include <stdio.h>
#include <string.h>
int main(void) {
  UmiIdentityRegistry *identities = NULL;
  UmiSessionRegistry *sessions = NULL;
  UmiPolicyEngine *policy = NULL;
  UmiRecordAccessAuthority *authority = NULL;
  UmiDataServer *server = NULL;
  UmiAuthorizedRecords *records = NULL;
  UmiStatus status = umi_identity_registry_create(&identities);
  if (status == UMI_STATUS_OK)
    status = umi_session_registry_create(&sessions);
  if (status == UMI_STATUS_OK)
    status = umi_policy_engine_create(&policy);
  UmiIdentityRecord identity = {.identity_id = "example.editor",
                                .display_name = "Fictional editor",
                                .kind = UMI_IDENTITY_HUMAN,
                                .enabled = 1};
  if (status == UMI_STATUS_OK)
    status = umi_identity_registry_register(identities, &identity);
  UmiSessionRecord session = {0};
  if (status == UMI_STATUS_OK)
    status = umi_session_registry_issue(sessions, identity.identity_id, 100, 1000, &session);
  const UmiPolicyRule rules[] = {
      {"example.editor", "record.create", "example.note", UMI_POLICY_ALLOW},
      {"example.editor", "record.read", "example.note", UMI_POLICY_ALLOW},
      {"example.editor", "record.purpose", "purpose/review/record/example.note", UMI_POLICY_ALLOW}};
  for (size_t i = 0; status == UMI_STATUS_OK && i < sizeof rules / sizeof rules[0]; ++i)
    status = umi_policy_engine_add(policy, &rules[i]);
  if (status == UMI_STATUS_OK)
    status = umi_record_access_authority_create(identities, sessions, policy, "Example host",
                                                "example.", &authority);
  if (status == UMI_STATUS_OK)
    status = umi_data_server_create_memory(&server);
  if (status == UMI_STATUS_OK)
    status = umi_authorized_records_create(server, authority, &records);
  UmiRecordAccessRequest request = {.entity_id = "example.note",
                                    .purpose = "review",
                                    .operation = UMI_RECORD_ACCESS_CREATE,
                                    .now_nanoseconds = 101};
  strcpy(request.session_id, session.session_id);
  UmiAuthorizedRecordRevision written;
  if (status == UMI_STATUS_OK)
    status = umi_authorized_record_append(records, &request, "Create fictional note", 0, "Example",
                                          7, 0, &written);
  UmiAuthorizedRecordDocument read;
  request.operation = UMI_RECORD_ACCESS_READ_HEAD;
  request.now_nanoseconds = 102;
  if (status == UMI_STATUS_OK)
    status = umi_authorized_record_latest(records, &request, &read);
  if (status == UMI_STATUS_OK &&
      (read.document.length != 7 || memcmp(read.document.content, "Example", 7) ||
       strcmp(read.document.record.amendment.actor, identity.identity_id)))
    status = UMI_STATUS_INVALID_STATE;
  if (status == UMI_STATUS_OK)
    puts("Fictional record saved and read through current identity and purpose policy.");
  else
    fprintf(stderr, "Record example failed with status %d.\n", (int)status);
  umi_authorized_records_destroy(records);
  umi_data_server_destroy(server);
  umi_record_access_authority_destroy(authority);
  umi_policy_engine_destroy(policy);
  umi_session_registry_destroy(sessions);
  umi_identity_registry_destroy(identities);
  return status == UMI_STATUS_OK ? 0 : 1;
}

/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * A restore is all-or-nothing; caller transactions and existing records survive. */
#include "include/record_bundle_internal.h"
#include "include/record_document_internal.h"
#include "include/record_history_internal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static UmiStatus rollback(UmiDataServer *server, UmiStatus status) {
  return umi_data_server_rollback(server) == UMI_STATUS_OK ? status : UMI_STATUS_IO_ERROR;
}
UmiStatus umi_record_bundle_capture(UmiDataServer *server, const char *entity_id,
                                    UmiRecordBundle **out) {
  if (server == NULL || entity_id == NULL || out == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  UmiRecordBundle *bundle = calloc(1, sizeof(*bundle));
  if (bundle == NULL)
    return UMI_STATUS_OUT_OF_MEMORY;
  UmiStatus status = umi_data_server_begin(server);
  if (status != UMI_STATUS_OK) {
    free(bundle);
    return status;
  }
  UmiRecordRevision head;
  status = umi_record_history_latest(server, entity_id, &head);
  if (status == UMI_STATUS_OK && head.revision > UMI_RECORD_BUNDLE_MAX_REVISIONS)
    status = UMI_STATUS_CAPACITY_EXCEEDED;
  if (status == UMI_STATUS_OK) {
    bundle->count = (size_t)head.revision;
    for (size_t i = 0; i < bundle->count && status == UMI_STATUS_OK; ++i)
      status =
          umi_record_document_read_in_transaction(server, entity_id, i + 1, &bundle->records[i]);
  }
  if (status == UMI_STATUS_OK)
    status = umi_record_bundle_validate(bundle);
  if (status == UMI_STATUS_OK)
    status = umi_data_server_commit(server);
  if (status != UMI_STATUS_OK) {
    status = rollback(server, status);
    free(bundle);
    return status;
  }
  *out = bundle;
  return UMI_STATUS_OK;
}
/* A missing head is not proof of an unused identity. Refuse every reserved row
 * for this entity, including orphan revisions beyond the incoming history and
 * malformed content suffixes. Visit only compares borrowed keys under the store
 * lock; it performs no nested data operation. A bounded scan fails closed. */
typedef struct BundleOccupancy {
  char head[192], history[192], content[192];
  size_t visited;
} BundleOccupancy;
static UmiStatus occupied_row(const char *key, const char *value, void *context) {
  (void)value;
  BundleOccupancy *scan = context;
  if (++scan->visited > UMI_RECORD_BUNDLE_SCAN_LIMIT)
    return UMI_STATUS_CAPACITY_EXCEEDED;
  return strcmp(key, scan->head) == 0 || strncmp(key, scan->history, strlen(scan->history)) == 0 ||
                 strncmp(key, scan->content, strlen(scan->content)) == 0
             ? UMI_STATUS_INVALID_STATE
             : UMI_STATUS_OK;
}
static UmiStatus unoccupied_entity(UmiDataServer *server, const char *entity) {
  BundleOccupancy scan = {0};
  UmiStatus status = umi_record_history_key(entity, 0, scan.head, sizeof(scan.head));
  if (status != UMI_STATUS_OK)
    return status;
  const char *encoded = scan.head + sizeof("repo.record-history.h.") - 1;
  int a = snprintf(scan.history, sizeof(scan.history), "repo.record-history.r.%s.", encoded);
  int b = snprintf(scan.content, sizeof(scan.content), "repo.record-content.%s.", encoded);
  if (a < 0 || (size_t)a >= sizeof(scan.history) || b < 0 || (size_t)b >= sizeof(scan.content))
    return UMI_STATUS_CAPACITY_EXCEEDED;
  return umi_data_server_visit(server, occupied_row, &scan);
}
/* Caller-owned staging supports atomic compositions without nested begin/commit. */
UmiStatus umi_record_bundle_stage_in_transaction(UmiDataServer *server,
                                                 const UmiRecordBundle *bundle) {
  UmiDataServerSnapshot state;
  UmiStatus status = umi_record_bundle_validate(bundle);
  if (status == UMI_STATUS_OK)
    status = umi_data_server_snapshot(server, &state);
  if (status == UMI_STATUS_OK && !state.transaction_active)
    status = UMI_STATUS_INVALID_STATE;
  if (status == UMI_STATUS_OK)
    status = unoccupied_entity(server, bundle->records[0].record.amendment.entity_id);
  for (size_t i = 0; status == UMI_STATUS_OK && i < bundle->count; ++i)
    status = umi_record_document_stage_in_transaction(server, &bundle->records[i]);
  return status;
}
UmiStatus umi_record_bundle_restore_new(UmiDataServer *server, const UmiRecordBundle *bundle,
                                        UmiRecordRevision *out) {
  if (server == NULL || out == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  UmiStatus status = umi_record_bundle_validate(bundle);
  if (status != UMI_STATUS_OK)
    return status;
  status = umi_data_server_begin(server);
  if (status != UMI_STATUS_OK)
    return status;
  /* Shared staging retains the complete occupancy and row checks while allowing
   * a receipt in the same transaction. Preserve the original inline sequence. */
#if 0
  status = unoccupied_entity(server, bundle->records[0].record.amendment.entity_id);
  for (size_t i = 0; i < bundle->count && status == UMI_STATUS_OK; ++i)
    status = umi_record_document_stage_in_transaction(server, &bundle->records[i]);
#endif
  status = umi_record_bundle_stage_in_transaction(server, bundle);
  if (status == UMI_STATUS_OK)
    status = umi_data_server_commit(server);
  if (status != UMI_STATUS_OK)
    return rollback(server, status);
  *out = bundle->records[bundle->count - 1].record;
  return UMI_STATUS_OK;
}

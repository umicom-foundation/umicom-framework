/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/financial_messaging/outbox_review.c
 *
 * PURPOSE:
 *   Page independently checked outgoing records without loading payloads into application views.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "outbox_internal.h"
#include "umicom/financial_messaging/outbox_review.h"
#include <inttypes.h>
#include <stdio.h>
typedef struct Candidates {
  char ids[UMI_UFMP_OUTBOX_PAGE_CAPACITY + 1U][65];
  size_t count, capacity;
  const char *after;
} Candidates;
static UmiStatus Collect(const char *key, const char *value, void *data) {
  (void)value;
  Candidates *c = data;
  const char *id = NULL;
  const char *body = "ufmp.outbox.v1/body/", *state = "ufmp.outbox.v1/state/";
  if (strncmp(key, body, strlen(body)) == 0) id = key + strlen(body);
  else if (strncmp(key, state, strlen(state)) == 0) id = key + strlen(state);
  else return UMI_STATUS_OK;
  if (!UmiUfmpInboxReceiptValid(id)) return UMI_STATUS_PARSE_ERROR;
  if (c->after != NULL && strcmp(id, c->after) <= 0) return UMI_STATUS_OK;
  size_t at = 0U;
  while (at < c->count && strcmp(c->ids[at], id) < 0) ++at;
  if (at < c->count && strcmp(c->ids[at], id) == 0) return UMI_STATUS_OK;
  if (at >= c->capacity) return UMI_STATUS_OK;
  /* Retain only the smallest requested IDs, regardless of storage order.
   * One extra ID proves that another page exists without unbounded allocation. */
  size_t new_count = c->count < c->capacity ? c->count + 1U : c->count;
  if (new_count > at + 1U)
    memmove(c->ids[at + 1U], c->ids[at], (new_count - at - 1U) * sizeof c->ids[0]);
  memcpy(c->ids[at], id, sizeof c->ids[0]); c->count = new_count;
  return UMI_STATUS_OK;
}
UmiStatus UmiUfmpOutboxReadPage(const UmiDataServer *server, const char *after,
  size_t limit, UmiUfmpOutboxPage *out) {
  if (server == NULL || out == NULL || limit == 0U || limit > UMI_UFMP_OUTBOX_PAGE_CAPACITY ||
      (after != NULL && after[0] != '\0' && !UmiUfmpInboxReceiptValid(after)))
    return UMI_STATUS_INVALID_ARGUMENT;
  if (umi_data_server_in_transaction(server)) return UMI_STATUS_BUSY;
  Candidates candidates = {0};
  candidates.capacity = limit + 1U; candidates.after = after;
  UmiStatus s = umi_data_server_visit(server, Collect, &candidates);
  if (s != UMI_STATUS_OK) return s;
  UmiUfmpOutboxPage page = {0};
  page.has_more = candidates.count > limit;
  page.count = page.has_more ? limit : candidates.count;
  for (size_t i = 0U; i < page.count; ++i) {
    UmiUfmpOutboxRecord r;
    s = UmiUfmpOutboxLoad(server, candidates.ids[i], &r);
    if (s != UMI_STATUS_OK) return s == UMI_STATUS_NOT_FOUND ? UMI_STATUS_PARSE_ERROR : s;
    UmiUfmpOutboxSummary *item = &page.items[i];
    memcpy(item->id, r.id, sizeof item->id);
    memcpy(item->sender, r.message.envelope.sender, sizeof item->sender);
    memcpy(item->recipient, r.message.envelope.recipient, sizeof item->recipient);
    item->type = r.message.envelope.type; item->state = r.state;
    item->revision = r.revision; item->updated_ms = r.updated_ms;
    item->next_attempt_ms = r.next_attempt_ms; item->attempts = r.attempts;
  }
  if (page.count) memcpy(page.next_cursor, page.items[page.count-1U].id, sizeof page.next_cursor);
  *out = page;
  return UMI_STATUS_OK;
}
UmiStatus UmiUfmpOutboxInspect(const char *database_path, const char *after,
  char *out, size_t capacity, size_t *required) {
  if (database_path == NULL || required == NULL || (out == NULL && capacity != 0U))
    return UMI_STATUS_INVALID_ARGUMENT;
  UmiDataServer *server = NULL;
  UmiStatus s = umi_data_server_open_sqlite_readonly(database_path, &server);
  UmiUfmpOutboxPage page;
  if (s == UMI_STATUS_OK) s = UmiUfmpOutboxReadPage(server, after, UMI_UFMP_OUTBOX_PAGE_CAPACITY, &page);
  umi_data_server_destroy(server);
  if (s != UMI_STATUS_OK) return s;
  char text[16384];
  int n = snprintf(text, sizeof text, "Outgoing delivery review (read-only)\nRecords: %zu\n", page.count);
  if (n < 0 || (size_t)n >= sizeof text) return UMI_STATUS_INTERNAL_ERROR;
  size_t used = (size_t)n;
  for (size_t i = 0U; i < page.count; ++i) {
    const UmiUfmpOutboxSummary *r = &page.items[i];
    n = snprintf(text + used, sizeof text - used,
      "%s | %s | %s -> %s | %s | revision=%" PRIu64 " attempts=%" PRIu32 " due=%" PRIu64 "\n",
      r->id, UmiUfmpMessageTypeName(r->type), r->sender, r->recipient,
      UmiUfmpOutboxStateName(r->state), r->revision, r->attempts, r->next_attempt_ms);
    if (n < 0 || (size_t)n >= sizeof text - used) return UMI_STATUS_CAPACITY_EXCEEDED;
    used += (size_t)n;
  }
  n = snprintf(text + used, sizeof text - used,
    "More: %s\nNext cursor: %s\nDelivery is not payment settlement.\n",
    page.has_more ? "yes" : "no", page.next_cursor);
  if (n < 0 || (size_t)n >= sizeof text - used) return UMI_STATUS_CAPACITY_EXCEEDED;
  used += (size_t)n; *required = used + 1U;
  if (capacity < *required) return UMI_STATUS_CAPACITY_EXCEEDED;
  memcpy(out, text, *required); return UMI_STATUS_OK;
}

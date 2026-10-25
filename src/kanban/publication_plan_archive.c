/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/kanban/publication_plan_archive.c
 *
 * PURPOSE:
 *   Serialize only reviewed fields with bounded heap-owned decoding and complete-envelope
 * admission.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "../base/value_archive_internal.h"
#include "publication_plan_internal.h"
#include <stdlib.h>
#define PLAN_SCHEMA UINT64_C(0x554d494b50554201)
static void Fields(UmiArchiveWriter *writer, const UmiKanbanPublicationPlan *plan) {
  UmiArchiveWriteText(writer, plan->info.board_id, sizeof plan->info.board_id);
  UmiArchiveWriteText(writer, plan->info.title, sizeof plan->info.title);
  UmiArchiveWriteUnsigned(writer, plan->info.reviewed_revision);
  UmiArchiveWriteUnsigned(writer, plan->info.card_count);
  for (size_t i = 0U; i < plan->info.card_count; ++i) {
    const UmiKanbanPublicCard *c = &plan->cards[i];
    UmiArchiveWriteText(writer, c->card_id, sizeof c->card_id);
    UmiArchiveWriteText(writer, c->title, sizeof c->title);
    UmiArchiveWriteText(writer, c->summary, sizeof c->summary);
    UmiArchiveWriteText(writer, c->developer, sizeof c->developer);
  }
}
UmiStatus UmiKanbanPublicationPlanEncode(const UmiKanbanPublicationPlan *plan, void *bytes,
                                         size_t capacity, size_t *out_size) {
  if (out_size == NULL || (bytes == NULL && capacity != 0U))
    return UMI_STATUS_INVALID_ARGUMENT;
  UmiStatus status = UmiKanbanPublicationPlanValidate(plan);
  if (status != UMI_STATUS_OK)
    return status;
  UmiArchiveWriter measure = {NULL, UMI_KANBAN_PUBLIC_PLAN_MAXIMUM_BYTES,
                              UMI_VALUE_ARCHIVE_HEADER_SIZE, UMI_STATUS_OK};
  Fields(&measure, plan);
  if (measure.status != UMI_STATUS_OK)
    return measure.status;
  *out_size = measure.offset;
  if (bytes == NULL)
    return UMI_STATUS_OK;
  if (capacity < measure.offset)
    return UMI_STATUS_CAPACITY_EXCEEDED;
  UmiArchiveWriter writer = {bytes, capacity, UMI_VALUE_ARCHIVE_HEADER_SIZE, UMI_STATUS_OK};
  Fields(&writer, plan);
  if (writer.status != UMI_STATUS_OK)
    return writer.status;
  UmiArchiveFinish(writer.bytes, writer.offset, PLAN_SCHEMA);
  return UMI_STATUS_OK;
}
UmiStatus UmiKanbanPublicationPlanDecode(const void *bytes, size_t size,
                                         UmiKanbanPublicationPlan **out) {
  if (out == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  *out = NULL;
  if (bytes == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  if (size > UMI_KANBAN_PUBLIC_PLAN_MAXIMUM_BYTES)
    return UMI_STATUS_PARSE_ERROR;
  UmiValueArchiveInfo envelope;
  UmiStatus status = umi_value_archive_inspect(bytes, size, &envelope);
  if (status != UMI_STATUS_OK)
    return status;
  if (envelope.schema != PLAN_SCHEMA)
    return UMI_STATUS_UNAVAILABLE;
  UmiKanbanPublicationPlan *plan = calloc(1U, sizeof *plan);
  if (plan == NULL)
    return UMI_STATUS_OUT_OF_MEMORY;
  UmiArchiveReader reader = {bytes, size, UMI_VALUE_ARCHIVE_HEADER_SIZE, UMI_STATUS_OK};
  UmiArchiveReadText(&reader, plan->info.board_id, sizeof plan->info.board_id);
  UmiArchiveReadText(&reader, plan->info.title, sizeof plan->info.title);
  plan->info.reviewed_revision = UmiArchiveReadUnsigned(&reader, UINT64_MAX);
  plan->info.card_count = (size_t)UmiArchiveReadUnsigned(&reader, UMI_KANBAN_MAX_CARDS);
  if (reader.status == UMI_STATUS_OK && plan->info.card_count != 0U) {
    plan->cards = calloc(plan->info.card_count, sizeof *plan->cards);
    if (plan->cards == NULL)
      reader.status = UMI_STATUS_OUT_OF_MEMORY;
  }
  for (size_t i = 0U; reader.status == UMI_STATUS_OK && i < plan->info.card_count; ++i) {
    UmiKanbanPublicCard *c = &plan->cards[i];
    UmiArchiveReadText(&reader, c->card_id, sizeof c->card_id);
    UmiArchiveReadText(&reader, c->title, sizeof c->title);
    UmiArchiveReadText(&reader, c->summary, sizeof c->summary);
    UmiArchiveReadText(&reader, c->developer, sizeof c->developer);
  }
  status = reader.status;
  if (status == UMI_STATUS_OK && reader.offset != reader.size)
    status = UMI_STATUS_PARSE_ERROR;
  if (status == UMI_STATUS_OK)
    status = UmiKanbanPublicationPlanValidate(plan);
  if (status != UMI_STATUS_OK) {
    UmiKanbanPublicationPlanDestroy(plan);
    return status;
  }
  *out = plan;
  return UMI_STATUS_OK;
}

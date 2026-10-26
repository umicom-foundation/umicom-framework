/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/kanban/completion_archive.c
 *
 * PURPOSE:
 *   Retain private completion decisions in bounded, independently owned archives.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "completion_plan_internal.h"
#include "../base/value_archive_internal.h"
#include <stdlib.h>
#define COMPLETION_SCHEMA UINT64_C(0x554d494b434d5001)

static void Fields(UmiArchiveWriter *writer, const UmiKanbanCompletionPlan *plan)
{
    UmiArchiveWriteText(writer, plan->info.board_id, sizeof plan->info.board_id);
    UmiArchiveWriteText(writer, plan->info.done_column, sizeof plan->info.done_column);
    UmiArchiveWriteText(writer, plan->info.reviewer, sizeof plan->info.reviewer);
    UmiArchiveWriteText(writer, plan->info.source_identity, sizeof plan->info.source_identity);
    UmiArchiveWriteText(writer, plan->info.configuration, sizeof plan->info.configuration);
    UmiArchiveWriteText(writer, plan->info.board_sha256, sizeof plan->info.board_sha256);
    UmiArchiveWriteUnsigned(writer, plan->info.reviewed_revision);
    UmiArchiveWriteUnsigned(writer, plan->info.card_count);
    for (size_t i = 0U; i < plan->info.card_count; ++i) {
        const UmiKanbanCompletionRecord *r = &plan->records[i];
        UmiArchiveWriteText(writer, r->card_id, sizeof r->card_id);
        UmiArchiveWriteUnsigned(writer, (uint64_t)r->decision);
        UmiArchiveWriteText(writer, r->evidence_sha256, sizeof r->evidence_sha256);
        UmiArchiveWriteText(writer, r->rationale, sizeof r->rationale);
    }
}
UmiStatus UmiKanbanCompletionPlanEncode(const UmiKanbanCompletionPlan *plan,
    void *bytes, size_t capacity, size_t *out_size)
{
    if (!out_size || (!bytes && capacity != 0U)) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = UmiKanbanCompletionValidate(plan);
    if (status != UMI_STATUS_OK) return status;
    UmiArchiveWriter measure = {NULL, UMI_KANBAN_COMPLETION_MAXIMUM_BYTES,
        UMI_VALUE_ARCHIVE_HEADER_SIZE, UMI_STATUS_OK};
    Fields(&measure, plan);
    if (measure.status != UMI_STATUS_OK) return measure.status;
    *out_size = measure.offset;
    if (!bytes) return UMI_STATUS_OK;
    if (capacity < measure.offset) return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiArchiveWriter writer = {bytes, capacity, UMI_VALUE_ARCHIVE_HEADER_SIZE, UMI_STATUS_OK};
    Fields(&writer, plan);
    if (writer.status != UMI_STATUS_OK) return writer.status;
    UmiArchiveFinish(writer.bytes, writer.offset, COMPLETION_SCHEMA);
    return UMI_STATUS_OK;
}
UmiStatus UmiKanbanCompletionPlanDecode(const void *bytes, size_t size,
    UmiKanbanCompletionPlan **out)
{
    if (!out) return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    if (!bytes) return UMI_STATUS_INVALID_ARGUMENT;
    if (size > UMI_KANBAN_COMPLETION_MAXIMUM_BYTES) return UMI_STATUS_PARSE_ERROR;
    UmiValueArchiveInfo envelope;
    UmiStatus status = umi_value_archive_inspect(bytes, size, &envelope);
    if (status != UMI_STATUS_OK) return status;
    if (envelope.schema != COMPLETION_SCHEMA) return UMI_STATUS_UNAVAILABLE;
    UmiKanbanCompletionPlan *plan = calloc(1U, sizeof *plan);
    if (!plan) return UMI_STATUS_OUT_OF_MEMORY;
    UmiArchiveReader reader = {bytes, size, UMI_VALUE_ARCHIVE_HEADER_SIZE, UMI_STATUS_OK};
    UmiArchiveReadText(&reader, plan->info.board_id, sizeof plan->info.board_id);
    UmiArchiveReadText(&reader, plan->info.done_column, sizeof plan->info.done_column);
    UmiArchiveReadText(&reader, plan->info.reviewer, sizeof plan->info.reviewer);
    UmiArchiveReadText(&reader, plan->info.source_identity, sizeof plan->info.source_identity);
    UmiArchiveReadText(&reader, plan->info.configuration, sizeof plan->info.configuration);
    UmiArchiveReadText(&reader, plan->info.board_sha256, sizeof plan->info.board_sha256);
    plan->info.reviewed_revision = UmiArchiveReadUnsigned(&reader, UINT64_MAX);
    plan->info.card_count = (size_t)UmiArchiveReadUnsigned(&reader, UMI_KANBAN_MAX_EDITS);
    if (reader.status == UMI_STATUS_OK && plan->info.card_count != 0U) {
        plan->records = calloc(plan->info.card_count, sizeof *plan->records);
        if (!plan->records) reader.status = UMI_STATUS_OUT_OF_MEMORY;
    }
    for (size_t i = 0U; reader.status == UMI_STATUS_OK && i < plan->info.card_count; ++i) {
        UmiKanbanCompletionRecord *r = &plan->records[i];
        UmiArchiveReadText(&reader, r->card_id, sizeof r->card_id);
        r->decision = (UmiKanbanCompletionDecision)UmiArchiveReadUnsigned(&reader, UMI_KANBAN_COMPLETION_EXCEPTION);
        UmiArchiveReadText(&reader, r->evidence_sha256, sizeof r->evidence_sha256);
        UmiArchiveReadText(&reader, r->rationale, sizeof r->rationale);
    }
    status = reader.status;
    if (status == UMI_STATUS_OK && reader.offset != reader.size) status = UMI_STATUS_PARSE_ERROR;
    if (status == UMI_STATUS_OK) status = UmiKanbanCompletionValidate(plan);
    if (status != UMI_STATUS_OK) { UmiKanbanCompletionPlanDestroy(plan); return status; }
    *out = plan;
    return UMI_STATUS_OK;
}

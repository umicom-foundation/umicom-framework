/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/enterprise_workspace/recovery_report.c
 * PURPOSE: Explain the old and newly calculated import without treating inspection as approval.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/enterprise_workspace/recovery.h"
#include <inttypes.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct Writer { char *data; size_t capacity, used; UmiStatus status; } Writer;
static void Append(Writer *writer, const char *format, ...)
{
    if (writer->status != UMI_STATUS_OK) return;
    va_list args; va_start(args, format);
    int count = vsnprintf(writer->data + writer->used, writer->capacity - writer->used, format, args);
    va_end(args);
    if (count < 0) writer->status = UMI_STATUS_IO_ERROR;
    else if ((size_t)count >= writer->capacity - writer->used) writer->status = UMI_STATUS_CAPACITY_EXCEEDED;
    else writer->used += (size_t)count;
}
UmiStatus UmiEnterpriseRecoveryFormat(const UmiEnterpriseRecoveryReview *review,
    char *buffer, size_t capacity)
{
    if (buffer != NULL && capacity != 0U) buffer[0] = '\0';
    if (review == NULL || buffer == NULL || capacity == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    UmiEnterpriseRecoveryInfo *info = malloc(sizeof(*info));
    if (info == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    UmiStatus status = UmiEnterpriseRecoveryDescribe(review, info);
    Writer writer = {buffer, capacity, 0U, status};
    if (status == UMI_STATUS_OK) {
        const UmiEnterprisePreview *preview = &info->proposedPreview;
        Append(&writer, "Original job: %s (%s)\nDataset: %s | recipe: %s\n"
            "Original generation: %" PRIu64 " | inspected generation: %" PRIu64 "\n"
            "Inspected workspace revision: %" PRIu64 " | preparer: %s\n"
            "Execution paused: %s\nOriginal job and approvals remain unchanged.\n"
            "Preparing creates a NEW job requiring a NEW review; this report applies no rows.\n\n",
            info->originalJob.id, UmiEnterpriseJobStateText(info->originalJob.state),
            preview->datasetId, preview->recipeId, info->originalJob.datasetGeneration,
            preview->datasetGeneration, preview->workspaceRevision, info->preparer,
            info->executionPaused ? "yes" : "no");
        Append(&writer, "Original plan: %zu insert, %zu update, %zu unchanged\n"
            "Current proposal: %zu insert, %zu update, %zu unchanged\n\n",
            info->originalPreview.insertCount, info->originalPreview.updateCount,
            info->originalPreview.unchangedCount, preview->insertCount,
            preview->updateCount, preview->unchangedCount);
        for (size_t i = 0U; i < preview->rowCount; ++i) {
            const UmiEnterpriseChange *row = &preview->changes[i];
            Append(&writer, "%s [%s]\n", row->after.id,
                row->kind == UMI_ENTERPRISE_INSERT ? "INSERT" : row->kind == UMI_ENTERPRISE_UPDATE ? "UPDATE" : "UNCHANGED");
            if (row->kind != UMI_ENTERPRISE_INSERT)
                Append(&writer, "Current: %s | quantity %" PRIu64 " | source job %s\n",
                    row->before.label, row->before.quantity, row->before.sourceJob);
            Append(&writer, "Proposed: %s | quantity %" PRIu64 "\n\n", row->after.label, row->after.quantity);
        }
    }
    free(info);
    if (writer.status != UMI_STATUS_OK) buffer[0] = '\0';
    return writer.status;
}

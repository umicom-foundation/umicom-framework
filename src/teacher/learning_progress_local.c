/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/teacher/learning_progress_local.c
 *
 * PURPOSE:
 *   Execute one checkpoint request without sharing a live storage connection between workers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "learning_progress_internal.h"
#include "umicom/teacher/learning_progress_local.h"
#include <string.h>

UmiStatus UmiLearningProgressLocalValidate(const UmiLearningProgressLocalRequest *request)
{
    if (request == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (request->action != UMI_LEARNING_PROGRESS_INSPECT && request->action != UMI_LEARNING_PROGRESS_RECORD)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(request->database_path, '\0', sizeof(request->database_path)) == NULL)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    if (!umi_path_is_absolute(request->database_path)) return UMI_STATUS_INVALID_ARGUMENT;
    for (size_t i = 0U; request->database_path[i] != '\0'; ++i) {
        unsigned char value = (unsigned char)request->database_path[i];
        if (value < 0x20U || value == 0x7fU) return UMI_STATUS_INVALID_ARGUMENT;
    }
    UmiStatus status = LpToken(request->learner_id);
    if (status == UMI_STATUS_OK) status = LpToken(request->checkpoint);
    if (status != UMI_STATUS_OK) return status;
    if (request->action == UMI_LEARNING_PROGRESS_RECORD) return LpToken(request->evidence_reference);
    return request->evidence_reference[0] == '\0' ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UmiStatus UmiLearningProgressLocalExecute(const UmiLearningReader *reader,
    const UmiLearningProgressLocalRequest *request, UmiLearningProgressLocalReceipt *out)
{
    if (reader == NULL || out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = UmiLearningProgressLocalValidate(request);
    if (status != UMI_STATUS_OK) return status;
    UmiLearningReaderSnapshot selected;
    status = UmiLearningReaderCopySnapshot(reader, &selected);
    if (status != UMI_STATUS_OK) return status;
    if (!selected.has_document || (selected.selection.resource.kind != UMI_LEARNING_RESOURCE_GUIDE &&
        selected.selection.resource.kind != UMI_LEARNING_RESOURCE_LESSON)) return UMI_STATUS_INVALID_STATE;
    UmiLearningProgressLocalReceipt receipt = {0};
    receipt.request = *request;
    receipt.selection = selected;
    UmiDataServer *server = NULL;
    UmiLearningProgress *progress = NULL;
    status = request->action == UMI_LEARNING_PROGRESS_INSPECT ?
        umi_data_server_open_sqlite_readonly(request->database_path, &server) :
        umi_data_server_create_sqlite(request->database_path, &server);
    if (status == UMI_STATUS_OK) {
        receipt.storage_opened = 1;
        status = UmiLearningProgressOpen(server, request->learner_id, &progress);
    }
    if (status == UMI_STATUS_OK) {
        receipt.journal_loaded = 1;
        if (request->action == UMI_LEARNING_PROGRESS_RECORD) {
            receipt.write_attempted = 1;
            status = UmiLearningProgressRecord(progress, reader, request->checkpoint,
                request->evidence_reference, &receipt.event);
        } else status = UmiLearningProgressFindCurrent(progress, reader, request->checkpoint, &receipt.event);
        receipt.has_event = status == UMI_STATUS_OK;
        /* Copy the last known journal even after a failed write, including its
         * uncertainty flag. Never relabel a refusal as an empty successful read. */
        UmiStatus copied = UmiLearningProgressCopySnapshot(progress, &receipt.journal);
        if (copied != UMI_STATUS_OK && status == UMI_STATUS_OK) status = copied;
    }
    receipt.status = status;
    UmiLearningProgressClose(progress);
    umi_data_server_destroy(server);
    *out = receipt;
    return status;
}

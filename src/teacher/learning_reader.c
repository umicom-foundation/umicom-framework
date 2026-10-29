/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/teacher/learning_reader.c
 *
 * PURPOSE:
 *   Commit lesson navigation only after the requested bytes pass verification.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/teacher/learning_reader.h"
#include <stdlib.h>
#include <string.h>

typedef struct LearningHistoryEntry {
    char resource_id[UMI_TEACHER_ID_CAPACITY];
    char fragment[UMI_LEARNING_FRAGMENT_CAPACITY];
} LearningHistoryEntry;
struct UmiLearningReader {
    UmiLearningPack *pack;
    size_t maximum_bytes;
    unsigned char *content;
    UmiLearningReaderSnapshot snapshot;
    LearningHistoryEntry history[UMI_LEARNING_READER_HISTORY_CAPACITY];
};

UmiStatus UmiLearningReaderCreate(UmiLearningPack **pack, size_t maximum,
    UmiLearningReader **out)
{
    if (out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    if (pack == NULL || *pack == NULL || maximum == 0U || maximum > UMI_LEARNING_READER_MAX_BYTES)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiLearningReader *reader = calloc(1U, sizeof(*reader));
    if (reader == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    UmiStatus status = UmiLearningPackCopyInfo(*pack, &reader->snapshot.pack);
    if (status != UMI_STATUS_OK) {
        free(reader);
        return status;
    }
    reader->pack = *pack;
    reader->maximum_bytes = maximum;
    *pack = NULL;
    *out = reader;
    return UMI_STATUS_OK;
}
void UmiLearningReaderDestroy(UmiLearningReader *reader)
{
    if (reader == NULL) return;
    UmiLearningPackFreeBytes(reader->content);
    UmiLearningPackDestroy(reader->pack);
    free(reader);
}
static void ReaderHistoryState(UmiLearningReader *reader)
{
    reader->snapshot.can_go_back = reader->snapshot.history_index != 0U;
    reader->snapshot.can_go_forward = reader->snapshot.history_count != 0U &&
        reader->snapshot.history_index + 1U < reader->snapshot.history_count;
}
/* An existing history destination is selected only after its content is read.
 * Source changes or missing files therefore cannot move the navigation cursor
 * while leaving an unrelated old document on screen. */
static UmiStatus ReaderCommit(UmiLearningReader *reader, const UmiLearningPackLink *selection,
    int history_move, size_t destination)
{
    if (reader->snapshot.revision == UINT64_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    unsigned char *bytes = NULL;
    size_t size = 0U;
    UmiStatus status = UmiLearningPackReadResource(reader->pack, selection->resource.id,
        reader->maximum_bytes, &bytes, &size);
    if (status != UMI_STATUS_OK) return status;
    if (history_move) {
        reader->snapshot.history_index = destination;
    } else {
        int same = reader->snapshot.has_document &&
            strcmp(reader->snapshot.selection.resource.id, selection->resource.id) == 0 &&
            strcmp(reader->snapshot.selection.fragment, selection->fragment) == 0;
        if (!same) {
            size_t count = reader->snapshot.has_document ? reader->snapshot.history_index + 1U : 0U;
            if (count == UMI_LEARNING_READER_HISTORY_CAPACITY) {
                memmove(reader->history, reader->history + 1U,
                    (UMI_LEARNING_READER_HISTORY_CAPACITY - 1U) * sizeof(reader->history[0]));
                --count;
            }
            LearningHistoryEntry *entry = &reader->history[count];
            memcpy(entry->resource_id, selection->resource.id, sizeof(entry->resource_id));
            memcpy(entry->fragment, selection->fragment, sizeof(entry->fragment));
            reader->snapshot.history_index = count;
            reader->snapshot.history_count = count + 1U;
        }
    }
    UmiLearningPackFreeBytes(reader->content);
    reader->content = bytes;
    reader->snapshot.selection = *selection;
    reader->snapshot.content_size = size;
    reader->snapshot.has_document = 1;
    ++reader->snapshot.revision;
    ReaderHistoryState(reader);
    return UMI_STATUS_OK;
}
UmiStatus UmiLearningReaderOpen(UmiLearningReader *reader, const char *id)
{
    if (reader == NULL || id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiLearningPackLink selection = {0};
    UmiStatus status = UmiLearningPackFindResource(reader->pack, id, &selection.resource);
    return status == UMI_STATUS_OK ? ReaderCommit(reader, &selection, 0, 0U) : status;
}
UmiStatus UmiLearningReaderFollow(UmiLearningReader *reader, const char *link)
{
    if (reader == NULL || link == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (!reader->snapshot.has_document) return UMI_STATUS_INVALID_STATE;
    UmiLearningPackLink selection = {0};
    UmiStatus status = UmiLearningPackResolveLink(reader->pack,
        reader->snapshot.selection.resource.id, link, &selection);
    return status == UMI_STATUS_OK ? ReaderCommit(reader, &selection, 0, 0U) : status;
}
static UmiStatus ReaderMove(UmiLearningReader *reader, size_t destination)
{
    const LearningHistoryEntry *entry = &reader->history[destination];
    UmiLearningPackLink selection = {0};
    UmiStatus status = UmiLearningPackFindResource(reader->pack, entry->resource_id, &selection.resource);
    if (status != UMI_STATUS_OK) return status;
    memcpy(selection.fragment, entry->fragment, sizeof(selection.fragment));
    return ReaderCommit(reader, &selection, 1, destination);
}
UmiStatus UmiLearningReaderBack(UmiLearningReader *reader)
{
    if (reader == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (!reader->snapshot.can_go_back) return UMI_STATUS_NOT_FOUND;
    return ReaderMove(reader, reader->snapshot.history_index - 1U);
}
UmiStatus UmiLearningReaderForward(UmiLearningReader *reader)
{
    if (reader == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (!reader->snapshot.can_go_forward) return UMI_STATUS_NOT_FOUND;
    return ReaderMove(reader, reader->snapshot.history_index + 1U);
}
UmiStatus UmiLearningReaderReload(UmiLearningReader *reader)
{
    if (reader == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (!reader->snapshot.has_document) return UMI_STATUS_INVALID_STATE;
    return ReaderMove(reader, reader->snapshot.history_index);
}
UmiStatus UmiLearningReaderCopySnapshot(const UmiLearningReader *reader, UmiLearningReaderSnapshot *out)
{
    if (reader == NULL || out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out = reader->snapshot;
    return UMI_STATUS_OK;
}
UmiStatus UmiLearningReaderCopyContent(const UmiLearningReader *reader,
    void *out, size_t capacity, size_t *out_size)
{
    if (out_size == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_size = 0U;
    if (reader == NULL || (out == NULL && capacity != 0U)) return UMI_STATUS_INVALID_ARGUMENT;
    if (!reader->snapshot.has_document) return UMI_STATUS_INVALID_STATE;
    *out_size = reader->snapshot.content_size;
    if (out == NULL) return UMI_STATUS_OK;
    if (capacity <= reader->snapshot.content_size) return UMI_STATUS_CAPACITY_EXCEEDED;
    memcpy(out, reader->content, reader->snapshot.content_size + 1U);
    return UMI_STATUS_OK;
}

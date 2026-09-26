/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/enterprise_workspace/wire.c
 *
 * PURPOSE:
 *   Encode explicit fields in a bounded, portable text snapshot; reject malformed input.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "internal.h"
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Writer { char *text; size_t used; UmiStatus status; } Writer;
typedef struct Reader { const char *text; size_t size, offset; UmiStatus status; } Reader;
static void WriteNumber(Writer *writer, uint64_t value)
{
    char text[32]; int n;
    if (writer->status != UMI_STATUS_OK) return;
    n = snprintf(text, sizeof(text), "%" PRIu64 "\n", value);
    if (n < 0 || (size_t)n >= sizeof(text) || (size_t)n >= EWS_WIRE_CAPACITY - writer->used) { writer->status = UMI_STATUS_CAPACITY_EXCEEDED; return; }
    (void)memcpy(writer->text + writer->used, text, (size_t)n); writer->used += (size_t)n;
}
static void WriteText(Writer *writer, const char *text)
{
    static const char hex[] = "0123456789abcdef";
    size_t length;
    if (writer->status != UMI_STATUS_OK) return;
    length = strlen(text);
    if (writer->used > EWS_WIRE_CAPACITY - 2U || length > (EWS_WIRE_CAPACITY - writer->used - 2U) / 2U) { writer->status = UMI_STATUS_CAPACITY_EXCEEDED; return; }
    for (size_t i = 0U; i < length; ++i) {
        unsigned char ch = (unsigned char)text[i];
        writer->text[writer->used++] = hex[ch >> 4U]; writer->text[writer->used++] = hex[ch & 15U];
    }
    writer->text[writer->used++] = '\n';
}
static uint64_t ReadNumber(Reader *reader, uint64_t maximum)
{
    uint64_t value = 0U;
    size_t start = reader->offset;
    if (reader->status != UMI_STATUS_OK) return 0U;
    while (reader->offset < reader->size && reader->text[reader->offset] != '\n') {
        unsigned char ch = (unsigned char)reader->text[reader->offset++];
        if (ch < '0' || ch > '9' || (uint64_t)(ch - '0') > maximum || value > (maximum - (ch - '0')) / 10U) { reader->status = UMI_STATUS_PARSE_ERROR; return 0U; }
        value = value * 10U + (ch - '0');
    }
    if (reader->offset == start || reader->offset == reader->size ||
        (reader->offset - start > 1U && reader->text[start] == '0')) { reader->status = UMI_STATUS_PARSE_ERROR; return 0U; }
    ++reader->offset;
    return value;
}
static int Hex(char ch)
{
    if (ch >= '0' && ch <= '9') return ch - '0';
    if (ch >= 'a' && ch <= 'f') return ch - 'a' + 10;
    return -1;
}
static void ReadText(Reader *reader, char *out, size_t capacity)
{
    size_t used = 0U;
    if (reader->status != UMI_STATUS_OK) return;
    while (reader->offset < reader->size && reader->text[reader->offset] != '\n') {
        int high, low;
        if (reader->size - reader->offset < 2U || used + 1U >= capacity) { reader->status = UMI_STATUS_PARSE_ERROR; return; }
        high = Hex(reader->text[reader->offset++]); low = Hex(reader->text[reader->offset++]);
        if (high < 0 || low < 0 || (high == 0 && low == 0)) { reader->status = UMI_STATUS_PARSE_ERROR; return; }
        out[used++] = (char)((unsigned)high * 16U + (unsigned)low);
    }
    if (reader->offset == reader->size) { reader->status = UMI_STATUS_PARSE_ERROR; return; }
    ++reader->offset; out[used] = '\0';
}

static void WriteRow(Writer *wire, const UmiEnterpriseRow *row)
{
    WriteText(wire, row->id);
    WriteText(wire, row->label);
    WriteNumber(wire, row->quantity);
    WriteText(wire, row->sourceJob);
}

static void ReadRow(Reader *wire, UmiEnterpriseRow *row)
{
    ReadText(wire, row->id, sizeof(row->id));
    ReadText(wire, row->label, sizeof(row->label));
    row->quantity = ReadNumber(wire, INT64_MAX);
    ReadText(wire, row->sourceJob, sizeof(row->sourceJob));
}

UmiStatus EwsEncode(const EwsState *state, char **outText, size_t *outLength)
{
    Writer writer = {0};
    Writer *wire = &writer;
    if (outText == NULL || outLength == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *outText = NULL; *outLength = 0U;
    if (EwsValidate(state) != UMI_STATUS_OK) return UMI_STATUS_PARSE_ERROR;
    wire->text = malloc(EWS_WIRE_CAPACITY);
    if (wire->text == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    WriteNumber(wire, 1U);
    WriteNumber(wire, state->revision);
    WriteNumber(wire, state->paused);
    WriteNumber(wire, state->recipes[0]);
    WriteNumber(wire, state->recipes[1]);
    WriteNumber(wire, state->datasetCount);
    WriteNumber(wire, state->jobCount);
    WriteNumber(wire, state->auditCount);
    for (size_t i = 0U; i < state->datasetCount; ++i) {
        const EwsDataset *dataset = &state->datasets[i];
        WriteText(wire, dataset->info.id);
        WriteText(wire, dataset->info.name);
        WriteNumber(wire, dataset->info.generation);
        WriteNumber(wire, dataset->info.rowCount);
        for (size_t j = 0U; j < dataset->info.rowCount; ++j) WriteRow(wire, &dataset->rows[j]);
    }
    for (size_t i = 0U; i < state->jobCount; ++i) {
        const EwsJob *job = &state->jobs[i];
        WriteText(wire, job->info.id);
        WriteText(wire, job->info.datasetId);
        WriteText(wire, job->info.recipeId);
        WriteText(wire, job->info.author);
        WriteText(wire, job->info.reviewer);
        WriteText(wire, job->info.executor);
        WriteText(wire, job->info.reason);
        WriteNumber(wire, (unsigned)job->info.state);
        WriteNumber(wire, job->info.datasetGeneration);
        WriteNumber(wire, job->info.preparedRevision);
        WriteNumber(wire, job->info.decisionRevision);
        WriteNumber(wire, job->info.appliedRevision);
        WriteNumber(wire, job->info.rowCount);
        WriteNumber(wire, job->info.insertCount);
        WriteNumber(wire, job->info.updateCount);
        WriteNumber(wire, job->info.unchangedCount);
        WriteText(wire, job->preview.datasetId);
        WriteText(wire, job->preview.recipeId);
        WriteNumber(wire, job->preview.datasetGeneration);
        WriteNumber(wire, job->preview.workspaceRevision);
        WriteNumber(wire, job->preview.rowCount);
        WriteNumber(wire, job->preview.insertCount);
        WriteNumber(wire, job->preview.updateCount);
        WriteNumber(wire, job->preview.unchangedCount);
        for (size_t j = 0U; j < job->preview.rowCount; ++j) {
            WriteNumber(wire, (unsigned)job->preview.changes[j].kind);
            WriteRow(wire, &job->preview.changes[j].before);
            WriteRow(wire, &job->preview.changes[j].after);
        }
        WriteText(wire, job->csv);
    }
    for (size_t i = 0U; i < state->auditCount; ++i) {
        WriteNumber(wire, state->audit[i].revision);
        WriteText(wire, state->audit[i].principal);
        WriteText(wire, state->audit[i].action);
        WriteText(wire, state->audit[i].target);
        WriteText(wire, state->audit[i].detail);
    }
    if (wire->status != UMI_STATUS_OK) { free(wire->text); return wire->status; }
    wire->text[wire->used] = '\0'; *outText = wire->text; *outLength = wire->used; return UMI_STATUS_OK;
}

UmiStatus EwsDecode(const char *text, size_t length, EwsState *state)
{
    Reader reader = {text, length, 0U, UMI_STATUS_OK};
    Reader *wire = &reader;
    if (text == NULL || state == NULL || length >= EWS_WIRE_CAPACITY) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(state, 0, sizeof(*state));
    if (ReadNumber(wire, 1U) != 1U) return UMI_STATUS_PARSE_ERROR;
    state->revision = ReadNumber(wire, UINT64_MAX);
    state->paused = ReadNumber(wire, 1U);
    state->recipes[0] = ReadNumber(wire, 1U);
    state->recipes[1] = ReadNumber(wire, 1U);
    state->datasetCount = (size_t)ReadNumber(wire, UMI_ENTERPRISE_MAX_DATASETS);
    state->jobCount = (size_t)ReadNumber(wire, UMI_ENTERPRISE_MAX_JOBS);
    state->auditCount = (size_t)ReadNumber(wire, UMI_ENTERPRISE_MAX_AUDIT);
    for (size_t i = 0U; i < state->datasetCount; ++i) {
        EwsDataset *dataset = &state->datasets[i];
        ReadText(wire, dataset->info.id, sizeof(dataset->info.id));
        ReadText(wire, dataset->info.name, sizeof(dataset->info.name));
        dataset->info.generation = ReadNumber(wire, UINT64_MAX);
        dataset->info.rowCount = (size_t)ReadNumber(wire, UMI_ENTERPRISE_MAX_ROWS);
        for (size_t j = 0U; j < dataset->info.rowCount; ++j) ReadRow(wire, &dataset->rows[j]);
    }
    for (size_t i = 0U; i < state->jobCount; ++i) {
        EwsJob *job = &state->jobs[i];
        ReadText(wire, job->info.id, sizeof(job->info.id));
        ReadText(wire, job->info.datasetId, sizeof(job->info.datasetId));
        ReadText(wire, job->info.recipeId, sizeof(job->info.recipeId));
        ReadText(wire, job->info.author, sizeof(job->info.author));
        ReadText(wire, job->info.reviewer, sizeof(job->info.reviewer));
        ReadText(wire, job->info.executor, sizeof(job->info.executor));
        ReadText(wire, job->info.reason, sizeof(job->info.reason));
        job->info.state = (UmiEnterpriseJobState)ReadNumber(wire, UMI_ENTERPRISE_JOB_CANCELLED);
        job->info.datasetGeneration = ReadNumber(wire, UINT64_MAX);
        job->info.preparedRevision = ReadNumber(wire, UINT64_MAX);
        job->info.decisionRevision = ReadNumber(wire, UINT64_MAX);
        job->info.appliedRevision = ReadNumber(wire, UINT64_MAX);
        job->info.rowCount = (size_t)ReadNumber(wire, UMI_ENTERPRISE_MAX_IMPORT_ROWS);
        job->info.insertCount = (size_t)ReadNumber(wire, UMI_ENTERPRISE_MAX_IMPORT_ROWS);
        job->info.updateCount = (size_t)ReadNumber(wire, UMI_ENTERPRISE_MAX_IMPORT_ROWS);
        job->info.unchangedCount = (size_t)ReadNumber(wire, UMI_ENTERPRISE_MAX_IMPORT_ROWS);
        ReadText(wire, job->preview.datasetId, sizeof(job->preview.datasetId));
        ReadText(wire, job->preview.recipeId, sizeof(job->preview.recipeId));
        job->preview.datasetGeneration = ReadNumber(wire, UINT64_MAX);
        job->preview.workspaceRevision = ReadNumber(wire, UINT64_MAX);
        job->preview.rowCount = (size_t)ReadNumber(wire, UMI_ENTERPRISE_MAX_IMPORT_ROWS);
        job->preview.insertCount = (size_t)ReadNumber(wire, UMI_ENTERPRISE_MAX_IMPORT_ROWS);
        job->preview.updateCount = (size_t)ReadNumber(wire, UMI_ENTERPRISE_MAX_IMPORT_ROWS);
        job->preview.unchangedCount = (size_t)ReadNumber(wire, UMI_ENTERPRISE_MAX_IMPORT_ROWS);
        for (size_t j = 0U; j < job->preview.rowCount; ++j) {
            job->preview.changes[j].kind = (UmiEnterpriseChangeKind)ReadNumber(wire, UMI_ENTERPRISE_UNCHANGED);
            ReadRow(wire, &job->preview.changes[j].before);
            ReadRow(wire, &job->preview.changes[j].after);
        }
        ReadText(wire, job->csv, sizeof(job->csv));
    }
    for (size_t i = 0U; i < state->auditCount; ++i) {
        state->audit[i].revision = ReadNumber(wire, UINT64_MAX);
        ReadText(wire, state->audit[i].principal, sizeof(state->audit[i].principal));
        ReadText(wire, state->audit[i].action, sizeof(state->audit[i].action));
        ReadText(wire, state->audit[i].target, sizeof(state->audit[i].target));
        ReadText(wire, state->audit[i].detail, sizeof(state->audit[i].detail));
    }
    if (wire->status != UMI_STATUS_OK || wire->offset != length) return UMI_STATUS_PARSE_ERROR;
    return EwsValidate(state);
}

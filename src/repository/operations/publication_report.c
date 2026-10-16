/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/repository/operations/publication_report.c
 * PURPOSE: Share publication evidence formatting between native commands and application hosts.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/repository/publication_report.h"
#include "umicom/platform/filesystem.h"
#include "umicom/protocol/json.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* JSON needs Unicode text even on platforms whose filesystem allows arbitrary
 * path bytes. Reject malformed fields instead of silently changing their names. */
static int valid_text(const char *text, size_t capacity)
{
    const unsigned char *bytes = (const unsigned char *)text;
    const char *end = memchr(text, '\0', capacity);
    if (!end) return 0;
    size_t size = (size_t)(end - text);
    for (size_t i = 0U; i < size;) {
        uint32_t value = bytes[i++], minimum;
        unsigned continuation;
        if (value < 0x80U) continue;
        if (value >= 0xc2U && value <= 0xdfU) {
            continuation = 1U; minimum = 0x80U; value &= 0x1fU;
        } else if (value >= 0xe0U && value <= 0xefU) {
            continuation = 2U; minimum = 0x800U; value &= 0x0fU;
        } else if (value >= 0xf0U && value <= 0xf4U) {
            continuation = 3U; minimum = 0x10000U; value &= 0x07U;
        } else return 0;
        if (continuation > size - i) return 0;
        for (unsigned j = 0U; j < continuation; ++j) {
            unsigned next = bytes[i++];
            if ((next & 0xc0U) != 0x80U) return 0;
            value = (value << 6U) | (next & 0x3fU);
        }
        if (value < minimum || value > 0x10ffffU ||
            (value >= 0xd800U && value <= 0xdfffU)) return 0;
    }
    return 1;
}

/* Validate every fixed field before a measuring or writing pass can inspect it. */
static UmiStatus validate_report(const UmiRepositoryPublicationReport *report,
    UmiRepositoryPublicationReportMode mode)
{
    if (!report || (mode != UMI_REPOSITORY_PUBLICATION_REPORT_PREVIEW &&
        mode != UMI_REPOSITORY_PUBLICATION_REPORT_CACHED &&
        mode != UMI_REPOSITORY_PUBLICATION_REPORT_APPLY) ||
        report->count > UMI_REPOSITORY_PUBLICATION_CAPACITY ||
        report->repository_count > report->count || report->blocked_count > report->count ||
        !valid_text(report->detail, sizeof(report->detail))) return UMI_STATUS_INVALID_ARGUMENT;
    for (size_t i = 0U; i < report->count; ++i) {
        const UmiRepositoryPublicationEntry *entry = &report->entries[i];
        if ((entry->parent != SIZE_MAX && entry->parent >= report->count) ||
            entry->primary >= report->count ||
            !valid_text(entry->path, sizeof(entry->path)) ||
            !valid_text(entry->head, sizeof(entry->head)) ||
            !valid_text(entry->remote_head, sizeof(entry->remote_head)) ||
            !valid_text(entry->branch, sizeof(entry->branch)) ||
            !valid_text(entry->detail, sizeof(entry->detail))) return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

/* The same emitter first measures and then writes. Capacity failures therefore
 * cannot leave an apparent JSON result containing only the first repositories.
 * Escaping reuses the existing protocol helper without depending on GUI code. */
typedef struct PublicationJson {
    char *output;
    size_t capacity;
    size_t length;
    char *escaped;
    size_t escape_capacity;
    UmiStatus status;
} PublicationJson;

/* Count bytes in the measuring pass; reserve the NUL in the writing pass. */
static void raw(PublicationJson *writer, const char *text)
{
    size_t length = strlen(text);
    if (writer->status != UMI_STATUS_OK) return;
    if (length > SIZE_MAX - writer->length - 1U ||
        (writer->output && length >= writer->capacity - writer->length)) {
        writer->status = UMI_STATUS_CAPACITY_EXCEEDED;
        return;
    }
    if (writer->output) memcpy(writer->output + writer->length, text, length);
    writer->length += length;
}

/* Scratch storage is private to one formatter call, so nested callers do not
 * share escape buffers or overwrite another report. */
static void string(PublicationJson *writer, const char *text)
{
    if (writer->status != UMI_STATUS_OK) return;
    writer->status = umi_json_escape(text, writer->escaped, writer->escape_capacity);
    if (writer->status != UMI_STATUS_OK) return;
    raw(writer, "\""); raw(writer, writer->escaped); raw(writer, "\"");
}

/* Format platform-sized counts without narrowing them to JSON helper integers. */
static void number(PublicationJson *writer, size_t value)
{
    char text[3U * sizeof(size_t) + 1U];
    int length = snprintf(text, sizeof(text), "%zu", value);
    if (length < 0 || (size_t)length >= sizeof(text)) {
        writer->status = UMI_STATUS_CAPACITY_EXCEEDED;
        return;
    }
    raw(writer, text);
}

/* Normalize C flags to JSON literals rather than implementation-specific numbers. */
static void boolean(PublicationJson *writer, int value)
{
    raw(writer, value ? "true" : "false");
}

/* Keep attempted mode, final status and completed actions separate in the schema. */
static void emit_report(PublicationJson *writer,
    const UmiRepositoryPublicationReport *report, UmiRepositoryPublicationReportMode mode)
{
    const char *modes[] = {"preview", "cached", "apply"};
    raw(writer, "{\n  \"schema\":\"umicom.repository.publication\",\n  \"schema_version\":1,\n  \"mode\":");
    string(writer, modes[mode]);
    raw(writer, ",\n  \"status\":"); string(writer, umi_status_text(report->status));
    raw(writer, ",\n  \"publication_completed\":");
    boolean(writer, mode == UMI_REPOSITORY_PUBLICATION_REPORT_APPLY && report->status == UMI_STATUS_OK);
    raw(writer, ",\n  \"checkout_count\":"); number(writer, report->count);
    raw(writer, ",\n  \"repository_count\":"); number(writer, report->repository_count);
    raw(writer, ",\n  \"blocked_count\":"); number(writer, report->blocked_count);
    raw(writer, ",\n  \"commits_completed\":"); number(writer, report->committed_count);
    raw(writer, ",\n  \"pushes_completed\":"); number(writer, report->pushed_count);
    raw(writer, ",\n  \"updates_completed\":"); number(writer, report->updated_count);
    raw(writer, ",\n  \"detail\":"); string(writer, report->detail);
    raw(writer, ",\n  \"checkouts\":[");
    for (size_t i = 0U; i < report->count; ++i) {
        const UmiRepositoryPublicationEntry *entry = &report->entries[i];
        raw(writer, i ? ",\n    {\"path\":" : "\n    {\"path\":"); string(writer, entry->path);
        raw(writer, ",\"head\":"); string(writer, entry->head);
        raw(writer, ",\"remote_head\":"); string(writer, entry->remote_head);
        raw(writer, ",\"branch\":");
        if (entry->branch[0]) string(writer, entry->branch); else raw(writer, "null");
        raw(writer, ",\"parent\":");
        if (entry->parent == SIZE_MAX) raw(writer, "null"); else number(writer, entry->parent);
        raw(writer, ",\"primary\":"); number(writer, entry->primary);
        raw(writer, ",\"changed_paths\":"); number(writer, entry->changed_paths);
        raw(writer, ",\"ahead\":"); number(writer, entry->ahead);
        raw(writer, ",\"behind\":"); number(writer, entry->behind);
        raw(writer, ",\"vendor\":"); boolean(writer, entry->vendor);
        raw(writer, ",\"blocked\":"); boolean(writer, entry->blocked);
        raw(writer, ",\"committed\":"); boolean(writer, entry->committed);
        raw(writer, ",\"pushed\":"); boolean(writer, entry->pushed);
        raw(writer, ",\"fast_forwarded\":"); boolean(writer, entry->fast_forwarded);
        raw(writer, ",\"locked\":"); boolean(writer, entry->locked);
        raw(writer, ",\"detail\":"); string(writer, entry->detail); raw(writer, "}");
    }
    raw(writer, "\n  ]\n}\n");
}

UmiStatus umi_repository_publication_report_format_json(
    const UmiRepositoryPublicationReport *report, UmiRepositoryPublicationReportMode mode,
    char *output, size_t capacity, size_t *required_capacity)
{
    if (!required_capacity) return UMI_STATUS_INVALID_ARGUMENT;
    *required_capacity = 0U;
    if (!output && capacity) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = validate_report(report, mode);
    if (status != UMI_STATUS_OK) return status;
    /* Six bytes per input byte covers a control character's Unicode escape.
     * The sum covers every fixed string field without assuming a path size. */
    size_t escape_capacity = 6U * (UMI_PATH_CAPACITY + 512U + 128U + 65U) + 1U;
    char *escaped = malloc(escape_capacity);
    if (!escaped) return UMI_STATUS_OUT_OF_MEMORY;
    PublicationJson writer = {NULL, 0U, 0U, escaped, escape_capacity, UMI_STATUS_OK};
    emit_report(&writer, report, mode);
    status = writer.status;
    if (status == UMI_STATUS_OK) {
        *required_capacity = writer.length + 1U;
        if (output && capacity < *required_capacity) status = UMI_STATUS_CAPACITY_EXCEEDED;
        else if (output) {
            writer.output = output;
            writer.capacity = capacity;
            writer.length = 0U;
            emit_report(&writer, report, mode);
            status = writer.status;
            if (status == UMI_STATUS_OK) output[writer.length] = '\0';
        }
    }
    free(escaped);
    return status;
}

UmiStatus umi_repository_publication_report_write_json(
    const UmiRepositoryPublicationReport *report, UmiRepositoryPublicationReportMode mode,
    UmiOutputFile *file)
{
    size_t required = 0U;
    if (!file) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = umi_repository_publication_report_format_json(report, mode, NULL, 0U, &required);
    if (status != UMI_STATUS_OK) return status;
    char *text = malloc(required);
    if (!text) return UMI_STATUS_OUT_OF_MEMORY;
    status = umi_repository_publication_report_format_json(report, mode, text, required, &required);
    if (status == UMI_STATUS_OK) status = UmiOutputFileWrite(file, text, required - 1U);
    free(text);
    return status;
}

UmiStatus umi_repository_publication_report_check_path(
    const UmiRepositoryPublicationReport *report, const char *path)
{
    if (!report || report->count > UMI_REPOSITORY_PUBLICATION_CAPACITY)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = UmiOutputFileValidatePath(path);
    if (status != UMI_STATUS_OK) return status;
    for (size_t i = 0U; i < report->count; ++i) {
        const char *root = report->entries[i].path;
        if (!valid_text(root, sizeof(report->entries[i].path))) return UMI_STATUS_INVALID_ARGUMENT;
        if (root[0] && umi_path_is_within(root, path)) return UMI_STATUS_PERMISSION_DENIED;
    }
    /* Checking ancestors also catches an unrelated repository that was not
     * discovered under the publication root. Missing parents are not created. */
    char parent[UMI_PATH_CAPACITY], ancestor[UMI_PATH_CAPACITY], marker[UMI_PATH_CAPACITY];
    status = umi_path_parent(path, parent, sizeof(parent));
    if (status != UMI_STATUS_OK) return status;
    if (!umi_fs_is_directory(parent)) return UMI_STATUS_NOT_FOUND;
    for (size_t depth = 0U; depth < UMI_PATH_CAPACITY; ++depth) {
        status = umi_path_join(parent, ".git", marker, sizeof(marker));
        if (status != UMI_STATUS_OK) return status;
        if (umi_fs_exists(marker)) return UMI_STATUS_PERMISSION_DENIED;
        status = umi_path_parent(parent, ancestor, sizeof(ancestor));
        if (status != UMI_STATUS_OK) return status;
        if (!ancestor[0] || umi_path_equal(parent, ancestor)) return UMI_STATUS_OK;
        memcpy(parent, ancestor, strlen(ancestor) + 1U);
    }
    return UMI_STATUS_CAPACITY_EXCEEDED;
}

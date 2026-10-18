/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/vcs/revision_changes.c
 * Purpose: Parse bounded binary committed changes without losing deleted paths or gitlinks.
 * Author and Organisation: Sammy Hegab, Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/vcs/revision_changes.h"
#include "revision_changes_internal.h"
#include <stdlib.h>
#include <string.h>

struct UmiVcsRevisionChanges {
    UmiVcsRevisionChange *items;
    size_t count, capacity;
};
typedef struct RevisionSpan { const char *bytes; size_t length; } RevisionSpan;
static bool Take(const char *bytes, size_t length, size_t *offset, RevisionSpan *out)
{
    if (*offset >= length) return false;
    const char *end = memchr(bytes + *offset, '\0', length - *offset);
    if (end == NULL) return false;
    out->bytes = bytes + *offset; out->length = (size_t)(end - out->bytes);
    *offset += out->length + 1U;
    return true;
}
static bool Mode(RevisionSpan value, uint32_t *out)
{
    if (value.length != 6U) return false;
    uint32_t mode = 0U;
    for (size_t i = 0U; i < value.length; ++i) {
        if (value.bytes[i] < '0' || value.bytes[i] > '7') return false;
        mode = mode * 8U + (uint32_t)(value.bytes[i] - '0');
    }
    if (mode != 0U && mode != 0100644U && mode != 0100755U &&
        mode != 0120000U && mode != 0160000U) return false;
    *out = mode;
    return true;
}
static bool Zero(RevisionSpan value)
{
    for (size_t i = 0U; i < value.length; ++i) if (value.bytes[i] != '0') return false;
    return true;
}
static UmiStatus Header(RevisionSpan header, UmiVcsRevisionChange *entry)
{
    if (header.length == 0U || header.length > 160U || header.bytes[0] != ':')
        return UMI_STATUS_PARSE_ERROR;
    RevisionSpan fields[5]; size_t begin = 1U;
    for (size_t i = 0U; i < 4U; ++i) {
        const char *space = memchr(header.bytes + begin, ' ', header.length - begin);
        if (space == NULL) return UMI_STATUS_PARSE_ERROR;
        fields[i] = (RevisionSpan){header.bytes + begin, (size_t)(space - header.bytes) - begin};
        begin = (size_t)(space - header.bytes) + 1U;
    }
    fields[4] = (RevisionSpan){header.bytes + begin, header.length - begin};
    if (!Mode(fields[0], &entry->old_mode) || !Mode(fields[1], &entry->new_mode) ||
        fields[2].length != fields[3].length ||
        !UmiVcsRevisionHex(fields[2].bytes, fields[2].length) ||
        !UmiVcsRevisionHex(fields[3].bytes, fields[3].length) || fields[4].length != 1U)
        return UMI_STATUS_PARSE_ERROR;
    entry->status = fields[4].bytes[0];
    if (entry->status != 'A' && entry->status != 'D' && entry->status != 'M' && entry->status != 'T')
        return UMI_STATUS_PARSE_ERROR;
    if ((entry->status == 'A') != (entry->old_mode == 0U) ||
        (entry->status == 'D') != (entry->new_mode == 0U) ||
        Zero(fields[2]) != (entry->old_mode == 0U) || Zero(fields[3]) != (entry->new_mode == 0U))
        return UMI_STATUS_PARSE_ERROR;
    entry->gitlink = entry->old_mode == 0160000U || entry->new_mode == 0160000U;
    return UMI_STATUS_OK;
}
static bool RelativePath(RevisionSpan path)
{
    if (path.length == 0U || path.bytes[0] == '/' ||
        (path.length > 1U && path.bytes[1] == ':')) return false;
    size_t begin = 0U;
    for (size_t i = 0U; i <= path.length; ++i) {
        if (i != path.length && path.bytes[i] != '/') continue;
        size_t count = i - begin;
        if (count == 0U || (count == 1U && path.bytes[begin] == '.') ||
            (count == 2U && path.bytes[begin] == '.' && path.bytes[begin + 1U] == '.')) return false;
        begin = i + 1U;
    }
    return true;
}
static UmiStatus Append(UmiVcsRevisionChanges *changes, UmiVcsRevisionChange entry, RevisionSpan path)
{
    if (changes->count == UMI_VCS_REVISION_CHANGE_LIMIT || path.length > UMI_VCS_REVISION_PATH_LIMIT)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    if (!RelativePath(path)) return UMI_STATUS_PARSE_ERROR;
    if (changes->count == changes->capacity) {
        size_t capacity = changes->capacity == 0U ? 32U : changes->capacity * 2U;
        if (capacity > UMI_VCS_REVISION_CHANGE_LIMIT) capacity = UMI_VCS_REVISION_CHANGE_LIMIT;
        UmiVcsRevisionChange *items = realloc(changes->items, capacity * sizeof(*items));
        if (items == NULL) return UMI_STATUS_OUT_OF_MEMORY;
        changes->items = items; changes->capacity = capacity;
    }
    char *copy = malloc(path.length + 1U);
    if (copy == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    memcpy(copy, path.bytes, path.length); copy[path.length] = '\0';
    entry.path = copy; changes->items[changes->count++] = entry;
    return UMI_STATUS_OK;
}
static int Order(const void *a, const void *b)
{
    return strcmp(((const UmiVcsRevisionChange *)a)->path, ((const UmiVcsRevisionChange *)b)->path);
}
void UmiVcsRevisionChangesDestroy(UmiVcsRevisionChanges *changes)
{
    if (changes == NULL) return;
    for (size_t i = 0U; i < changes->count; ++i) free((void *)changes->items[i].path);
    free(changes->items); free(changes);
}
size_t UmiVcsRevisionChangesCount(const UmiVcsRevisionChanges *changes)
{
    return changes == NULL ? 0U : changes->count;
}
const UmiVcsRevisionChange *UmiVcsRevisionChangesAt(const UmiVcsRevisionChanges *changes, size_t index)
{
    return changes != NULL && index < changes->count ? &changes->items[index] : NULL;
}
UmiStatus UmiVcsRevisionChangesParse(const void *bytes, size_t length, UmiVcsRevisionChanges **outChanges)
{
    if (outChanges == NULL || *outChanges != NULL || (bytes == NULL && length != 0U))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (length > UMI_VCS_REVISION_CHANGE_BYTE_LIMIT) return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiVcsRevisionChanges *changes = calloc(1U, sizeof(*changes));
    if (changes == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    size_t offset = 0U;
    UmiStatus status = UMI_STATUS_OK;
    while (offset < length && status == UMI_STATUS_OK) {
        RevisionSpan header, path;
        UmiVcsRevisionChange entry = {0};
        if (!Take(bytes, length, &offset, &header) || !Take(bytes, length, &offset, &path))
            status = UMI_STATUS_PARSE_ERROR;
        else {
            status = Header(header, &entry);
            if (status == UMI_STATUS_OK) status = Append(changes, entry, path);
        }
    }
    if (status == UMI_STATUS_OK && changes->count > 1U) {
        qsort(changes->items, changes->count, sizeof(*changes->items), Order);
        for (size_t i = 1U; i < changes->count; ++i)
            if (strcmp(changes->items[i - 1U].path, changes->items[i].path) == 0) {
                status = UMI_STATUS_PARSE_ERROR; break;
            }
    }
    if (status != UMI_STATUS_OK) UmiVcsRevisionChangesDestroy(changes);
    else *outChanges = changes;
    return status;
}

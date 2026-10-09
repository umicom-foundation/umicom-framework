/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/vcs/working_tree.c
 * PURPOSE: Parse bounded binary Git status while preserving exact paths and child repository state.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/vcs/working_tree.h"
#include <stdlib.h>
#include <string.h>

struct UmiVcsWorkingTree
{
    UmiVcsWorkingTreeSummary summary;
    UmiVcsWorkingTreeEntry *entries;
    size_t capacity;
};

/* Keep spans separate from C strings: a filename may contain whitespace, but not a NUL byte. */
typedef struct StatusSpan
{
    const char *data;
    size_t length;
} StatusSpan;

/* Exact comparisons prevent recognised header prefixes from accepting extra trailing data. */
static int SpanEquals(StatusSpan span, const char *text)
{
    const size_t length = strlen(text);
    return span.length == length && memcmp(span.data, text, length) == 0;
}

/* Reject truncation before touching the destination so callers never mistake a shortened path. */
static UmiStatus CopySpan(char *target, size_t capacity, StatusSpan span)
{
    if (span.length == 0U)
        return UMI_STATUS_PARSE_ERROR;
    if (span.length >= capacity)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    memcpy(target, span.data, span.length);
    target[span.length] = '\0';
    return UMI_STATUS_OK;
}

/* Use checked decimal accumulation for branch counters and rename similarity scores. */
static UmiStatus ParseCount(StatusSpan span, size_t *out)
{
    size_t value = 0U;
    if (span.length == 0U)
        return UMI_STATUS_PARSE_ERROR;
    for (size_t index = 0U; index < span.length; ++index)
    {
        unsigned char digit = (unsigned char)span.data[index];
        if (digit < '0' || digit > '9')
            return UMI_STATUS_PARSE_ERROR;
        if (value > (SIZE_MAX - (size_t)(digit - '0')) / 10U)
            return UMI_STATUS_CAPACITY_EXCEEDED;
        value = value * 10U + (size_t)(digit - '0');
    }
    *out = value;
    return UMI_STATUS_OK;
}

/* Git supports SHA-1 and SHA-256 repositories; zero object IDs represent absent objects. */
static int ValidObject(StatusSpan span)
{
    if (span.length != 40U && span.length != 64U)
        return 0;
    for (size_t index = 0U; index < span.length; ++index)
    {
        char digit = span.data[index];
        if (!((digit >= '0' && digit <= '9') || (digit >= 'a' && digit <= 'f')))
            return 0;
    }
    return 1;
}

/* Validate metadata even though the presentation does not need to store Unix file modes. */
static int ValidMode(StatusSpan span)
{
    if (span.length != 6U)
        return 0;
    for (size_t index = 0U; index < span.length; ++index)
        if (span.data[index] < '0' || span.data[index] > '7')
            return 0;
    return 1;
}

/* Split only the fixed metadata prefix. Everything after it belongs to the filename. */
static UmiStatus PrefixFields(StatusSpan record, StatusSpan *fields, size_t count, StatusSpan *path)
{
    size_t offset = 0U;
    for (size_t index = 0U; index < count; ++index)
    {
        const char *space = memchr(record.data + offset, ' ', record.length - offset);
        if (space == NULL || space == record.data + offset)
            return UMI_STATUS_PARSE_ERROR;
        fields[index].data = record.data + offset;
        fields[index].length = (size_t)(space - fields[index].data);
        offset = (size_t)(space - record.data) + 1U;
    }
    path->data = record.data + offset;
    path->length = record.length - offset;
    return path->length != 0U ? UMI_STATUS_OK : UMI_STATUS_PARSE_ERROR;
}

/* Unknown headers are reserved for future Git extensions and must remain ignorable. */
static UmiStatus ParseHeader(UmiVcsWorkingTree *tree, StatusSpan record, unsigned *seen)
{
    static const char *const keys[] = {"# branch.oid ", "# branch.head ", "# branch.upstream ",
                                       "# branch.ab ", "# stash "};
    UmiVcsWorkingTreeSummary *summary = &tree->summary;
    size_t key;
    StatusSpan value;
    for (key = 0U; key < sizeof(keys) / sizeof(keys[0]); ++key)
    {
        size_t prefix = strlen(keys[key]);
        if (record.length >= prefix && memcmp(record.data, keys[key], prefix) == 0)
        {
            value.data = record.data + prefix;
            value.length = record.length - prefix;
            break;
        }
    }
    if (key == sizeof(keys) / sizeof(keys[0]))
        return UMI_STATUS_OK;
    if ((*seen & (1U << key)) != 0U)
        return UMI_STATUS_PARSE_ERROR;
    *seen |= 1U << key;
    switch (key)
    {
    case 0U:
        if (SpanEquals(value, "(initial)"))
        {
            summary->unborn = 1;
            return UMI_STATUS_OK;
        }
        if (!ValidObject(value))
            return UMI_STATUS_PARSE_ERROR;
        summary->commit_known = 1;
        return CopySpan(summary->commit_id, sizeof(summary->commit_id), value);
    case 1U:
        summary->branch_known = !SpanEquals(value, "(unknown)");
        summary->detached = SpanEquals(value, "(detached)");
        return CopySpan(summary->branch, sizeof(summary->branch), value);
    case 2U:
        return CopySpan(summary->upstream, sizeof(summary->upstream), value);
    case 3U:
    {
        const char *space = memchr(value.data, ' ', value.length);
        StatusSpan ahead, behind;
        UmiStatus status;
        if (value.length < 5U || value.data[0] != '+' || space == NULL ||
            (size_t)(space - value.data) + 2U >= value.length || space[1] != '-')
            return UMI_STATUS_PARSE_ERROR;
        ahead = (StatusSpan){value.data + 1, (size_t)(space - value.data) - 1U};
        behind = (StatusSpan){space + 2, value.length - (size_t)(space - value.data) - 2U};
        /* Git can deliberately omit expensive divergence counts. Unknown is not zero. */
        if (SpanEquals(ahead, "?") && SpanEquals(behind, "?"))
            return UMI_STATUS_OK;
        status = ParseCount(ahead, &summary->ahead);
        if (status == UMI_STATUS_OK)
            status = ParseCount(behind, &summary->behind);
        summary->divergence_known = status == UMI_STATUS_OK;
        return status;
    }
    default:
        summary->stash_known = 1;
        return ParseCount(value, &summary->stashes);
    }
}

/* Keep type changes visible without changing the legacy public enumeration. */
static UmiStatus DecodeState(char code, UmiVcsChangeState *out)
{
    switch (code)
    {
    case '.':
        *out = UMI_VCS_CHANGE_UNMODIFIED;
        break;
    case 'A':
        *out = UMI_VCS_CHANGE_ADDED;
        break;
    case 'M':
    case 'T':
        *out = UMI_VCS_CHANGE_MODIFIED;
        break;
    case 'D':
        *out = UMI_VCS_CHANGE_DELETED;
        break;
    case 'R':
        *out = UMI_VCS_CHANGE_RENAMED;
        break;
    case 'C':
        *out = UMI_VCS_CHANGE_COPIED;
        break;
    default:
        return UMI_STATUS_PARSE_ERROR;
    }
    return UMI_STATUS_OK;
}

/* The four submodule characters describe three independent reasons a child appears dirty. */
static UmiStatus ParseChild(StatusSpan span, UmiVcsWorkingTreeEntry *entry)
{
    if (SpanEquals(span, "N..."))
        return UMI_STATUS_OK;
    if (span.length != 4U || span.data[0] != 'S' || (span.data[1] != '.' && span.data[1] != 'C') ||
        (span.data[2] != '.' && span.data[2] != 'M') ||
        (span.data[3] != '.' && span.data[3] != 'U'))
        return UMI_STATUS_PARSE_ERROR;
    entry->submodule = 1;
    entry->child_commit_changed = span.data[1] == 'C';
    entry->child_tracked_changed = span.data[2] == 'M';
    entry->child_untracked = span.data[3] == 'U';
    return UMI_STATUS_OK;
}

/* Recognise only the conflict pairs documented by Git, without counting them as commit-ready. */
static int ConflictPair(StatusSpan pair)
{
    return SpanEquals(pair, "DD") || SpanEquals(pair, "AU") || SpanEquals(pair, "UD") ||
           SpanEquals(pair, "UA") || SpanEquals(pair, "DU") || SpanEquals(pair, "AA") ||
           SpanEquals(pair, "UU");
}

/* Decode a whole record before adding anything to the observation. */
static UmiStatus ParseEntry(StatusSpan record, UmiVcsWorkingTreeEntry *entry, int *needs_source)
{
    StatusSpan fields[10], path;
    UmiStatus status;
    size_t prefix_count;
    memset(entry, 0, sizeof(*entry));
    *needs_source = 0;
    if (record.length < 3U || record.data[1] != ' ')
        return UMI_STATUS_PARSE_ERROR;
    if (record.data[0] == '?' || record.data[0] == '!')
    {
        path = (StatusSpan){record.data + 2, record.length - 2U};
        entry->index_code = entry->worktree_code = record.data[0];
        entry->change.index_state = UMI_VCS_CHANGE_UNMODIFIED;
        entry->change.worktree_state =
            record.data[0] == '?' ? UMI_VCS_CHANGE_UNTRACKED : UMI_VCS_CHANGE_IGNORED;
        return CopySpan(entry->change.path, sizeof(entry->change.path), path);
    }
    switch (record.data[0])
    {
    case '1':
        prefix_count = 8U;
        break;
    case '2':
        prefix_count = 9U;
        *needs_source = 1;
        break;
    case 'u':
        prefix_count = 10U;
        entry->unmerged = 1;
        break;
    default:
        return UMI_STATUS_PARSE_ERROR;
    }
    status = PrefixFields(record, fields, prefix_count, &path);
    if (status != UMI_STATUS_OK || fields[0].length != 1U || fields[1].length != 2U)
        return UMI_STATUS_PARSE_ERROR;
    status = ParseChild(fields[2], entry);
    if (status != UMI_STATUS_OK)
        return status;
    entry->index_code = fields[1].data[0];
    entry->worktree_code = fields[1].data[1];
    if (entry->unmerged)
    {
        if (!ConflictPair(fields[1]))
            return UMI_STATUS_PARSE_ERROR;
        for (size_t index = 3U; index < 7U; ++index)
            if (!ValidMode(fields[index]))
                return UMI_STATUS_PARSE_ERROR;
        for (size_t index = 7U; index < 10U; ++index)
            if (!ValidObject(fields[index]))
                return UMI_STATUS_PARSE_ERROR;
        entry->change.index_state = entry->change.worktree_state = UMI_VCS_CHANGE_CONFLICTED;
    }
    else
    {
        for (size_t index = 3U; index < 6U; ++index)
            if (!ValidMode(fields[index]))
                return UMI_STATUS_PARSE_ERROR;
        if (!ValidObject(fields[6]) || !ValidObject(fields[7]) ||
            fields[6].length != fields[7].length)
            return UMI_STATUS_PARSE_ERROR;
        status = DecodeState(entry->index_code, &entry->change.index_state);
        if (status == UMI_STATUS_OK)
            status = DecodeState(entry->worktree_code, &entry->change.worktree_state);
        if (status != UMI_STATUS_OK)
            return status;
        entry->change.staged = entry->index_code != '.';
        if (*needs_source)
        {
            size_t similarity;
            StatusSpan score = fields[8];
            if (score.length < 2U || (score.data[0] != 'R' && score.data[0] != 'C') ||
                (score.data[0] != entry->index_code && score.data[0] != entry->worktree_code))
                return UMI_STATUS_PARSE_ERROR;
            ++score.data;
            --score.length;
            status = ParseCount(score, &similarity);
            if (status != UMI_STATUS_OK)
                return status;
            if (similarity > 100U)
                return UMI_STATUS_PARSE_ERROR;
            entry->similarity = (unsigned)similarity;
        }
        else if (entry->index_code == 'R' || entry->index_code == 'C' ||
                 entry->worktree_code == 'R' || entry->worktree_code == 'C')
        {
            return UMI_STATUS_PARSE_ERROR;
        }
    }
    return CopySpan(entry->change.path, sizeof(entry->change.path), path);
}

/* A complete NUL terminator is required even for the final filename. */
static UmiStatus NextRecord(const char *bytes, size_t length, size_t *offset, StatusSpan *record)
{
    const char *end;
    if (*offset >= length)
        return UMI_STATUS_PARSE_ERROR;
    end = memchr(bytes + *offset, '\0', length - *offset);
    if (end == NULL || end == bytes + *offset)
        return UMI_STATUS_PARSE_ERROR;
    record->data = bytes + *offset;
    record->length = (size_t)(end - record->data);
    *offset = (size_t)(end - bytes) + 1U;
    return UMI_STATUS_OK;
}

/* Duplicate destinations are ambiguous. Grow storage on the heap within the shared list bound. */
static UmiStatus AppendEntry(UmiVcsWorkingTree *tree, const UmiVcsWorkingTreeEntry *entry)
{
    size_t count = tree->summary.entries;
    for (size_t index = 0U; index < count; ++index)
        if (strcmp(tree->entries[index].change.path, entry->change.path) == 0)
            return UMI_STATUS_ALREADY_EXISTS;
    if (count == UMI_VCS_MAX_CHANGES)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    if (count == tree->capacity)
    {
        size_t capacity = tree->capacity == 0U ? 32U : tree->capacity * 2U;
        UmiVcsWorkingTreeEntry *entries;
        if (capacity > UMI_VCS_MAX_CHANGES)
            capacity = UMI_VCS_MAX_CHANGES;
        entries = realloc(tree->entries, capacity * sizeof(*entries));
        if (entries == NULL)
            return UMI_STATUS_OUT_OF_MEMORY;
        tree->entries = entries;
        tree->capacity = capacity;
    }
    tree->entries[count] = *entry;
    ++tree->summary.entries;
    tree->summary.staged += (size_t)(entry->change.staged != 0);
    tree->summary.conflicts += (size_t)(entry->unmerged != 0);
    tree->summary.untracked += (size_t)(entry->change.worktree_state == UMI_VCS_CHANGE_UNTRACKED);
    tree->summary.child_commits += (size_t)(entry->child_commit_changed != 0);
    tree->summary.dirty_children +=
        (size_t)(entry->child_tracked_changed || entry->child_untracked);
    return UMI_STATUS_OK;
}

/* Build privately, then publish ownership once all records and headers agree. */
UmiStatus UmiVcsWorkingTreeParse(const void *bytes, size_t length, UmiVcsWorkingTree **out_tree)
{
    UmiVcsWorkingTree *tree;
    UmiStatus status = UMI_STATUS_OK;
    size_t offset = 0U;
    unsigned seen = 0U;
    if (out_tree == NULL || (bytes == NULL && length != 0U))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (length > UMI_VCS_WORKING_TREE_BYTE_LIMIT)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    tree = calloc(1U, sizeof(*tree));
    if (tree == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    while (offset < length && status == UMI_STATUS_OK)
    {
        StatusSpan record;
        status = NextRecord(bytes, length, &offset, &record);
        if (status != UMI_STATUS_OK)
            break;
        if (record.length >= 2U && record.data[0] == '#' && record.data[1] == ' ')
        {
            status = ParseHeader(tree, record, &seen);
        }
        else
        {
            UmiVcsWorkingTreeEntry entry;
            int needs_source;
            status = ParseEntry(record, &entry, &needs_source);
            if (status == UMI_STATUS_OK && needs_source)
            {
                status = NextRecord(bytes, length, &offset, &record);
                if (status == UMI_STATUS_OK)
                    status = CopySpan(entry.change.original_path,
                                      sizeof(entry.change.original_path), record);
            }
            if (status == UMI_STATUS_OK)
                status = AppendEntry(tree, &entry);
        }
    }
    /* Divergence without an upstream is not a meaningful Git observation. */
    if (status == UMI_STATUS_OK && (seen & (1U << 3U)) != 0U && tree->summary.upstream[0] == '\0')
        status = UMI_STATUS_PARSE_ERROR;
    if (status != UMI_STATUS_OK)
    {
        UmiVcsWorkingTreeDestroy(tree);
        return status;
    }
    *out_tree = tree;
    return UMI_STATUS_OK;
}

/* No entry outlives its containing observation. */
void UmiVcsWorkingTreeDestroy(UmiVcsWorkingTree *tree)
{
    if (tree == NULL)
        return;
    free(tree->entries);
    free(tree);
}

/* Value copying lets a UI retain counts without retaining a large entry collection. */
UmiStatus UmiVcsWorkingTreeDescribe(const UmiVcsWorkingTree *tree,
                                    UmiVcsWorkingTreeSummary *out_summary)
{
    if (tree == NULL || out_summary == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out_summary = tree->summary;
    return UMI_STATUS_OK;
}

/* Borrowed entries remain stable because a completed observation cannot be edited. */
const UmiVcsWorkingTreeEntry *UmiVcsWorkingTreeEntryAt(const UmiVcsWorkingTree *tree, size_t index)
{
    return tree != NULL && index < tree->summary.entries ? &tree->entries[index] : NULL;
}

/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/repository/inspection/porcelain_bytes.c
 * PURPOSE: Parse complete Git status records without losing unusual paths or failed evidence.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/


#include "umicom/repository/porcelain_parser.h"
#include "umicom/repository/git_capture.h"
#include <stdint.h>
#include <string.h>

/* Preserve the aggregate parser for engineering comparison. The active
 * adapter below delegates grammar, path limits and conflict interpretation to
 * the established VCS reader, preventing independent parsers from diverging. */
#if 0
typedef struct PorcelainSpan {
    const unsigned char *bytes;
    size_t length;
} PorcelainSpan;

/* Fields are borrowed spans into the capture. Only metadata uses spaces as
 * delimiters; the remaining filename is opaque and is never split at whitespace. */
static int PorcelainToken(PorcelainSpan *remaining, PorcelainSpan *token)
{
    const unsigned char *end = memchr(remaining->bytes, ' ', remaining->length);
    if (end == NULL || end == remaining->bytes) return 0;
    token->bytes = remaining->bytes;
    token->length = (size_t)(end - remaining->bytes);
    remaining->bytes = end + 1;
    remaining->length -= token->length + 1U;
    return 1;
}
static int PorcelainEqual(PorcelainSpan span, const char *text)
{
    size_t length = strlen(text);
    return span.length == length && memcmp(span.bytes, text, length) == 0;
}

/* Decimal counters are checked before multiplication rather than relying on
 * sscanf conversion, which can accept an overflowing or partially parsed value. */
static int PorcelainNumber(PorcelainSpan text, size_t *out)
{
    size_t number = 0U;
    if (text.length == 0U) return 0;
    for (size_t i = 0U; i < text.length; ++i) {
        if (text.bytes[i] < '0' || text.bytes[i] > '9') return 0;
        size_t digit = (size_t)(text.bytes[i] - '0');
        if (number > (SIZE_MAX - digit) / 10U) return 0;
        number = number * 10U + digit;
    }
    *out = number;
    return 1;
}
static int PorcelainHash(PorcelainSpan text)
{
    if (text.length != 40U && text.length != 64U) return 0;
    for (size_t i = 0U; i < text.length; ++i)
        if (!((text.bytes[i] >= '0' && text.bytes[i] <= '9') ||
            (text.bytes[i] >= 'a' && text.bytes[i] <= 'f') ||
            (text.bytes[i] >= 'A' && text.bytes[i] <= 'F'))) return 0;
    return 1;
}
static int PorcelainMode(PorcelainSpan text)
{
    if (text.length != 6U) return 0;
    for (size_t i = 0U; i < text.length; ++i)
        if (text.bytes[i] < '0' || text.bytes[i] > '7') return 0;
    return 1;
}
/* Branch labels are copied into the existing public model. A long label must
 * fail visibly instead of becoming a different, truncated branch name. */
static UmiStatus PorcelainLabel(PorcelainSpan text, char *out, size_t capacity)
{
    if (text.length == 0U) return UMI_STATUS_PARSE_ERROR;
    if (text.length >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    for (size_t i = 0U; i < text.length; ++i)
        if (text.bytes[i] < 32U || text.bytes[i] == 127U) return UMI_STATUS_PARSE_ERROR;
    memcpy(out, text.bytes, text.length);
    out[text.length] = '\0';
    return UMI_STATUS_OK;
}

/* Known headers may appear once each. Git explicitly permits future optional
 * headers; ignore their contents after validating the header envelope. */
static UmiStatus PorcelainHeader(PorcelainSpan line, unsigned *seen,
    UmiRepositoryPorcelainStatus *status)
{
    if (line.length < 3U || line.bytes[1] != ' ') return UMI_STATUS_PARSE_ERROR;
    line.bytes += 2U; line.length -= 2U;
    PorcelainSpan key;
    if (!PorcelainToken(&line, &key) || line.length == 0U) return UMI_STATUS_PARSE_ERROR;
    unsigned bit = PorcelainEqual(key, "branch.oid") ? 1U :
        PorcelainEqual(key, "branch.head") ? 2U :
        PorcelainEqual(key, "branch.upstream") ? 4U :
        PorcelainEqual(key, "branch.ab") ? 8U : 0U;
    if (bit == 0U) return UMI_STATUS_OK;
    if ((*seen & bit) != 0U) return UMI_STATUS_PARSE_ERROR;
    *seen |= bit;
    if (bit == 1U)
        return PorcelainEqual(line, "(initial)") || PorcelainHash(line)
            ? UMI_STATUS_OK : UMI_STATUS_PARSE_ERROR;
    if (bit == 2U) {
        UmiStatus result = PorcelainLabel(line, status->branch.branch, sizeof status->branch.branch);
        if (result == UMI_STATUS_OK) status->branch.detached = PorcelainEqual(line, "(detached)");
        return result;
    }
    if (bit == 4U) return PorcelainLabel(line, status->branch.upstream, sizeof status->branch.upstream);
    PorcelainSpan ahead;
    if (!PorcelainToken(&line, &ahead) || ahead.length < 2U || ahead.bytes[0] != '+' ||
        line.length < 2U || line.bytes[0] != '-') return UMI_STATUS_PARSE_ERROR;
    ++ahead.bytes; --ahead.length; ++line.bytes; --line.length;
    return PorcelainNumber(ahead, &status->branch.ahead) &&
        PorcelainNumber(line, &status->branch.behind) ? UMI_STATUS_OK : UMI_STATUS_PARSE_ERROR;
}

/* Validate tracked prefixes before contributing any counts. Unmerged paths are
 * counted as conflicts, not as committable staged changes. Submodule flags refer
 * to exact columns, so a clean S... entry is not mistakenly considered dirty. */
static UmiStatus PorcelainTracked(unsigned char kind, PorcelainSpan line,
    UmiRepositoryPorcelainStatus *status)
{
    PorcelainSpan fields[9];
    size_t count = kind == '1' ? 7U : kind == '2' ? 8U : 9U;
    for (size_t i = 0U; i < count; ++i)
        if (!PorcelainToken(&line, &fields[i])) return UMI_STATUS_PARSE_ERROR;
    if (line.length == 0U || fields[0].length != 2U || fields[1].length != 4U)
        return UMI_STATUS_PARSE_ERROR;
    unsigned char x = fields[0].bytes[0], y = fields[0].bytes[1];
    if (strchr(".MTADRCU", (int)x) == NULL || strchr(".MTADRCU", (int)y) == NULL)
        return UMI_STATUS_PARSE_ERROR;
    const unsigned char *sub = fields[1].bytes;
    if (memcmp(sub, "N...", 4U) != 0 &&
        !(sub[0] == 'S' && (sub[1] == '.' || sub[1] == 'C') &&
          (sub[2] == '.' || sub[2] == 'M') && (sub[3] == '.' || sub[3] == 'U')))
        return UMI_STATUS_PARSE_ERROR;
    size_t modeCount = kind == 'u' ? 4U : 3U;
    size_t hashCount = kind == 'u' ? 3U : 2U;
    for (size_t i = 0U; i < modeCount; ++i)
        if (!PorcelainMode(fields[2U + i])) return UMI_STATUS_PARSE_ERROR;
    /* Retain the independent hash check for comparison. A single Git record
     * cannot mix object formats; every object name now shares the first width. */
#if 0
    for (size_t i = 0U; i < hashCount; ++i)
        if (!PorcelainHash(fields[2U + modeCount + i])) return UMI_STATUS_PARSE_ERROR;
#endif
    for (size_t i = 0U; i < hashCount; ++i)
        if (!PorcelainHash(fields[2U + modeCount + i]) ||
            fields[2U + modeCount + i].length != fields[2U + modeCount].length)
            return UMI_STATUS_PARSE_ERROR;
    if (kind == '1' && (x == 'R' || x == 'C' || y == 'R' || y == 'C'))
        return UMI_STATUS_PARSE_ERROR;
    if (kind == '2') {
        PorcelainSpan score = fields[7];
        size_t percent;
        if (score.length < 2U || (score.bytes[0] != 'R' && score.bytes[0] != 'C'))
            return UMI_STATUS_PARSE_ERROR;
        /* The score identifies the same rename/copy operation as XY. */
        if (x != score.bytes[0] && y != score.bytes[0]) return UMI_STATUS_PARSE_ERROR;
        ++score.bytes; --score.length;
        if (!PorcelainNumber(score, &percent) || percent > 100U) return UMI_STATUS_PARSE_ERROR;
    }
    if (kind == 'u') {
        int pair = (x == 'D' && (y == 'D' || y == 'U')) ||
            (x == 'A' && (y == 'A' || y == 'U')) ||
            (x == 'U' && (y == 'A' || y == 'D' || y == 'U'));
        if (!pair) return UMI_STATUS_PARSE_ERROR;
        ++status->worktree.conflicted; ++status->index.conflicted_paths;
    } else {
        if (x == 'U' || y == 'U') return UMI_STATUS_PARSE_ERROR;
        if (x != '.') {
            ++status->index.staged_paths;
            if (PorcelainEqual(fields[2], "160000") || PorcelainEqual(fields[3], "160000"))
                ++status->index.staged_gitlinks;
        }
        if (y == 'D') ++status->worktree.deleted;
        else if (y != '.') ++status->worktree.modified;
    }
    if (sub[0] == 'S' && memcmp(sub + 1, "...", 3U) != 0) ++status->submodule_dirty;
    return UMI_STATUS_OK;
}

UmiStatus umi_repository_porcelain_parse_bytes(const void *bytes, size_t length,
    UmiRepositoryPorcelainStatus *out_status)
{
    if (out_status == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(out_status, 0, sizeof *out_status);
    if (bytes == NULL && length != 0U) return UMI_STATUS_INVALID_ARGUMENT;
    if (length > UMI_REPOSITORY_GIT_CAPTURE_LIMIT) return UMI_STATUS_CAPACITY_EXCEEDED;
    const unsigned char *input = bytes;
    UmiRepositoryPorcelainStatus candidate = {0};
    size_t position = 0U;
    unsigned headers = 0U;
    int renameSource = 0;
    /* The input bound limits record counts below SIZE_MAX on supported hosts.
     * Publish the candidate only after every record and rename source is present. */
    while (position < length) {
        const unsigned char *record = input + position;
        const unsigned char *end = memchr(record, 0, length - position);
        if (end == NULL || end == record) return UMI_STATUS_PARSE_ERROR;
        PorcelainSpan line = {record, (size_t)(end - record)};
        position += line.length + 1U;
        if (renameSource) { renameSource = 0; continue; }
        UmiStatus result;
        if (line.bytes[0] == '#') result = PorcelainHeader(line, &headers, &candidate);
        else {
            if (line.length < 3U || line.bytes[1] != ' ') return UMI_STATUS_PARSE_ERROR;
            unsigned char kind = line.bytes[0];
            line.bytes += 2U; line.length -= 2U;
            if (kind == '?') { ++candidate.worktree.untracked; result = UMI_STATUS_OK; }
            else if (kind == '!') result = UMI_STATUS_OK;
            else if (kind == '1' || kind == '2' || kind == 'u') {
                result = PorcelainTracked(kind, line, &candidate);
                renameSource = kind == '2';
            } else return UMI_STATUS_PARSE_ERROR;
        }
        if (result != UMI_STATUS_OK) return result;
    }
    if (renameSource) return UMI_STATUS_PARSE_ERROR;
    *out_status = candidate;
    return UMI_STATUS_OK;
}

#endif

/* Repository maintenance is a projection of the same immutable VCS observation
 * used by Source Control. It must not maintain a second status grammar. */
UmiStatus UmiRepositoryPorcelainFromWorkingTree(const UmiVcsWorkingTree *tree,
    UmiRepositoryPorcelainStatus *out_status)
{
    if (out_status == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(out_status, 0, sizeof *out_status);
    if (tree == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiVcsWorkingTreeSummary summary;
    UmiRepositoryPorcelainStatus candidate = {0};
    UmiStatus status = UmiVcsWorkingTreeDescribe(tree, &summary);
    if (status != UMI_STATUS_OK) return status;
    status = umi_repository_inspection_copy_text(candidate.branch.branch,
        sizeof candidate.branch.branch, summary.branch);
    if (status == UMI_STATUS_OK)
        status = umi_repository_inspection_copy_text(candidate.branch.upstream,
            sizeof candidate.branch.upstream, summary.upstream);
    if (status != UMI_STATUS_OK) return status;
    /* The older maintenance value has no "unknown divergence" flag. Preserve
     * its existing counts while callers needing that distinction retain tree. */
    candidate.branch.ahead = summary.ahead;
    candidate.branch.behind = summary.behind;
    candidate.branch.detached = summary.detached;
    candidate.index.staged_paths = summary.staged;
    candidate.index.conflicted_paths = summary.conflicts;
    candidate.worktree.conflicted = summary.conflicts;
    candidate.worktree.untracked = summary.untracked;
    for (size_t i = 0U; i < summary.entries; ++i) {
        const UmiVcsWorkingTreeEntry *entry = UmiVcsWorkingTreeEntryAt(tree, i);
        if (entry == NULL) return UMI_STATUS_INVALID_STATE;
        if (!entry->unmerged) {
            if (entry->worktree_code == 'D') ++candidate.worktree.deleted;
            else if (entry->worktree_code != '.' && entry->worktree_code != '?' && entry->worktree_code != '!')
                ++candidate.worktree.modified;
        }
        if (entry->staged_gitlink) ++candidate.index.staged_gitlinks;
        if (entry->child_commit_changed || entry->child_tracked_changed || entry->child_untracked)
            ++candidate.submodule_dirty;
    }
    *out_status = candidate;
    return UMI_STATUS_OK;
}

UmiStatus umi_repository_porcelain_parse_bytes(const void *bytes, size_t length,
    UmiRepositoryPorcelainStatus *out_status)
{
    if (out_status == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(out_status, 0, sizeof *out_status);
    if (bytes == NULL && length != 0U) return UMI_STATUS_INVALID_ARGUMENT;
    if (length > UMI_REPOSITORY_GIT_CAPTURE_LIMIT) return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiVcsWorkingTree *tree = NULL;
    UmiStatus status = UmiVcsWorkingTreeParse(bytes, length, &tree);
    if (status == UMI_STATUS_OK)
        status = UmiRepositoryPorcelainFromWorkingTree(tree, out_status);
    UmiVcsWorkingTreeDestroy(tree);
    return status;
}

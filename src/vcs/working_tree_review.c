/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/vcs/working_tree_review.c
 * PURPOSE: Render source-control observations without turning path text into commands.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/vcs/working_tree_review.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Escaping is display-only. Preserve every path byte in the observation used by Git commands. */
static void DisplayName(const char *input, char *output)
{
    static const char digits[] = "0123456789abcdef";
    while (*input != '\0')
    {
        unsigned char byte = (unsigned char)*input++;
        if (byte < 32U || byte == 127U)
        {
            *output++ = '\\';
            *output++ = 'x';
            *output++ = digits[byte >> 4U];
            *output++ = digits[byte & 15U];
        }
        else if (byte == '\\' || byte == '"')
        {
            *output++ = '\\';
            *output++ = (char)byte;
        }
        else
        {
            *output++ = (char)byte;
        }
    }
    *output = '\0';
}

/* Complete formatting happens privately, so a short caller buffer cannot publish half a message. */
static UmiStatus PublishText(const char *text, int written, size_t internal_capacity, char *output,
                             size_t capacity)
{
    if (written < 0)
        return UMI_STATUS_INTERNAL_ERROR;
    if ((size_t)written >= internal_capacity || (size_t)written >= capacity)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    memcpy(output, text, (size_t)written + 1U);
    return UMI_STATUS_OK;
}

/* Describe what can enter this repository's commit without implying it was built or published. */
UmiStatus UmiVcsWorkingTreeSummaryText(const UmiVcsWorkingTree *tree, char *output, size_t capacity)
{
    UmiVcsWorkingTreeSummary summary;
    char text[UMI_VCS_WORKING_TREE_SUMMARY_TEXT_CAPACITY];
    char branch[UMI_VCS_NAME_CAPACITY * 4U];
    char divergence[192];
    const char *advice;
    UmiStatus status;
    int written;
    if (tree == NULL || output == NULL || capacity == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = UmiVcsWorkingTreeDescribe(tree, &summary);
    if (status != UMI_STATUS_OK)
        return status;
    DisplayName(summary.branch_known ? summary.branch : "(not reported)", branch);
    if (summary.divergence_known)
        (void)snprintf(divergence, sizeof(divergence),
                       "%zu ahead, %zu behind the locally recorded upstream.", summary.ahead,
                       summary.behind);
    else
        (void)snprintf(divergence, sizeof(divergence), "Upstream comparison was not reported.");
    if (summary.conflicts != 0U)
        advice = "Resolve the conflicts and review the index before attempting a commit.";
    else if (summary.staged == 0U && summary.dirty_children != 0U)
        advice = "Nothing is staged here. Commit files inside each child repository first; "
                 "then review and stage the changed child revision in this parent.";
    else if (summary.staged == 0U)
        advice = "Nothing is staged here. Review and stage the intended paths before committing.";
    else if (summary.dirty_children != 0U)
        advice = "This index has staged changes, but child repositories still contain uncommitted "
                 "files. "
                 "A parent commit records child revisions; it does not include those files.";
    else
        advice = "This index has staged changes. Review the staged diff before committing.";
    written = snprintf(
        text, sizeof(text),
        "Branch: \"%s\"%s%s\n%zu entries; %zu staged; %zu conflicts; %zu untracked paths.\n"
        "%zu children at a different commit; %zu children with uncommitted contents.\n\n%s\n\n%s\n"
        "This is a local status observation. It does not fetch, verify publication, or qualify a "
        "build.\n"
        "Refresh after changing files, switching branches, staging, or committing.",
        branch, summary.detached ? " (detached HEAD)" : "",
        summary.unborn ? " (no commit yet)" : "", summary.entries, summary.staged,
        summary.conflicts, summary.untracked, summary.child_commits, summary.dirty_children, advice,
        divergence);
    return PublishText(text, written, sizeof(text), output, capacity);
}

/* Include independent child reasons instead of reducing them to one generic modified flag. */
UmiStatus UmiVcsWorkingTreeEntryText(const UmiVcsWorkingTree *tree, size_t index, char *output,
                                     size_t capacity)
{
    const UmiVcsWorkingTreeEntry *entry;
    char *storage, *path, *original, *text;
    const size_t name_capacity = UMI_VCS_PATH_CAPACITY * 4U;
    const size_t text_capacity = UMI_VCS_WORKING_TREE_ENTRY_TEXT_CAPACITY;
    UmiStatus status;
    int written;
    if (tree == NULL || output == NULL || capacity == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    entry = UmiVcsWorkingTreeEntryAt(tree, index);
    if (entry == NULL)
        return UMI_STATUS_NOT_FOUND;
    storage = malloc(name_capacity * 2U + text_capacity);
    if (storage == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    path = storage;
    original = path + name_capacity;
    text = original + name_capacity;
    DisplayName(entry->change.path, path);
    DisplayName(entry->change.original_path, original);
    written = snprintf(
        text, text_capacity, "\"%s\"\nIndex: %c; worktree: %c.%s%s%s%s\n%s%s%s%s", path,
        entry->index_code, entry->worktree_code,
        entry->unmerged        ? " Unresolved conflict."
        : entry->change.staged ? " Staged change."
                               : " Not staged.",
        original[0] ? "\nOriginal path: \"" : "", original, original[0] ? "\"" : "",
        entry->submodule ? "Child repository: the parent stores a commit identifier.\n" : "",
        entry->child_commit_changed
            ? "The checked-out child commit differs from the parent's index.\n"
            : "",
        entry->child_tracked_changed
            ? "The child has tracked file changes; commit those within the child.\n"
            : "",
        entry->child_untracked ? "The child has untracked files; review them within the child.\n"
                               : "");
    status = PublishText(text, written, text_capacity, output, capacity);
    free(storage);
    return status;
}

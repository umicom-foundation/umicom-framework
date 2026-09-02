/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/vcs/status.c
 *
 * PURPOSE:
 *   Implement Git porcelain v1 status parsing including untracked, ignored and renamed paths.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/vcs/status.h"

#include <string.h>

/*
 * Provide the vcs status copy path operation used by this module and its client
 * applications.
 */
static void umi_vcs_status_copy_path(char *destination,
                                     size_t capacity,
                                     const char *source)
{
    size_t length;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (destination == NULL || capacity == 0U) return;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (source == NULL) source = "";
    length = strlen(source);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length >= capacity) length = capacity - 1U;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length > 0U) (void)memcpy(destination, source, length);
    destination[length] = '\0';
}

/*
 * Provide the vcs status code state operation used by this module and its client
 * applications.
 */
UmiVcsChangeState umi_vcs_status_code_state(char code)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (code) {
        case ' ': return UMI_VCS_CHANGE_UNMODIFIED;
        case 'A': return UMI_VCS_CHANGE_ADDED;
        case 'M': return UMI_VCS_CHANGE_MODIFIED;
        case 'D': return UMI_VCS_CHANGE_DELETED;
        case 'R': return UMI_VCS_CHANGE_RENAMED;
        case 'C': return UMI_VCS_CHANGE_COPIED;
        case '?': return UMI_VCS_CHANGE_UNTRACKED;
        case '!': return UMI_VCS_CHANGE_IGNORED;
        case 'U': return UMI_VCS_CHANGE_CONFLICTED;
        default: return UMI_VCS_CHANGE_UNMODIFIED;
    }
}

/*
 * Provide the vcs status parse porcelain operation used by this module and its client
 * applications.
 */
UmiStatus umi_vcs_status_parse_porcelain(const char *output,
                                         UmiVcsChangeList *out_changes,
                                         size_t *out_parsed)
{
    const char *cursor;
    const char *end;
    char line[UMI_VCS_PATH_CAPACITY * 2U + 16U];
    size_t length;
    size_t parsed = 0U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (output == NULL || out_changes == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    umi_vcs_change_list_clear(out_changes);
    cursor = output;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (*cursor != '\0') {
        UmiVcsChange change;
        const char *path;
        const char *arrow;
        end = strchr(cursor, '\n');
        length = end != NULL ? (size_t)(end - cursor) : strlen(cursor);
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (length >= sizeof(line)) {
            length = sizeof(line) - 1U;
        }
        (void)memcpy(line, cursor, length);
        line[length] = '\0';
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (length >= 3U) {
            (void)memset(&change, 0, sizeof(change));
            change.index_state = umi_vcs_status_code_state(line[0]);
            change.worktree_state = umi_vcs_status_code_state(line[1]);
            change.staged = line[0] != ' ' &&
                            line[0] != '?' &&
                            line[0] != '!';
            path = line + 3;
            arrow = strstr(path, " -> ");
            /*
             * Protect caller-owned memory by checking that required state is available before it is
             * used.
             */
            if (arrow != NULL) {
                size_t old_length = (size_t)(arrow - path);
                /* Keep the operation inside its valid bounds before reading, writing or adding data. */
                if (old_length >= sizeof(change.original_path)) {
                    old_length = sizeof(change.original_path) - 1U;
                }
                (void)memcpy(change.original_path, path, old_length);
                change.original_path[old_length] = '\0';
                umi_vcs_status_copy_path(change.path,
                                         sizeof(change.path),
                                         arrow + 4);
            } /* Use this fallback path when the earlier condition does not apply. */ else {
                umi_vcs_status_copy_path(change.path,
                                         sizeof(change.path),
                                         path);
            }
            /* Apply this branch only when its contract condition is satisfied. */
            if (umi_vcs_change_list_add(out_changes, &change) ==
                UMI_STATUS_OK) {
                parsed += 1U;
            }
        }
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (end == NULL) {
            break;
        }
        cursor = end + 1;
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_parsed != NULL) {
        *out_parsed = parsed;
    }
    return UMI_STATUS_OK;
}

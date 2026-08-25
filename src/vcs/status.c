/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/vcs/status.c
 *
 * PURPOSE:
 *   Implement Git porcelain v1 status parsing including untracked, ignored and renamed paths.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/vcs/status.h"

#include <string.h>

static void umi_vcs_status_copy_path(char *destination,
                                     size_t capacity,
                                     const char *source)
{
    size_t length;
    if (destination == NULL || capacity == 0U) return;
    if (source == NULL) source = "";
    length = strlen(source);
    if (length >= capacity) length = capacity - 1U;
    if (length > 0U) (void)memcpy(destination, source, length);
    destination[length] = '\0';
}

UmiVcsChangeState umi_vcs_status_code_state(char code)
{
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

UmiStatus umi_vcs_status_parse_porcelain(const char *output,
                                         UmiVcsChangeList *out_changes,
                                         size_t *out_parsed)
{
    const char *cursor;
    const char *end;
    char line[UMI_VCS_PATH_CAPACITY * 2U + 16U];
    size_t length;
    size_t parsed = 0U;
    if (output == NULL || out_changes == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    umi_vcs_change_list_clear(out_changes);
    cursor = output;
    while (*cursor != '\0') {
        UmiVcsChange change;
        const char *path;
        const char *arrow;
        end = strchr(cursor, '\n');
        length = end != NULL ? (size_t)(end - cursor) : strlen(cursor);
        if (length >= sizeof(line)) {
            length = sizeof(line) - 1U;
        }
        (void)memcpy(line, cursor, length);
        line[length] = '\0';
        if (length >= 3U) {
            (void)memset(&change, 0, sizeof(change));
            change.index_state = umi_vcs_status_code_state(line[0]);
            change.worktree_state = umi_vcs_status_code_state(line[1]);
            change.staged = line[0] != ' ' &&
                            line[0] != '?' &&
                            line[0] != '!';
            path = line + 3;
            arrow = strstr(path, " -> ");
            if (arrow != NULL) {
                size_t old_length = (size_t)(arrow - path);
                if (old_length >= sizeof(change.original_path)) {
                    old_length = sizeof(change.original_path) - 1U;
                }
                (void)memcpy(change.original_path, path, old_length);
                change.original_path[old_length] = '\0';
                umi_vcs_status_copy_path(change.path,
                                         sizeof(change.path),
                                         arrow + 4);
            } else {
                umi_vcs_status_copy_path(change.path,
                                         sizeof(change.path),
                                         path);
            }
            if (umi_vcs_change_list_add(out_changes, &change) ==
                UMI_STATUS_OK) {
                parsed += 1U;
            }
        }
        if (end == NULL) {
            break;
        }
        cursor = end + 1;
    }
    if (out_parsed != NULL) {
        *out_parsed = parsed;
    }
    return UMI_STATUS_OK;
}

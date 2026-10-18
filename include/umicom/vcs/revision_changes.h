/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/vcs/revision_changes.h
 * Purpose: Observe committed path changes between explicit immutable Git commit identities.
 * Author and Organisation: Sammy Hegab, Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_VCS_REVISION_CHANGES_H
#define UMICOM_VCS_REVISION_CHANGES_H
#include "umicom/base/status.h"
#include "umicom/platform/process.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_VCS_REVISION_CHANGE_BYTE_LIMIT (32U * 1024U * 1024U)
#define UMI_VCS_REVISION_CHANGE_LIMIT 100000U
#define UMI_VCS_REVISION_PATH_LIMIT 4096U

typedef struct UmiVcsRevisionChanges UmiVcsRevisionChanges;
/** Paths are owned raw Git filename bytes, not shell commands. A and D preserve
 * both sides of a rename because discovery disables rename collapsing. Modes
 * are octal Git modes represented as integers. A gitlink change requires child
 * repository qualification as well as parent integration checks. */
typedef struct UmiVcsRevisionChange {
    const char *path;
    char status;
    uint32_t old_mode, new_mode;
    bool gitlink;
} UmiVcsRevisionChange;
/** Explicit complete 40- or 64-character hexadecimal commit identities, not
 * branch names or revision expressions. Both must use the same hash format.
 * Zero timeout selects 30 seconds. All inputs are borrowed through the call. */
typedef struct UmiVcsRevisionChangesRequest {
    const char *repository_root;
    const char *git_program;
    const char *base_commit;
    const char *head_commit;
    uint32_t timeout_ms;
    const UmiCancellationToken *cancellation;
} UmiVcsRevisionChangesRequest;

/** Parse the complete NUL-delimited git diff --raw --no-abbrev --no-renames -z
 * format. Accept A, D, M and T only. Reject combined/unmerged/rename output,
 * inconsistent modes, duplicate paths, truncated records and exhausted bounds.
 * Empty output is an empty comparison, never qualification evidence. Input is
 * copied; results are sorted by exact path bytes. *outChanges must be NULL and
 * stays unchanged on failure. No process or filesystem operation occurs. */
UmiStatus UmiVcsRevisionChangesParse(const void *bytes, size_t length,
    UmiVcsRevisionChanges **outChanges);
/** Read one comparison between the two explicit commits. External diff and
 * text-conversion helpers, replacement objects and lazy fetching are disabled.
 * Git must support --no-lazy-fetch; unsupported tools fail instead of retrying
 * with weaker semantics. This does not fetch, stage, commit,
 * switch branches, inspect uncommitted changes or certify source freshness.
 * Errors never publish a partial list. Use from a worker in interactive hosts.
 * Repository/environment trust remains the caller's responsibility. */
UmiStatus UmiVcsRevisionChangesRead(const UmiVcsRevisionChangesRequest *request,
    UmiVcsRevisionChanges **outChanges);
/** Release the observation and its paths. NULL is safe. */
void UmiVcsRevisionChangesDestroy(UmiVcsRevisionChanges *changes);
/** Count committed path changes; NULL returns zero. */
size_t UmiVcsRevisionChangesCount(const UmiVcsRevisionChanges *changes);
/** Borrow an entry until destruction; an invalid index returns NULL. */
const UmiVcsRevisionChange *UmiVcsRevisionChangesAt(
    const UmiVcsRevisionChanges *changes, size_t index);
#ifdef __cplusplus
}
#endif
#endif

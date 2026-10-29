/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/teacher/learning_pack_changes.h
 *
 * PURPOSE:
 *   Describe immutable resource metadata changes before a learning pack is replaced.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEACHER_LEARNING_PACK_CHANGES_H
#define UMICOM_TEACHER_LEARNING_PACK_CHANGES_H
#include "umicom/teacher/learning_pack.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef enum UmiLearningResourceChangeKind {
    UMI_LEARNING_RESOURCE_ADDED = 1,
    UMI_LEARNING_RESOURCE_REMOVED = 2,
    UMI_LEARNING_RESOURCE_MODIFIED = 3
} UmiLearningResourceChangeKind;
#define UMI_LEARNING_CHANGE_TITLE 1U
#define UMI_LEARNING_CHANGE_TOPIC 2U
#define UMI_LEARNING_CHANGE_KIND 4U
#define UMI_LEARNING_CHANGE_PATH 8U
#define UMI_LEARNING_CHANGE_DIGEST 16U
#define UMI_LEARNING_CHANGE_EXAMPLE 32U

typedef struct UmiLearningResourceChange {
    UmiLearningResourceChangeKind kind;
    unsigned int fields;
    /* For additions previous is zero; for removals proposed is zero.
     * Modified entries contain both complete metadata values. */
    UmiLearningPackResource previous;
    UmiLearningPackResource proposed;
} UmiLearningResourceChange;

typedef struct UmiLearningPackChangeSummary {
    size_t added;
    size_t removed;
    size_t modified;
    size_t unchanged;
    /* Modified resources whose admitted SHA-256 differs. Added and removed
     * resources are counted separately; no resource file has been read. */
    size_t changed_digests;
    size_t change_count;
} UmiLearningPackChangeSummary;
typedef struct UmiLearningPackChanges UmiLearningPackChanges;

/* Compare two admitted manifests by exact resource ID, independently of array
 * ordering. At most 512 changes can result from two bounded 256-resource packs.
 * Modified/removed entries follow previous order, then added entries follow
 * proposed order. Unchanged resources are counted but have no change entry.
 * Copies metadata; packs may be destroyed after return. No filesystem access,
 * content verification, publisher authentication or replacement occurs.
 * Pack identity/revision compatibility is deliberately a separate admission
 * decision: use ShelfReplace for its stricter replacement policy.
 * Every failure clears out_changes. Serialize source ownership during the call. */
UmiStatus UmiLearningPackCompare(const UmiLearningPack *previous,
    const UmiLearningPack *proposed, UmiLearningPackChanges **out_changes);
void UmiLearningPackChangesDestroy(UmiLearningPackChanges *changes);
/* Copy pointer-free results. Refusal preserves output. */
UmiStatus UmiLearningPackChangesCopySummary(const UmiLearningPackChanges *changes,
    UmiLearningPackChangeSummary *out_summary);
UmiStatus UmiLearningPackChangeAt(const UmiLearningPackChanges *changes,
    size_t index, UmiLearningResourceChange *out_change);
#ifdef __cplusplus
}
#endif
#endif

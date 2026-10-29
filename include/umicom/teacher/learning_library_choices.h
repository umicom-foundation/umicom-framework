/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/teacher/learning_library_choices.h
 *
 * PURPOSE:
 *   Remember explicitly chosen learning roots without granting trust or executing content.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TEACHER_LEARNING_LIBRARY_CHOICES_H
#define UMICOM_TEACHER_LEARNING_LIBRARY_CHOICES_H
#include "umicom/teacher/learning_pack.h"
#include "umicom/platform/path.h"
#include "umicom/platform/rooted_files.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_LEARNING_LIBRARY_CHOICES_BYTES (256U * 1024U)
#define UMI_LEARNING_LIBRARY_CHOICES_MAX 32U

typedef struct UmiLearningLibraryChoice {
    char root[UMI_PATH_CAPACITY];
    char manifest[UMI_TEACHER_PATH_CAPACITY];
    char pack_id[UMI_TEACHER_ID_CAPACITY];
    char product[UMI_TEACHER_ID_CAPACITY];
    char locale[UMI_LEARNING_PACK_LOCALE_CAPACITY];
    uint64_t revision;
} UmiLearningLibraryChoice;
typedef struct UmiLearningLibraryChoices UmiLearningLibraryChoices;

/* The owner keeps up to 32 copied choices, keyed by pack ID. Serialize all
 * reads against mutation/destruction. Creating, remembering, forgetting and
 * encoding choices perform no file access and do not change any shelf. */
UmiStatus UmiLearningLibraryChoicesCreate(UmiLearningLibraryChoices **out_choices);
void UmiLearningLibraryChoicesDestroy(UmiLearningLibraryChoices *choices);
UmiStatus UmiLearningLibraryChoicesClone(const UmiLearningLibraryChoices *choices,
    UmiLearningLibraryChoices **out_choices);
/* Capture admitted pack metadata and its absolute root; manifest is a portable
 * relative name. Duplicate IDs return ALREADY_EXISTS, including identical
 * choices. Forget explicitly before changing a root or recorded revision.
 * Metadata admission does not authenticate a publisher or verify all resources. */
UmiStatus UmiLearningLibraryChoicesRemember(UmiLearningLibraryChoices *choices,
    const UmiLearningPack *pack, const char *manifest);
UmiStatus UmiLearningLibraryChoicesForget(UmiLearningLibraryChoices *choices,
    const char *pack_id);
size_t UmiLearningLibraryChoicesCount(const UmiLearningLibraryChoices *choices);
/* Failure preserves caller output. Copied choices have no owning pointers. */
UmiStatus UmiLearningLibraryChoicesAt(const UmiLearningLibraryChoices *choices,
    size_t index, UmiLearningLibraryChoice *out_choice);

/* Strict versioned JSON: reject unknown/duplicate fields, relative/ambiguous
 * roots, invalid UTF-8, unsupported versions and repeated pack IDs. Decode
 * owns all strings and reads no folders. Failure clears out_choices. Encode
 * writes complete JSON plus NUL only on success. Query size with NULL/0;
 * CAPACITY_EXCEEDED reports required bytes excluding NUL and preserves bytes.
 * Other failures preserve out_size. Inputs and outputs must not alias owners. */
UmiStatus UmiLearningLibraryChoicesDecode(const void *bytes, size_t size,
    UmiLearningLibraryChoices **out_choices);
UmiStatus UmiLearningLibraryChoicesEncode(const UmiLearningLibraryChoices *choices,
    char *out_bytes, size_t capacity, size_t *out_size);

/* Explicit worker-side I/O. Load reads only this settings file: it never scans
 * roots, opens a pack, fetches a URL or starts a process. SaveNew exclusively
 * creates a new file and never overwrites existing settings; inspect report
 * on failure because an incomplete new file may remain. Choose an existing
 * private folder outside a project repository. These APIs do not enforce an
 * operating-system privacy policy or create parents. */
UmiStatus UmiLearningLibraryChoicesLoad(const char *root, const char *relative_path,
    UmiLearningLibraryChoices **out_choices);
UmiStatus UmiLearningLibraryChoicesSaveNew(const UmiLearningLibraryChoices *choices,
    const char *root, const char *relative_path, UmiRootedFileCreation *report);

/* Reopen one copied choice only when requested. Re-read the manifest through
 * bounded rooted access and host-version admission; require the recorded ID,
 * product, case-insensitive locale and exact revision. A changed identity or
 * revision returns INVALID_STATE for explicit review. Missing folders/files
 * are reported, never searched elsewhere. Each later resource read still
 * verifies its bytes against the newly admitted manifest. This does not prove
 * manifest authenticity or that same-revision bytes match a previous session.
 * Success transfers an independently owned pack; failure clears out_pack.
 * Call on a worker with an owned clone, not a mutable GUI-owned choices object. */
UmiStatus UmiLearningLibraryChoicesReopen(const UmiLearningLibraryChoices *choices,
    size_t index, UmiVersion host_version, UmiLearningPack **out_pack);
#ifdef __cplusplus
}
#endif
#endif

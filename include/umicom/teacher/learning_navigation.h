/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/teacher/learning_navigation.h
 *
 * PURPOSE:
 *   Resolve local lesson links against an admitted offline learning pack.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TEACHER_LEARNING_NAVIGATION_H
#define UMICOM_TEACHER_LEARNING_NAVIGATION_H
#include "umicom/teacher/learning_pack.h"
#ifdef __cplusplus
extern "C" {
#endif

#define UMI_LEARNING_FRAGMENT_CAPACITY 128U

typedef struct UmiLearningPackLink {
    UmiLearningPackResource resource;
    char fragment[UMI_LEARNING_FRAGMENT_CAPACITY];
} UmiLearningPackLink;

/* Resolve an already-decoded, portable pack-relative filename. The file must
 * appear in the manifest. Slash spelling is canonicalized for comparison;
 * multiple declarations of the same path return ALREADY_EXISTS, never the first
 * arbitrary match. Syntax and lookup only: read through ReadResource to check
 * storage policy and content integrity. Refusal leaves output unchanged. */
UmiStatus UmiLearningPackFindPath(const UmiLearningPack *pack, const char *relative_path,
    UmiLearningPackResource *out);

/* Resolve a local href relative to the current resource's containing folder.
 * An empty href or #fragment stays on that resource. Nested forward-slash paths
 * are supported, but '.', '..', absolute paths, backslashes, schemes, query
 * strings and percent escapes are refused. This deliberately small offline
 * profile does not implement general browser URL processing. Use literal
 * filenames; author portable links without percent escapes or parent traversal.
 * A fragment is empty or ASCII letters/digits/underscore/hyphen/period/colon.
 * It is a requested anchor, not proof that the document contains that anchor.
 *
 * No file is opened and no navigation, network request or program is launched.
 * The result copies metadata and never replaces the current document on error.
 * Preserve the pack's lifetime until its selected resource has been read.
 * Existing foundations allow-list resolution remains a separate API. */
UmiStatus UmiLearningPackResolveLink(const UmiLearningPack *pack, const char *current_resource,
    const char *href, UmiLearningPackLink *out);

#ifdef __cplusplus
}
#endif
#endif

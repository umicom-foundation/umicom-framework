/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/teacher/learning_module.h
 *
 * PURPOSE:
 *   Locate bundled content and prepare a verified module reading session.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEACHER_LEARNING_MODULE_H
#define UMICOM_TEACHER_LEARNING_MODULE_H
#include "umicom/teacher/learning_reader.h"
#ifdef __cplusplus
extern "C" {
#endif

/* Resolve <executable directory>/learning-modules/<name>, without filesystem
 * access or current-directory fallback. name is one ASCII identifier composed
 * of letters, digits, '-' and '_'. Output is unchanged on failure. */
UmiStatus UmiLearningModuleRoot(const char *executable, const char *name,
    char *out_root, size_t capacity);

/* Blocking operation: use a worker, never the GUI thread. Clone the input shelf
 * and prepare a reader before publishing either. If pack_id is already loaded,
 * use that admitted pack; never silently replace it from directory. Otherwise
 * load directory/learning-pack.json using the host compatibility version.
 * Require the exact product and pack identities and verify the entry resource
 * bytes. A missing module, bad digest or incompatible version leaves the input
 * shelf and any existing reader unchanged. On failure both outputs are NULL.
 * On success the caller owns both outputs; commit them together or destroy both.
 * Serialize the input shelf for this call. Use distinct output storage. A digest
 * establishes content consistency, not publisher authenticity or execution trust.
 * This content module cannot load native code, execute scripts or install tools. */
UmiStatus UmiLearningModuleOpen(const UmiLearningShelf *shelf, const char *directory,
    const char *pack_id, const char *product, const char *resource_id,
    UmiVersion host, size_t maximum_bytes, UmiLearningShelf **out_shelf,
    UmiLearningReader **out_reader);
#ifdef __cplusplus
}
#endif
#endif

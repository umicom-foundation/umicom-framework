/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/teacher/learning_practice.h
 *
 * PURPOSE:
 *   Copy a verified learning example into an explicitly chosen new practice file.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TEACHER_LEARNING_PRACTICE_H
#define UMICOM_TEACHER_LEARNING_PRACTICE_H
#include "umicom/teacher/learning_pack.h"
#include "umicom/platform/rooted_files.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_LEARNING_PRACTICE_MAX_BYTES (16U * 1024U * 1024U)

typedef struct UmiLearningPracticeResult {
    UmiStatus status;
    UmiLearningPackResource example;
    size_t source_size;
    int source_verified;
    int destination_attempted;
    UmiRootedFileCreation destination;
} UmiLearningPracticeResult;

/* Return a copied EXAMPLE resource, either selected directly or referenced by
 * the selected lesson/guide/image's manifest example field. No disk access is
 * performed. Missing selection or missing example link returns NOT_FOUND.
 * Output remains unchanged on failure. Applications can preview the title and
 * destination before requesting a copy; this lookup does not verify bytes. */
UmiStatus UmiLearningPracticeResolveExample(const UmiLearningPack *pack,
    const char *selected_resource_id, UmiLearningPackResource *out_example);

/* Read and verify the selected example completely before attempting to create
 * a destination beneath the caller's chosen existing root. Never overwrite an
 * existing leaf, create parents, delete partial output, download, build or run.
 * maximum_bytes must be 1..16 MiB. Calls can block and belong on a worker.
 *
 * result is required and initialized even on failure. destination is meaningful
 * only when destination_attempted is true. A creation failure can leave a new
 * partial file; destination.created/bytes_written/complete distinguish this
 * from refusal before creation. Do not treat file existence as a successful
 * copy. Use the returned status, then present the result to the user.
 *
 * Keep pack and input strings alive, serialize conflicting client edits and
 * do not alias inputs with result. Source hashes check consistency, not author
 * identity. The destination root's ancestors are trusted, as documented by
 * rooted_files. Copying grants no project or command execution permission.
 * This operation copies one file; it does not claim to create a buildable
 * multi-file project or preserve source filesystem metadata. */
UmiStatus UmiLearningPracticeCopy(const UmiLearningPack *pack,
    const char *selected_resource_id, const char *destination_root,
    const char *destination_path, size_t maximum_bytes, UmiLearningPracticeResult *result);
#ifdef __cplusplus
}
#endif
#endif

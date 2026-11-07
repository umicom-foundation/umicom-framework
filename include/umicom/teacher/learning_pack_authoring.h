/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/teacher/learning_pack_authoring.h
 *
 * PURPOSE:
 *   Prepare a reviewed revision of an offline learning pack from its current local files.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEACHER_LEARNING_PACK_AUTHORING_H
#define UMICOM_TEACHER_LEARNING_PACK_AUTHORING_H
#include "umicom/teacher/learning_pack.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_LEARNING_AUTHORING_RESOURCE_BYTES (1024U * 1024U)

/* Re-read each declared resource below the admitted pack root and prepare JSON
 * with current SHA-256 values. This is an explicit authoring operation: it
 * intentionally accepts changed lesson bytes instead of checking old hashes.
 * It does not approve their meaning, establish a signer or execute examples.
 *
 * new_revision must be greater than source's revision and at most INT64_MAX.
 * maximum_resource_bytes is 1..UMI_LEARNING_AUTHORING_RESOURCE_BYTES. Rooted-file
 * rules reject links below the root. Keep source alive and serialize edits while
 * preparing; independent file reads are not an atomic directory snapshot.
 *
 * No file is written. Success returns owned UTF-8 JSON, NUL-terminated with a
 * byte length excluding the terminator; free using UmiLearningPackFreeBytes.
 * Failure clears both outputs. Use separate valid output storage which does
 * not alias source. Filesystem reads can block and belong on a worker in a GUI.
 * Existing pack metadata, reader history and shelf membership stay unchanged. */
UmiStatus UmiLearningPackPrepareRevision(const UmiLearningPack *source,
    uint64_t new_revision, size_t maximum_resource_bytes,
    unsigned char **out_json, size_t *out_length);
#ifdef __cplusplus
}
#endif
#endif

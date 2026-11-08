/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/toolchain/file_observation.h
 *
 * PURPOSE:
 *   Capture explicitly selected file bytes as immutable SHA-256 evidence for development workflows.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TOOLCHAIN_FILE_OBSERVATION_H
#define UMICOM_TOOLCHAIN_FILE_OBSERVATION_H
#include "umicom/base/status.h"
#include <stddef.h>
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_FILE_OBSERVATION_MAX_FILES 64U
#define UMI_FILE_OBSERVATION_MAX_BYTES (64U * 1024U * 1024U)
#define UMI_FILE_OBSERVATION_TOTAL_BYTES (256U * 1024U * 1024U)
#define UMI_FILE_OBSERVATION_JSON_CAPACITY (1024U * 1024U)
typedef struct UmiFileObservation UmiFileObservation;
typedef struct UmiFileObservationItem {
    const char *path;
    size_t bytes;
    char sha256[65];
} UmiFileObservationItem;
/* Read 1..64 explicitly selected ordinary absolute UTF-8 paths, in order.
 * Validate the complete selection before the first read. Exact duplicate path
 * strings are rejected; aliases/case differences are not coalesced. All input
 * strings remain caller-owned and unchanged. maximumFileBytes must be nonzero
 * and no greater than MAX_BYTES. Combined file bytes cannot exceed TOTAL_BYTES.
 *
 * *outObservation must initially be NULL. Success owns all path and digest
 * storage. Failure preserves that pointer and returns no partial observation.
 * Optional outFailedIndex is SIZE_MAX for general errors, or the selected path
 * index for validation/read/hash failure. It must not alias other arguments.
 *
 * Reuses UmiInputFileRead: final links/devices are refused, trusted parent
 * aliases can be followed, and observable changes on the open file are checked.
 * Reads may block; call from a worker in GUI clients. This is a sequence of byte
 * observations, not an atomic filesystem snapshot or a claim of trusted origin.
 * It does not identify loaded modules, execute files, check vendor patches or
 * prove build/test success. Keep inputs stable and compare with trusted evidence. */
UmiStatus UmiFileObservationCapture(const char *const *paths, size_t count,
    size_t maximumFileBytes, UmiFileObservation **outObservation, size_t *outFailedIndex);
void UmiFileObservationDestroy(UmiFileObservation *observation);
size_t UmiFileObservationCount(const UmiFileObservation *observation);
const UmiFileObservationItem *UmiFileObservationAt(const UmiFileObservation *observation, size_t index);
/* Format schema-1 JSON into separate caller storage. On failure clear writable
 * output and zero optional outSize. Size excludes the NUL. No reread occurs. */
UmiStatus UmiFileObservationFormat(const UmiFileObservation *observation,
    char *json, size_t capacity, size_t *outSize);
/* Format before exclusive creation. The absolute destination's parent must
 * already exist. Never replace old evidence or remove a partial failed write.
 * Captured local paths may be private; choose an appropriate report location. */
UmiStatus UmiFileObservationSave(const UmiFileObservation *observation, const char *path);
#ifdef __cplusplus
}
#endif
#endif

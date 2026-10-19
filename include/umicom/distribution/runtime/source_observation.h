/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/distribution/runtime/source_observation.h
 * Purpose: Observe inventory-listed source bytes before and after qualification.
 * Author and Organisation: Sammy Hegab, Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_DISTRIBUTION_RUNTIME_SOURCE_OBSERVATION_H
#define UMICOM_DISTRIBUTION_RUNTIME_SOURCE_OBSERVATION_H
#include "umicom/distribution/runtime/inventory.h"
#include "umicom/base/sha256.h"
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_RELEASE_SOURCE_FILE_LIMIT (16U * 1024U * 1024U)
#define UMI_RELEASE_SOURCE_TOTAL_LIMIT (512U * 1024U * 1024U)
#define UMI_RELEASE_SOURCE_PATH_BYTES_LIMIT (64U * 1024U * 1024U)

typedef struct UmiReleaseSourceObservation UmiReleaseSourceObservation;
/* A provider runs synchronously and must hash the exact bytes it reads. It may
 * not return OK for a missing, oversized, unreadable or observably changing
 * file. The path is borrowed for the call. Write a lowercase SHA-256 and actual
 * size only on success. Providers must respect maximumBytes before allocating.
 * NULL selects the Framework regular-file reader. Injection allows a host to
 * supply its own read-only storage service and tests to avoid real files. */
typedef UmiStatus (*UmiReleaseSourceReadFn)(void *context, const char *path,
    size_t maximumBytes, size_t *outSize, char outSha256[65]);
typedef bool (*UmiReleaseSourceCancelledFn)(void *context);
/* Optional diagnostic identifies the failing inventory input or read path. The
 * string is borrowed only for this callback; do not destroy inventory here.
 * Called at most once, before temporary observation storage is released. */
typedef void (*UmiReleaseSourceDiagnosticFn)(void *context, UmiStatus status, const char *identity);
typedef struct UmiReleaseSourceOptions {
    size_t maximumFileBytes;
    size_t maximumTotalBytes;
    UmiReleaseSourceReadFn read;
    UmiReleaseSourceCancelledFn cancelled;
    void *context;
    UmiReleaseSourceDiagnosticFn diagnostic;
} UmiReleaseSourceOptions;
typedef struct UmiReleaseSourceItem {
    const char *path;
    size_t bytes;
    char sha256[65];
} UmiReleaseSourceItem;
typedef struct UmiReleaseSourceComparison {
    size_t files;
    size_t changedFiles;
    bool same;
} UmiReleaseSourceComparison;

/* Capture all present sources and declared or unassigned public headers in a
 * CMake inventory, once per exact normalised path. Reject unresolved inputs,
 * missing header path metadata, non-absolute paths and an empty input set.
 * All paths are validated before the first provider call. Only separators are
 * normalised; case and filesystem aliases are not folded. Inventories are
 * trusted file-selection inputs, not a directory confinement boundary.
 *
 * NULL options chooses the limits above; explicit limits must be nonzero and
 * no greater than those maxima. Cancellation is checked between file reads;
 * one read may block. Invoke this service on a worker in interactive software.
 * *outObservation must be NULL; failure preserves it and returns no partial
 * result. The successful result owns paths/digests independently of inventory.
 *
 * A fingerprint binds inventory context, every inventory record and the
 * observed bytes of listed files. It does NOT observe unlisted files, undeclared private
 * headers, compiler/toolchain bytes, environment variables or external inputs.
 * Reconfigure when configuration or file membership changes. Before/after
 * observations are not an atomic workspace snapshot: an edit followed by a
 * reversal between reads is not detected. Keep qualification inputs stable;
 * equality is neither test success nor permission to accept mixed-source work.
 * This is an unkeyed content digest, not trusted provenance or a signature. */
UmiStatus UmiReleaseSourceObserve(const UmiReleaseInventory *inventory,
    const UmiReleaseSourceOptions *options, UmiReleaseSourceObservation **outObservation);
void UmiReleaseSourceObservationDestroy(UmiReleaseSourceObservation *observation);
size_t UmiReleaseSourceObservationCount(const UmiReleaseSourceObservation *observation);
const UmiReleaseSourceItem *UmiReleaseSourceObservationAt(
    const UmiReleaseSourceObservation *observation, size_t index);
const char *UmiReleaseSourceObservationFingerprint(const UmiReleaseSourceObservation *observation);
/* Compare only observations from identical inventory context and records.
 * A different generation, configuration, file set or graph is INVALID_STATE,
 * never a successful match. Failure leaves *outComparison unchanged. */
UmiStatus UmiReleaseSourceObservationCompare(const UmiReleaseSourceObservation *before,
    const UmiReleaseSourceObservation *after, UmiReleaseSourceComparison *outComparison);
/** Borrow the configured-inventory fingerprint until observation destruction.
 * NULL returns NULL. Match this with the plan before attributing run results. */
const char *UmiReleaseSourceObservationInventoryFingerprint(
    const UmiReleaseSourceObservation *observation);

#ifdef __cplusplus
}
#endif
#endif

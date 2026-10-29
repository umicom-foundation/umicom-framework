/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/distribution/runtime/source_observation_archive.h
 *
 * PURPOSE:
 *   Preserve declared source observations as bounded historical data across restarts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DISTRIBUTION_SOURCE_OBSERVATION_ARCHIVE_H
#define UMICOM_DISTRIBUTION_SOURCE_OBSERVATION_ARCHIVE_H
#include "umicom/distribution/runtime/source_observation.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_RELEASE_SOURCE_ARCHIVE_LIMIT (256U * 1024U * 1024U)

/** Encode a canonical version-one historical observation without file I/O.
 * *outBytes must be NULL. Returned storage includes a terminating null byte,
 * but outLength excludes it and outSha256 hashes only the encoded data bytes.
 * Free storage with FreeEncoded. All three outputs are unchanged on failure.
 * Serialize access to caller output storage; it must not overlap the inputs.
 * Paths may reveal private workstation details: hosts must choose private storage.
 */
UmiStatus UmiReleaseSourceObservationEncode(const UmiReleaseSourceObservation *observation,
    char **outBytes, size_t *outLength, char outSha256[65]);
void UmiReleaseSourceObservationFreeEncoded(char *bytes);

/** Reopen against the exact configured inventory, without reading source files.
 * expectedSha256 is a mandatory lowercase digest retained separately by the host.
 * A digest mismatch or different inventory is INVALID_STATE; malformed encoding,
 * unsupported version, inconsistent path membership or fingerprint is refused.
 * The inventory collector validates and owns the expected paths. Encoded paths
 * must match them exactly in sorted order, so no unlisted path is trusted.
 * Counts, file sizes, total source bytes, text size and path storage are bounded.
 * *outObservation must be NULL and is unchanged on failure. The successful object
 * owns its paths independently of encoded bytes and inventory. Destroy it using
 * UmiReleaseSourceObservationDestroy, like a live observation.
 *
 * This reconstructs historical observations, not historical source file contents
 * or an atomic filesystem snapshot. A matching digest is integrity against a
 * retained receipt, not authentication or proof of a completed build/test. Hosts
 * must trust the origin separately. Never relabel a reopened record as current:
 * capture a fresh observation before planning or attributing new execution.
 */
UmiStatus UmiReleaseSourceObservationDecode(const UmiReleaseInventory *inventory,
    const char *bytes, size_t length, const char *expectedSha256,
    UmiReleaseSourceObservation **outObservation);

typedef struct UmiSourceObservationReceipt {
    size_t bytes;
    char archiveSha256[65];
    char sourceSha256[65];
} UmiSourceObservationReceipt;
/** Save by exclusive creation in an existing owner-selected private directory.
 * No overwrite, directory creation or cleanup occurs. Failed/partial files are
 * retained and must not be treated as successful archives. The receipt is
 * assigned only after writing and closing succeed. I/O can block a worker.
 * Parent aliases may be followed; the path is not a confinement boundary. */
UmiStatus UmiReleaseSourceObservationSave(const UmiReleaseSourceObservation *observation,
    const char *absolutePath, UmiSourceObservationReceipt *outReceipt);
/** Read one bounded regular file and apply Decode, including its mandatory
 * retained digest and exact inventory checks. Source paths in the archive are
 * never read. Final symlinks/devices are refused by the native input service.
 * Relative paths are refused. No output changes on failure. */
UmiStatus UmiReleaseSourceObservationLoad(const UmiReleaseInventory *inventory,
    const char *absolutePath, const char *expectedSha256,
    UmiReleaseSourceObservation **outObservation);
#ifdef __cplusplus
}
#endif
#endif

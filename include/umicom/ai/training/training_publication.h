/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/training/training_publication.h
 *
 * PURPOSE:
 *   Publish and independently verify a compact completion marker for an
 *   approved native AdamW checkpoint and its three existing policy records.
 *
 * ARCHITECTURE AND TRUST:
 *   Reuse the Framework's SHA-256 service and the existing combined checkpoint,
 *   clipping, run-lock and source-lock formats without modifying any of them.
 *   The marker binds the EXACT bytes present in four approved local files;
 *   it does not parse their semantic settings, authenticate a publisher,
 *   attest the host, validate the checkpoint codec, or sign the result.
 *   Callers must perform the established strict-policy/dataset verification
 *   independently. This is a local integrity mechanism, not a sandbox.
 *
 *   Marker creation uses a same-directory exclusive staging file. An
 *   existing marker is never replaced, including after a failed run. Its
 *   presence does not make the four underlying files one atomic filesystem
 *   transaction. A reader must call VerifyFile before trusting their bytes.
 *
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_TRAINING_TRAINING_PUBLICATION_H
#define UMICOM_AI_TRAINING_TRAINING_PUBLICATION_H

#include <stdint.h>
#include "umicom/base/sha256.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_AI_TRAINING_PUBLICATION_MAX_PATH 4096U
#define UMI_AI_TRAINING_PUBLICATION_RECORD_CAPACITY 1024U
#define UMI_AI_TRAINING_PUBLICATION_MAX_COMPANION_BYTES 4096U

/* Fully copied digests; no buffers are retained by the Framework. */
typedef struct UmiAiTrainingPublicationFileIdentity {
    uint64_t byteCount;
    char sha256[UMI_SHA256_HEX_CAPACITY];
} UmiAiTrainingPublicationFileIdentity;

typedef struct UmiAiTrainingPublicationEvidence {
    UmiAiTrainingPublicationFileIdentity checkpoint;
    UmiAiTrainingPublicationFileIdentity clipPolicy;
    UmiAiTrainingPublicationFileIdentity trainingLock;
    UmiAiTrainingPublicationFileIdentity sourceLock;
} UmiAiTrainingPublicationEvidence;

/* Hash the four fixed paths derived from checkpointPath. Reject absent,
 * empty and oversized files, including changed-in-read files that exceed the
 * size caps. On failure *outEvidence remains unchanged. The returned digests
 * describe read bytes, not a cryptographic assertion about their origin. */
UmiStatus UmiAiTrainingPublicationInspect(
    const char *checkpointPath,
    UmiAiTrainingPublicationEvidence *outEvidence);

/* No-clobber publication of checkpointPath + ".publication". The four
 * original files must already exist. The marker is published last through
 * an exclusive staging file and exclusive final-path creation; prior marker
 * bytes are never overwritten or automatically repaired. There is no
 * rollback of existing checkpoint/sidecar files if publication fails. */
UmiStatus UmiAiTrainingPublicationSaveNewFile(const char *checkpointPath);

/* Rehash all four files and compare a canonical, length-bounded marker.
 * Every byte, line order, size and digest must match. An incomplete marker,
 * changed file or missing companion is rejected. This does not independently
 * establish that the source bytes or optimiser settings were authorised. */
UmiStatus UmiAiTrainingPublicationVerifyFile(const char *checkpointPath);

/* Verify and return the SAME four file identities used for comparison. The
 * result is published only if the record matches; errors preserve output.
 * Avoid a second inspect call after verification, which could observe changed
 * files in between. The caller must still protect the files from concurrent
 * changes before any subsequent use. */
UmiStatus UmiAiTrainingPublicationVerifyAndInspectFile(
    const char *checkpointPath,
    UmiAiTrainingPublicationEvidence *outEvidence);

#ifdef __cplusplus
}
#endif
#endif /* UMICOM_AI_TRAINING_TRAINING_PUBLICATION_H */

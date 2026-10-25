/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/training/source_admission.h
 * PURPOSE:
 *   Apply explicit source-rights and human-review gates before extracted text
 *   can enter a training/evaluation manifest. Document and media extractors
 *   remain independent providers; this contract admits their reviewed output.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_TRAINING_SOURCE_ADMISSION_H
#define UMICOM_AI_TRAINING_SOURCE_ADMISSION_H

#include <stddef.h>
#include "umicom/ai/training/dataset_manifest.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_AI_SOURCE_DIGEST_TEXT_CAPACITY 65U
#define UMI_AI_SOURCE_MANIFEST_LINE_CAPACITY (UMI_AI_DATASET_MAX_PATH + 8U)

typedef enum UmiAiTrainingSourceMedium {
    UMI_AI_TRAINING_SOURCE_MARKDOWN = 1,
    UMI_AI_TRAINING_SOURCE_PLAIN_TEXT = 2,
    UMI_AI_TRAINING_SOURCE_DOCUMENT_TEXT = 3,
    UMI_AI_TRAINING_SOURCE_AUDIO_TRANSCRIPT = 4,
    UMI_AI_TRAINING_SOURCE_VIDEO_TRANSCRIPT = 5,
    UMI_AI_TRAINING_SOURCE_IMAGE_CAPTION = 6,
    UMI_AI_TRAINING_SOURCE_SOURCE_CODE = 7
} UmiAiTrainingSourceMedium;

typedef enum UmiAiTrainingSourceVisibility {
    UMI_AI_TRAINING_SOURCE_VISIBILITY_PUBLIC = 1,
    UMI_AI_TRAINING_SOURCE_VISIBILITY_RESTRICTED = 2,
    UMI_AI_TRAINING_SOURCE_VISIBILITY_PRIVATE = 3
} UmiAiTrainingSourceVisibility;

/* Path identifies a reviewed UTF-8 text derivative in the source manifest
 * directory. The original asset digest groups ALL derivatives of the same
 * document, recording or image into the same train/evaluation partition.
 * Prepared text and original asset digests are both caller-provided SHA-256
 * representations; the caller must VERIFY them against actual bytes through
 * the existing Framework cryptographic service before admitting the source.
 * The admission gate cannot establish licence ownership, redact personal
 * information, transcribe media or inspect the underlying file by itself. */
typedef struct UmiAiTrainingSourceReview {
    const char *relativeTextPath;
    const char *originalAssetSha256;
    const char *preparedTextSha256;
    const char *licenceId;
    const char *rightsEvidenceId;
    UmiAiTrainingSourceMedium medium;
    UmiAiTrainingSourceVisibility visibility;
    int rightsReviewed;
    int trainingUseApproved;
    int personalDataReviewed;
    int extractedTextReviewed;
} UmiAiTrainingSourceReview;

typedef struct UmiAiTrainingAdmittedSource {
    char relativeTextPath[UMI_AI_DATASET_MAX_PATH];
    char originalAssetSha256[UMI_AI_SOURCE_DIGEST_TEXT_CAPACITY];
    char preparedTextSha256[UMI_AI_SOURCE_DIGEST_TEXT_CAPACITY];
    UmiAiTrainingSourceMedium medium;
    UmiAiTrainingPartition partition;
} UmiAiTrainingAdmittedSource;

/* Fail closed: only explicitly approved, public and reviewed sources may
 * enter the manifest. For non-text inputs, the extracted transcript/caption
 * must also have been reviewed. evaluationPercentage is 1..50. The caller
 * remains responsible for checking near-duplicates and distribution balance.
 * No private text, internal provenance notes or media bytes are returned.
 * Failure preserves the entire output record. */
UmiStatus UmiAiTrainingSourceAdmit(
    const UmiAiTrainingSourceReview *review,
    unsigned evaluationPercentage,
    UmiAiTrainingAdmittedSource *outSource);

/* Format one line for the existing UMICOM-AI-DATASET-1 manifest parser, using
 * the original-source partition and the exact vetted relative pathname.
 * This does not write a file, execute a converter or access the network.
 * Invalid/capacity requests leave caller buffer unchanged. */
UmiStatus UmiAiTrainingSourceManifestLine(
    const UmiAiTrainingAdmittedSource *source,
    char *outLine, size_t capacity);

#ifdef __cplusplus
}
#endif
#endif /* UMICOM_AI_TRAINING_SOURCE_ADMISSION_H */

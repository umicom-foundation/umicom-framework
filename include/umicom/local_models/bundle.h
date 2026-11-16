/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/local_models/bundle.h
 *
 * PURPOSE:
 *   Describe pinned multi-file model selections without claiming download, compatibility or inference readiness.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#ifndef UMICOM_LOCAL_MODELS_BUNDLE_H
#define UMICOM_LOCAL_MODELS_BUNDLE_H
#include "umicom/local_models/manager.h"
#ifdef __cplusplus
extern "C" {
#endif

#define UMI_MODEL_BUNDLE_MAX_FILES 128U
typedef enum UmiModelArtifactFormat {
    UMI_MODEL_ARTIFACT_GGUF = 1,
    UMI_MODEL_ARTIFACT_SAFETENSORS,
    UMI_MODEL_ARTIFACT_JSON,
    UMI_MODEL_ARTIFACT_TEXT,
    UMI_MODEL_ARTIFACT_TOKENIZER,
    UMI_MODEL_ARTIFACT_DOCUMENTATION
} UmiModelArtifactFormat;

typedef struct UmiModelBundleFile {
    char filename[512];
    char sha256[65];
    uint64_t bytes;
    UmiModelArtifactFormat format;
} UmiModelBundleFile;

typedef struct UmiModelBundlePlan {
    char repository[128];
    char revision[41];
    UmiModelBundleFile files[UMI_MODEL_BUNDLE_MAX_FILES];
    size_t count;
    uint64_t total_bytes;
} UmiModelBundlePlan;

/* Plan 1..128 selected files from one owner/repository and full lowercase
 * 40-hex commit. Every file needs an exact lowercase SHA-256 and positive size;
 * GGUF needs at least 24 bytes, Safetensors at least 10. Total selected bytes
 * cannot exceed UMI_MODEL_MAX_BYTES. Empty drafting plans are not admitted.
 *
 * Reuse the model service's restricted ASCII relative-name grammar and current
 * host output-path syntax. Refuse case-insensitive duplicate destinations and
 * file/directory prefix collisions on every host. This does not resolve a real
 * destination, inspect filesystem links, reserve disk space or validate content.
 * Revalidate on the destination host before materialization.
 *
 * Formats admit lowercase .gguf, .safetensors, .json, .txt and tokenizer
 * .model/.tiktoken/.vocab/.merges names; documentation admits README.md,
 * LICENSE, LICENCE, LICENSE.md and LICENCE.md (case-insensitive basenames).
 * Filename classification does not establish that the bytes are safe or valid.
 * Executable modules and pickle checkpoint extensions have no supported format.
 *
 * The constructor allocates staging storage and preserves out_plan on every
 * failure. Inputs may borrow its existing fields. The selected set does not
 * establish shard completeness, architecture support, tensor validity, model
 * licence acceptance or execution readiness. No files or network are accessed. */
UmiStatus UmiModelBundlePlanCreate(const char *repository, const char *revision,
    const UmiModelBundleFile *files, size_t count, UmiModelBundlePlan *out_plan);
/* Validate every live record and recompute its exact total. Publicly edited
 * totals that disagree return INVALID_STATE. Unused slots are ignored.
 * Serialize caller access; all input objects must remain complete/readable. */
UmiStatus UmiModelBundlePlanValidate(const UmiModelBundlePlan *plan);
/* Copy a selected GGUF record into the established single-file source contract.
 * Other admitted data formats return NOT_IMPLEMENTED: the existing downloader
 * does not gain a generic data-file execution path through this planning API.
 * Validate the full bundle before publishing; out_source stays unchanged on
 * failure and must not overlap plan. No download or model execution occurs. */
UmiStatus UmiModelBundleGgufSourceAt(const UmiModelBundlePlan *plan, size_t index,
    UmiModelSource *out_source);
/* Produce a pinned public HTTPS URL from validated identity, never an arbitrary
 * supplied URL. Buffer failure preserves all output bytes. Storage must be
 * separate from plan; no request, redirect or credential lookup is performed. */
UmiStatus UmiModelBundleUrlAt(const UmiModelBundlePlan *plan, size_t index,
    char *out_url, size_t capacity);

/* Select one GGUF file, expanding only explicit prefix-NNNNN-of-NNNNN.gguf
 * names into every numbered companion from the same pinned catalogue. The
 * basename prefix must be nonempty, part/total must be decimal and nonzero,
 * part cannot exceed total, and total is bounded by UMI_MODEL_BUNDLE_MAX_FILES.
 * Exact companion spelling is required; a missing part returns NOT_FOUND.
 * Other filename patterns remain single-file selections, with completeness
 * unassessed. Non-GGUF formats return NOT_IMPLEMENTED.
 *
 * Validate the entire catalogue before selection and preserve out_plan on
 * every failure. out_plan may be catalogue itself; otherwise the complete
 * objects must not overlap. Allocated selection storage is private and freed
 * before return. Serialize caller access. Companion naming establishes no
 * tensor validity, architecture support or runtime readiness. No I/O occurs. */
UmiStatus UmiModelBundleSelectGguf(const UmiModelBundlePlan *catalogue,
    size_t selected_index, UmiModelBundlePlan *out_plan);

#ifdef __cplusplus
}
#endif
#endif

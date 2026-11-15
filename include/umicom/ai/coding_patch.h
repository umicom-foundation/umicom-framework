/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/coding_patch.h
 *
 * PURPOSE:
 *   Represent reviewable coding patches and apply or revert them through a
 *   conflict-aware, workspace-owned file adapter.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * The Framework never grants an AI direct filesystem access.  It first records
 * exact before/after text and hashes, then a product supplies narrow read,
 * write and remove callbacks after explicit approval.
 */
#ifndef UMICOM_AI_CODING_PATCH_H
#define UMICOM_AI_CODING_PATCH_H

#include "umicom/ai/coding_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the ai coding patch policy data shared with callers of this public contract.
 */
typedef struct UmiAiCodingPatchPolicy {
    size_t maximum_files;
    uint32_t maximum_changed_lines;
    int allow_create;
    int allow_delete;
    int require_approval;
} UmiAiCodingPatchPolicy;

/**
 * Represent the ai coding patch file data shared with callers of this public contract.
 */
typedef struct UmiAiCodingPatchFile {
    char path[UMI_AI_TEXT_CAPACITY];
    UmiAiCodingPatchOperation operation;
    char before_text[UMI_AI_CODING_PATCH_CONTENT_CAPACITY];
    char after_text[UMI_AI_CODING_PATCH_CONTENT_CAPACITY];
    size_t before_length;
    size_t after_length;
    uint64_t before_hash;
    uint64_t after_hash;
    uint32_t removed_lines;
    uint32_t added_lines;
} UmiAiCodingPatchFile;

/**
 * Represent the ai coding patch data shared with callers of this public contract.
 */
typedef struct UmiAiCodingPatch {
    uint32_t structure_size;
    uint32_t abi_version;
    char patch_id[UMI_AI_ID_CAPACITY];
    char request_id[UMI_AI_ID_CAPACITY];
    char title[UMI_AI_SMALL_TEXT_CAPACITY];
    char rationale[UMI_AI_TEXT_CAPACITY];
    char approved_by[UMI_AI_ID_CAPACITY];
    UmiAiCodingPatchState state;
    UmiAiCodingPatchFile files[UMI_AI_CODING_PATCH_FILE_MAX];
    size_t file_count;
    uint32_t changed_lines;
    uint64_t revision;
} UmiAiCodingPatch;

/* All calls require serialized ownership of the live patch and adapter.
 * Callbacks must not mutate/reenter that patch or invalidate the adapter.
 * A successful read initializes exactly out_length bytes, with out_length
 * strictly below capacity. Hashing consumes those bytes without requiring a
 * terminator. The host enforces workspace containment and stable file access.
 * Failure compensation is attempted, but callback errors do not certify that
 * restoration succeeded; inspect the affected files before retrying. */
typedef UmiStatus (*UmiAiCodingFileRead)(
    void *user_data,
    const char *relative_path,
    char *out_text,
    size_t capacity,
    size_t *out_length);
typedef UmiStatus (*UmiAiCodingFileWrite)(
    void *user_data,
    const char *relative_path,
    const char *text,
    size_t length);
typedef UmiStatus (*UmiAiCodingFileRemove)(
    void *user_data,
    const char *relative_path);

/**
 * Represent the ai coding file adapter data shared with callers of this public contract.
 */
typedef struct UmiAiCodingFileAdapter {
    uint32_t structure_size;
    uint32_t abi_version;
    UmiAiCodingFileRead read;
    UmiAiCodingFileWrite write;
    UmiAiCodingFileRemove remove;
    void *user_data;
} UmiAiCodingFileAdapter;

/** Check ABI size/version and required callback slots without invoking them.
 * Host storage and user_data lifetime remain the caller responsibility. */
UmiStatus UmiAiCodingFileAdapterValidate(const UmiAiCodingFileAdapter *adapter);

/**
 * Provide the ai coding patch policy default operation used by this module and its client
 * applications.
 */
UmiAiCodingPatchPolicy umi_ai_coding_patch_policy_default(void);
/** Policy file limits must be 1..UMI_AI_CODING_PATCH_FILE_MAX and the line
 * budget must be positive. Boolean fields retain nonzero-is-true semantics. */
UmiStatus UmiAiCodingPatchPolicyValidate(const UmiAiCodingPatchPolicy *policy);
/** Admit fixed text, physical counts, known domains, stored lengths, hashes,
 * line counts and a nonzero revision. An initialized empty patch is valid here;
 * policy validation/application still requires at least one file. This is
 * metadata integrity, not proof of approval authenticity or filesystem safety. */
UmiStatus UmiAiCodingPatchCheckIntegrity(const UmiAiCodingPatch *patch);
/**
 * Initialise ai coding patch from caller-provided values so later operations receive a
 * known state.
 */
/** Construct a complete value before replacing the destination. Any failure,
 * including allocation failure, leaves its previous bytes unchanged. Text inputs
 * may refer to its existing inline fields. This call allocates temporary storage;
 * it does not acquire external resources or synchronize concurrent callers. */
UmiStatus umi_ai_coding_patch_init(UmiAiCodingPatch *patch,
                                   const char *patch_id,
                                   const char *request_id,
                                   const char *title,
                                   const char *rationale);
/**
 * Provide the ai coding patch add file operation used by this module and its client
 * applications.
 */
/** Addition and approval preserve every patch byte on refusal. Text inputs
 * may borrow existing fields. Addition allocates a temporary complete value;
 * duplicate paths are refused before publication. Mutation at UINT64_MAX is
 * refused; reading and validating that final revision remain possible. */
UmiStatus umi_ai_coding_patch_add_file(UmiAiCodingPatch *patch,
                                       const char *path,
                                       UmiAiCodingPatchOperation operation,
                                       const char *before_text,
                                       const char *after_text);
/**
 * Check that ai coding patch satisfies its contract before another service relies on it.
 */
UmiStatus umi_ai_coding_patch_validate(
    const UmiAiCodingPatch *patch,
    const UmiAiCodingPatchPolicy *policy);
/**
 * Provide the ai coding patch approve operation used by this module and its client
 * applications.
 */
UmiStatus umi_ai_coding_patch_approve(UmiAiCodingPatch *patch,
                                      const char *approved_by);
/**
 * Provide the ai coding patch reject operation used by this module and its client
 * applications.
 */
UmiStatus umi_ai_coding_patch_reject(UmiAiCodingPatch *patch);
/**
 * Perform ai coding patch through the module contract so client applications do not
 * duplicate its policy.
 */
UmiStatus umi_ai_coding_patch_apply(UmiAiCodingPatch *patch,
                                    const UmiAiCodingPatchPolicy *policy,
                                    const UmiAiCodingFileAdapter *adapter);
/**
 * Provide the ai coding patch revert operation used by this module and its client
 * applications.
 */
UmiStatus umi_ai_coding_patch_revert(UmiAiCodingPatch *patch,
                                     const UmiAiCodingFileAdapter *adapter);

/* Read-only synchronous reentry preflight. BUSY means an outer file callback
 * still borrows this patch. NULL is INVALID_ARGUMENT. No lock is acquired. */
UmiStatus UmiAiCodingPatchCheckIdle(const UmiAiCodingPatch *patch);

typedef struct UmiAiCodingPatchTransactionReport {
    int admitted;
    UmiStatus primary_status;
    UmiStatus compensation_status;
    size_t failed_file_index;
    size_t verified_files;
    size_t mutation_attempts;
    size_t mutations_completed;
    size_t compensation_attempts;
    size_t compensation_failures;
    size_t restoration_checks;
    size_t restoration_failures;
    uint64_t revision_before;
    uint64_t revision_after;
} UmiAiCodingPatchTransactionReport;

/* Admission failure leaves outReport and the patch unchanged. After admission,
 * every return publishes a complete report with admitted=1, including verification
 * failure. Initialize a fresh report to zero before each attempt to distinguish
 * admission refusal from a completed report; do not reuse a previous admitted flag.
 * failed_file_index is SIZE_MAX if no file failed. The return value remains the
 * primary status; compensation_status is the first failed compensation callback
 * or subsequent restoration check. Counts describe attempted and completed work.
 * A zero compensation_status alone does not imply compensation was needed.
 *
 * Restoration checks read through the host adapter after all compensation
 * callbacks. They do not prove durability or prevent later external changes.
 * A failing operation retains the last committed patch state; that state does
 * not claim that the workspace was restored. Revision advances once whenever
 * mutation callbacks were attempted, even when the operation failed.
 *
 * All mutation APIs refuse reentry on this patch from its synchronous callbacks.
 * Read-only validation remains available. This is not cross-thread locking.
 * Callbacks must return normally and must not directly mutate/free the patch,
 * output report or borrowed user data. The report must not overlap any of them.
 * Captured callback slots remain fixed; user_data lifetime belongs to the host.
 * Legacy apply/revert delegate here and return the same primary status. */
UmiStatus UmiAiCodingPatchApplyWithReport(UmiAiCodingPatch *patch,
    const UmiAiCodingPatchPolicy *policy, const UmiAiCodingFileAdapter *adapter,
    UmiAiCodingPatchTransactionReport *outReport);
UmiStatus UmiAiCodingPatchRevertWithReport(UmiAiCodingPatch *patch,
    const UmiAiCodingFileAdapter *adapter, UmiAiCodingPatchTransactionReport *outReport);

#ifdef __cplusplus
}
#endif

#endif

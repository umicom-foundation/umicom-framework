/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/toolchain/cvm_document_check.h
 *
 * PURPOSE:
 *   Bind bounded source-check results and Problems rows to a live document revision.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#ifndef UMICOM_TOOLCHAIN_CVM_DOCUMENT_CHECK_H
#define UMICOM_TOOLCHAIN_CVM_DOCUMENT_CHECK_H
#include "umicom/toolchain/cvm_source_check.h"
#include "umicom/platform/document_store.h"
#include "umicom/diagnostics/diagnostic.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiCvmDocumentCheckResult {
    const UmiDocumentStore *owner; /* Borrowed identity; never dereferenced after destruction. */
    UmiDocumentSnapshot document;
    UmiCvmSourceCheckResult source;
    UmiStatus check_status;
    bool captured;
} UmiCvmDocumentCheckResult;

/* Check one atomic, bounded snapshot without saving or executing it. The copy
 * budget is the smaller of max_source_bytes and UMC_SOURCE_LIMIT. Zero admits
 * only an empty snapshot (which the compiler then diagnoses).
 * A valid-sized result is cleared before admission; stale successes cannot
 * survive a refused new request. Incorrect result size leaves storage untouched.
 * Prefer a heap owner for this result, which contains the bounded warning set.
 * Store and result lifetimes are caller-owned; keep them disjoint. */
UmiStatus umi_cvm_check_document(const UmiDocumentStore *store, UmiDocumentId document,
    size_t max_source_bytes, UmiCvmDocumentCheckResult *result, size_t result_size);

/* Freshness is meaningful for a captured syntax error as well as valid code.
 * Use the same still-live store. This check does not lock future UI rendering:
 * serialize editing/navigation on the host UI thread and recheck before acting.
 * Destroying/recreating a store invalidates all results, even at the same address. */
bool umi_cvm_document_check_matches(const UmiDocumentStore *store,
    const UmiCvmDocumentCheckResult *result, size_t result_size);

/* Project one warning or the compiler error into the shared Problems record.
 * These functions never mutate a diagnostic store. Output is unchanged on
 * stale input, invalid arguments or a missing row. URI is the captured path
 * (empty for untitled text); document ID and revision remain authoritative.
 * Locations are one-based byte columns, not display/UTF-16 character columns. */
UmiStatus umi_cvm_document_check_warning(const UmiDocumentStore *store,
    const UmiCvmDocumentCheckResult *result, size_t result_size, size_t index,
    UmiDiagnosticSnapshot *out);
UmiStatus umi_cvm_document_check_error(const UmiDocumentStore *store,
    const UmiCvmDocumentCheckResult *result, size_t result_size,
    UmiDiagnosticSnapshot *out);
#ifdef __cplusplus
}
#endif
#endif

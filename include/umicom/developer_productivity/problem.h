/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_productivity/problem.h
 *
 * PURPOSE:
 *   Define one normalized compiler/build/test/runtime problem record for the
 *   Problems panel, navigation, AI repair workflows and machine reports.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_PRODUCTIVITY_PROBLEM_H
#define UMICOM_DEVELOPER_PRODUCTIVITY_PROBLEM_H

#include "umicom/developer_productivity/types.h"
#include "umicom/base/value_archive.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the developer problem data shared with callers of this public contract.
 */
typedef struct UmiDeveloperProblem {
    uint64_t problem_id;
    char source[UMI_DEVELOPER_PRODUCTIVITY_ID_CAPACITY];
    char code[UMI_DEVELOPER_PRODUCTIVITY_ID_CAPACITY];
    char message[UMI_DEVELOPER_PRODUCTIVITY_TEXT_CAPACITY];
    UmiDeveloperProductivitySeverity severity;
    UmiDeveloperProductivityLocation location;
    int suppressible;
    int transient;
    uint64_t revision;
} UmiDeveloperProblem;

/**
 * Check that developer problem satisfies its contract before another service relies on it.
 */
UmiStatus umi_developer_problem_validate(
    const UmiDeveloperProblem *problem);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_developer_problem_archive_encode(const UmiDeveloperProblem *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_developer_problem_archive_decode(const void *bytes, size_t byte_count,
    UmiDeveloperProblem *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_developer_problem_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiDeveloperProblem *expected, UmiDeveloperProblem *value);


#ifdef __cplusplus
}
#endif

#endif

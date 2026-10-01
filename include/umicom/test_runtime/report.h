/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/report.h
 *
 * PURPOSE:
 *   Build deterministic human-readable and machine-readable regression summaries.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_REPORT
#define UMICOM_TEST_RUNTIME_REPORT

#include "umicom/test_runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the test runtime report data shared with callers of this public contract.
 */
typedef struct UmiTestRuntimeReport {
    uint32_t structure_size;
    char id[UMI_TEST_RUNTIME_ID_CAPACITY];
    char name[UMI_TEST_RUNTIME_ID_CAPACITY];
    char detail[UMI_TEST_RUNTIME_TEXT_CAPACITY];
    uint64_t passed;
    uint64_t failed;
    uint64_t updated_at_ms;
    uint64_t revision;
    bool enabled;
} UmiTestRuntimeReport;

/**
 * Initialise test runtime report from caller-provided values so later operations receive a
 * known state.
 */
void umi_test_runtime_report_init(UmiTestRuntimeReport *value, const char *id);
/**
 * Check that test runtime report satisfies its contract before another service relies on
 * it.
 */
UmiStatus umi_test_runtime_report_validate(const UmiTestRuntimeReport *value);
/**
 * Provide the test runtime report set name operation used by this module and its client
 * applications.
 */
UmiStatus umi_test_runtime_report_set_name(UmiTestRuntimeReport *value, const char *name);
/**
 * Provide the test runtime report set detail operation used by this module and its client
 * applications.
 */
UmiStatus umi_test_runtime_report_set_detail(UmiTestRuntimeReport *value, const char *detail);
/**
 * Provide the test runtime report set passed operation used by this module and its client
 * applications.
 */
UmiStatus umi_test_runtime_report_set_passed(UmiTestRuntimeReport *value, uint64_t number);
/**
 * Provide the test runtime report set failed operation used by this module and its client
 * applications.
 */
UmiStatus umi_test_runtime_report_set_failed(UmiTestRuntimeReport *value, uint64_t number);
/**
 * Provide the test runtime report touch operation used by this module and its client
 * applications.
 */
UmiStatus umi_test_runtime_report_touch(UmiTestRuntimeReport *value, uint64_t updated_at_ms);
/**
 * Provide the test runtime report same identity operation used by this module and its
 * client applications.
 */
bool umi_test_runtime_report_same_identity(const UmiTestRuntimeReport *left, const UmiTestRuntimeReport *right);


/** Publish a complete reviewed value while expected_revision still matches.
 * Copy the live record, edit that copy with the ordinary setters, and retain
 * the revision observed before editing. Both values must validate and have
 * the same ID. A stale review returns INVALID_STATE; exhausted revision space
 * returns CAPACITY_EXCEEDED. All failures leave the live value unchanged.
 * Success copies every proposal field and assigns one new live revision;
 * the proposal's own revision does not control publication. Self-assignment
 * is allowed and also advances once. Other overlapping storage is unsupported.
 * Serialize access on the owner. This value operation allocates nothing and
 * does not perform I/O, authenticate evidence or run the described workflow. */
UmiStatus umi_test_runtime_report_replace_if_current(UmiTestRuntimeReport *value,
    uint64_t expected_revision, const UmiTestRuntimeReport *proposal);

#ifdef __cplusplus
}
#endif
#endif

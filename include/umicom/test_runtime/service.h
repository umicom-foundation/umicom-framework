/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/service.h
 *
 * PURPOSE:
 *   Own test-runtime catalogues, profiles, execution evidence and health state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_SERVICE
#define UMICOM_TEST_RUNTIME_SERVICE

#include "umicom/test_runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the test runtime service data shared with callers of this public contract.
 */
typedef struct UmiTestRuntimeService {
    uint32_t structure_size;
    char id[UMI_TEST_RUNTIME_ID_CAPACITY];
    char name[UMI_TEST_RUNTIME_ID_CAPACITY];
    char detail[UMI_TEST_RUNTIME_TEXT_CAPACITY];
    uint64_t state;
    uint64_t generation;
    uint64_t revision;
    uint64_t updated_at_ms;
    bool enabled;
} UmiTestRuntimeService;

/**
 * Initialise test runtime service from caller-provided values so later operations receive
 * a known state.
 */
void umi_test_runtime_service_init(UmiTestRuntimeService *value, const char *id);
/**
 * Check that test runtime service satisfies its contract before another service relies on
 * it.
 */
UmiStatus umi_test_runtime_service_validate(const UmiTestRuntimeService *value);
/**
 * Provide the test runtime service set name operation used by this module and its client
 * applications.
 */
UmiStatus umi_test_runtime_service_set_name(UmiTestRuntimeService *value, const char *name);
/**
 * Provide the test runtime service set detail operation used by this module and its client
 * applications.
 */
UmiStatus umi_test_runtime_service_set_detail(UmiTestRuntimeService *value, const char *detail);
/**
 * Provide the test runtime service set state operation used by this module and its client
 * applications.
 */
UmiStatus umi_test_runtime_service_set_state(UmiTestRuntimeService *value, uint64_t number);
/**
 * Provide the test runtime service set generation operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_runtime_service_set_generation(UmiTestRuntimeService *value, uint64_t number);
/**
 * Provide the test runtime service touch operation used by this module and its client
 * applications.
 */
UmiStatus umi_test_runtime_service_touch(UmiTestRuntimeService *value, uint64_t updated_at_ms);
/**
 * Provide the test runtime service same identity operation used by this module and its
 * client applications.
 */
bool umi_test_runtime_service_same_identity(const UmiTestRuntimeService *left, const UmiTestRuntimeService *right);


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
UmiStatus umi_test_runtime_service_replace_if_current(UmiTestRuntimeService *value,
    uint64_t expected_revision, const UmiTestRuntimeService *proposal);

#ifdef __cplusplus
}
#endif
#endif

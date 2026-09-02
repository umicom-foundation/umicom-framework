/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/catalogue.h
 *
 * PURPOSE:
 *   Maintain deterministic registered-test inventory and revision evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_CATALOGUE
#define UMICOM_TEST_RUNTIME_CATALOGUE

#include "umicom/test_runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the test runtime catalogue data shared with callers of this public contract.
 */
typedef struct UmiTestRuntimeCatalogue {
    uint32_t structure_size;
    char id[UMI_TEST_RUNTIME_ID_CAPACITY];
    char name[UMI_TEST_RUNTIME_ID_CAPACITY];
    char detail[UMI_TEST_RUNTIME_TEXT_CAPACITY];
    uint64_t test_count;
    uint64_t generation;
    uint64_t revision;
    uint64_t updated_at_ms;
    bool enabled;
} UmiTestRuntimeCatalogue;

/**
 * Initialise test runtime catalogue from caller-provided values so later operations
 * receive a known state.
 */
void umi_test_runtime_catalogue_init(UmiTestRuntimeCatalogue *value, const char *id);
/**
 * Check that test runtime catalogue satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_test_runtime_catalogue_validate(const UmiTestRuntimeCatalogue *value);
/**
 * Provide the test runtime catalogue set name operation used by this module and its client
 * applications.
 */
UmiStatus umi_test_runtime_catalogue_set_name(UmiTestRuntimeCatalogue *value, const char *name);
/**
 * Provide the test runtime catalogue set detail operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_runtime_catalogue_set_detail(UmiTestRuntimeCatalogue *value, const char *detail);
/**
 * Return the number of records represented by test runtime catalogue set test without
 * changing their state.
 */
UmiStatus umi_test_runtime_catalogue_set_test_count(UmiTestRuntimeCatalogue *value, uint64_t number);
/**
 * Provide the test runtime catalogue set generation operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_runtime_catalogue_set_generation(UmiTestRuntimeCatalogue *value, uint64_t number);
/**
 * Provide the test runtime catalogue touch operation used by this module and its client
 * applications.
 */
UmiStatus umi_test_runtime_catalogue_touch(UmiTestRuntimeCatalogue *value, uint64_t updated_at_ms);
/**
 * Provide the test runtime catalogue same identity operation used by this module and its
 * client applications.
 */
bool umi_test_runtime_catalogue_same_identity(const UmiTestRuntimeCatalogue *left, const UmiTestRuntimeCatalogue *right);

#ifdef __cplusplus
}
#endif
#endif

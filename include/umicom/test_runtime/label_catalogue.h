/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/label_catalogue.h
 *
 * PURPOSE:
 *   Maintain stable test labels without depending on filename patterns.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_LABEL_CATALOGUE
#define UMICOM_TEST_RUNTIME_LABEL_CATALOGUE
#include "umicom/test_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the test runtime label catalogue data shared with callers of this public
 * contract.
 */
typedef struct UmiTestRuntimeLabelCatalogue {
    uint32_t structure_size;
    char id[UMI_TEST_RUNTIME_ID_CAPACITY];
    char category[UMI_TEST_RUNTIME_ID_CAPACITY];
    char detail[UMI_TEST_RUNTIME_TEXT_CAPACITY];
    uint64_t label_count;
    uint64_t generation;
    uint64_t revision;
    bool active;
} UmiTestRuntimeLabelCatalogue;
/**
 * Initialise test runtime label catalogue from caller-provided values so later operations
 * receive a known state.
 */
void umi_test_runtime_label_catalogue_init(UmiTestRuntimeLabelCatalogue *value,const char *id);
/**
 * Check that test runtime label catalogue satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_test_runtime_label_catalogue_validate(const UmiTestRuntimeLabelCatalogue *value);
/**
 * Provide the test runtime label catalogue set category operation used by this module and
 * its client applications.
 */
UmiStatus umi_test_runtime_label_catalogue_set_category(UmiTestRuntimeLabelCatalogue *value,const char *category);
/**
 * Provide the test runtime label catalogue set detail operation used by this module and
 * its client applications.
 */
UmiStatus umi_test_runtime_label_catalogue_set_detail(UmiTestRuntimeLabelCatalogue *value,const char *detail);
/**
 * Return the number of records represented by test runtime label catalogue set label
 * without changing their state.
 */
UmiStatus umi_test_runtime_label_catalogue_set_label_count(UmiTestRuntimeLabelCatalogue *value,uint64_t number);
/**
 * Provide the test runtime label catalogue set generation operation used by this module
 * and its client applications.
 */
UmiStatus umi_test_runtime_label_catalogue_set_generation(UmiTestRuntimeLabelCatalogue *value,uint64_t number);
/**
 * Provide the test runtime label catalogue set active operation used by this module and
 * its client applications.
 */
UmiStatus umi_test_runtime_label_catalogue_set_active(UmiTestRuntimeLabelCatalogue *value,bool active);
/**
 * Provide the test runtime label catalogue same identity operation used by this module and
 * its client applications.
 */
bool umi_test_runtime_label_catalogue_same_identity(const UmiTestRuntimeLabelCatalogue *left,const UmiTestRuntimeLabelCatalogue *right);
#ifdef __cplusplus
}
#endif
#endif

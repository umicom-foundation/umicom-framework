/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/run_identity.h
 *
 * PURPOSE:
 *   Provide stable identity for one complete regression execution.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_RUN_IDENTITY
#define UMICOM_TEST_RUNTIME_RUN_IDENTITY
#include "umicom/test_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the test runtime run identity data shared with callers of this public
 * contract.
 */
typedef struct UmiTestRuntimeRunIdentity
{
    uint32_t structure_size;
    char id[UMI_TEST_RUNTIME_ID_CAPACITY];
    char detail[UMI_TEST_RUNTIME_TEXT_CAPACITY];
    uint64_t sequence;
    uint64_t generation;
    uint64_t revision;bool enabled;} UmiTestRuntimeRunIdentity;
/**
 * Initialise test runtime run identity from caller-provided values so later operations
 * receive a known state.
 */
void umi_test_runtime_run_identity_init(UmiTestRuntimeRunIdentity *value,const char *id);
/**
 * Check that test runtime run identity satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_test_runtime_run_identity_validate(const UmiTestRuntimeRunIdentity *value);
/**
 * Provide the test runtime run identity set detail operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_runtime_run_identity_set_detail(UmiTestRuntimeRunIdentity *value,const char *detail);
/**
 * Provide the test runtime run identity set sequence operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_runtime_run_identity_set_sequence(UmiTestRuntimeRunIdentity *value,uint64_t number);
/**
 * Provide the test runtime run identity set generation operation used by this module and
 * its client applications.
 */
UmiStatus umi_test_runtime_run_identity_set_generation(UmiTestRuntimeRunIdentity *value,uint64_t number);
/**
 * Provide the test runtime run identity same identity operation used by this module and
 * its client applications.
 */
bool umi_test_runtime_run_identity_same_identity(const UmiTestRuntimeRunIdentity *left,const UmiTestRuntimeRunIdentity *right);
#ifdef __cplusplus
}
#endif
#endif

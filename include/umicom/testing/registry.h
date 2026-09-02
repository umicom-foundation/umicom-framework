/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/testing/registry.h
 *
 * PURPOSE:
 *   Register and resolve test suites from Framework modules and products.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TESTING_REGISTRY_H
#define UMICOM_TESTING_REGISTRY_H

#include <stddef.h>

#include "umicom/base/status.h"
#include "umicom/testing/suite.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the test registry data shared with callers of this public contract.
 */
typedef struct UmiTestRegistry UmiTestRegistry;

/**
 * Initialise test registry from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_test_registry_create(UmiTestRegistry **out_registry);
/**
 * Release or reset state held by test registry so the same storage can be reused safely.
 */
void umi_test_registry_destroy(UmiTestRegistry *registry);
/**
 * Add test registry only after its inputs and available capacity have been checked.
 */
UmiStatus umi_test_registry_add(UmiTestRegistry *registry,
                                UmiTestSuite *suite);
/**
 * Remove test registry while keeping the remaining records in a valid and discoverable
 * state.
 */
UmiStatus umi_test_registry_remove(UmiTestRegistry *registry,
                                   const char *suite_id);
/**
 * Return the number of records represented by test registry without changing their state.
 */
size_t umi_test_registry_count(const UmiTestRegistry *registry);
/**
 * Find test registry while leaving the underlying catalogue or model owned by this module.
 */
UmiTestSuite *umi_test_registry_at(UmiTestRegistry *registry,
                                   size_t index);
/**
 * Find test registry while leaving the underlying catalogue or model owned by this module.
 */
UmiTestSuite *umi_test_registry_find(UmiTestRegistry *registry,
                                     const char *suite_id);

#ifdef __cplusplus
}
#endif

#endif

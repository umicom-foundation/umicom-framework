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

typedef struct UmiTestRegistry UmiTestRegistry;

UmiStatus umi_test_registry_create(UmiTestRegistry **out_registry);
void umi_test_registry_destroy(UmiTestRegistry *registry);
UmiStatus umi_test_registry_add(UmiTestRegistry *registry,
                                UmiTestSuite *suite);
UmiStatus umi_test_registry_remove(UmiTestRegistry *registry,
                                   const char *suite_id);
size_t umi_test_registry_count(const UmiTestRegistry *registry);
UmiTestSuite *umi_test_registry_at(UmiTestRegistry *registry,
                                   size_t index);
UmiTestSuite *umi_test_registry_find(UmiTestRegistry *registry,
                                     const char *suite_id);

#ifdef __cplusplus
}
#endif

#endif

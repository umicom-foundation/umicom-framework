/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/profile_catalogue.h
 *
 * PURPOSE:
 *   Maintain the standard Framework execution-profile catalogue.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_PROFILE_CATALOGUE
#define UMICOM_TEST_RUNTIME_PROFILE_CATALOGUE

#include "umicom/test_runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiTestRuntimeProfileCatalogue {
    uint32_t structure_size;
    char id[UMI_TEST_RUNTIME_ID_CAPACITY];
    char name[UMI_TEST_RUNTIME_ID_CAPACITY];
    char detail[UMI_TEST_RUNTIME_TEXT_CAPACITY];
    uint64_t profile_count;
    uint64_t generation;
    uint64_t revision;
    uint64_t updated_at_ms;
    bool enabled;
} UmiTestRuntimeProfileCatalogue;

void umi_test_runtime_profile_catalogue_init(UmiTestRuntimeProfileCatalogue *value, const char *id);
UmiStatus umi_test_runtime_profile_catalogue_validate(const UmiTestRuntimeProfileCatalogue *value);
UmiStatus umi_test_runtime_profile_catalogue_set_name(UmiTestRuntimeProfileCatalogue *value, const char *name);
UmiStatus umi_test_runtime_profile_catalogue_set_detail(UmiTestRuntimeProfileCatalogue *value, const char *detail);
UmiStatus umi_test_runtime_profile_catalogue_set_profile_count(UmiTestRuntimeProfileCatalogue *value, uint64_t number);
UmiStatus umi_test_runtime_profile_catalogue_set_generation(UmiTestRuntimeProfileCatalogue *value, uint64_t number);
UmiStatus umi_test_runtime_profile_catalogue_touch(UmiTestRuntimeProfileCatalogue *value, uint64_t updated_at_ms);
bool umi_test_runtime_profile_catalogue_same_identity(const UmiTestRuntimeProfileCatalogue *left, const UmiTestRuntimeProfileCatalogue *right);

#ifdef __cplusplus
}
#endif
#endif

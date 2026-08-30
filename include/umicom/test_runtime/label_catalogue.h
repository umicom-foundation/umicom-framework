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
void umi_test_runtime_label_catalogue_init(UmiTestRuntimeLabelCatalogue *value,const char *id);
UmiStatus umi_test_runtime_label_catalogue_validate(const UmiTestRuntimeLabelCatalogue *value);
UmiStatus umi_test_runtime_label_catalogue_set_category(UmiTestRuntimeLabelCatalogue *value,const char *category);
UmiStatus umi_test_runtime_label_catalogue_set_detail(UmiTestRuntimeLabelCatalogue *value,const char *detail);
UmiStatus umi_test_runtime_label_catalogue_set_label_count(UmiTestRuntimeLabelCatalogue *value,uint64_t number);
UmiStatus umi_test_runtime_label_catalogue_set_generation(UmiTestRuntimeLabelCatalogue *value,uint64_t number);
UmiStatus umi_test_runtime_label_catalogue_set_active(UmiTestRuntimeLabelCatalogue *value,bool active);
bool umi_test_runtime_label_catalogue_same_identity(const UmiTestRuntimeLabelCatalogue *left,const UmiTestRuntimeLabelCatalogue *right);
#ifdef __cplusplus
}
#endif
#endif

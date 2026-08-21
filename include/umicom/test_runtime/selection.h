/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/selection.h
 *
 * PURPOSE:
 *   Represent an immutable selected test set and selection reason.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_SELECTION
#define UMICOM_TEST_RUNTIME_SELECTION

#include "umicom/test_runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiTestRuntimeSelection {
    uint32_t structure_size;
    char id[UMI_TEST_RUNTIME_ID_CAPACITY];
    char name[UMI_TEST_RUNTIME_ID_CAPACITY];
    char detail[UMI_TEST_RUNTIME_TEXT_CAPACITY];
    uint64_t selected_count;
    uint64_t generation;
    uint64_t revision;
    uint64_t updated_at_ms;
    bool enabled;
} UmiTestRuntimeSelection;

void umi_test_runtime_selection_init(UmiTestRuntimeSelection *value, const char *id);
UmiStatus umi_test_runtime_selection_validate(const UmiTestRuntimeSelection *value);
UmiStatus umi_test_runtime_selection_set_name(UmiTestRuntimeSelection *value, const char *name);
UmiStatus umi_test_runtime_selection_set_detail(UmiTestRuntimeSelection *value, const char *detail);
UmiStatus umi_test_runtime_selection_set_selected_count(UmiTestRuntimeSelection *value, uint64_t number);
UmiStatus umi_test_runtime_selection_set_generation(UmiTestRuntimeSelection *value, uint64_t number);
UmiStatus umi_test_runtime_selection_touch(UmiTestRuntimeSelection *value, uint64_t updated_at_ms);
bool umi_test_runtime_selection_same_identity(const UmiTestRuntimeSelection *left, const UmiTestRuntimeSelection *right);

#ifdef __cplusplus
}
#endif
#endif

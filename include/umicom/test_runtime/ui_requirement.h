/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/ui_requirement.h
 *
 * PURPOSE:
 *   Describe headless or GUI runtime requirements for a test.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_UI_REQUIREMENT
#define UMICOM_TEST_RUNTIME_UI_REQUIREMENT
#include "umicom/test_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTestRuntimeUiRequirement
{
    uint32_t structure_size;
    char id[UMI_TEST_RUNTIME_ID_CAPACITY];
    char detail[UMI_TEST_RUNTIME_TEXT_CAPACITY];
    uint64_t required;
    uint64_t display_available;
    uint64_t revision;
    bool enabled;
    } UmiTestRuntimeUiRequirement;
void umi_test_runtime_ui_requirement_init(UmiTestRuntimeUiRequirement *value,const char *id);
UmiStatus umi_test_runtime_ui_requirement_validate(const UmiTestRuntimeUiRequirement *value);
UmiStatus umi_test_runtime_ui_requirement_set_detail(UmiTestRuntimeUiRequirement *value,const char *detail);
UmiStatus umi_test_runtime_ui_requirement_set_required(UmiTestRuntimeUiRequirement *value,uint64_t number);
UmiStatus umi_test_runtime_ui_requirement_set_display_available(UmiTestRuntimeUiRequirement *value,uint64_t number);
bool umi_test_runtime_ui_requirement_same_identity(const UmiTestRuntimeUiRequirement *left,const UmiTestRuntimeUiRequirement *right);
#ifdef __cplusplus
}
#endif
#endif

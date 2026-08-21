/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/quarantine.h
 *
 * PURPOSE:
 *   Represent explicit temporary quarantine without silently disabling a test.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_QUARANTINE
#define UMICOM_TEST_RUNTIME_QUARANTINE
#include "umicom/test_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTestRuntimeQuarantine {
    uint32_t structure_size;
    char id[UMI_TEST_RUNTIME_ID_CAPACITY];
    char category[UMI_TEST_RUNTIME_ID_CAPACITY];
    char detail[UMI_TEST_RUNTIME_TEXT_CAPACITY];
    uint64_t expires_at_ms;
    uint64_t issue_count;
    uint64_t revision;
    bool active;
} UmiTestRuntimeQuarantine;
void umi_test_runtime_quarantine_init(UmiTestRuntimeQuarantine *value,const char *id);
UmiStatus umi_test_runtime_quarantine_validate(const UmiTestRuntimeQuarantine *value);
UmiStatus umi_test_runtime_quarantine_set_category(UmiTestRuntimeQuarantine *value,const char *category);
UmiStatus umi_test_runtime_quarantine_set_detail(UmiTestRuntimeQuarantine *value,const char *detail);
UmiStatus umi_test_runtime_quarantine_set_expires_at_ms(UmiTestRuntimeQuarantine *value,uint64_t number);
UmiStatus umi_test_runtime_quarantine_set_issue_count(UmiTestRuntimeQuarantine *value,uint64_t number);
UmiStatus umi_test_runtime_quarantine_set_active(UmiTestRuntimeQuarantine *value,bool active);
bool umi_test_runtime_quarantine_same_identity(const UmiTestRuntimeQuarantine *left,const UmiTestRuntimeQuarantine *right);
#ifdef __cplusplus
}
#endif
#endif

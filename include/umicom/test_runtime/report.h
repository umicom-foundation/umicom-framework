/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/report.h
 *
 * PURPOSE:
 *   Build deterministic human-readable and machine-readable regression summaries.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_REPORT
#define UMICOM_TEST_RUNTIME_REPORT

#include "umicom/test_runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiTestRuntimeReport {
    uint32_t structure_size;
    char id[UMI_TEST_RUNTIME_ID_CAPACITY];
    char name[UMI_TEST_RUNTIME_ID_CAPACITY];
    char detail[UMI_TEST_RUNTIME_TEXT_CAPACITY];
    uint64_t passed;
    uint64_t failed;
    uint64_t updated_at_ms;
    uint64_t revision;
    bool enabled;
} UmiTestRuntimeReport;

void umi_test_runtime_report_init(UmiTestRuntimeReport *value, const char *id);
UmiStatus umi_test_runtime_report_validate(const UmiTestRuntimeReport *value);
UmiStatus umi_test_runtime_report_set_name(UmiTestRuntimeReport *value, const char *name);
UmiStatus umi_test_runtime_report_set_detail(UmiTestRuntimeReport *value, const char *detail);
UmiStatus umi_test_runtime_report_set_passed(UmiTestRuntimeReport *value, uint64_t number);
UmiStatus umi_test_runtime_report_set_failed(UmiTestRuntimeReport *value, uint64_t number);
UmiStatus umi_test_runtime_report_touch(UmiTestRuntimeReport *value, uint64_t updated_at_ms);
bool umi_test_runtime_report_same_identity(const UmiTestRuntimeReport *left, const UmiTestRuntimeReport *right);

#ifdef __cplusplus
}
#endif
#endif

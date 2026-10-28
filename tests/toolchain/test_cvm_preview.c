/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/toolchain/test_cvm_preview.c
 *
 * PURPOSE:
 *   Expose bounded source preview with explicit result ownership.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/toolchain/cvm_preview.h"
#include <stdio.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%d: %s\n", __LINE__, #x); return 1; } } while (0)
int main(void)
{
    UmiCvmPreviewWorkspace workspace;
    UmiCvmPreviewResult result;
    const char *good = "int main(void) { return 6 * 7; }";
    CHECK(umi_cvm_preview(good, strlen(good), 4U, &workspace, sizeof workspace,
        &result, sizeof result) == UMI_STATUS_OK);
    CHECK(result.completed && result.phase == UMI_CVM_COMPLETE &&
        result.execution.value == 42 && result.execution.instructions_executed == 4U);
    const char *bad = "int main(void) { return ; }";
    CHECK(umi_cvm_preview(bad, strlen(bad), 4U, &workspace, sizeof workspace,
        &result, sizeof result) == UMI_STATUS_PARSE_ERROR);
    CHECK(!result.completed && result.phase == UMI_CVM_COMPILING &&
        result.execution.value == 0 && result.instruction_count == 0U);
    CHECK(umi_cvm_preview(good, strlen(good), 3U, &workspace, sizeof workspace,
        &result, sizeof result) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(!result.completed && result.phase == UMI_CVM_EXECUTING &&
        result.diagnostic.status == UMC_FUEL_EXHAUSTED);
    const char *overflow = "int main(void) { return 2147483647 + 1; }";
    CHECK(umi_cvm_preview(overflow, strlen(overflow), 4U, &workspace, sizeof workspace,
        &result, sizeof result) == UMI_STATUS_INVALID_STATE);
    CHECK(!result.completed && result.diagnostic.status == UMC_ARITHMETIC_ERROR &&
        result.diagnostic.line == 1U && result.diagnostic.column > 1U);
    result.phase = UMI_CVM_NOT_STARTED;
    CHECK(umi_cvm_preview(good, strlen(good), 4U, &workspace, sizeof workspace - 1U,
        &result, sizeof result) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(result.phase == UMI_CVM_NOT_STARTED);
    CHECK(umi_cvm_preview(NULL, 0U, 4U, &workspace, sizeof workspace,
        &result, sizeof result) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(!result.completed && result.diagnostic.status == UMC_INVALID_ARGUMENT);
    return 0;
}

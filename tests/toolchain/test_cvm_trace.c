/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/toolchain/test_cvm_trace.c
 *
 * PURPOSE:
 *   Check trace lifetime, result invalidation and storage refusal in the optional Framework adapter.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/toolchain/cvm_trace.h"
#include <stdio.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%d: %s\n", __LINE__, #x); return 1; } } while (0)

static const char good[] = "int main(void) { return 6 * 7; }";
static int request(const char *name)
{
    UmiCvmPreviewWorkspace workspace;
    UmiCvmPreviewResult result;
    UmiCvmPreviewTrace trace;
    CHECK(umi_cvm_preview_traced(good, strlen(good), 4U, &workspace, sizeof workspace,
        &result, sizeof result, &trace, sizeof trace) == UMI_STATUS_OK);
    CHECK(result.completed && result.phase == UMI_CVM_COMPLETE && trace.count == 4U);
    CHECK(result.execution.value == 42 && trace.steps[3].opcode == UMC_RETURN);
    CHECK(trace.steps[3].status == UMC_OK && trace.steps[3].top == 42);
    if (strcmp(name, "success") == 0) return 0;
    if (strcmp(name, "ordinary-parity") == 0) {
        UmiCvmPreviewResult ordinary;
        CHECK(umi_cvm_preview(good, strlen(good), 4U, &workspace, sizeof workspace,
            &ordinary, sizeof ordinary) == UMI_STATUS_OK);
        CHECK(ordinary.phase == result.phase && ordinary.completed == result.completed);
        CHECK(ordinary.execution.value == result.execution.value &&
            ordinary.execution.instructions_executed == result.execution.instructions_executed);
        return 0;
    }
    if (strcmp(name, "syntax-after-success") == 0 || strcmp(name, "null-source") == 0) {
        const char bad[] = "int main(void) { return ; }";
        const int null_source = strcmp(name, "null-source") == 0;
        CHECK(umi_cvm_preview_traced(null_source ? NULL : bad, null_source ? 0U : strlen(bad),
            4U, &workspace, sizeof workspace, &result, sizeof result, &trace, sizeof trace) ==
            (null_source ? UMI_STATUS_INVALID_ARGUMENT : UMI_STATUS_PARSE_ERROR));
        CHECK(!result.completed && result.phase == UMI_CVM_COMPILING && trace.count == 0U);
        CHECK(result.instruction_count == 0U && result.execution.value == 0 &&
            result.execution.instructions_executed == 0U);
        return 0;
    }
    if (strcmp(name, "arithmetic-failure") == 0) {
        const char fault[] = "int main(void) {\n return 7 / 0;\n}";
        CHECK(umi_cvm_preview_traced(fault, strlen(fault), 4U, &workspace, sizeof workspace,
            &result, sizeof result, &trace, sizeof trace) == UMI_STATUS_INVALID_STATE);
        CHECK(!result.completed && result.phase == UMI_CVM_EXECUTING && trace.count == 3U);
        CHECK(result.diagnostic.status == UMC_ARITHMETIC_ERROR && result.diagnostic.line == 2U);
        CHECK(trace.steps[2].status == UMC_ARITHMETIC_ERROR && trace.steps[2].stack_depth == 2U &&
            trace.steps[2].top == 0 && result.execution.value == 0);
        return 0;
    }
    if (strcmp(name, "fuel-zero") == 0 || strcmp(name, "fuel-return") == 0) {
        const size_t fuel = strcmp(name, "fuel-zero") == 0 ? 0U : 3U;
        CHECK(umi_cvm_preview_traced(good, strlen(good), fuel, &workspace, sizeof workspace,
            &result, sizeof result, &trace, sizeof trace) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(!result.completed && result.phase == UMI_CVM_EXECUTING && trace.count == fuel + 1U);
        CHECK(trace.steps[fuel].status == UMC_FUEL_EXHAUSTED && result.execution.value == 0);
        CHECK(trace.steps[fuel].stack_depth == (fuel == 0U ? 0U : 1U));
        CHECK(trace.steps[fuel].top == (fuel == 0U ? 0 : 42));
        return 0;
    }
    if (strcmp(name, "shorter-request") == 0) {
        const char short_source[] = "int main(void) { return 9; }";
        CHECK(umi_cvm_preview_traced(short_source, strlen(short_source), 2U, &workspace, sizeof workspace,
            &result, sizeof result, &trace, sizeof trace) == UMI_STATUS_OK);
        CHECK(result.completed && result.execution.value == 9 && trace.count == 2U);
        CHECK(trace.steps[1].opcode == UMC_RETURN && trace.steps[1].top == 9);
        return 0;
    }
    if (strcmp(name, "independent-storage") == 0) {
        UmiCvmPreviewWorkspace second_workspace;
        UmiCvmPreviewResult second_result;
        UmiCvmPreviewTrace second_trace;
        CHECK(umi_cvm_preview_traced(good, strlen(good), 0U, &second_workspace, sizeof second_workspace,
            &second_result, sizeof second_result, &second_trace, sizeof second_trace) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(second_trace.count == 1U && !second_result.completed);
        CHECK(trace.count == 4U && result.completed && result.execution.value == 42);
        return 0;
    }
    CHECK(0);
}

static int storage(const char *name)
{
    UmiCvmPreviewWorkspace workspace;
    UmiCvmPreviewResult result;
    UmiCvmPreviewTrace trace;
    memset(&workspace, 0x4a, sizeof workspace);
    memset(&result, 0x5a, sizeof result);
    memset(&trace, 0x6a, sizeof trace);
    unsigned char before_workspace[sizeof workspace], before_result[sizeof result], before_trace[sizeof trace];
    memcpy(before_workspace, &workspace, sizeof workspace);
    memcpy(before_result, &result, sizeof result);
    memcpy(before_trace, &trace, sizeof trace);
    size_t workspace_size = sizeof workspace, result_size = sizeof result, trace_size = sizeof trace;
    UmiCvmPreviewWorkspace *workspace_pointer = &workspace;
    UmiCvmPreviewResult *result_pointer = &result;
    UmiCvmPreviewTrace *trace_pointer = &trace;
    if (strcmp(name, "workspace-size") == 0) --workspace_size;
    else if (strcmp(name, "result-size") == 0) --result_size;
    else if (strcmp(name, "trace-size") == 0) --trace_size;
    else if (strcmp(name, "null-workspace") == 0) workspace_pointer = NULL;
    else if (strcmp(name, "null-result") == 0) result_pointer = NULL;
    else if (strcmp(name, "null-trace") == 0) trace_pointer = NULL;
    else CHECK(0);
    CHECK(umi_cvm_preview_traced(good, strlen(good), 4U, workspace_pointer, workspace_size,
        result_pointer, result_size, trace_pointer, trace_size) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(memcmp(before_workspace, &workspace, sizeof workspace) == 0);
    CHECK(memcmp(before_result, &result, sizeof result) == 0);
    CHECK(memcmp(before_trace, &trace, sizeof trace) == 0);
    return 0;
}

int main(int argc, char **argv)
{
    CHECK(argc == 2);
    if (strstr(argv[1], "-size") != NULL || strcmp(argv[1], "null-workspace") == 0 ||
        strcmp(argv[1], "null-result") == 0 || strcmp(argv[1], "null-trace") == 0) return storage(argv[1]);
    return request(argv[1]);
}

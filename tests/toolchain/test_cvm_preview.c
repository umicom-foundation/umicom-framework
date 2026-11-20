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
#include "umicom/toolchain/cvm_trace.h"
#include <stdio.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%d: %s\n", __LINE__, #x); return 1; } } while (0)

static int check_bytecode(void)
{
    const char program[] = "int main(void) { return 6 * 7; }";
    UmiCvmPreviewWorkspace workspace;
    UmiCvmPreviewResult result;
    UmiCvmPreviewTrace trace;
    uint8_t artifact[UMC_BYTECODE_HEADER_SIZE + UMC_PROGRAM_LIMIT * UMC_BYTECODE_RECORD_SIZE];
    size_t length = 0U;
    CHECK(umi_cvm_preview(program, sizeof program - 1U, 4U, &workspace, sizeof workspace,
        &result, sizeof result) == UMI_STATUS_OK);
    CHECK(umc_encode(workspace.instructions, result.instruction_count, artifact, sizeof artifact,
        &length, &result.diagnostic) == UMC_OK);
    CHECK(umi_cvm_preview_bytecode(artifact, length, 4U, &workspace, sizeof workspace,
        &result, sizeof result) == UMI_STATUS_OK);
    CHECK(result.completed && result.phase == UMI_CVM_COMPLETE && result.execution.value == 42);
    CHECK(umi_cvm_preview_bytecode_traced(artifact, length, 4U, &workspace, sizeof workspace,
        &result, sizeof result, &trace, sizeof trace) == UMI_STATUS_OK);
    CHECK(result.completed && result.execution.value == 42 &&
        result.execution.instructions_executed == 4U && trace.count == 4U);

    CHECK(umi_cvm_preview_bytecode_traced(artifact, length, 4U, &workspace, sizeof workspace,
        &result, sizeof result, &trace, sizeof trace - 1U) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(result.completed && result.execution.value == 42 && trace.count == 4U);
    CHECK(umi_cvm_preview_bytecode_traced(artifact, length - 1U, 4U, &workspace, sizeof workspace,
        &result, sizeof result, &trace, sizeof trace) == UMI_STATUS_PARSE_ERROR);
    CHECK(!result.completed && result.phase == UMI_CVM_DECODING &&
        result.instruction_count == 0U && result.execution.value == 0 && trace.count == 0U);
    CHECK(result.diagnostic.status == UMC_BAD_BYTECODE);

    CHECK(umi_cvm_preview_bytecode_traced(artifact, length, 3U, &workspace, sizeof workspace,
        &result, sizeof result, &trace, sizeof trace) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(!result.completed && result.phase == UMI_CVM_EXECUTING &&
        result.execution.value == 0 && result.diagnostic.status == UMC_FUEL_EXHAUSTED);
    CHECK(result.diagnostic.line == 0U && result.diagnostic.column == 0U);
    CHECK(trace.count == 4U && trace.steps[3].status == UMC_FUEL_EXHAUSTED);
    return 0;
}

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
    /* The adapter reuses the linked compiler; it must not reimplement update
     * arithmetic or expose the successful result of an earlier preview after
     * a compound statement fails. These checks require the matching compiler. */
    const char *compound = "int main(void){int value=6;value*=7;return value;}";
    CHECK(umi_cvm_preview(compound, strlen(compound), 8U, &workspace, sizeof workspace,
        &result, sizeof result) == UMI_STATUS_OK);
    CHECK(result.completed && result.phase == UMI_CVM_COMPLETE &&
        result.execution.value == 42 && result.execution.instructions_executed == 8U);
    const char *compound_fault = "int main(void){int value=2147483647;value+=1;return value;}";
    CHECK(umi_cvm_preview(compound_fault, strlen(compound_fault), 8U, &workspace, sizeof workspace,
        &result, sizeof result) == UMI_STATUS_INVALID_STATE);
    CHECK(!result.completed && result.phase == UMI_CVM_EXECUTING &&
        result.execution.value == 0 && result.diagnostic.status == UMC_ARITHMETIC_ERROR);
    const char *compound_invalid = "int main(void){int value=6;return value+=1;}";
    CHECK(umi_cvm_preview(compound_invalid, strlen(compound_invalid), 8U, &workspace, sizeof workspace,
        &result, sizeof result) == UMI_STATUS_PARSE_ERROR);
    CHECK(!result.completed && result.phase == UMI_CVM_COMPILING &&
        result.instruction_count == 0U && result.execution.value == 0);
    /* The shared adapter must follow selected branch execution and clear
     * earlier completion on fuel, arithmetic or grammar failure. */
    const char *conditional = "int main(void){int x=1;if(0){x=7;}else{x=42;}return x;}";
    CHECK(umi_cvm_preview(conditional, strlen(conditional), 8U, &workspace, sizeof workspace,
        &result, sizeof result) == UMI_STATUS_OK);
    CHECK(result.completed && result.execution.value == 42 && result.instruction_count == 11U &&
        result.execution.instructions_executed == 8U);
    CHECK(umi_cvm_preview(conditional, strlen(conditional), 7U, &workspace, sizeof workspace,
        &result, sizeof result) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(!result.completed && result.phase == UMI_CVM_EXECUTING && result.execution.value == 0 &&
        result.diagnostic.status == UMC_FUEL_EXHAUSTED);
    const char *conditional_fault = "int main(void){int x=1;if(1){x/=0;}return x;}";
    CHECK(umi_cvm_preview(conditional_fault, strlen(conditional_fault), 16U, &workspace, sizeof workspace,
        &result, sizeof result) == UMI_STATUS_INVALID_STATE);
    CHECK(!result.completed && result.execution.value == 0 && result.diagnostic.status == UMC_ARITHMETIC_ERROR);
/* Branch-local declarations are now supported by the selected compiler. Retain the former refusal source as positive coverage and test actual scope escape as the replacement failure. The former implementation is retained for owner review. */
#if 0
    const char *conditional_invalid = "int main(void){if(1){int x=42;}return 0;}";
#endif
    const char *branch_local = "int main(void){if(1){int x=42;}return 0;}";
    CHECK(umi_cvm_preview(branch_local, strlen(branch_local), 16U, &workspace, sizeof workspace,
        &result, sizeof result) == UMI_STATUS_OK);
    CHECK(result.completed && result.execution.value == 0 && result.instruction_count == 6U);
    const char *conditional_invalid = "int main(void){if(1){int x=42;}return x;}";
    CHECK(umi_cvm_preview(conditional_invalid, strlen(conditional_invalid), 16U, &workspace, sizeof workspace,
        &result, sizeof result) == UMI_STATUS_PARSE_ERROR);
    CHECK(!result.completed && result.phase == UMI_CVM_COMPILING && result.instruction_count == 0U);
    /* Chain spelling is owned by the linked compiler, while preview keeps
     * the same completed-result boundary and instruction-fuel contract. */
    const char *chain = "int main(void){int x=1;if(0){x=7;}else if(1){x=42;}else{x=9;}return x;}";
    CHECK(umi_cvm_preview(chain, strlen(chain), 11U, &workspace, sizeof workspace,
        &result, sizeof result) == UMI_STATUS_OK);
    CHECK(result.completed && result.execution.value == 42 && result.instruction_count == 16U &&
        result.execution.instructions_executed == 11U);
    CHECK(umi_cvm_preview(chain, strlen(chain), 10U, &workspace, sizeof workspace,
        &result, sizeof result) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(!result.completed && result.execution.value == 0 && result.diagnostic.status == UMC_FUEL_EXHAUSTED);
    const char *chain_fault = "int main(void){int x=1;if(0){}else if(1/0){x=42;}return x;}";
    CHECK(umi_cvm_preview(chain_fault, strlen(chain_fault), 32U, &workspace, sizeof workspace,
        &result, sizeof result) == UMI_STATUS_INVALID_STATE);
    CHECK(!result.completed && result.execution.value == 0 && result.diagnostic.status == UMC_ARITHMETIC_ERROR);
    const char *chain_invalid = "int main(void){int x=1;if(0){}else if(1)x=42;return x;}";
    CHECK(umi_cvm_preview(chain_invalid, strlen(chain_invalid), 32U, &workspace, sizeof workspace,
        &result, sizeof result) == UMI_STATUS_PARSE_ERROR);
    CHECK(!result.completed && result.phase == UMI_CVM_COMPILING && result.instruction_count == 0U);
    /* Remainder is supplied by the canonical compiler/runtime pair; the
     * Framework preview must clear previous results after its checked faults. */
    const char *remainder = "int main(void){int value=137;value%=95;return value;}";
    CHECK(umi_cvm_preview(remainder, strlen(remainder), 8U, &workspace, sizeof workspace,
        &result, sizeof result) == UMI_STATUS_OK);
    CHECK(result.completed && result.execution.value == 42 && result.execution.instructions_executed == 8U);
    const char *remainder_fault = "int main(void){return (-2147483647-1)%-1;}";
    CHECK(umi_cvm_preview(remainder_fault, strlen(remainder_fault), 16U, &workspace, sizeof workspace,
        &result, sizeof result) == UMI_STATUS_INVALID_STATE);
    CHECK(!result.completed && result.execution.value == 0 && result.diagnostic.status == UMC_ARITHMETIC_ERROR);
    CHECK(umi_cvm_preview(remainder, strlen(remainder), 7U, &workspace, sizeof workspace,
        &result, sizeof result) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(!result.completed && result.execution.value == 0 && result.diagnostic.status == UMC_FUEL_EXHAUSTED);

    /* Complement is compiled by Umicom C. Preview owns its displayed result
     * and must not retain an earlier success after a budget or operand fault. */
    const char *complement="int main(void){return ~-43;}";
    CHECK(umi_cvm_preview(complement,strlen(complement),5U,&workspace,sizeof workspace,
        &result,sizeof result)==UMI_STATUS_OK);
    CHECK(result.completed && result.execution.value==42 && result.instruction_count==5U);
    CHECK(umi_cvm_preview(complement,strlen(complement),4U,&workspace,sizeof workspace,
        &result,sizeof result)==UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(!result.completed && result.execution.value==0 && result.diagnostic.status==UMC_FUEL_EXHAUSTED);
    const char *complement_minimum="int main(void){return ~(-2147483647-1);}";
    CHECK(umi_cvm_preview(complement_minimum,strlen(complement_minimum),16U,&workspace,sizeof workspace,
        &result,sizeof result)==UMI_STATUS_OK);
    CHECK(result.completed && result.execution.value==INT32_MAX);
    const char *complement_fault="int main(void){return ~(1/0);}";
    CHECK(umi_cvm_preview(complement_fault,strlen(complement_fault),16U,&workspace,sizeof workspace,
        &result,sizeof result)==UMI_STATUS_INVALID_STATE);
    CHECK(!result.completed && result.execution.value==0 && result.diagnostic.status==UMC_ARITHMETIC_ERROR);
    const char *complement_invalid="int main(void){return ~=1;}";
    CHECK(umi_cvm_preview(complement_invalid,strlen(complement_invalid),16U,&workspace,sizeof workspace,
        &result,sizeof result)==UMI_STATUS_PARSE_ERROR);
    CHECK(!result.completed && result.phase==UMI_CVM_COMPILING && result.instruction_count==0U);
    /* Bitwise semantics remain in the canonical compiler/runtime pair. The
     * shared adapter clears earlier success before any new preview attempt. */
    const char *bitwise="int main(void){return 63 & 47 ^ 5 | 0;}";
    CHECK(umi_cvm_preview(bitwise,strlen(bitwise),8U,&workspace,sizeof workspace,
        &result,sizeof result)==UMI_STATUS_OK);
    CHECK(result.completed && result.execution.value==42 && result.instruction_count==8U);
    CHECK(umi_cvm_preview(bitwise,strlen(bitwise),7U,&workspace,sizeof workspace,
        &result,sizeof result)==UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(!result.completed && result.execution.value==0 && result.diagnostic.status==UMC_FUEL_EXHAUSTED);
    const char *bitwise_update="int main(void){int x=63;x&=47;x^=5;x|=0;return x;}";
    CHECK(umi_cvm_preview(bitwise_update,strlen(bitwise_update),16U,&workspace,sizeof workspace,
        &result,sizeof result)==UMI_STATUS_OK);
    CHECK(result.completed && result.execution.value==42 && result.instruction_count==16U);
    const char *bitwise_fault="int main(void){return 0 & (1/0);}";
    CHECK(umi_cvm_preview(bitwise_fault,strlen(bitwise_fault),16U,&workspace,sizeof workspace,
        &result,sizeof result)==UMI_STATUS_INVALID_STATE);
    CHECK(!result.completed && result.execution.value==0 && result.diagnostic.status==UMC_ARITHMETIC_ERROR);
    const char *bitwise_invalid="int main(void){int x=63;return x&=42;}";
    CHECK(umi_cvm_preview(bitwise_invalid,strlen(bitwise_invalid),16U,&workspace,sizeof workspace,
        &result,sizeof result)==UMI_STATUS_PARSE_ERROR);
    CHECK(!result.completed && result.phase==UMI_CVM_COMPILING && result.instruction_count==0U);
    /* Scope lifetime remains compiler-owned; preview must accept the same
     * source and never make a retired local name visible again. */
    const char *scoped = "int main(void){int result=0;{const int value=42;result=value;}return result;}";
    CHECK(umi_cvm_preview(scoped, strlen(scoped), 8U, &workspace, sizeof workspace,
        &result, sizeof result) == UMI_STATUS_OK);
    CHECK(result.completed && result.execution.value == 42 && result.instruction_count == 8U);
    const char *reused = "int main(void){int r=0;{int x=7;r=x;}{int x=6;r*=x;}return r;}";
    CHECK(umi_cvm_preview(reused, strlen(reused), 16U, &workspace, sizeof workspace,
        &result, sizeof result) == UMI_STATUS_OK);
    CHECK(result.completed && result.execution.value == 42);
    const char *escaped = "int main(void){{int x=42;}return x;}";
    CHECK(umi_cvm_preview(escaped, strlen(escaped), 16U, &workspace, sizeof workspace,
        &result, sizeof result) == UMI_STATUS_PARSE_ERROR);
    CHECK(!result.completed && result.phase == UMI_CVM_COMPILING && result.instruction_count == 0U);
    return check_bytecode();
}

/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/cvm_preview/main.c
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
/* Keep the earlier consumer for engineering comparison. The replacement
 * preserves its ordinary source argument and output while exposing Framework
 * trace state through an explicit option, shared by the OS wrapper project. */
#if 0
int main(int argc, char **argv)
{
    static const char demonstration[] = "int main(void) { return 6 * 7; }";
    if (argc > 2) {
        fputs("Usage: umicom-cvm-preview [source-expression-program]\n", stderr);
        return 2;
    }
    const char *source = argc == 2 ? argv[1] : demonstration;
    UmiCvmPreviewWorkspace workspace;
    UmiCvmPreviewResult result;
    const UmiStatus status = umi_cvm_preview(source, strlen(source), UMC_PROGRAM_LIMIT,
        &workspace, sizeof workspace, &result, sizeof result);
    if (status != UMI_STATUS_OK) {
        fprintf(stderr, "%zu:%zu: %s\n", result.diagnostic.line,
            result.diagnostic.column, result.diagnostic.message);
        return 1;
    }
    printf("%ld\n", (long)result.execution.value);
    return 0;
}
#endif

int main(int argc, char **argv)
{
    static const char demonstration[] = "int main(void) { return 6 * 7; }";
    const bool tracing = argc >= 2 && strcmp(argv[1], "--trace") == 0;
    if (argc > 3 || (argc == 3 && !tracing)) {
        fputs("Usage: umicom-cvm-preview [--trace] [source-expression-program]\n", stderr);
        return 2;
    }
    const char *source = tracing ? (argc == 3 ? argv[2] : demonstration) :
        (argc == 2 ? argv[1] : demonstration);
    UmiCvmPreviewWorkspace workspace;
    UmiCvmPreviewResult result;
    UmiCvmPreviewTrace trace;
    UmiStatus status;
    if (tracing) {
        status = umi_cvm_preview_traced(source, strlen(source), UMC_PROGRAM_LIMIT,
            &workspace, sizeof workspace, &result, sizeof result, &trace, sizeof trace);
        for (size_t i = 0U; i < trace.count; ++i) {
            printf("%zu %s depth=%zu top=%ld status=%s\n", trace.steps[i].instruction_index,
                umc_opcode_text(trace.steps[i].opcode), trace.steps[i].stack_depth,
                (long)trace.steps[i].top, umc_status_text(trace.steps[i].status));
        }
    } else {
        status = umi_cvm_preview(source, strlen(source), UMC_PROGRAM_LIMIT,
            &workspace, sizeof workspace, &result, sizeof result);
    }
    if (status != UMI_STATUS_OK) {
        fprintf(stderr, "%zu:%zu: %s\n", result.diagnostic.line,
            result.diagnostic.column, result.diagnostic.message);
        return 1;
    }
    printf("%ld\n", (long)result.execution.value);
    return 0;
}

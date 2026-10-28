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
#include <stdio.h>
#include <string.h>
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

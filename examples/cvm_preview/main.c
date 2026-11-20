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
#include "umicom/toolchain/cvm_debug.h"
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


/* This thin consumer demonstrates the lifecycle used by an editor or OS tool.
 * The shared service owns source locations and stale-result invalidation. */
static int stepped_preview(const char *source)
{
    UmiCvmDebugSession session;
    UmcStatus status=umi_cvm_debug_start(source,strlen(source),UMC_PROGRAM_LIMIT,&session,sizeof session);
    while(status==UMC_OK && session.phase==UMI_CVM_DEBUG_PAUSED) {
        const size_t pc=session.machine.frame.pc;
        status=umi_cvm_debug_step(&session,sizeof session);
        printf("step=%zu line=%zu column=%zu status=%s\n",pc,
            session.lines[pc],session.columns[pc],umc_status_text(status));
    }
    if(status!=UMC_OK) {
        fprintf(stderr,"%zu:%zu: %s\n",session.diagnostic.line,
            session.diagnostic.column,session.diagnostic.message);
        return 1;
    }
    printf("%ld\n",(long)session.result.value);
    return fflush(stdout)!=0 || ferror(stdout)?2:0;
}

/* A command-line demonstration drains slices immediately; an interactive host
 * would schedule the next slice on a later owner-thread turn. No timeout or
 * responsiveness claim follows from this synchronous teaching command. */
static int sliced_preview(const char *source, size_t budget)
{
    UmiCvmDebugSession session;
    UmcStatus status=umi_cvm_debug_start(source,strlen(source),UMC_PROGRAM_LIMIT,&session,sizeof session);
    size_t slices=0U;
    while(status==UMC_OK && session.phase==UMI_CVM_DEBUG_PAUSED) {
        status=umi_cvm_debug_resume(&session,sizeof session,budget);
        ++slices;
    }
    if(status!=UMC_OK) {
        fprintf(stderr,"%zu:%zu: %s\n",session.diagnostic.line,
            session.diagnostic.column,session.diagnostic.message);
        return 1;
    }
    printf("slices=%zu instructions=%zu value=%ld\n",slices,
        session.result.instructions_executed,(long)session.result.value);
    return fflush(stdout)!=0 || ferror(stdout)?2:0;
}
/* Parse only the documented decimal count. Bound before multiplying and never
 * accept signs, whitespace, empty text or a silently truncated integer. */
static bool slice_budget(const char *text, size_t *out)
{
    size_t value=0U;
    if(text==NULL || *text=='\0')return false;
    for(size_t i=0U;text[i]!='\0';++i) {
        if(text[i]<'0'||text[i]>'9')return false;
        const size_t digit=(size_t)(text[i]-'0');
        if(value>(UMC_PROGRAM_LIMIT-digit)/10U)return false;
        value=value*10U+digit;
    }
    if(value==0U)return false;
    *out=value;
    return true;
}

/* The teaching tool admits one bounded binary file. The shared API owns the
 * byte format and execution; the CLI only reads the requested file. */
static int artifact_preview(const char *path)
{
    enum { maximum = UMC_BYTECODE_HEADER_SIZE + UMC_PROGRAM_LIMIT * UMC_BYTECODE_RECORD_SIZE };
    uint8_t bytes[maximum + 1U];
    FILE *file = fopen(path, "rb");
    if (file == NULL) {
        fputs("Cannot open bytecode file.\n", stderr);
        return 1;
    }
    const size_t length = fread(bytes, 1U, sizeof bytes, file);
    const bool read_error = ferror(file) != 0;
    const int close_status = fclose(file);
    if (read_error || close_status != 0 || length > maximum) {
        fputs("Cannot read a complete bytecode file within the format limit.\n", stderr);
        return 1;
    }
    UmiCvmPreviewWorkspace workspace;
    UmiCvmPreviewResult result;
    const UmiStatus status = umi_cvm_preview_bytecode(bytes, length, UMC_PROGRAM_LIMIT,
        &workspace, sizeof workspace, &result, sizeof result);
    if (status != UMI_STATUS_OK) {
        fprintf(stderr, "Bytecode preview: %s\n", result.diagnostic.message);
        return 1;
    }
    printf("%ld\n", (long)result.execution.value);
    return fflush(stdout) != 0 || ferror(stdout) ? 2 : 0;
}

int main(int argc, char **argv)
{
    static const char demonstration[] = "int main(void) { return 6 * 7; }";
    if (argc >= 2 && strcmp(argv[1], "--bytecode") == 0) {
        if (argc != 3) {
            fputs("Usage: umicom-cvm-preview --bytecode program.umcb\n", stderr);
            return 2;
        }
        return artifact_preview(argv[2]);
    }
    if(argc>=2 && strcmp(argv[1],"--slice")==0) {
        size_t budget=0U;
        if(argc<3 || argc>4 || !slice_budget(argv[2],&budget)) {
            fputs("Usage: umicom-cvm-preview --slice 1..256 [source-program]\n",stderr);
            return 2;
        }
        return sliced_preview(argc==4?argv[3]:demonstration,budget);
    }
    if(argc>=2 && strcmp(argv[1],"--steps")==0) {
        if(argc>3){fputs("Usage: umicom-cvm-preview --steps [source-program]\n",stderr);return 2;}
        return stepped_preview(argc==3?argv[2]:demonstration);
    }
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

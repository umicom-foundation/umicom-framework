/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/compiler/target.c
 *
 * PURPOSE:
 *   Implement the target behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework | Compiler targets | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/compiler/target.h"
#include <stdio.h>
#include <string.h>
/* Provide the classify operation used by this module and its client applications. */
static void classify(UmiCompilerTarget *target)
{
    target->little_endian = true;
    target->pointer_width = strstr(target->architecture,"64") != NULL ? 64U : 32U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (strstr(target->operating_system,"windows") != NULL || strstr(target->environment,"mingw") != NULL || strstr(target->environment,"msvc") != NULL) target->object_format = UMI_COMPILER_OBJECT_COFF;
    else /* Protect caller-owned memory by checking that required state is available before it is used. */ if (strstr(target->operating_system,"darwin") != NULL || strstr(target->operating_system,"macos") != NULL) target->object_format = UMI_COMPILER_OBJECT_MACH_O;
    else /* Protect caller-owned memory by checking that required state is available before it is used. */ if (strstr(target->architecture,"wasm") != NULL) target->object_format = UMI_COMPILER_OBJECT_WASM;
    /* Use this fallback path when the earlier condition does not apply. */
    else target->object_format = UMI_COMPILER_OBJECT_ELF;
}
/*
 * Read compiler target into validated module state and return a status when input cannot
 * be used.
 */
UmiStatus umi_compiler_target_parse(const char *triple,UmiCompilerTarget *out_target)
{
    int matched;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (triple == NULL || triple[0] == '\0' || out_target == NULL || strlen(triple) >= sizeof(out_target->triple)) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_target,0,sizeof(*out_target));
    (void)snprintf(out_target->triple,sizeof(out_target->triple),"%s",triple);
    matched = sscanf(triple,"%63[^-]-%63[^-]-%63[^-]-%63s",out_target->architecture,out_target->vendor,out_target->operating_system,out_target->environment);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (matched < 3) return UMI_STATUS_PARSE_ERROR;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (matched == 3) (void)snprintf(out_target->environment,sizeof(out_target->environment),"unknown");
    classify(out_target);
    return UMI_STATUS_OK;
}
/*
 * Provide the compiler target host operation used by this module and its client
 * applications.
 */
UmiStatus umi_compiler_target_host(UmiCompilerTarget *out_target)
{
#if defined(_WIN32) && defined(__x86_64__)
    return umi_compiler_target_parse("x86_64-pc-windows-gnu",out_target);
#elif defined(_WIN32)
    return umi_compiler_target_parse("i686-pc-windows-gnu",out_target);
#elif defined(__APPLE__) && defined(__aarch64__)
    return umi_compiler_target_parse("aarch64-apple-darwin-unknown",out_target);
#elif defined(__APPLE__)
    return umi_compiler_target_parse("x86_64-apple-darwin-unknown",out_target);
#elif defined(__aarch64__)
    return umi_compiler_target_parse("aarch64-unknown-linux-gnu",out_target);
#elif defined(__riscv) && (__riscv_xlen == 64)
    return umi_compiler_target_parse("riscv64-unknown-linux-gnu",out_target);
#else
    return umi_compiler_target_parse("x86_64-unknown-linux-gnu",out_target);
#endif
}
/*
 * Provide the compiler target compatible operation used by this module and its client
 * applications.
 */
bool umi_compiler_target_compatible(const UmiCompilerTarget *left,const UmiCompilerTarget *right) { return left != NULL && right != NULL && strcmp(left->architecture,right->architecture) == 0 && strcmp(left->operating_system,right->operating_system) == 0 && left->object_format == right->object_format && left->pointer_width == right->pointer_width; }
/*
 * Provide the compiler object format name operation used by this module and its client
 * applications.
 */
const char *umi_compiler_object_format_name(UmiCompilerObjectFormat format) { /* Select the behaviour associated with the requested command or state value. */ switch (format) { case UMI_COMPILER_OBJECT_COFF: return "COFF"; case UMI_COMPILER_OBJECT_ELF: return "ELF"; case UMI_COMPILER_OBJECT_MACH_O: return "Mach-O"; case UMI_COMPILER_OBJECT_WASM: return "WebAssembly"; default: return "Unknown"; } }

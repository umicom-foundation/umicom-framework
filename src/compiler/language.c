/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/compiler/language.c
 *
 * PURPOSE:
 *   Implement the language behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework | Compiler languages | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/compiler/language.h"
#include <string.h>
/*
 * Provide the compiler language name operation used by this module and its client
 * applications.
 */
const char *umi_compiler_language_name(UmiCompilerLanguage language)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (language) { case UMI_COMPILER_LANGUAGE_C: return "C"; case UMI_COMPILER_LANGUAGE_CPP: return "C++"; case UMI_COMPILER_LANGUAGE_ASSEMBLY: return "Assembly"; case UMI_COMPILER_LANGUAGE_RUST: return "Rust"; case UMI_COMPILER_LANGUAGE_ZIG: return "Zig"; case UMI_COMPILER_LANGUAGE_UAI: return "UAI"; default: return "Unknown"; }
}
/*
 * Provide the compiler language default standard operation used by this module and its
 * client applications.
 */
const char *umi_compiler_language_default_standard(UmiCompilerLanguage language)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (language) { case UMI_COMPILER_LANGUAGE_C: return "c23"; case UMI_COMPILER_LANGUAGE_CPP: return "c++23"; case UMI_COMPILER_LANGUAGE_RUST: return "2024"; case UMI_COMPILER_LANGUAGE_ZIG: return "master"; case UMI_COMPILER_LANGUAGE_UAI: return "uai1"; default: return "native"; }
}
/*
 * Provide the compiler language extension operation used by this module and its client
 * applications.
 */
const char *umi_compiler_language_extension(UmiCompilerLanguage language)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (language) { case UMI_COMPILER_LANGUAGE_C: return ".c"; case UMI_COMPILER_LANGUAGE_CPP: return ".cpp"; case UMI_COMPILER_LANGUAGE_ASSEMBLY: return ".S"; case UMI_COMPILER_LANGUAGE_RUST: return ".rs"; case UMI_COMPILER_LANGUAGE_ZIG: return ".zig"; case UMI_COMPILER_LANGUAGE_UAI: return ".uai"; default: return ""; }
}
/*
 * Provide the compiler language from extension operation used by this module and its
 * client applications.
 */
UmiCompilerLanguage umi_compiler_language_from_extension(const char *path)
{
    const char *dot;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (path == NULL || (dot = strrchr(path,'.')) == NULL) return UMI_COMPILER_LANGUAGE_UNKNOWN;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(dot,".c") == 0) return UMI_COMPILER_LANGUAGE_C;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(dot,".cc") == 0 || strcmp(dot,".cpp") == 0 || strcmp(dot,".cxx") == 0) return UMI_COMPILER_LANGUAGE_CPP;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(dot,".s") == 0 || strcmp(dot,".S") == 0 || strcmp(dot,".asm") == 0) return UMI_COMPILER_LANGUAGE_ASSEMBLY;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(dot,".rs") == 0) return UMI_COMPILER_LANGUAGE_RUST;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(dot,".zig") == 0) return UMI_COMPILER_LANGUAGE_ZIG;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(dot,".uai") == 0) return UMI_COMPILER_LANGUAGE_UAI;
    return UMI_COMPILER_LANGUAGE_UNKNOWN;
}
/*
 * Provide the compiler language uses c abi operation used by this module and its client
 * applications.
 */
bool umi_compiler_language_uses_c_abi(UmiCompilerLanguage language) { return language == UMI_COMPILER_LANGUAGE_C || language == UMI_COMPILER_LANGUAGE_ASSEMBLY || language == UMI_COMPILER_LANGUAGE_ZIG || language == UMI_COMPILER_LANGUAGE_RUST || language == UMI_COMPILER_LANGUAGE_UAI; }

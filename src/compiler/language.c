/* Umicom Framework | Compiler languages | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/compiler/language.h"
#include <string.h>
const char *umi_compiler_language_name(UmiCompilerLanguage language)
{
    switch (language) { case UMI_COMPILER_LANGUAGE_C: return "C"; case UMI_COMPILER_LANGUAGE_CPP: return "C++"; case UMI_COMPILER_LANGUAGE_ASSEMBLY: return "Assembly"; case UMI_COMPILER_LANGUAGE_RUST: return "Rust"; case UMI_COMPILER_LANGUAGE_ZIG: return "Zig"; case UMI_COMPILER_LANGUAGE_UAI: return "UAI"; default: return "Unknown"; }
}
const char *umi_compiler_language_default_standard(UmiCompilerLanguage language)
{
    switch (language) { case UMI_COMPILER_LANGUAGE_C: return "c23"; case UMI_COMPILER_LANGUAGE_CPP: return "c++23"; case UMI_COMPILER_LANGUAGE_RUST: return "2024"; case UMI_COMPILER_LANGUAGE_ZIG: return "master"; case UMI_COMPILER_LANGUAGE_UAI: return "uai1"; default: return "native"; }
}
const char *umi_compiler_language_extension(UmiCompilerLanguage language)
{
    switch (language) { case UMI_COMPILER_LANGUAGE_C: return ".c"; case UMI_COMPILER_LANGUAGE_CPP: return ".cpp"; case UMI_COMPILER_LANGUAGE_ASSEMBLY: return ".S"; case UMI_COMPILER_LANGUAGE_RUST: return ".rs"; case UMI_COMPILER_LANGUAGE_ZIG: return ".zig"; case UMI_COMPILER_LANGUAGE_UAI: return ".uai"; default: return ""; }
}
UmiCompilerLanguage umi_compiler_language_from_extension(const char *path)
{
    const char *dot;
    if (path == NULL || (dot = strrchr(path,'.')) == NULL) return UMI_COMPILER_LANGUAGE_UNKNOWN;
    if (strcmp(dot,".c") == 0) return UMI_COMPILER_LANGUAGE_C;
    if (strcmp(dot,".cc") == 0 || strcmp(dot,".cpp") == 0 || strcmp(dot,".cxx") == 0) return UMI_COMPILER_LANGUAGE_CPP;
    if (strcmp(dot,".s") == 0 || strcmp(dot,".S") == 0 || strcmp(dot,".asm") == 0) return UMI_COMPILER_LANGUAGE_ASSEMBLY;
    if (strcmp(dot,".rs") == 0) return UMI_COMPILER_LANGUAGE_RUST;
    if (strcmp(dot,".zig") == 0) return UMI_COMPILER_LANGUAGE_ZIG;
    if (strcmp(dot,".uai") == 0) return UMI_COMPILER_LANGUAGE_UAI;
    return UMI_COMPILER_LANGUAGE_UNKNOWN;
}
bool umi_compiler_language_uses_c_abi(UmiCompilerLanguage language) { return language == UMI_COMPILER_LANGUAGE_C || language == UMI_COMPILER_LANGUAGE_ASSEMBLY || language == UMI_COMPILER_LANGUAGE_ZIG || language == UMI_COMPILER_LANGUAGE_RUST || language == UMI_COMPILER_LANGUAGE_UAI; }

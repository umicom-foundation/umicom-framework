/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/toolchain/tool.c
 *
 * PURPOSE:
 *   Map tool identifiers to stable names and platform executable names used by
 *   discovery, diagnostics, manifests, build profiles, and Studio views.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/toolchain/tool.h"

const char *umi_tool_kind_name(UmiToolKind kind)
{
    static const char *const names[UMI_TOOL_COUNT] = {
        "Clang C compiler",
        "Clang C++ compiler",
        "GCC C compiler",
        "GCC C++ compiler",
        "CMake",
        "CTest",
        "Ninja",
        "pkg-config",
        "GNU windres",
        "Git",
        "GitHub CLI",
        "GDB",
        "clang-format",
        "clang-tidy",
        "Doxygen",
        "Graphviz dot",
        "Java runtime",
        "Java compiler",
        "Microsoft C/C++ compiler",
        "Microsoft linker",
        "Visual Studio locator",
        "CPack"
    };
    return kind >= 0 && kind < UMI_TOOL_COUNT
        ? names[(size_t)kind]
        : "Unknown tool";
}

const char *umi_tool_default_executable(UmiToolKind kind)
{
#ifdef _WIN32
    static const char *const names[UMI_TOOL_COUNT] = {
        "clang.exe", "clang++.exe", "gcc.exe", "g++.exe",
        "cmake.exe", "ctest.exe", "ninja.exe", "pkg-config.exe",
        "windres.exe", "git.exe", "gh.exe", "gdb.exe",
        "clang-format.exe", "clang-tidy.exe", "doxygen.exe", "dot.exe",
        "java.exe", "javac.exe", "cl.exe", "link.exe", "vswhere.exe",
        "cpack.exe"
    };
#else
    static const char *const names[UMI_TOOL_COUNT] = {
        "clang", "clang++", "gcc", "g++", "cmake", "ctest", "ninja",
        "pkg-config", "windres", "git", "gh", "gdb", "clang-format",
        "clang-tidy", "doxygen", "dot", "java", "javac", "cl", "link",
        "vswhere", "cpack"
    };
#endif
    return kind >= 0 && kind < UMI_TOOL_COUNT
        ? names[(size_t)kind]
        : "";
}

const char *umi_tool_state_text(UmiToolState state)
{
    switch (state) {
        case UMI_TOOL_MISSING: return "MISSING";
        case UMI_TOOL_FOUND: return "FOUND";
        case UMI_TOOL_VALIDATED: return "PASS";
        case UMI_TOOL_FAILED: return "FAILED";
        default: return "UNKNOWN";
    }
}

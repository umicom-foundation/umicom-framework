/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/toolchain/kit/kit_cmake.c
 * PURPOSE: Render CMake toolchain files directly from validated kit state.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/toolchain/kit_cmake.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "umicom/platform/filesystem.h"

#define UMI_KIT_CMAKE_WRITE_CAPACITY 32768U

/* Provide the append format operation used by this module and its client applications. */
static UmiStatus append_format(char *output,
                               size_t capacity,
                               size_t *position,
                               const char *format,
                               ...)
{
    int written;
    va_list arguments;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (*position >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    va_start(arguments, format);
    written = vsnprintf(output + *position, capacity - *position, format, arguments);
    va_end(arguments);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (written < 0 || (size_t)written >= capacity - *position)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    *position += (size_t)written;
    return UMI_STATUS_OK;
}

/* Provide the cmake escape operation used by this module and its client applications. */
static UmiStatus cmake_escape(const char *source, char *destination, size_t capacity)
{
    size_t input = 0U;
    size_t output = 0U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (source == NULL || destination == NULL || capacity == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (source[input] != '\0') {
        char value = source[input++];
        /* Apply this branch only when its contract condition is satisfied. */
        if (value == '\\') value = '/';
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if ((value == '"' || value == ';') && output + 2U >= capacity)
            return UMI_STATUS_CAPACITY_EXCEEDED;
        /* Apply this branch only when its contract condition is satisfied. */
        if (value == '"' || value == ';') destination[output++] = '\\';
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (output + 1U >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
        destination[output++] = value;
    }
    destination[output] = '\0';
    return UMI_STATUS_OK;
}

/* Copy append into module-owned storage so callers keep ownership of their input values. */
static UmiStatus append_set(char *output,
                            size_t capacity,
                            size_t *position,
                            const char *name,
                            const char *value)
{
    char escaped[UMI_TOOL_PATH_CAPACITY * 2U];
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL || value[0] == '\0') return UMI_STATUS_OK;
    status = cmake_escape(value, escaped, sizeof(escaped));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    return append_format(output, capacity, position,
                         "set(%s \"%s\" CACHE STRING \"Umicom kit\" FORCE)\n",
                         name, escaped);
}

/* Provide the system name operation used by this module and its client applications. */
static const char *system_name(const char *triple)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (triple == NULL) return "Generic";
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (strstr(triple, "windows") != NULL || strstr(triple, "mingw") != NULL)
        return "Windows";
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (strstr(triple, "linux") != NULL) return "Linux";
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (strstr(triple, "darwin") != NULL || strstr(triple, "apple") != NULL)
        return "Darwin";
    return "Generic";
}

/* Provide the processor name operation used by this module and its client applications. */
static void processor_name(const char *triple, char *output, size_t capacity)
{
    const char *dash;
    size_t length;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (output == NULL || capacity == 0U) return;
    output[0] = '\0';
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (triple == NULL) return;
    dash = strchr(triple, '-');
    length = dash != NULL ? (size_t)(dash - triple) : strlen(triple);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length >= capacity) length = capacity - 1U;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length > 0U) (void)memcpy(output, triple, length);
    output[length] = '\0';
}

/*
 * Provide the toolchain kit cmake render operation used by this module and its client
 * applications.
 */
UmiStatus umi_toolchain_kit_cmake_render(
    const UmiToolchainKitSnapshot *kit,
    char *out_text,
    size_t capacity)
{
    size_t position = 0U;
    char processor[UMI_TOOLCHAIN_KIT_TRIPLE_CAPACITY];
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (kit == NULL || out_text == NULL || capacity == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    out_text[0] = '\0';
    status = append_format(out_text, capacity, &position,
        "# Generated by Umicom Framework from kit: %s\n"
        "# Do not duplicate kit discovery in application repositories.\n",
        kit->id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_toolchain_kit_is_cross_compile(kit) != 0) {
        processor_name(kit->target_triple, processor, sizeof(processor));
        status = append_set(out_text, capacity, &position,
                            "CMAKE_SYSTEM_NAME", system_name(kit->target_triple));
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        status = append_set(out_text, capacity, &position,
                            "CMAKE_SYSTEM_PROCESSOR", processor);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }
#define SET(name, value)                                                           \
    do {                                                                            \
        status = append_set(out_text, capacity, &position, (name), (value));        \
        if (status != UMI_STATUS_OK) return status;                                 \
    } while (0)
    SET("CMAKE_C_COMPILER", kit->c_compiler);
    SET("CMAKE_CXX_COMPILER", kit->cpp_compiler);
    SET("CMAKE_ASM_COMPILER", kit->assembler);
    SET("CMAKE_LINKER", kit->linker);
    SET("CMAKE_MAKE_PROGRAM", kit->ninja);
    SET("CMAKE_SYSROOT", kit->sysroot);
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_toolchain_kit_is_cross_compile(kit) != 0) {
        SET("CMAKE_C_COMPILER_TARGET", kit->target_triple);
        SET("CMAKE_CXX_COMPILER_TARGET", kit->target_triple);
        SET("CMAKE_ASM_COMPILER_TARGET", kit->target_triple);
    }
#undef SET
    return UMI_STATUS_OK;
}

/*
 * Write toolchain kit cmake in its stable representation and report capacity or input
 * failures to the caller.
 */
UmiStatus umi_toolchain_kit_cmake_write(
    const UmiToolchainKitSnapshot *kit,
    const char *path)
{
    char text[UMI_KIT_CMAKE_WRITE_CAPACITY];
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (path == NULL || path[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_toolchain_kit_cmake_render(kit, text, sizeof(text));
    return status == UMI_STATUS_OK ? umi_fs_write_text(path, text) : status;
}

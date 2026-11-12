/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/toolchain/user_presets.h
 *
 * PURPOSE:
 *   Format owned toolchain values and create user presets without replacing an existing file.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#ifndef UMICOM_TOOLCHAIN_USER_PRESETS_H
#define UMICOM_TOOLCHAIN_USER_PRESETS_H
#include "umicom/toolchain/profile.h"
#ifdef __cplusplus
extern "C" {
#endif
/* Four bounded path fields, worst-case literal-dollar expansion, JSON framing
 * and a final NUL. Consumers may allocate this capacity for complete output. */
#define UMI_BUILD_USER_PRESETS_CAPACITY (4U * UMI_TOOL_PATH_CAPACITY * 9U + 4096U)
/* Format the established version-6 Ninja Debug configure/build/test presets.
 * Validate consumed profile slots and bounded UTF-8 before JSON emission.
 * Profile values are literal: dollar signs use CMake's literal-dollar macro;
 * JSON quotes, backslashes and control bytes are escaped by Protocol.
 * The fixed binaryDir intentionally uses the sourceDir macro. Prefix values
 * retain CMake's existing list semantics; this is not a compiler/tool probe.
 * Output and optional size change only on success. Size excludes NUL. Inputs,
 * output and size storage must be disjoint and stable during the call. */
UmiStatus UmiBuildUserPresetsFormat(const UmiToolchainProfile *profile,
    char *out_json, size_t capacity, size_t *out_size);
/* Create an explicitly selected new file inside an absolute project directory.
 * dry_run 1 validates/formats and checks for an existing destination without
 * writing; it does not reserve that name. dry_run 0 uses exclusive creation,
 * so a racing file/link is never replaced. The parent must already exist.
 * A failed write/flush may leave a partial new file: preserve and inspect it.
 * There is no preset merge, tool execution or rollback. The caller owns and
 * serializes the directory; trusted parent aliases may be followed. */
UmiStatus UmiBuildUserPresetsCreate(const UmiToolchainProfile *profile,
    const char *project_root, const char *path, int dry_run);
#ifdef __cplusplus
}
#endif
#endif

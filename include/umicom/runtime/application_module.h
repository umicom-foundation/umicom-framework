/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/runtime/application_module.h
 * PURPOSE: Describe application build identities for native repository tools and Studio.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/


#ifndef UMICOM_RUNTIME_APPLICATION_MODULE_H
#define UMICOM_RUNTIME_APPLICATION_MODULE_H

#include <stddef.h>
#include "umicom/base/status.h"
#include "umicom/runtime/application_manifest.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_APPLICATION_MODULE_MAX_BYTES 65536U
#define UMI_APPLICATION_MODULE_OPTION_CAPACITY 288U

/** Build metadata accompanies the existing manifest without changing its ABI.
 * All fields are owned copies; the descriptor never grants permission to run a
 * module or proves that its executable has been built or installed. */
typedef struct UmiApplicationModuleDescriptor {
    char id[UMI_MANIFEST_TEXT_CAPACITY];
    char name[UMI_MANIFEST_TEXT_CAPACITY];
    char executable[UMI_MANIFEST_TEXT_CAPACITY];
    char native_executable[UMI_MANIFEST_TEXT_CAPACITY];
    char console_executable[UMI_MANIFEST_TEXT_CAPACITY];
    char component[UMI_MANIFEST_TEXT_CAPACITY];
    char option[UMI_APPLICATION_MODULE_OPTION_CAPACITY];
} UmiApplicationModuleDescriptor;

/** Parse a borrowed byte span using the nested application schema.
 * Checks the runtime manifest and the stricter ASCII build identity subset,
 * including duplicate identity fields. The output must not overlap input.
 * On failure, clears the caller-owned output. No tools or modules are run. */
UmiStatus umi_application_module_parse(
    const char *text, size_t length, UmiApplicationModuleDescriptor *output);

/** Validate a copied build identity without reading files or changing it.
 * Every fixed-size field must terminate within its own array. Required names,
 * portable executable spellings and the derived build option must agree with
 * the manifest parser. This checks metadata only, not frontend availability,
 * installed binaries, repository membership or permission to execute code. */
UmiStatus umi_application_module_validate(
    const UmiApplicationModuleDescriptor *module);

/** Check a portable application folder slug: 1..63 lower-case ASCII letters,
 * digits or hyphens, beginning with a letter and ending with a letter or digit.
 * Windows device names are rejected on every host so source remains portable.
 * text is borrowed and must be NUL-terminated. No filesystem operation occurs. */
UmiStatus umi_application_module_slug_validate(const char *text);

/** Inspect one absolute manifest file with the same size bound as the configure helper.
 * Clears output on failure. This reads source only; Git state and settings are
 * unchanged. The caller remains responsible for selecting a trusted file. */
UmiStatus umi_application_module_load(
    const char *path, UmiApplicationModuleDescriptor *output);

#ifdef __cplusplus
}
#endif
#endif

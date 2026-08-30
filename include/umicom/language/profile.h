/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language/profile.h
 *
 * PURPOSE:
 *   Describe editing, analysis, execution and delivery capabilities for every
 *   language understood by an Umicom development application.  The profile is
 *   provider-neutral: commands name integration points, while execution stays
 *   behind Framework services and adapters.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_LANGUAGE_PROFILE_H
#define UMICOM_LANGUAGE_PROFILE_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_LANGUAGE_PROFILE_API_VERSION 1U
#define UMI_LANGUAGE_PROFILE_CAPACITY 128U
#define UMI_LANGUAGE_PROFILE_ID_CAPACITY 64U
#define UMI_LANGUAGE_PROFILE_NAME_CAPACITY 128U
#define UMI_LANGUAGE_PROFILE_LIST_CAPACITY 512U
#define UMI_LANGUAGE_PROFILE_TOOL_CAPACITY 256U

typedef enum UmiLanguageExecutionKind {
    UMI_LANGUAGE_EXECUTION_UNKNOWN = 0,
    UMI_LANGUAGE_EXECUTION_NATIVE_COMPILED = 1,
    UMI_LANGUAGE_EXECUTION_TRANSPILED_TO_C = 2,
    UMI_LANGUAGE_EXECUTION_BYTECODE_VM = 3,
    UMI_LANGUAGE_EXECUTION_INTERPRETED = 4,
    UMI_LANGUAGE_EXECUTION_MARKUP = 5,
    UMI_LANGUAGE_EXECUTION_DATA = 6,
    UMI_LANGUAGE_EXECUTION_BUILD_CONFIGURATION = 7,
    UMI_LANGUAGE_EXECUTION_SHELL = 8
} UmiLanguageExecutionKind;

typedef uint64_t UmiLanguageCapabilityFlags;

#define UMI_LANGUAGE_CAPABILITY_EDIT             (UINT64_C(1) << 0U)
#define UMI_LANGUAGE_CAPABILITY_SYNTAX            (UINT64_C(1) << 1U)
#define UMI_LANGUAGE_CAPABILITY_SEMANTIC          (UINT64_C(1) << 2U)
#define UMI_LANGUAGE_CAPABILITY_COMPLETION        (UINT64_C(1) << 3U)
#define UMI_LANGUAGE_CAPABILITY_FORMAT            (UINT64_C(1) << 4U)
#define UMI_LANGUAGE_CAPABILITY_LINT              (UINT64_C(1) << 5U)
#define UMI_LANGUAGE_CAPABILITY_BUILD             (UINT64_C(1) << 6U)
#define UMI_LANGUAGE_CAPABILITY_RUN               (UINT64_C(1) << 7U)
#define UMI_LANGUAGE_CAPABILITY_DEBUG             (UINT64_C(1) << 8U)
#define UMI_LANGUAGE_CAPABILITY_TEST              (UINT64_C(1) << 9U)
#define UMI_LANGUAGE_CAPABILITY_PACKAGE           (UINT64_C(1) << 10U)
#define UMI_LANGUAGE_CAPABILITY_REPL              (UINT64_C(1) << 11U)
#define UMI_LANGUAGE_CAPABILITY_LANGUAGE_SERVER   (UINT64_C(1) << 12U)
#define UMI_LANGUAGE_CAPABILITY_DEBUG_ADAPTER     (UINT64_C(1) << 13U)
#define UMI_LANGUAGE_CAPABILITY_TRANSPILE_TO_C    (UINT64_C(1) << 14U)
#define UMI_LANGUAGE_CAPABILITY_MACHINE_CODE      (UINT64_C(1) << 15U)
#define UMI_LANGUAGE_CAPABILITY_DOCUMENTATION     (UINT64_C(1) << 16U)
#define UMI_LANGUAGE_CAPABILITY_PREVIEW           (UINT64_C(1) << 17U)

typedef struct UmiLanguageProfileSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[UMI_LANGUAGE_PROFILE_ID_CAPACITY];
    char name[UMI_LANGUAGE_PROFILE_NAME_CAPACITY];
    UmiLanguageExecutionKind execution_kind;
    UmiLanguageCapabilityFlags capabilities;
    char file_extensions[UMI_LANGUAGE_PROFILE_LIST_CAPACITY];
    char file_names[UMI_LANGUAGE_PROFILE_LIST_CAPACITY];
    char mime_types[UMI_LANGUAGE_PROFILE_LIST_CAPACITY];
    char default_standard[UMI_LANGUAGE_PROFILE_NAME_CAPACITY];
    char compiler_family[UMI_LANGUAGE_PROFILE_TOOL_CAPACITY];
    char language_server[UMI_LANGUAGE_PROFILE_TOOL_CAPACITY];
    char debug_adapter[UMI_LANGUAGE_PROFILE_TOOL_CAPACITY];
    char formatter[UMI_LANGUAGE_PROFILE_TOOL_CAPACITY];
    char linter[UMI_LANGUAGE_PROFILE_TOOL_CAPACITY];
    char test_runner[UMI_LANGUAGE_PROFILE_TOOL_CAPACITY];
    char package_manager[UMI_LANGUAGE_PROFILE_TOOL_CAPACITY];
    char runtime[UMI_LANGUAGE_PROFILE_TOOL_CAPACITY];
    char transpiler[UMI_LANGUAGE_PROFILE_TOOL_CAPACITY];
    int enabled;
    uint64_t revision;
} UmiLanguageProfileSnapshot;

typedef struct UmiLanguageProfileRegistrySnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    size_t profile_count;
    size_t enabled_count;
    size_t native_compiled_count;
    size_t transpiled_to_c_count;
    size_t language_server_count;
    size_t debug_adapter_count;
    uint64_t revision;
} UmiLanguageProfileRegistrySnapshot;

typedef struct UmiLanguageProfileRegistry UmiLanguageProfileRegistry;

void umi_language_profile_init(UmiLanguageProfileSnapshot *profile);
UmiStatus umi_language_profile_registry_create(
    UmiLanguageProfileRegistry **out_registry);
void umi_language_profile_registry_destroy(UmiLanguageProfileRegistry *registry);
UmiStatus umi_language_profile_registry_upsert(
    UmiLanguageProfileRegistry *registry,
    const UmiLanguageProfileSnapshot *profile);
UmiStatus umi_language_profile_registry_remove(
    UmiLanguageProfileRegistry *registry,
    const char *profile_id);
UmiStatus umi_language_profile_registry_find(
    const UmiLanguageProfileRegistry *registry,
    const char *profile_id,
    UmiLanguageProfileSnapshot *out_profile);
UmiStatus umi_language_profile_registry_find_for_path(
    const UmiLanguageProfileRegistry *registry,
    const char *path,
    UmiLanguageProfileSnapshot *out_profile);
UmiStatus umi_language_profile_registry_at(
    const UmiLanguageProfileRegistry *registry,
    size_t index,
    UmiLanguageProfileSnapshot *out_profile);
size_t umi_language_profile_registry_count(
    const UmiLanguageProfileRegistry *registry);
UmiStatus umi_language_profile_registry_snapshot(
    const UmiLanguageProfileRegistry *registry,
    UmiLanguageProfileRegistrySnapshot *out_snapshot);
int umi_language_profile_supports(
    const UmiLanguageProfileSnapshot *profile,
    UmiLanguageCapabilityFlags required_capabilities);
UmiStatus umi_language_profile_register_builtins(
    UmiLanguageProfileRegistry *registry);
size_t umi_language_profile_builtin_count(void);
const char *umi_language_execution_kind_text(UmiLanguageExecutionKind kind);

#ifdef __cplusplus
}
#endif

#endif

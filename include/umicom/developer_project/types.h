/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_project/types.h
 *
 * PURPOSE:
 *   Define stable, toolkit-neutral primitives for Framework-owned project
 *   models, language packs, generators and project templates.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_PROJECT_TYPES_H
#define UMICOM_DEVELOPER_PROJECT_TYPES_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_PROJECT_API_VERSION 1U
#define UMI_DEVELOPER_PROJECT_ID_CAPACITY 192U
#define UMI_DEVELOPER_PROJECT_NAME_CAPACITY 256U
#define UMI_DEVELOPER_PROJECT_TEXT_CAPACITY 1024U
#define UMI_DEVELOPER_PROJECT_PATH_CAPACITY 2048U
#define UMI_DEVELOPER_PROJECT_LANGUAGE_CAPACITY 32U
#define UMI_DEVELOPER_PROJECT_TEMPLATE_CAPACITY 64U
#define UMI_DEVELOPER_PROJECT_FILE_CAPACITY 256U
#define UMI_DEVELOPER_PROJECT_VARIABLE_CAPACITY 64U
#define UMI_DEVELOPER_PROJECT_VARIABLE_NAME_CAPACITY 96U
#define UMI_DEVELOPER_PROJECT_VARIABLE_VALUE_CAPACITY 512U
#define UMI_DEVELOPER_PROJECT_MODEL_CAPACITY 128U
#define UMI_DEVELOPER_PROJECT_TREE_CAPACITY 4096U
#define UMI_DEVELOPER_PROJECT_COMMAND_ARGUMENT_CAPACITY 32U

typedef enum UmiDeveloperProjectKind {
    UMI_DEVELOPER_PROJECT_EXECUTABLE = 1,
    UMI_DEVELOPER_PROJECT_STATIC_LIBRARY = 2,
    UMI_DEVELOPER_PROJECT_SHARED_LIBRARY = 3,
    UMI_DEVELOPER_PROJECT_EXTENSION = 4,
    UMI_DEVELOPER_PROJECT_SCRIPT = 5,
    UMI_DEVELOPER_PROJECT_WEB = 6,
    UMI_DEVELOPER_PROJECT_META = 7
} UmiDeveloperProjectKind;

typedef enum UmiDeveloperProjectBuildSystem {
    UMI_DEVELOPER_PROJECT_BUILD_NONE = 0,
    UMI_DEVELOPER_PROJECT_BUILD_CMAKE = 1,
    UMI_DEVELOPER_PROJECT_BUILD_ZIG = 2,
    UMI_DEVELOPER_PROJECT_BUILD_CARGO = 3,
    UMI_DEVELOPER_PROJECT_BUILD_SCRIPT = 4,
    UMI_DEVELOPER_PROJECT_BUILD_CUSTOM = 5
} UmiDeveloperProjectBuildSystem;

typedef enum UmiDeveloperProjectLanguageCapability {
    UMI_DEVELOPER_PROJECT_LANGUAGE_SYNTAX = 1U << 0,
    UMI_DEVELOPER_PROJECT_LANGUAGE_BUILD = 1U << 1,
    UMI_DEVELOPER_PROJECT_LANGUAGE_RUN = 1U << 2,
    UMI_DEVELOPER_PROJECT_LANGUAGE_DEBUG = 1U << 3,
    UMI_DEVELOPER_PROJECT_LANGUAGE_TEST = 1U << 4,
    UMI_DEVELOPER_PROJECT_LANGUAGE_FORMAT = 1U << 5,
    UMI_DEVELOPER_PROJECT_LANGUAGE_LSP = 1U << 6,
    UMI_DEVELOPER_PROJECT_LANGUAGE_PACKAGE = 1U << 7,
    UMI_DEVELOPER_PROJECT_LANGUAGE_REFACTOR = 1U << 8,
    UMI_DEVELOPER_PROJECT_LANGUAGE_SYMBOLS = 1U << 9
} UmiDeveloperProjectLanguageCapability;

typedef uint32_t UmiDeveloperProjectLanguageCapabilities;

typedef struct UmiDeveloperProjectVariable {
    char name[UMI_DEVELOPER_PROJECT_VARIABLE_NAME_CAPACITY];
    char value[UMI_DEVELOPER_PROJECT_VARIABLE_VALUE_CAPACITY];
} UmiDeveloperProjectVariable;

const char *umi_developer_project_kind_text(UmiDeveloperProjectKind kind);
const char *umi_developer_project_build_system_text(
    UmiDeveloperProjectBuildSystem build_system);

#ifdef __cplusplus
}
#endif

#endif

/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/context.h
 *
 * PURPOSE:
 *   Track the active developer context shared by project, language, debugger,
 *   source-control, test and workbench services.  The context also provides a
 *   small versioned text codec suitable for Studio session persistence.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * The context owns bounded copies of identifiers and paths.  Callers never
 * supply the revision number: Framework increments it monotonically after each
 * successful update.  The codec escapes separator characters so paths and
 * identifiers containing '%' or '|' round-trip safely.
 */
#ifndef UMICOM_DEVELOPER_CONTEXT_H
#define UMICOM_DEVELOPER_CONTEXT_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/developer/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_CONTEXT_API_VERSION 1U
#define UMI_DEVELOPER_CONTEXT_CODEC_PREFIX "UDC1"

typedef struct UmiDeveloperContextSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char project_id[UMI_DEVELOPER_ID_CAPACITY];
    char configuration_id[UMI_DEVELOPER_ID_CAPACITY];
    char target_id[UMI_DEVELOPER_ID_CAPACITY];
    char launch_profile_id[UMI_DEVELOPER_ID_CAPACITY];
    char repository_id[UMI_DEVELOPER_ID_CAPACITY];
    char test_item_id[UMI_DEVELOPER_ID_CAPACITY];
    char active_document[UMI_DEVELOPER_PATH_CAPACITY];
    char workspace_directory[UMI_DEVELOPER_PATH_CAPACITY];
    uint64_t revision;
} UmiDeveloperContextSnapshot;

typedef struct UmiDeveloperContext UmiDeveloperContext;

#define UMI_DEVELOPER_CONTEXT_PATCH_API_VERSION 1U

typedef enum UmiDeveloperContextPatchField {
    UMI_DEVELOPER_CONTEXT_PATCH_PROJECT = 1U << 0,
    UMI_DEVELOPER_CONTEXT_PATCH_CONFIGURATION = 1U << 1,
    UMI_DEVELOPER_CONTEXT_PATCH_TARGET = 1U << 2,
    UMI_DEVELOPER_CONTEXT_PATCH_LAUNCH_PROFILE = 1U << 3,
    UMI_DEVELOPER_CONTEXT_PATCH_REPOSITORY = 1U << 4,
    UMI_DEVELOPER_CONTEXT_PATCH_TEST_ITEM = 1U << 5,
    UMI_DEVELOPER_CONTEXT_PATCH_ACTIVE_DOCUMENT = 1U << 6,
    UMI_DEVELOPER_CONTEXT_PATCH_WORKSPACE_DIRECTORY = 1U << 7,
    UMI_DEVELOPER_CONTEXT_PATCH_ALL = 0xFFU
} UmiDeveloperContextPatchField;

typedef struct UmiDeveloperContextPatch {
    uint32_t struct_size;
    uint32_t api_version;
    uint32_t field_mask;
    const char *project_id;
    const char *configuration_id;
    const char *target_id;
    const char *launch_profile_id;
    const char *repository_id;
    const char *test_item_id;
    const char *active_document;
    const char *workspace_directory;
} UmiDeveloperContextPatch;


UmiStatus umi_developer_context_create(UmiDeveloperContext **out_context);
void umi_developer_context_destroy(UmiDeveloperContext *context);

UmiStatus umi_developer_context_set(
    UmiDeveloperContext *context,
    const UmiDeveloperContextSnapshot *snapshot);

UmiStatus umi_developer_context_snapshot(
    const UmiDeveloperContext *context,
    UmiDeveloperContextSnapshot *out_snapshot);

UmiStatus umi_developer_context_patch(
    UmiDeveloperContext *context,
    const UmiDeveloperContextPatch *patch,
    UmiDeveloperContextSnapshot *out_snapshot);

UmiStatus umi_developer_context_encode(
    const UmiDeveloperContext *context,
    char *out_text,
    size_t capacity,
    size_t *out_length);

UmiStatus umi_developer_context_decode(
    const char *text,
    UmiDeveloperContextSnapshot *out_snapshot);

#ifdef __cplusplus
}
#endif

#endif

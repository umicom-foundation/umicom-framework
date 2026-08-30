/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/context_channel/project.h
 *
 * PURPOSE:
 *   Define the canonical project context shared by applications and panels.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CONTEXT_CHANNEL_PROJECT_H
#define UMICOM_CONTEXT_CHANNEL_PROJECT_H
#include "umicom/context_channel/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiProjectContext {
    uint32_t structure_size;
    char project_id[UMI_CONTEXT_TEXT_CAPACITY];
    char repository_id[UMI_CONTEXT_TEXT_CAPACITY];
    char root_path[UMI_CONTEXT_PATH_CAPACITY];
    char target_id[UMI_CONTEXT_TEXT_CAPACITY];
    char configuration_id[UMI_CONTEXT_TEXT_CAPACITY];
    char language_id[64U];
    uint64_t revision;
} UmiProjectContext;
void umi_project_context_init(UmiProjectContext *context);
UmiStatus umi_project_context_validate(const UmiProjectContext *context);
UmiStatus umi_project_context_copy(UmiProjectContext *destination, const UmiProjectContext *source);
UmiStatus umi_project_context_set_project_id(UmiProjectContext *context, const char *value);
UmiStatus umi_project_context_set_repository_id(UmiProjectContext *context, const char *value);
UmiStatus umi_project_context_set_root_path(UmiProjectContext *context, const char *value);
UmiStatus umi_project_context_set_target_id(UmiProjectContext *context, const char *value);
UmiStatus umi_project_context_set_configuration_id(UmiProjectContext *context, const char *value);
UmiStatus umi_project_context_set_language_id(UmiProjectContext *context, const char *value);
#ifdef __cplusplus
}
#endif
#endif

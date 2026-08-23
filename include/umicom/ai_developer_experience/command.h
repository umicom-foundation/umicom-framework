/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_developer_experience/command.h
 *
 * PURPOSE:
 *   Define stable AI developer commands independently of toolkit action systems.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_DEVELOPER_EXPERIENCE_COMMAND_H
#define UMICOM_AI_DEVELOPER_EXPERIENCE_COMMAND_H
#include "umicom/ai_developer_experience/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAiDeveloperCommandDescriptor {
    const char *command_id;
    const char *label;
    const char *description;
    UmiAiDeveloperPaneKind target_pane;
    int requires_review;
    int requires_active_approval;
    int mutates_state;
} UmiAiDeveloperCommandDescriptor;

size_t umi_ai_developer_command_count(void);
const UmiAiDeveloperCommandDescriptor *
umi_ai_developer_command_at(size_t index);
const UmiAiDeveloperCommandDescriptor *
umi_ai_developer_command_find(const char *command_id);

#ifdef __cplusplus
}
#endif
#endif

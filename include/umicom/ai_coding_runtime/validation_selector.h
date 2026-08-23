/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_coding_runtime/validation_selector.h
 *
 * PURPOSE:
 *   Detect a repository's practical validation toolchain from marker files and
 *   build the corresponding reusable validation plan.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_CODING_RUNTIME_VALIDATION_SELECTOR_H
#define UMICOM_AI_CODING_RUNTIME_VALIDATION_SELECTOR_H

#include "umicom/ai_coding_runtime/validation_profiles/profiles.h"
#include "umicom/ai_coding_runtime/workspace.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAiCodingValidationSelection {
    char profile_id[64];
    char marker_path[256];
    int detected;
} UmiAiCodingValidationSelection;

UmiStatus umi_ai_coding_validation_select(
    const UmiAiCodingWorkspaceAdapter *workspace,
    const char *workspace_root,
    UmiAiCodingValidationSelection *out_selection,
    UmiAiCodingValidationPlan *out_plan);

#ifdef __cplusplus
}
#endif
#endif

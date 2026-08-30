/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_developer_experience/views/validation.h
 *
 * PURPOSE:
 *   Create the toolkit-neutral AI Validation view model.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_DEVELOPER_EXPERIENCE_VIEW_VALIDATION_H
#define UMICOM_AI_DEVELOPER_EXPERIENCE_VIEW_VALIDATION_H
#include "umicom/ai_developer_experience/view_helpers.h"
#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/ai_coding_runtime/types.h"
UmiStatus umi_ai_developer_validation_view_create(
    const char *view_id,
    const UmiAiCodingValidationReport *report,
    size_t visible_rows,
    UmiUiViewModel **out_view);
#ifdef __cplusplus
}
#endif
#endif

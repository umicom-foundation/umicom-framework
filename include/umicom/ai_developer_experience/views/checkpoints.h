/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_developer_experience/views/checkpoints.h
 *
 * PURPOSE:
 *   Create the toolkit-neutral AI Checkpoints view model.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_DEVELOPER_EXPERIENCE_VIEW_CHECKPOINTS_H
#define UMICOM_AI_DEVELOPER_EXPERIENCE_VIEW_CHECKPOINTS_H
#include "umicom/ai_developer_experience/view_helpers.h"
#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/ai_coding_tools/checkpoint.h"
UmiStatus umi_ai_developer_checkpoints_view_create(
    const char *view_id,
    const UmiAiCodingCheckpointStore *checkpoints,
    size_t visible_rows,
    UmiUiViewModel **out_view);
#ifdef __cplusplus
}
#endif
#endif

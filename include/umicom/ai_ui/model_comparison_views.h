/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_ui/model_comparison_views.h
 *
 * PURPOSE:
 *   Project a multi-model comparison report into a toolkit-neutral view model.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * The view contains ordinary properties. Desktop, web and test frontends can
 * render the same provider results without duplicating model-selection logic.
 */
#ifndef UMICOM_AI_UI_MODEL_COMPARISON_VIEWS_H
#define UMICOM_AI_UI_MODEL_COMPARISON_VIEWS_H

#include "umicom/ai/model_ensemble.h"
#include "umicom/ui/view_model.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_ai_ui_model_comparison_view_create(
    const char *view_id,
    const UmiAiModelEnsembleReport *report,
    UmiUiViewModel **out_view);

#ifdef __cplusplus
}
#endif

#endif

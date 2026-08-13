/* Umicom Framework toolkit-neutral Problems view. Sammy Hegab, Umicom Foundation, MIT. */
#ifndef UMICOM_DIAGNOSTIC_UI_PROBLEMS_VIEW_H
#define UMICOM_DIAGNOSTIC_UI_PROBLEMS_VIEW_H

#include "umicom/diagnostics/pipeline.h"
#include "umicom/ui/view_model.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DIAGNOSTIC_PROBLEMS_VIEW_ROW_MAX 32U

UmiStatus umi_diagnostic_problems_view_create(const char *view_id,
                                              UmiDiagnosticPipeline *pipeline,
                                              UmiUiViewModel **out_view);

#ifdef __cplusplus
}
#endif
#endif

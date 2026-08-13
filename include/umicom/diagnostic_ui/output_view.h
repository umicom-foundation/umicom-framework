/* Umicom Framework toolkit-neutral Output view. Sammy Hegab, Umicom Foundation, MIT. */
#ifndef UMICOM_DIAGNOSTIC_UI_OUTPUT_VIEW_H
#define UMICOM_DIAGNOSTIC_UI_OUTPUT_VIEW_H

#include "umicom/diagnostics/pipeline.h"
#include "umicom/ui/view_model.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DIAGNOSTIC_OUTPUT_VIEW_ROW_MAX 56U

UmiStatus umi_diagnostic_output_view_create(const char *view_id,
                                            UmiDiagnosticPipeline *pipeline,
                                            const char *channel_id,
                                            UmiUiViewModel **out_view);

#ifdef __cplusplus
}
#endif
#endif

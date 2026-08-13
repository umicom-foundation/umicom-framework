/* Umicom Framework build diagnostics projection. Sammy Hegab, Umicom Foundation, MIT. */
#ifndef UMICOM_DIAGNOSTIC_UI_BUILD_PROJECTION_H
#define UMICOM_DIAGNOSTIC_UI_BUILD_PROJECTION_H

#include "umicom/build/result.h"
#include "umicom/diagnostics/pipeline.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_diagnostic_build_result_ingest(UmiDiagnosticPipeline *pipeline,
                                             const UmiBuildResult *result,
                                             const char *source);

#ifdef __cplusplus
}
#endif
#endif

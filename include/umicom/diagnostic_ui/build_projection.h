/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/diagnostic_ui/build_projection.h
 *
 * PURPOSE:
 *   Publish the public build projection contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DIAGNOSTIC_UI_BUILD_PROJECTION_H
#define UMICOM_DIAGNOSTIC_UI_BUILD_PROJECTION_H

#include "umicom/build/result.h"
#include "umicom/diagnostics/pipeline.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the diagnostic build result ingest operation used by this module and its client
 * applications.
 */
UmiStatus umi_diagnostic_build_result_ingest(UmiDiagnosticPipeline *pipeline,
                                             const UmiBuildResult *result,
                                             const char *source);

#ifdef __cplusplus
}
#endif
#endif

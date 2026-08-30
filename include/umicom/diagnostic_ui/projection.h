/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/diagnostic_ui/projection.h
 *
 * PURPOSE:
 *   Publish the public projection contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DIAGNOSTIC_UI_PROJECTION_H
#define UMICOM_DIAGNOSTIC_UI_PROJECTION_H

#include "umicom/diagnostics/pipeline.h"
#include "umicom/ui/output_channel.h"
#include "umicom/ui/problem.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_diagnostic_ui_project_problem(const UmiDiagnosticSnapshot *diagnostic,
                                            UmiUiProblemSnapshot *out_problem);
UmiStatus umi_diagnostic_ui_project_output_channel(const UmiOutputBuffer *buffer,
                                                   const char *channel_id,
                                                   UmiUiOutputChannelSnapshot *out_channel);
UmiStatus umi_diagnostic_ui_sync_registries(UmiDiagnosticPipeline *pipeline,
                                            UmiUiProblemRegistry *problems,
                                            UmiUiOutputChannelRegistry *channels);

#ifdef __cplusplus
}
#endif
#endif

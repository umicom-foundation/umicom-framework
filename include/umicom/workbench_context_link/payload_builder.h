/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_link/payload_builder.h
 *
 * PURPOSE:
 *   Build canonical typed contexts for common Studio, Trader and shared workbench interactions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_LINK_PAYLOAD_BUILDER_H
#define UMICOM_WORKBENCH_CONTEXT_LINK_PAYLOAD_BUILDER_H

#include "umicom/workbench_context_link/types.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_workbench_context_link_build_source_location(
    UmiContextPayload *out_payload,
    const char *context_id,
    const char *application_id,
    const char *panel_id,
    const char *workspace_id,
    const char *file_path,
    uint32_t line,
    uint32_t column);
UmiStatus umi_workbench_context_link_build_instrument(
    UmiContextPayload *out_payload,
    const char *context_id,
    const char *application_id,
    const char *panel_id,
    const char *instrument_id,
    const char *symbol,
    const char *venue);
UmiStatus umi_workbench_context_link_build_project(
    UmiContextPayload *out_payload,
    const char *context_id,
    const char *application_id,
    const char *panel_id,
    const char *project_id,
    const char *root_path,
    const char *language_id);
UmiStatus umi_workbench_context_link_build_selection(
    UmiContextPayload *out_payload,
    const char *context_id,
    const char *application_id,
    const char *panel_id,
    const char *selection_id,
    const char *selection_type,
    const char *primary_id);

#ifdef __cplusplus
}
#endif

#endif

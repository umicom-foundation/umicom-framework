/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection/resolver.h
 *
 * PURPOSE:
 *   Resolve structured selections into Workbench Context Source samples without parsing display text.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_RESOLVER_H
#define UMICOM_WORKBENCH_SELECTION_RESOLVER_H

#include "umicom/workbench_selection/selection.h"
#include "umicom/workbench_context_source/sample.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_workbench_selection_resolve_source_sample(
    const UmiWorkbenchSelection *selection,
    const char *source_id,
    UmiWorkbenchContextSourceKind source_kind,
    UmiWorkbenchContextSourceTrigger trigger,
    UmiWorkbenchContextSourceSample *out_sample);

#ifdef __cplusplus
}
#endif
#endif

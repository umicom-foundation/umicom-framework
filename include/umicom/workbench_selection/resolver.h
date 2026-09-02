/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection/resolver.h
 *
 * PURPOSE:
 *   Resolve structured selections into Workbench Context Source samples without parsing display text.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_RESOLVER_H
#define UMICOM_WORKBENCH_SELECTION_RESOLVER_H

#include "umicom/workbench_selection/selection.h"
#include "umicom/workbench_context_source/sample.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the workbench selection resolve source sample operation used by this module and
 * its client applications.
 */
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

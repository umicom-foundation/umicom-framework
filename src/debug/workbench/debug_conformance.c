/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug/workbench/debug_conformance.c
 *
 * PURPOSE:
 *   Evaluate required debugger-workbench capabilities for frontend conformance.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral capability orchestrates canonical Debug Service/DAP
 *   runtime state; Studio remains a thin frontend and owns no reusable debug
 *   semantics, adapter protocol, breakpoint engine or inspection engine.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/debug/workbench/debug_conformance.h"
#include <string.h>

/*
 * Initialise debug workbench debug conformance from caller-provided values so later
 * operations receive a known state.
 */
void umi_debug_workbench_debug_conformance_init(UmiDebugWorkbenchDebugConformance *model, uint64_t required)
{ /* Protect caller-owned memory by checking that required state is available before it is used. */ if (model != NULL) { memset(model, 0, sizeof *model); model->required = required; model->revision = 1U; } }
/*
 * Provide the debug workbench debug conformance set available operation used by this
 * module and its client applications.
 */
UmiStatus umi_debug_workbench_debug_conformance_set_available(UmiDebugWorkbenchDebugConformance *model, uint64_t capability, bool available)
{ /* Protect caller-owned memory by checking that required state is available before it is used. */ if (model == NULL || capability == 0U) return UMI_STATUS_INVALID_ARGUMENT; /* Protect caller-owned memory by checking that required state is available before it is used. */ if (available) model->available |= capability; /* Use this fallback path when the earlier condition does not apply. */ else model->available &= ~capability; model->revision++; return UMI_STATUS_OK; }
/*
 * Provide the debug workbench debug conformance missing operation used by this module and
 * its client applications.
 */
uint64_t umi_debug_workbench_debug_conformance_missing(const UmiDebugWorkbenchDebugConformance *model)
{ return model == NULL ? UINT64_MAX : model->required & ~model->available; }
/*
 * Provide the debug workbench debug conformance passes operation used by this module and
 * its client applications.
 */
int umi_debug_workbench_debug_conformance_passes(const UmiDebugWorkbenchDebugConformance *model)
{ return model != NULL && model->revision > 0U && umi_debug_workbench_debug_conformance_missing(model) == 0U; }

/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/helix/helix.h
 *
 * PURPOSE:
 *   Aggregate the public Umicom Helix governance and evolutionary-workflow contracts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This umbrella header exposes Helix as a governed workflow platform rather than a single unrestricted autonomous agent.
 */

#ifndef INCLUDE_UMICOM_HELIX_HELIX_H
#define INCLUDE_UMICOM_HELIX_HELIX_H

#include "umicom/helix/types.h"
#include "umicom/helix/evidence.h"
#include "umicom/helix/observation.h"
#include "umicom/helix/diagnosis.h"
#include "umicom/helix/plan.h"
#include "umicom/helix/candidate.h"
#include "umicom/helix/fitness.h"
#include "umicom/helix/generation.h"
#include "umicom/helix/provenance.h"
#include "umicom/helix/policy.h"
#include "umicom/helix/approval.h"
#include "umicom/helix/rollback.h"
#include "umicom/helix/memory.h"
#include "umicom/helix/agent.h"
#include "umicom/helix/agent_registry.h"
#include "umicom/helix/workflow.h"
#include "umicom/helix/experiment.h"
#include "umicom/helix/mutation.h"
#include "umicom/helix/release_gate.h"
#include "umicom/helix/runtime.h" 

/* Helix Agent Runtime v2: bounded specialist agents, exact human approval
 * gates, deterministic state, evidence journalling and controlled rollback. */
#include "umicom/helix/execution_types.h"
#include "umicom/helix/action.h"
#include "umicom/helix/operation.h"
#include "umicom/helix/agent_runtime.h"
#include "umicom/helix/approval_gate.h"
#include "umicom/helix/journal.h"
#include "umicom/helix/execution_adapter.h"
#include "umicom/helix/retry_loop.h"
#include "umicom/helix/candidate_set.h"
#include "umicom/helix/compensation_plan.h"
#include "umicom/helix/candidate_provenance.h"
#include "umicom/helix/orchestrator.h"
#include "umicom/helix/helix_ui.h"

#endif

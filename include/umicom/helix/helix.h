/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/helix/helix.h
 *
 * PURPOSE:
 *   Aggregate the public Umicom Helix governance and evolutionary-workflow contracts.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
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
#include "umicom/helix/v2_types.h"
#include "umicom/helix/action_v2.h"
#include "umicom/helix/operation_v2.h"
#include "umicom/helix/agent_runtime_v2.h"
#include "umicom/helix/approval_gate_v2.h"
#include "umicom/helix/journal_v2.h"
#include "umicom/helix/execution_adapter_v2.h"
#include "umicom/helix/retry_loop_v2.h"
#include "umicom/helix/candidate_set_v2.h"
#include "umicom/helix/rollback_v2.h"
#include "umicom/helix/provenance_v2.h"
#include "umicom/helix/orchestrator_v2.h"
#include "umicom/helix/helix_ui_v2.h"

#endif

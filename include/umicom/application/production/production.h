/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/production/production.h
 *
 * PURPOSE:
 *   Provide the complete Framework-owned application production control-plane
 *   API from one public C23 umbrella header.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_PRODUCTION_PRODUCTION_H
#define UMICOM_APPLICATION_PRODUCTION_PRODUCTION_H

#include "umicom/application/production/types.h"
#include "umicom/application/production/identifier.h"
#include "umicom/application/production/application_binding.h"
#include "umicom/application/production/panel_binding.h"
#include "umicom/application/production/layout_binding.h"
#include "umicom/application/production/feature_binding.h"
#include "umicom/application/production/command_binding.h"
#include "umicom/application/production/registry.h"
#include "umicom/application/production/capability_requirement.h"
#include "umicom/application/production/capability_probe.h"
#include "umicom/application/production/capability_readiness.h"
#include "umicom/application/production/readiness_report.h"
#include "umicom/application/production/workspace_checkpoint.h"
#include "umicom/application/production/checkpoint_store.h"
#include "umicom/application/production/workspace_recovery.h"
#include "umicom/application/production/manifest_contract.h"
#include "umicom/application/production/manifest_snapshot.h"
#include "umicom/application/production/manifest_drift.h"
#include "umicom/application/production/evidence_requirement.h"
#include "umicom/application/production/evidence_record.h"
#include "umicom/application/production/evidence_registry.h"
#include "umicom/application/production/acceptance_rule.h"
#include "umicom/application/production/acceptance_report.h"
#include "umicom/application/production/lifecycle_gate.h"
#include "umicom/application/production/launch_stage.h"
#include "umicom/application/production/launch_plan.h"
#include "umicom/application/production/runtime.h"
#include "umicom/application/production/portfolio.h"
#include "umicom/application/production/portfolio_report.h"
#include "umicom/application/production/diagnostic_entry.h"
#include "umicom/application/production/diagnostic_report.h"
#include "umicom/application/production/command_catalogue.h"
#include "umicom/application/production/control_plane.h"
#include "umicom/application/production/audit.h"

#endif


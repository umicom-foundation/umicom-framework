/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/codeguard/codeguard.h
 *
 * PURPOSE:
 *   Provide the public umbrella include for the complete CodeGuard API.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This file is intentionally small and focused. CodeGuard separates scanning
 * rules, analysis engines and reports so new developers can understand one
 * responsibility at a time and test it independently.
 */
#ifndef UMICOM_CODEGUARD_CODEGUARD_H
#define UMICOM_CODEGUARD_CODEGUARD_H
#include "umicom/codeguard/severity.h"
#include "umicom/codeguard/category.h"
#include "umicom/codeguard/finding.h"
#include "umicom/codeguard/rule.h"
#include "umicom/codeguard/rule_registry.h"
#include "umicom/codeguard/result.h"
#include "umicom/codeguard/profile.h"
#include "umicom/codeguard/config.h"
#include "umicom/codeguard/summary.h"
#include "umicom/codeguard/source.h"
#include "umicom/codeguard/lifetime.h"
#include "umicom/codeguard/normalizer.h"
#include "umicom/codeguard/duplicate.h"
#include "umicom/codeguard/architecture.h"
#include "umicom/codeguard/scanner.h"
#include "umicom/codeguard/report.h"
#include "umicom/codeguard/suppression.h"
#include "umicom/codeguard/quality_gate.h"
#include "umicom/codeguard/external_tool.h"
#include "umicom/codeguard/dependency_audit.h"
#include "umicom/codeguard/service.h"
#include "umicom/codeguard/quality_types.h"
#include "umicom/codeguard/evidence.h"
#include "umicom/codeguard/architecture_policy.h"
#include "umicom/codeguard/unsafe_api.h"
#include "umicom/codeguard/memory_audit.h"
#include "umicom/codeguard/portability_audit.h"
#include "umicom/codeguard/duplicate_evidence.h"
#include "umicom/codeguard/api_baseline.h"
#include "umicom/codeguard/quality_evaluation.h"
#include "umicom/codeguard/remediation_plan.h"
#include "umicom/codeguard/quality_session.h"
#include "umicom/codeguard/quality_catalogue.h"
#endif

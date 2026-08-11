/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/codeguard/codeguard.h
 *
 * PURPOSE:
 *   Provide the public umbrella include for the complete CodeGuard API.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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
#include "umicom/codeguard/service.h"
#endif

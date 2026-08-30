/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/diagnostics/diagnostics.h
 *
 * PURPOSE:
 *   Aggregate the reusable diagnostics, Problems, policy, suppression,
 *   baseline, SARIF and observability contracts exposed by Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DIAGNOSTICS_DIAGNOSTICS_H
#define UMICOM_DIAGNOSTICS_DIAGNOSTICS_H

#include "umicom/diagnostics/diagnostic.h"
#include "umicom/diagnostics/log.h"
#include "umicom/diagnostics/hub.h"
#include "umicom/diagnostics/store.h"
#include "umicom/diagnostics/model.h"
#include "umicom/diagnostics/filter.h"
#include "umicom/diagnostics/query.h"
#include "umicom/diagnostics/output.h"
#include "umicom/diagnostics/output_buffer.h"
#include "umicom/diagnostics/output_filter.h"
#include "umicom/diagnostics/parser.h"
#include "umicom/diagnostics/compiler_parser.h"
#include "umicom/diagnostics/runtime_parser.h"
#include "umicom/diagnostics/event.h"
#include "umicom/diagnostics/pipeline.h"
#include "umicom/diagnostics/metrics.h"
#include "umicom/diagnostics/trace.h"
#include "umicom/diagnostics/audit.h"
#include "umicom/diagnostics/profiler.h"
#include "umicom/diagnostics/readiness.h"
#include "umicom/diagnostics/event_log.h"
#include "umicom/diagnostics/privacy_filter.h"
#include "umicom/diagnostics/crash_report.h"
#include "umicom/diagnostics/exporter.h"
#include "umicom/diagnostics/snapshot.h"

/* Professional diagnostics, Problems and SARIF platform. */
#include "umicom/diagnostics/diagnostic_provider.h"
#include "umicom/diagnostics/severity_policy.h"
#include "umicom/diagnostics/suppression.h"
#include "umicom/diagnostics/baseline.h"
#include "umicom/diagnostics/problem_model.h"
#include "umicom/diagnostics/sarif.h"
#include "umicom/diagnostics/orchestration.h"
#include "umicom/diagnostics/command.h"

#endif /* UMICOM_DIAGNOSTICS_DIAGNOSTICS_H */

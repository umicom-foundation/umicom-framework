/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_platform/test_platform.h
 *
 * PURPOSE:
 *   Aggregate the complete Umicom test-platform API.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This module uses a small, explicit C API and bounded storage.  The public
 * contract does not expose toolkit objects, C++ types, or private structures.
 */
#ifndef FRAMEWORK_INCLUDE_UMICOM_TEST_PLATFORM_TEST_PLATFORM_H
#define FRAMEWORK_INCLUDE_UMICOM_TEST_PLATFORM_TEST_PLATFORM_H

#include "umicom/test_platform/item.h"
#include "umicom/test_platform/suite.h"
#include "umicom/test_platform/run_profile.h"
#include "umicom/test_platform/run_session.h"
#include "umicom/test_platform/result.h"
#include "umicom/test_platform/output.h"
#include "umicom/test_platform/coverage.h"
#include "umicom/test_platform/discovery.h"
#include "umicom/test_platform/attachment.h"
#include "umicom/test_platform/benchmark.h"
#include "umicom/test_platform/filter.h"
#include "umicom/test_platform/hierarchy.h"
#include "umicom/test_platform/history.h"
#include "umicom/test_platform/operation.h"
#include "umicom/test_platform/artifact_contract.h"
#include "umicom/test_platform/ctest.h"
#include "umicom/test_platform/structured_report.h"
#include "umicom/test_platform/execution.h"
#include "umicom/test_platform/service.h"
#include "umicom/test_platform/workspace.h"

/* Professional Test Explorer, rerun, coverage, benchmark and history layer. */
#include "umicom/test_platform/provider_registry.h"
#include "umicom/test_platform/explorer_model.h"
#include "umicom/test_platform/rerun_policy.h"
#include "umicom/test_platform/coverage_analysis.h"
#include "umicom/test_platform/benchmark_analysis.h"
#include "umicom/test_platform/history_analysis.h"
#include "umicom/test_platform/explorer_session.h"
#include "umicom/test_platform/command.h"

#endif

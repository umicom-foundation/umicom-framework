/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_context_host/test_support.h
 *
 * PURPOSE:
 *   Provide reusable fixtures for Workbench Context Host regression tests.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_HOST_TEST_SUPPORT_H
#define UMICOM_WORKBENCH_CONTEXT_HOST_TEST_SUPPORT_H

#include "umicom/workbench_context_host/workbench_context_host.h"

/**
 * Represent the workbench context host test fixture data shared with callers of this
 * public contract.
 */
typedef struct UmiWorkbenchContextHostTestFixture {
    UmiWorkbenchContextLinkSlaveController link_controller;
    UmiWorkbenchContextHost *host;
    UmiWorkbenchContextHostProfile profile;
} UmiWorkbenchContextHostTestFixture;

/**
 * Initialise wch test fixture from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_wch_test_fixture_init(UmiWorkbenchContextHostTestFixture *fixture);
/**
 * Release or reset state held by wch test fixture so the same storage can be reused
 * safely.
 */
void umi_wch_test_fixture_destroy(UmiWorkbenchContextHostTestFixture *fixture);
/**
 * Exercise wch test selection payload and return a clear result when the behaviour no
 * longer matches its contract.
 */
UmiStatus umi_wch_test_selection_payload(
    UmiContextPayload *out_payload,
    const char *context_id,
    const char *source_panel_id);
/**
 * Exercise wch test project payload and return a clear result when the behaviour no longer
 * matches its contract.
 */
UmiStatus umi_wch_test_project_payload(
    UmiContextPayload *out_payload,
    const char *context_id,
    const char *source_panel_id);

#endif

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

typedef struct UmiWorkbenchContextHostTestFixture {
    UmiWorkbenchContextLinkSlaveController link_controller;
    UmiWorkbenchContextHost *host;
    UmiWorkbenchContextHostProfile profile;
} UmiWorkbenchContextHostTestFixture;

UmiStatus umi_wch_test_fixture_init(UmiWorkbenchContextHostTestFixture *fixture);
void umi_wch_test_fixture_destroy(UmiWorkbenchContextHostTestFixture *fixture);
UmiStatus umi_wch_test_selection_payload(
    UmiContextPayload *out_payload,
    const char *context_id,
    const char *source_panel_id);
UmiStatus umi_wch_test_project_payload(
    UmiContextPayload *out_payload,
    const char *context_id,
    const char *source_panel_id);

#endif

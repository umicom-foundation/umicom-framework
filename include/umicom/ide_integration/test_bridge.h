/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ide_integration/test_bridge.h
 *
 * PURPOSE:
 *   Project Test Platform items and results into editor navigation and AI repair
 *   context while preserving Test Platform ownership.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_IDE_INTEGRATION_TEST_BRIDGE_H
#define UMICOM_IDE_INTEGRATION_TEST_BRIDGE_H
#include "umicom/test_platform/service.h"
#include "umicom/ide_integration/location.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the ide test item target operation used by this module and its client
 * applications.
 */
UmiStatus umi_ide_test_item_target(
    UmiTestPlatformService *tests,
    const char *item_id,
    UmiIdeNavigationTarget *out_target);

/**
 * Provide the ide test result summary operation used by this module and its client
 * applications.
 */
UmiStatus umi_ide_test_result_summary(
    UmiTestPlatformService *tests,
    const char *result_id,
    char *out_text,
    size_t capacity);

#ifdef __cplusplus
}
#endif
#endif

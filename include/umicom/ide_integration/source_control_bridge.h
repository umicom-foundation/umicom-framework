/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ide_integration/source_control_bridge.h
 *
 * PURPOSE:
 *   Normalize source-control file selection and controller status for editor and
 *   AI workflows.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_IDE_INTEGRATION_SOURCE_CONTROL_BRIDGE_H
#define UMICOM_IDE_INTEGRATION_SOURCE_CONTROL_BRIDGE_H
#include "umicom/developer_productivity/source_control.h"
#include "umicom/ide_integration/location.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the ide source control target operation used by this module and its client
 * applications.
 */
UmiStatus umi_ide_source_control_target(
    const char *path,
    uint32_t line,
    UmiIdeNavigationTarget *out_target);

/**
 * Provide the ide source control summary operation used by this module and its client
 * applications.
 */
UmiStatus umi_ide_source_control_summary(
    const UmiDeveloperSourceControl *source_control,
    char *out_text,
    size_t capacity);

#ifdef __cplusplus
}
#endif
#endif

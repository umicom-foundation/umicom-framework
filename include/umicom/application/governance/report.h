/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/governance/report.h
 *
 * PURPOSE:
 *   Render deterministic text, Markdown or JSON component inventories for
 *   developers, CI pipelines, documentation and future Studio dashboards.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_GOVERNANCE_REPORT_H
#define UMICOM_APPLICATION_GOVERNANCE_REPORT_H

#include "umicom/application/governance/release_gate.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * List the named component report format values accepted by this public contract.
 */
typedef enum UmiComponentReportFormat {
  UMI_COMPONENT_REPORT_TEXT = 1,
  UMI_COMPONENT_REPORT_MARKDOWN = 2,
  UMI_COMPONENT_REPORT_JSON = 3
} UmiComponentReportFormat;

/**
 * Write component inventory report in its stable representation and report capacity or
 * input failures to the caller.
 */
UmiStatus umi_component_inventory_report_write(const UmiComponentInventory *inventory,
                                               const UmiComponentQuery *query,
                                               UmiComponentReportFormat format, char *output,
                                               size_t output_capacity,
                                               size_t *out_required_capacity);

#ifdef __cplusplus
}
#endif

#endif

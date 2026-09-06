/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/productisation/surface_audit.h
 *
 * PURPOSE:
 *   Report whether every portfolio application has the metadata needed for a
 *   visible user interface, a default layout and a safe start entry point.
 *
 * ARCHITECTURE:
 *   The audit joins the portfolio, experience and presentation catalogues. It
 *   does not launch a process and it does not inspect private credentials. A
 *   launcher can use the result before offering an application to the user.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_PRODUCTISATION_SURFACE_AUDIT_H
#define UMICOM_APPLICATION_PRODUCTISATION_SURFACE_AUDIT_H

#include <stdbool.h>
#include <stddef.h>

#include "umicom/application/productisation/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Evidence for one application in the Framework-owned portfolio audit. */
typedef struct UmiProductApplicationSurfaceAuditEntry {
    char application_id[UMI_PRODUCTISATION_ID_CAPACITY];
    char display_name[UMI_PRODUCTISATION_ID_CAPACITY];
    char executable_name[UMI_PRODUCTISATION_ID_CAPACITY];
    char default_layout_id[UMI_PRODUCTISATION_ID_CAPACITY];
    char presentation_layout_id[UMI_PRODUCTISATION_ID_CAPACITY];
    bool experience_available;
    bool presentation_available;
    bool layout_available;
    bool user_interface_ready;
    bool start_entrypoint_ready;
    bool launch_surface_ready;
    UmiStatus status;
} UmiProductApplicationSurfaceAuditEntry;

/** Bounded portfolio report consumed by Desk, installers and documentation tools. */
typedef struct UmiProductPortfolioSurfaceAudit {
    UmiProductApplicationSurfaceAuditEntry
        applications[UMI_PRODUCTISATION_MAX_APPLICATIONS];
    size_t application_count;
    size_t experience_count;
    size_t presentation_count;
    size_t layout_ready_count;
    size_t user_interface_ready_count;
    size_t start_entrypoint_ready_count;
    size_t launch_surface_ready_count;
    size_t issue_count;
} UmiProductPortfolioSurfaceAudit;

/**
 * Build a deterministic report for every portfolio application.
 *
 * Missing metadata is recorded in the row rather than hidden by an early
 * return, so a caller can show every outstanding issue in one screen.
 */
UmiStatus umi_product_portfolio_surface_audit_build(
    UmiProductPortfolioSurfaceAudit *out_audit);

/** Find one audit row by its exact portfolio application identifier. */
const UmiProductApplicationSurfaceAuditEntry *
umi_product_portfolio_surface_audit_find(
    const UmiProductPortfolioSurfaceAudit *audit,
    const char *application_id);

#ifdef __cplusplus
}
#endif

#endif

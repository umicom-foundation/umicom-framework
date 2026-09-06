/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/productisation/surface_audit.c
 *
 * PURPOSE:
 *   Implement the portfolio-wide surface audit used to prove that each
 *   application has a visible layout and a declared start entry point.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/productisation/surface_audit.h"

#include <string.h>

#include "umicom/application/experience_catalogue.h"
#include "umicom/application/portfolio.h"
#include "umicom/application/presentation.h"

/* Copy a bounded catalogue string without allowing truncation. */
static UmiStatus copy_text(char *destination, size_t capacity,
                           const char *source)
{
    size_t length;
    if (destination == NULL || capacity == 0U || source == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    length = strlen(source);
    if (length >= capacity)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)memcpy(destination, source, length + 1U);
    return UMI_STATUS_OK;
}

/* Record the first missing contract so diagnostics explain the earliest cause. */
static UmiStatus first_issue(const UmiProductApplicationSurfaceAuditEntry *entry)
{
    if (!entry->experience_available || !entry->presentation_available)
        return UMI_STATUS_NOT_FOUND;
    if (!entry->layout_available || !entry->user_interface_ready)
        return UMI_STATUS_INVALID_STATE;
    if (!entry->start_entrypoint_ready)
        return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}

/* Build one row from the three authoritative Framework catalogues. */
static UmiStatus build_entry(const UmiApplicationDefinition *portfolio,
                             UmiProductApplicationSurfaceAuditEntry *entry)
{
    const UmiApplicationExperienceDefinition *experience;
    const UmiApplicationPresentation *presentation;
    UmiStatus copy_status;

    if (portfolio == NULL || entry == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(entry, 0, sizeof(*entry));

    copy_status = copy_text(entry->application_id,
                            sizeof(entry->application_id),
                            portfolio->application_id);
    if (copy_status != UMI_STATUS_OK)
        return copy_status;
    copy_status = copy_text(entry->display_name, sizeof(entry->display_name),
                            portfolio->display_name);
    if (copy_status != UMI_STATUS_OK)
        return copy_status;
    copy_status = copy_text(entry->executable_name,
                            sizeof(entry->executable_name),
                            portfolio->executable_name);
    if (copy_status != UMI_STATUS_OK)
        return copy_status;

    /* The catalogue resolves supported historical IDs to one canonical experience. */
    experience = umi_application_experience_catalogue_find(
        portfolio->application_id);
    presentation = umi_application_presentation_find(
        portfolio->application_id);
    entry->experience_available = experience != NULL;
    entry->presentation_available = presentation != NULL;
    entry->start_entrypoint_ready = portfolio->executable_name != NULL &&
        portfolio->executable_name[0] != '\0';

    if (experience != NULL) {
        copy_status = copy_text(entry->default_layout_id,
                                sizeof(entry->default_layout_id),
                                experience->default_layout_id);
        if (copy_status != UMI_STATUS_OK)
            return copy_status;
        entry->layout_available =
            experience->layout_count > 0U &&
            umi_application_experience_layout_find(
                experience, experience->default_layout_id) != NULL;
        entry->user_interface_ready = entry->layout_available &&
            experience->panel_count > 0U;
    }
    if (presentation != NULL) {
        copy_status = copy_text(entry->presentation_layout_id,
                                sizeof(entry->presentation_layout_id),
                                presentation->default_layout_id);
        if (copy_status != UMI_STATUS_OK)
            return copy_status;
    }
    entry->launch_surface_ready = entry->experience_available &&
        entry->presentation_available && entry->user_interface_ready &&
        entry->start_entrypoint_ready;
    entry->status = first_issue(entry);
    return UMI_STATUS_OK;
}

/* Build every bounded row while preserving issues for the caller to review. */
UmiStatus umi_product_portfolio_surface_audit_build(
    UmiProductPortfolioSurfaceAudit *out_audit)
{
    size_t index;
    if (out_audit == NULL ||
        umi_application_portfolio_count() > UMI_PRODUCTISATION_MAX_APPLICATIONS)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (umi_application_experience_catalogue_validate() != UMI_STATUS_OK ||
        umi_application_presentation_catalogue_validate() != UMI_STATUS_OK)
        return UMI_STATUS_INVALID_STATE;

    (void)memset(out_audit, 0, sizeof(*out_audit));
    out_audit->experience_count = umi_application_experience_catalogue_count();
    out_audit->presentation_count = umi_application_presentation_count();
    for (index = 0U; index < umi_application_portfolio_count(); ++index) {
        UmiProductApplicationSurfaceAuditEntry *entry =
            &out_audit->applications[out_audit->application_count];
        UmiStatus status = build_entry(
            umi_application_portfolio_at(index), entry);
        if (status != UMI_STATUS_OK)
            return status;
        out_audit->application_count += 1U;
        if (entry->layout_available) out_audit->layout_ready_count += 1U;
        if (entry->user_interface_ready)
            out_audit->user_interface_ready_count += 1U;
        if (entry->start_entrypoint_ready)
            out_audit->start_entrypoint_ready_count += 1U;
        if (entry->launch_surface_ready)
            out_audit->launch_surface_ready_count += 1U;
        if (entry->status != UMI_STATUS_OK)
            out_audit->issue_count += 1U;
    }
    return UMI_STATUS_OK;
}

/* Find an existing row without exposing mutable catalogue storage. */
const UmiProductApplicationSurfaceAuditEntry *
umi_product_portfolio_surface_audit_find(
    const UmiProductPortfolioSurfaceAudit *audit,
    const char *application_id)
{
    size_t index;
    if (audit == NULL || application_id == NULL)
        return NULL;
    for (index = 0U; index < audit->application_count; ++index) {
        if (strcmp(audit->applications[index].application_id,
                   application_id) == 0)
            return &audit->applications[index];
    }
    return NULL;
}

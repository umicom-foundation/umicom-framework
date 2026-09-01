/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/productisation/adoption.c
 *
 * PURPOSE:
 *   Validate thin application contributions and calculate adoption evidence
 *   from canonical experience and Framework component catalogues.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/productisation/adoption.h"

#include <string.h>

#include "umicom/application/experience_catalogue.h"

/* Copy text into fixed snapshot storage and reject truncation explicitly. */
static UmiStatus copy_text(char *destination, size_t capacity,
                           const char *source)
{
    size_t length;
    if (destination == NULL || capacity == 0U || source == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    length = strlen(source);
    if (length == 0U || length + 1U > capacity)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)memcpy(destination, source, length + 1U);
    return UMI_STATUS_OK;
}

/* Confirm that the application contribution is complete and names the same
 * product identity as its immutable Framework experience. */
UmiStatus umi_product_application_adoption_validate(
    const UmiProductApplicationAdoption *adoption)
{
    const UmiApplicationExperienceDefinition *experience;
    /* All identity and availability fields are read before any catalogue work,
     * so malformed application data cannot reach deeper Framework services. */
    if (adoption == NULL ||
        adoption->structure_size != sizeof(*adoption) ||
        adoption->module_id == NULL || adoption->module_id[0] == '\0' ||
        adoption->application_id == NULL || adoption->application_id[0] == '\0' ||
        adoption->display_name == NULL || adoption->display_name[0] == '\0' ||
        adoption->executable_id == NULL || adoption->executable_id[0] == '\0' ||
        adoption->frontend_flags == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    experience = umi_application_experience_catalogue_find(
        adoption->application_id);
    /* An application without a canonical experience cannot borrow Framework
     * panels, layouts or features and is therefore not a valid contribution. */
    if (experience == NULL) return UMI_STATUS_NOT_FOUND;
    return strcmp(experience->display_name, adoption->display_name) == 0
        ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}

/* Combine application-supplied evidence with canonical layout and component
 * evidence to produce one truthful, value-only readiness snapshot. */
UmiStatus umi_product_application_adoption_snapshot(
    const UmiProductApplicationAdoption *adoption,
    UmiProductApplicationAdoptionSnapshot *out_snapshot)
{
    const UmiApplicationExperienceDefinition *experience;
    UmiApplicationSuiteLayoutRuntime layout_runtime;
    size_t panel_index;
    size_t layout_index;
    UmiStatus status;

    /* A destination is required before validation because every successful
     * path writes a complete snapshot rather than returning borrowed state. */
    if (out_snapshot == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_product_application_adoption_validate(adoption);
    if (status != UMI_STATUS_OK) return status;
    experience = umi_application_experience_catalogue_find(
        adoption->application_id);
    if (experience == NULL) return UMI_STATUS_NOT_FOUND;

    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->structure_size = sizeof(*out_snapshot);
    status = copy_text(out_snapshot->module_id,
                       sizeof(out_snapshot->module_id), adoption->module_id);
    if (status != UMI_STATUS_OK) return status;
    status = copy_text(out_snapshot->application_id,
                       sizeof(out_snapshot->application_id),
                       adoption->application_id);
    if (status != UMI_STATUS_OK) return status;
    status = copy_text(out_snapshot->display_name,
                       sizeof(out_snapshot->display_name),
                       adoption->display_name);
    if (status != UMI_STATUS_OK) return status;
    status = copy_text(out_snapshot->executable_id,
                       sizeof(out_snapshot->executable_id),
                       adoption->executable_id);
    if (status != UMI_STATUS_OK) return status;

    out_snapshot->frontend_flags = adoption->frontend_flags;
    out_snapshot->manifest_available = adoption->manifest_available != 0;
    out_snapshot->canonical_experience_available = 1;
    out_snapshot->feature_count = experience->feature_count;
    out_snapshot->panel_count = experience->panel_count;
    out_snapshot->layout_count = experience->layout_count;
    status = umi_product_application_adoption_layout_load(
        adoption, &layout_runtime);
    if (status != UMI_STATUS_OK) return status;
    out_snapshot->layout_runtime_ready = layout_runtime.loaded;
    out_snapshot->default_layout_window_count =
        layout_runtime.active_layout.window_count;
    /* Select every layout through the real shared runtime. Merely declaring a
     * layout is insufficient evidence that it can be projected safely. */
    for (layout_index = 0U; layout_index < experience->layout_count;
         ++layout_index) {
        status = umi_application_suite_layout_runtime_select(
            &layout_runtime, experience->layouts[layout_index].layout_id);
        if (status != UMI_STATUS_OK) return status;
        out_snapshot->projected_layout_count += 1U;
        out_snapshot->projected_window_count +=
            layout_runtime.active_layout.window_count;
    }
    out_snapshot->layout_projection_complete =
        out_snapshot->layout_runtime_ready &&
        out_snapshot->projected_layout_count == out_snapshot->layout_count &&
        out_snapshot->default_layout_window_count > 0U;
    /* Each visible panel must resolve through a reusable component capability;
     * this keeps presentation logic out of the thin application repository. */
    for (panel_index = 0U; panel_index < experience->panel_count;
         ++panel_index) {
        if (umi_application_component_capability_count(
                experience->panels[panel_index].required_capability) > 0U)
            out_snapshot->covered_surface_count += 1U;
        else
            out_snapshot->missing_surface_count += 1U;
    }
    out_snapshot->surface_complete =
        out_snapshot->missing_surface_count == 0U;
    status = umi_application_module_status_init(
        experience, adoption->composition_available,
        adoption->executable_available, adoption->tests_available,
        &out_snapshot->module_status);
    if (status != UMI_STATUS_OK) return status;
    out_snapshot->runnable = umi_application_module_status_runnable(
        &out_snapshot->module_status);
    /* Acceptance is deliberately conjunctive: a runnable executable cannot
     * compensate for missing layouts, components, manifest or test evidence. */
    out_snapshot->acceptance_ready =
        out_snapshot->manifest_available &&
        out_snapshot->layout_projection_complete &&
        out_snapshot->surface_complete &&
        out_snapshot->runnable && out_snapshot->module_status.tests_available;
    return UMI_STATUS_OK;
}

/* Initialise and load the canonical default layout for one contribution. */
UmiStatus umi_product_application_adoption_layout_load(
    const UmiProductApplicationAdoption *adoption,
    UmiApplicationSuiteLayoutRuntime *out_runtime)
{
    UmiStatus status;
    if (out_runtime == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_product_application_adoption_validate(adoption);
    if (status != UMI_STATUS_OK) return status;
    umi_application_suite_layout_runtime_init(out_runtime);
    return umi_application_suite_layout_runtime_load(
        out_runtime, adoption->application_id);
}

/* Provide a null-safe convenience check for launchers and release gates. */
int umi_product_application_adoption_snapshot_accepted(
    const UmiProductApplicationAdoptionSnapshot *snapshot)
{
    return snapshot != NULL && snapshot->acceptance_ready;
}

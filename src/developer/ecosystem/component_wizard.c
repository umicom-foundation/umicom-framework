/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer/ecosystem/component_wizard.c
 * PURPOSE: Implement toolkit-neutral component wizard and file-plan generation.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/ecosystem/component_wizard.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>

/*
 * Initialise ecosystem component template from caller-provided values so later operations
 * receive a known state.
 */
void umi_ecosystem_component_template_init(
    UmiEcosystemComponentTemplate *component_template,
    const char *template_id,
    const char *display_name,
    UmiEcosystemComponentKind kind)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (component_template == NULL) return;
    (void)memset(component_template, 0, sizeof(*component_template));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (template_id != NULL) {
        (void)umi_ecosystem_copy_text(component_template->template_id,
            sizeof(component_template->template_id), template_id);
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (display_name != NULL) {
        (void)umi_ecosystem_copy_text(component_template->display_name,
            sizeof(component_template->display_name), display_name);
    }
    component_template->kind = kind;
    component_template->generates_header = true;
    component_template->generates_source = true;
    component_template->generates_test = true;
    component_template->generates_manifest = true;
    component_template->toolkit_neutral = true;
}

/*
 * Initialise ecosystem component wizard from caller-provided values so later operations
 * receive a known state.
 */
void umi_ecosystem_component_wizard_init(UmiEcosystemComponentWizard *wizard)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (wizard == NULL) return;
    (void)memset(wizard, 0, sizeof(*wizard));
    wizard->revision = 1U;
}

/*
 * Provide the ecosystem component wizard add template operation used by this module and
 * its client applications.
 */
UmiStatus umi_ecosystem_component_wizard_add_template(
    UmiEcosystemComponentWizard *wizard,
    const UmiEcosystemComponentTemplate *component_template)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (wizard == NULL || component_template == NULL ||
        component_template->template_id[0] == '\0' ||
        component_template->display_name[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < wizard->template_count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(wizard->templates[index].template_id,
            component_template->template_id) == 0) return UMI_STATUS_ALREADY_EXISTS;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (wizard->template_count >= UMI_ECOSYSTEM_MAX_TEMPLATES) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    wizard->templates[wizard->template_count++] = *component_template;
    wizard->revision++;
    return UMI_STATUS_OK;
}

/*
 * Provide the ecosystem component wizard select operation used by this module and its
 * client applications.
 */
UmiStatus umi_ecosystem_component_wizard_select(
    UmiEcosystemComponentWizard *wizard,
    const char *template_id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (wizard == NULL || template_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < wizard->template_count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(wizard->templates[index].template_id, template_id) == 0) {
            wizard->selected_index = index;
            wizard->has_selection = true;
            wizard->revision++;
            return UMI_STATUS_OK;
        }
    }
    return UMI_STATUS_NOT_FOUND;
}

/*
 * Find ecosystem component wizard while leaving the underlying catalogue or model owned by
 * this module.
 */
const UmiEcosystemComponentTemplate *umi_ecosystem_component_wizard_selected(
    const UmiEcosystemComponentWizard *wizard)
{
    return wizard != NULL && wizard->has_selection &&
        wizard->selected_index < wizard->template_count
        ? &wizard->templates[wizard->selected_index] : NULL;
}

/*
 * Check that ecosystem component identifier satisfies its contract before another service
 * relies on it.
 */
bool umi_ecosystem_component_identifier_valid(const char *identifier)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (identifier == NULL || identifier[0] == '\0') return false;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (!isalpha((unsigned char)identifier[0]) && identifier[0] != '_') {
        return false;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 1U; identifier[index] != '\0'; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (!isalnum((unsigned char)identifier[index]) &&
            identifier[index] != '_') return false;
    }
    return true;
}

/* Provide the add file operation used by this module and its client applications. */
static UmiStatus add_file(
    UmiEcosystemComponentScaffoldPlan *plan,
    const char *role,
    const char *format,
    const char *name)
{
    UmiEcosystemGeneratedFile *file;
    int written;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (plan->file_count >= UMI_ECOSYSTEM_MAX_GENERATED_FILES) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    file = &plan->files[plan->file_count];
    (void)memset(file, 0, sizeof(*file));
    written = snprintf(file->path, sizeof(file->path), format,
        plan->output_root, name);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (written < 0 || (size_t)written >= sizeof(file->path)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_ecosystem_copy_text(file->role, sizeof(file->role), role) !=
        UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED;
    plan->file_count++;
    return UMI_STATUS_OK;
}

/*
 * Provide the ecosystem component wizard build plan operation used by this module and its
 * client applications.
 */
UmiStatus umi_ecosystem_component_wizard_build_plan(
    const UmiEcosystemComponentWizard *wizard,
    const char *component_id,
    const char *display_name,
    const char *namespace_name,
    const char *output_root,
    bool package_installed,
    bool sdk_selected,
    UmiEcosystemComponentScaffoldPlan *out_plan)
{
    const UmiEcosystemComponentTemplate *component_template;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (wizard == NULL || display_name == NULL || namespace_name == NULL ||
        output_root == NULL || out_plan == NULL ||
        !umi_ecosystem_component_identifier_valid(component_id) ||
        !umi_ecosystem_component_identifier_valid(namespace_name) ||
        display_name[0] == '\0' || output_root[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    component_template = umi_ecosystem_component_wizard_selected(wizard);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (component_template == NULL) return UMI_STATUS_INVALID_STATE;
    (void)memset(out_plan, 0, sizeof(*out_plan));
    status = umi_ecosystem_copy_text(out_plan->template_id,
        sizeof(out_plan->template_id), component_template->template_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_ecosystem_copy_text(out_plan->component_id,
        sizeof(out_plan->component_id), component_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_ecosystem_copy_text(out_plan->display_name,
        sizeof(out_plan->display_name), display_name);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_ecosystem_copy_text(out_plan->namespace_name,
        sizeof(out_plan->namespace_name), namespace_name);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_ecosystem_copy_text(out_plan->output_root,
        sizeof(out_plan->output_root), output_root);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    out_plan->requires_package_install =
        component_template->package_id[0] != '\0' && !package_installed;
    out_plan->requires_sdk_selection =
        component_template->required_sdk_id[0] != '\0' && !sdk_selected;
    /* Apply this branch only when its contract condition is satisfied. */
    if (out_plan->requires_package_install) out_plan->blocker_count++;
    /* Apply this branch only when its contract condition is satisfied. */
    if (out_plan->requires_sdk_selection) out_plan->blocker_count++;
    /* Apply this branch only when its contract condition is satisfied. */
    if (component_template->generates_header) {
        status = add_file(out_plan, "public-header", "%s/include/%s.h", component_id);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (component_template->generates_source) {
        status = add_file(out_plan, "implementation", "%s/src/%s.c", component_id);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (component_template->generates_test) {
        status = add_file(out_plan, "test", "%s/tests/test_%s.c", component_id);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (component_template->generates_manifest) {
        status = add_file(out_plan, "manifest", "%s/%s.umicom.yaml", component_id);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }
    out_plan->ready = out_plan->blocker_count == 0U && out_plan->file_count > 0U;
    return UMI_STATUS_OK;
}

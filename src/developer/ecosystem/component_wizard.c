/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer/ecosystem/component_wizard.c
 * PURPOSE: Implement toolkit-neutral component wizard and file-plan generation.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/ecosystem/component_wizard.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>

void umi_ecosystem_component_template_init(
    UmiEcosystemComponentTemplate *component_template,
    const char *template_id,
    const char *display_name,
    UmiEcosystemComponentKind kind)
{
    if (component_template == NULL) return;
    (void)memset(component_template, 0, sizeof(*component_template));
    if (template_id != NULL) {
        (void)umi_ecosystem_copy_text(component_template->template_id,
            sizeof(component_template->template_id), template_id);
    }
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

void umi_ecosystem_component_wizard_init(UmiEcosystemComponentWizard *wizard)
{
    if (wizard == NULL) return;
    (void)memset(wizard, 0, sizeof(*wizard));
    wizard->revision = 1U;
}

UmiStatus umi_ecosystem_component_wizard_add_template(
    UmiEcosystemComponentWizard *wizard,
    const UmiEcosystemComponentTemplate *component_template)
{
    size_t index;
    if (wizard == NULL || component_template == NULL ||
        component_template->template_id[0] == '\0' ||
        component_template->display_name[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    for (index = 0U; index < wizard->template_count; ++index) {
        if (strcmp(wizard->templates[index].template_id,
            component_template->template_id) == 0) return UMI_STATUS_ALREADY_EXISTS;
    }
    if (wizard->template_count >= UMI_ECOSYSTEM_MAX_TEMPLATES) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    wizard->templates[wizard->template_count++] = *component_template;
    wizard->revision++;
    return UMI_STATUS_OK;
}

UmiStatus umi_ecosystem_component_wizard_select(
    UmiEcosystemComponentWizard *wizard,
    const char *template_id)
{
    size_t index;
    if (wizard == NULL || template_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    for (index = 0U; index < wizard->template_count; ++index) {
        if (strcmp(wizard->templates[index].template_id, template_id) == 0) {
            wizard->selected_index = index;
            wizard->has_selection = true;
            wizard->revision++;
            return UMI_STATUS_OK;
        }
    }
    return UMI_STATUS_NOT_FOUND;
}

const UmiEcosystemComponentTemplate *umi_ecosystem_component_wizard_selected(
    const UmiEcosystemComponentWizard *wizard)
{
    return wizard != NULL && wizard->has_selection &&
        wizard->selected_index < wizard->template_count
        ? &wizard->templates[wizard->selected_index] : NULL;
}

bool umi_ecosystem_component_identifier_valid(const char *identifier)
{
    size_t index;
    if (identifier == NULL || identifier[0] == '\0') return false;
    if (!isalpha((unsigned char)identifier[0]) && identifier[0] != '_') {
        return false;
    }
    for (index = 1U; identifier[index] != '\0'; ++index) {
        if (!isalnum((unsigned char)identifier[index]) &&
            identifier[index] != '_') return false;
    }
    return true;
}

static UmiStatus add_file(
    UmiEcosystemComponentScaffoldPlan *plan,
    const char *role,
    const char *format,
    const char *name)
{
    UmiEcosystemGeneratedFile *file;
    int written;
    if (plan->file_count >= UMI_ECOSYSTEM_MAX_GENERATED_FILES) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    file = &plan->files[plan->file_count];
    (void)memset(file, 0, sizeof(*file));
    written = snprintf(file->path, sizeof(file->path), format,
        plan->output_root, name);
    if (written < 0 || (size_t)written >= sizeof(file->path)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    if (umi_ecosystem_copy_text(file->role, sizeof(file->role), role) !=
        UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED;
    plan->file_count++;
    return UMI_STATUS_OK;
}

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
    if (wizard == NULL || display_name == NULL || namespace_name == NULL ||
        output_root == NULL || out_plan == NULL ||
        !umi_ecosystem_component_identifier_valid(component_id) ||
        !umi_ecosystem_component_identifier_valid(namespace_name) ||
        display_name[0] == '\0' || output_root[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    component_template = umi_ecosystem_component_wizard_selected(wizard);
    if (component_template == NULL) return UMI_STATUS_INVALID_STATE;
    (void)memset(out_plan, 0, sizeof(*out_plan));
    status = umi_ecosystem_copy_text(out_plan->template_id,
        sizeof(out_plan->template_id), component_template->template_id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_ecosystem_copy_text(out_plan->component_id,
        sizeof(out_plan->component_id), component_id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_ecosystem_copy_text(out_plan->display_name,
        sizeof(out_plan->display_name), display_name);
    if (status != UMI_STATUS_OK) return status;
    status = umi_ecosystem_copy_text(out_plan->namespace_name,
        sizeof(out_plan->namespace_name), namespace_name);
    if (status != UMI_STATUS_OK) return status;
    status = umi_ecosystem_copy_text(out_plan->output_root,
        sizeof(out_plan->output_root), output_root);
    if (status != UMI_STATUS_OK) return status;
    out_plan->requires_package_install =
        component_template->package_id[0] != '\0' && !package_installed;
    out_plan->requires_sdk_selection =
        component_template->required_sdk_id[0] != '\0' && !sdk_selected;
    if (out_plan->requires_package_install) out_plan->blocker_count++;
    if (out_plan->requires_sdk_selection) out_plan->blocker_count++;
    if (component_template->generates_header) {
        status = add_file(out_plan, "public-header", "%s/include/%s.h", component_id);
        if (status != UMI_STATUS_OK) return status;
    }
    if (component_template->generates_source) {
        status = add_file(out_plan, "implementation", "%s/src/%s.c", component_id);
        if (status != UMI_STATUS_OK) return status;
    }
    if (component_template->generates_test) {
        status = add_file(out_plan, "test", "%s/tests/test_%s.c", component_id);
        if (status != UMI_STATUS_OK) return status;
    }
    if (component_template->generates_manifest) {
        status = add_file(out_plan, "manifest", "%s/%s.umicom.yaml", component_id);
        if (status != UMI_STATUS_OK) return status;
    }
    out_plan->ready = out_plan->blocker_count == 0U && out_plan->file_count > 0U;
    return UMI_STATUS_OK;
}

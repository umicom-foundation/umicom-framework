/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/ecosystem/component_wizard.h
 * PURPOSE: Model component-template selection and reviewable scaffold output.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_ECOSYSTEM_COMPONENT_WIZARD_H
#define UMICOM_DEVELOPER_ECOSYSTEM_COMPONENT_WIZARD_H

#include "umicom/developer/ecosystem/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum UmiEcosystemComponentKind {
    UMI_ECOSYSTEM_COMPONENT_WIDGET = 0,
    UMI_ECOSYSTEM_COMPONENT_PANEL = 1,
    UMI_ECOSYSTEM_COMPONENT_PROVIDER = 2,
    UMI_ECOSYSTEM_COMPONENT_PROJECT_TEMPLATE = 3,
    UMI_ECOSYSTEM_COMPONENT_ADAPTER = 4
} UmiEcosystemComponentKind;

typedef struct UmiEcosystemComponentTemplate {
    char template_id[UMI_ECOSYSTEM_ID_CAPACITY];
    char display_name[UMI_ECOSYSTEM_NAME_CAPACITY];
    char description[UMI_ECOSYSTEM_TEXT_CAPACITY];
    char package_id[UMI_ECOSYSTEM_ID_CAPACITY];
    char required_sdk_id[UMI_ECOSYSTEM_ID_CAPACITY];
    UmiEcosystemComponentKind kind;
    bool generates_header;
    bool generates_source;
    bool generates_test;
    bool generates_manifest;
    bool toolkit_neutral;
} UmiEcosystemComponentTemplate;

typedef struct UmiEcosystemGeneratedFile {
    char path[UMI_ECOSYSTEM_PATH_CAPACITY];
    char role[UMI_ECOSYSTEM_ID_CAPACITY];
    bool overwrite;
} UmiEcosystemGeneratedFile;

typedef struct UmiEcosystemComponentScaffoldPlan {
    char template_id[UMI_ECOSYSTEM_ID_CAPACITY];
    char component_id[UMI_ECOSYSTEM_ID_CAPACITY];
    char display_name[UMI_ECOSYSTEM_NAME_CAPACITY];
    char namespace_name[UMI_ECOSYSTEM_ID_CAPACITY];
    char output_root[UMI_ECOSYSTEM_PATH_CAPACITY];
    UmiEcosystemGeneratedFile files[UMI_ECOSYSTEM_MAX_GENERATED_FILES];
    size_t file_count;
    size_t blocker_count;
    bool requires_package_install;
    bool requires_sdk_selection;
    bool ready;
} UmiEcosystemComponentScaffoldPlan;

typedef struct UmiEcosystemComponentWizard {
    UmiEcosystemComponentTemplate templates[UMI_ECOSYSTEM_MAX_TEMPLATES];
    size_t template_count;
    size_t selected_index;
    bool has_selection;
    uint64_t revision;
} UmiEcosystemComponentWizard;

void umi_ecosystem_component_template_init(
    UmiEcosystemComponentTemplate *component_template,
    const char *template_id,
    const char *display_name,
    UmiEcosystemComponentKind kind);
void umi_ecosystem_component_wizard_init(UmiEcosystemComponentWizard *wizard);
UmiStatus umi_ecosystem_component_wizard_add_template(
    UmiEcosystemComponentWizard *wizard,
    const UmiEcosystemComponentTemplate *component_template);
UmiStatus umi_ecosystem_component_wizard_select(
    UmiEcosystemComponentWizard *wizard,
    const char *template_id);
const UmiEcosystemComponentTemplate *umi_ecosystem_component_wizard_selected(
    const UmiEcosystemComponentWizard *wizard);
UmiStatus umi_ecosystem_component_wizard_build_plan(
    const UmiEcosystemComponentWizard *wizard,
    const char *component_id,
    const char *display_name,
    const char *namespace_name,
    const char *output_root,
    bool package_installed,
    bool sdk_selected,
    UmiEcosystemComponentScaffoldPlan *out_plan);
bool umi_ecosystem_component_identifier_valid(const char *identifier);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEVELOPER_ECOSYSTEM_COMPONENT_WIZARD_H */

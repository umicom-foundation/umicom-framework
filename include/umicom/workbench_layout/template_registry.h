/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_layout/template_registry.h
 *
 * PURPOSE:
 *   Register immutable Framework and application layout templates and clone them into user-owned working documents without copying implementation code.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_LAYOUT_TEMPLATE_REGISTRY_H
#define UMICOM_WORKBENCH_LAYOUT_TEMPLATE_REGISTRY_H

#include "umicom/workbench_layout/document.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench layout template data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchLayoutTemplate {
    uint32_t structure_size;
    char template_id[UMI_WORKBENCH_LAYOUT_ID_CAPACITY];
    char display_name[UMI_WORKBENCH_LAYOUT_NAME_CAPACITY];
    char category[UMI_WORKBENCH_LAYOUT_ID_CAPACITY];
    char owner_application_id[UMI_WORKBENCH_LAYOUT_ID_CAPACITY];
    char description[UMI_WORKBENCH_LAYOUT_DESCRIPTION_CAPACITY];
    UmiWorkbenchLayoutTag tags[UMI_WORKBENCH_LAYOUT_MAX_TAGS];
    size_t tag_count;
    UmiWorkbenchLayoutDocument document;
    bool built_in;
    bool recommended;
    uint64_t revision;
} UmiWorkbenchLayoutTemplate;

/**
 * Represent the workbench layout template registry data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchLayoutTemplateRegistry {
    uint32_t structure_size;
    UmiWorkbenchLayoutTemplate
        templates[UMI_WORKBENCH_LAYOUT_MAX_TEMPLATES];
    size_t count;
    uint64_t revision;
} UmiWorkbenchLayoutTemplateRegistry;

/**
 * Initialise workbench layout template registry from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_layout_template_registry_init(
    UmiWorkbenchLayoutTemplateRegistry *registry);

/**
 * Add workbench layout template registry only after its inputs and available capacity have
 * been checked.
 */
UmiStatus umi_workbench_layout_template_registry_add(
    UmiWorkbenchLayoutTemplateRegistry *registry,
    const UmiWorkbenchLayoutTemplate *layout_template);

/**
 * Remove workbench layout template registry while keeping the remaining records in a valid
 * and discoverable state.
 */
UmiStatus umi_workbench_layout_template_registry_remove(
    UmiWorkbenchLayoutTemplateRegistry *registry,
    const char *template_id);

/**
 * Find workbench layout template registry while leaving the underlying catalogue or model
 * owned by this module.
 */
const UmiWorkbenchLayoutTemplate *
umi_workbench_layout_template_registry_find(
    const UmiWorkbenchLayoutTemplateRegistry *registry,
    const char *template_id);

/**
 * Find workbench layout template registry while leaving the underlying catalogue or model
 * owned by this module.
 */
const UmiWorkbenchLayoutTemplate *
umi_workbench_layout_template_registry_at(
    const UmiWorkbenchLayoutTemplateRegistry *registry,
    size_t index);

/**
 * Provide the workbench layout template registry clone operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_layout_template_registry_clone(
    const UmiWorkbenchLayoutTemplateRegistry *registry,
    const char *template_id,
    const UmiWorkbenchLayoutIdentity *identity,
    const char *name,
    UmiWorkbenchLayoutDocument *out_document);

/**
 * Provide the workbench layout template registry count category operation used by this
 * module and its client applications.
 */
size_t umi_workbench_layout_template_registry_count_category(
    const UmiWorkbenchLayoutTemplateRegistry *registry,
    const char *category);

/**
 * Provide the workbench layout template registry count owner operation used by this module
 * and its client applications.
 */
size_t umi_workbench_layout_template_registry_count_owner(
    const UmiWorkbenchLayoutTemplateRegistry *registry,
    const char *owner_application_id);

/**
 * Provide the workbench layout template registry seed framework operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_layout_template_registry_seed_framework(
    UmiWorkbenchLayoutTemplateRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif

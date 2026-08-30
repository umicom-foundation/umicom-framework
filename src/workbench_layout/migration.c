/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_layout/migration.c
 *
 * PURPOSE:
 *   Implement explicit schema migration steps for portable layout documents and retain a report of every compatibility transformation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/workbench_layout/migration.h"

#include <stdio.h>
#include <string.h>

#include "internal.h"

static UmiStatus add_step(
    UmiWorkbenchLayoutMigrationReport *report,
    uint32_t from_version,
    uint32_t to_version,
    UmiWorkbenchLayoutMigrationAction action,
    const char *description,
    bool changed)
{
    UmiWorkbenchLayoutMigrationStep *step;
    UmiStatus status;

    if (report == NULL || description == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (report->step_count >=
        sizeof(report->steps) / sizeof(report->steps[0])) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    step = &report->steps[report->step_count];
    (void)memset(step, 0, sizeof(*step));
    step->structure_size = sizeof(*step);
    step->from_schema_version = from_version;
    step->to_schema_version = to_version;
    step->action = action;
    step->changed = changed;
    status = umi_workbench_layout_copy_text(
        step->description,
        sizeof(step->description),
        description,
        false);
    if (status != UMI_STATUS_OK) {
        return status;
    }

    report->step_count += 1U;
    report->changed = report->changed || changed;
    return UMI_STATUS_OK;
}

static bool node_visibility_valid(
    UmiWorkbenchLayoutVisibility visibility)
{
    return visibility >= UMI_WORKBENCH_LAYOUT_VISIBILITY_VISIBLE &&
           visibility <= UMI_WORKBENCH_LAYOUT_VISIBILITY_AUTO;
}

static bool node_structure_size_valid(
    const UmiWorkbenchLayoutNode *node)
{
    return node != NULL &&
           node->structure_size >= sizeof(UmiWorkbenchLayoutNode);
}

static UmiStatus migrate_schema_one_to_two(
    UmiWorkbenchLayoutDocument *document,
    UmiWorkbenchLayoutMigrationReport *report)
{
    size_t index;
    bool structure_changed = false;
    bool visibility_changed = false;
    bool monitor_changed = false;
    UmiStatus status;

    for (index = 0U; index < document->node_count; ++index) {
        UmiWorkbenchLayoutNode *node = &document->nodes[index];

        if (!node_structure_size_valid(node)) {
            node->structure_size = sizeof(*node);
            structure_changed = true;
        }
        if (!node_visibility_valid(node->visibility)) {
            node->visibility = UMI_WORKBENCH_LAYOUT_VISIBILITY_VISIBLE;
            visibility_changed = true;
        }
        if ((node->kind == UMI_WORKBENCH_LAYOUT_NODE_WINDOW ||
             node->kind ==
                 UMI_WORKBENCH_LAYOUT_NODE_FLOATING_WINDOW) &&
            node->monitor_id[0] == '\0') {
            (void)umi_workbench_layout_copy_text(
                node->monitor_id,
                sizeof(node->monitor_id),
                "primary",
                false);
            monitor_changed = true;
        }
    }

    status = add_step(
        report,
        1U,
        2U,
        UMI_WORKBENCH_LAYOUT_MIGRATION_ADD_STRUCTURE_SIZES,
        "Initialise public structure_size fields for stable ABI consumers.",
        structure_changed);
    if (status == UMI_STATUS_OK) {
        status = add_step(
            report,
            1U,
            2U,
            UMI_WORKBENCH_LAYOUT_MIGRATION_ADD_VISIBILITY,
            "Initialise explicit semantic visibility for every layout node.",
            visibility_changed);
    }
    if (status == UMI_STATUS_OK) {
        status = add_step(
            report,
            1U,
            2U,
            UMI_WORKBENCH_LAYOUT_MIGRATION_ADD_MONITOR_PLACEMENT,
            "Assign unplaced top-level windows to the primary monitor.",
            monitor_changed);
    }
    if (status != UMI_STATUS_OK) {
        return status;
    }

    document->version.schema_version = 2U;
    document->structure_size = sizeof(*document);
    umi_workbench_layout_document_increment_revision(document);
    return add_step(
        report,
        1U,
        2U,
        UMI_WORKBENCH_LAYOUT_MIGRATION_REWRITE_SCHEMA_VERSION,
        "Record the current portable workbench layout schema version.",
        true);
}

void umi_workbench_layout_migration_report_init(
    UmiWorkbenchLayoutMigrationReport *report)
{
    if (report == NULL) {
        return;
    }
    (void)memset(report, 0, sizeof(*report));
    report->structure_size = sizeof(*report);
    report->compatible = true;
}

UmiStatus umi_workbench_layout_migrate(
    const UmiWorkbenchLayoutDocument *source,
    uint32_t target_schema_version,
    UmiWorkbenchLayoutDocument *out_document,
    UmiWorkbenchLayoutMigrationReport *out_report)
{
    UmiWorkbenchLayoutMigrationReport local_report;
    UmiWorkbenchLayoutMigrationReport *report;
    UmiStatus status;

    if (source == NULL || out_document == NULL ||
        target_schema_version == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    report = out_report != NULL ? out_report : &local_report;
    umi_workbench_layout_migration_report_init(report);
    report->original_schema_version = source->version.schema_version;
    report->resulting_schema_version = source->version.schema_version;

    if (!umi_workbench_layout_schema_is_supported(
            source->version.schema_version) ||
        !umi_workbench_layout_schema_is_supported(
            target_schema_version)) {
        report->compatible = false;
        (void)umi_workbench_layout_copy_text(
            report->message,
            sizeof(report->message),
            "The requested schema version is not supported.",
            false);
        return UMI_STATUS_NOT_IMPLEMENTED;
    }
    if (source->version.schema_version > target_schema_version) {
        report->compatible = false;
        (void)umi_workbench_layout_copy_text(
            report->message,
            sizeof(report->message),
            "Downgrading a layout schema is not supported.",
            false);
        return UMI_STATUS_NOT_IMPLEMENTED;
    }

    status = umi_workbench_layout_document_copy(
        out_document, source);
    if (status != UMI_STATUS_OK) {
        return status;
    }

    while (out_document->version.schema_version <
           target_schema_version) {
        if (out_document->version.schema_version == 1U) {
            status = migrate_schema_one_to_two(
                out_document, report);
        } else {
            status = UMI_STATUS_NOT_IMPLEMENTED;
        }
        if (status != UMI_STATUS_OK) {
            report->compatible = false;
            return status;
        }
    }

    report->resulting_schema_version =
        out_document->version.schema_version;
    (void)umi_workbench_layout_copy_text(
        report->message,
        sizeof(report->message),
        report->changed
            ? "The layout document was migrated successfully."
            : "The layout document already uses the requested schema.",
        false);
    return UMI_STATUS_OK;
}

UmiStatus umi_workbench_layout_migrate_in_place(
    UmiWorkbenchLayoutDocument *document,
    uint32_t target_schema_version,
    UmiWorkbenchLayoutMigrationReport *out_report)
{
    UmiWorkbenchLayoutDocument migrated;
    UmiStatus status;

    if (document == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_workbench_layout_migrate(
        document,
        target_schema_version,
        &migrated,
        out_report);
    if (status == UMI_STATUS_OK) {
        *document = migrated;
    }
    return status;
}

bool umi_workbench_layout_schema_is_supported(
    uint32_t schema_version)
{
    return schema_version >= 1U &&
           schema_version <= UMI_WORKBENCH_LAYOUT_SCHEMA_VERSION;
}

uint32_t umi_workbench_layout_schema_current(void)
{
    return UMI_WORKBENCH_LAYOUT_SCHEMA_VERSION;
}

/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_layout/migration.h
 *
 * PURPOSE:
 *   Upgrade portable layout documents through explicit schema steps while retaining evidence of every transformation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_LAYOUT_MIGRATION_H
#define UMICOM_WORKBENCH_LAYOUT_MIGRATION_H

#include "umicom/workbench_layout/document.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchLayoutMigrationStep {
    uint32_t structure_size;
    uint32_t from_schema_version;
    uint32_t to_schema_version;
    UmiWorkbenchLayoutMigrationAction action;
    char description[UMI_WORKBENCH_LAYOUT_DESCRIPTION_CAPACITY];
    bool changed;
} UmiWorkbenchLayoutMigrationStep;

typedef struct UmiWorkbenchLayoutMigrationReport {
    uint32_t structure_size;
    UmiWorkbenchLayoutMigrationStep steps[16U];
    size_t step_count;
    uint32_t original_schema_version;
    uint32_t resulting_schema_version;
    bool changed;
    bool compatible;
    char message[UMI_WORKBENCH_LAYOUT_ERROR_CAPACITY];
} UmiWorkbenchLayoutMigrationReport;

void umi_workbench_layout_migration_report_init(
    UmiWorkbenchLayoutMigrationReport *report);

UmiStatus umi_workbench_layout_migrate(
    const UmiWorkbenchLayoutDocument *source,
    uint32_t target_schema_version,
    UmiWorkbenchLayoutDocument *out_document,
    UmiWorkbenchLayoutMigrationReport *out_report);

UmiStatus umi_workbench_layout_migrate_in_place(
    UmiWorkbenchLayoutDocument *document,
    uint32_t target_schema_version,
    UmiWorkbenchLayoutMigrationReport *out_report);

bool umi_workbench_layout_schema_is_supported(
    uint32_t schema_version);

uint32_t umi_workbench_layout_schema_current(void);

#ifdef __cplusplus
}
#endif

#endif

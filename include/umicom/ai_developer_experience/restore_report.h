/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_developer_experience/restore_report.h
 *
 * PURPOSE:
 *   Report which durable AI developer state categories were restored.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_DEVELOPER_EXPERIENCE_RESTORE_REPORT_H
#define UMICOM_AI_DEVELOPER_EXPERIENCE_RESTORE_REPORT_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAiDeveloperRestoreReport {
    uint32_t schema_version;
    size_t chat_sessions;
    size_t tasks;
    size_t approvals;
    int presentation_restored;
    int preferences_restored;
    int migrated;
    UmiStatus status;
} UmiAiDeveloperRestoreReport;

void umi_ai_developer_restore_report_init(
    UmiAiDeveloperRestoreReport *report);

#ifdef __cplusplus
}
#endif
#endif

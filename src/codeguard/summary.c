/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/codeguard/summary.c
 *
 * PURPOSE:
 *   Summarise findings for humans, CI and release gates.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This file is intentionally small and focused. CodeGuard separates scanning
 * rules, analysis engines and reports so new developers can understand one
 * responsibility at a time and test it independently.
 */

#include "umicom/codeguard/summary.h"
UmiCodeGuardSummary umi_codeguard_summary_build(const UmiCodeGuardResult *r){UmiCodeGuardSummary s={0};s.total=umi_codeguard_result_count(r);s.critical=umi_codeguard_result_count_severity(r,UMI_CODEGUARD_CRITICAL);s.high=umi_codeguard_result_count_severity(r,UMI_CODEGUARD_HIGH);s.medium=umi_codeguard_result_count_severity(r,UMI_CODEGUARD_MEDIUM);s.low=umi_codeguard_result_count_severity(r,UMI_CODEGUARD_LOW);s.info=umi_codeguard_result_count_severity(r,UMI_CODEGUARD_INFO);s.memory=umi_codeguard_result_count_category(r,UMI_CODEGUARD_CATEGORY_MEMORY);s.buffers=umi_codeguard_result_count_category(r,UMI_CODEGUARD_CATEGORY_BUFFER);s.architecture=umi_codeguard_result_count_category(r,UMI_CODEGUARD_CATEGORY_ARCHITECTURE);s.duplicates=umi_codeguard_result_count_category(r,UMI_CODEGUARD_CATEGORY_DUPLICATION);return s;}
int umi_codeguard_summary_failed(const UmiCodeGuardSummary *s,UmiCodeGuardSeverity t){if(s==NULL)return 1;if(t<=UMI_CODEGUARD_CRITICAL&&s->critical>0U)return 1;if(t<=UMI_CODEGUARD_HIGH&&s->high>0U)return 1;if(t<=UMI_CODEGUARD_MEDIUM&&s->medium>0U)return 1;if(t<=UMI_CODEGUARD_LOW&&s->low>0U)return 1;if(t<=UMI_CODEGUARD_INFO&&s->info>0U)return 1;return 0;}

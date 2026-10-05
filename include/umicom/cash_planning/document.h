/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/cash_planning/document.h
 * PURPOSE: Transfer cash assumptions explicitly through bounded JSON and CSV files.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CASH_PLANNING_DOCUMENT_H
#define UMICOM_CASH_PLANNING_DOCUMENT_H
#include "umicom/cash_planning/plan.h"
#include "umicom/platform/output_file.h"
#ifdef __cplusplus
extern "C"
{
#endif
#define UMI_CASH_PLAN_DOCUMENT_LIMIT (1024U * 1024U)
    /* Owned UTF-8 JSON contains only assumptions, never account credentials or
 * execution permission. Decode rejects unknown/duplicate members and invalid
 * entries as a whole. A decoded plan is independent; callers choose whether to
 * replace their current plan. Format identity is umicom.cash-plan. Integer
 * amounts are minor units, with one explicit currency/scale for the document.
 * Decode clears *out; Encode clears output bytes/size on failure. FreeBytes
 * accepts NULL. Revisions and native handles are not serialized. */
    UmiStatus UmiCashPlanEncode(const UmiCashPlan *plan, char **out, size_t *out_size);
    UmiStatus UmiCashPlanDecode(const void *bytes, size_t size, UmiCashPlan **out);
    void UmiCashPlanFreeBytes(char *bytes);
    /* Export the opening balance, one row per active date, and the final balance.
 * Columns label integer minor units and scale explicitly; no float conversion.
 * Typed CSV numbers remain numbers. Text uses the shared spreadsheet-escape
 * policy. This report is not an import format. Outputs clear on failure. */
    UmiStatus UmiCashPlanCsv(const UmiCashPlan *plan, char **out, size_t *out_size);
    /* Absolute paths only; bounded regular input, exclusive new output. Disk I/O
 * belongs on a worker. Existing files and partial outputs are retained. The
 * optional receipt records actual output progress; it does not authorize a
 * payment. Save captures only the given plan, so hosts should copy before
 * dispatching a worker and never mutate that copy concurrently. */
    UmiStatus UmiCashPlanLoad(const char *path, UmiCashPlan **out);
    UmiStatus UmiCashPlanSaveNew(const char *path, const UmiCashPlan *plan, UmiOutputFileSnapshot *receipt);
    UmiStatus UmiCashPlanExportNew(const char *path, const UmiCashPlan *plan, UmiOutputFileSnapshot *receipt);
#ifdef __cplusplus
}
#endif
#endif

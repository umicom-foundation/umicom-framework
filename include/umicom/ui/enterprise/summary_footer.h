/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/enterprise/summary_footer.h
 *
 * PURPOSE:
 *   Store named aggregate values displayed in a virtual grid footer.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_ENTERPRISE_SUMMARY_FOOTER_H
#define UMICOM_UI_ENTERPRISE_SUMMARY_FOOTER_H

#include "umicom/ui/enterprise/types.h"
/**
 * Represent the ui ent summary item data shared with callers of this public contract.
 */
typedef struct UmiUiEntSummaryItem { char id[UMI_UI_ENT_ID_CAPACITY]; double value; } UmiUiEntSummaryItem;
/**
 * Represent the ui ent summary footer data shared with callers of this public contract.
 */
typedef struct UmiUiEntSummaryFooter { UmiUiEntSummaryItem items[UMI_UI_ENT_MAX_RULES]; size_t count; } UmiUiEntSummaryFooter;
/**
 * Initialise ui ent summary footer from caller-provided values so later operations receive
 * a known state.
 */
void umi_ui_ent_summary_footer_init(UmiUiEntSummaryFooter *f);
/**
 * Copy ui ent summary footer into module-owned storage so callers keep ownership of their
 * input values.
 */
UmiStatus umi_ui_ent_summary_footer_set(UmiUiEntSummaryFooter *f,const char *id,double value);
/**
 * Provide the ui ent summary footer get operation used by this module and its client
 * applications.
 */
int umi_ui_ent_summary_footer_get(const UmiUiEntSummaryFooter *f,const char *id,double *out_value);

#endif

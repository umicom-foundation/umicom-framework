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
typedef struct UmiUiEntSummaryItem { char id[UMI_UI_ENT_ID_CAPACITY]; double value; } UmiUiEntSummaryItem;
typedef struct UmiUiEntSummaryFooter { UmiUiEntSummaryItem items[UMI_UI_ENT_MAX_RULES]; size_t count; } UmiUiEntSummaryFooter;
void umi_ui_ent_summary_footer_init(UmiUiEntSummaryFooter *f);
UmiStatus umi_ui_ent_summary_footer_set(UmiUiEntSummaryFooter *f,const char *id,double value);
int umi_ui_ent_summary_footer_get(const UmiUiEntSummaryFooter *f,const char *id,double *out_value);

#endif

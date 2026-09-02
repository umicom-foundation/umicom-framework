/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/enterprise/data_delta.c
 *
 * PURPOSE:
 *   Implement data-delta validation and range tests.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/data_delta.h"
/* Check that ui ent data delta satisfies its contract before another service relies on it. */
int umi_ui_ent_data_delta_validate(const UmiUiEntDataDelta *d){return d&&d->kind>=UMI_UI_ENT_DELTA_INSERT&&d->kind<=UMI_UI_ENT_DELTA_RESET&&(d->kind==UMI_UI_ENT_DELTA_RESET||d->rows.count>0U);}
/*
 * Provide the ui ent data delta touches operation used by this module and its client
 * applications.
 */
int umi_ui_ent_data_delta_touches(const UmiUiEntDataDelta *d,size_t row){return umi_ui_ent_data_delta_validate(d)&&(d->kind==UMI_UI_ENT_DELTA_RESET||umi_ui_ent_span_contains(d->rows,row));}

/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/enterprise/focus_model.c
 *
 * PURPOSE:
 *   Implement clamped logical focus state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/focus_model.h"
/*
 * Release or reset state held by ui ent focus model so the same storage can be reused
 * safely.
 */
void umi_ui_ent_focus_model_clear(UmiUiEntFocusModel *m){/* Apply this operation only while the related capability or state is available. */ if(m){m->row=0U;m->column=0U;m->active=0;m->revision++;}}
/*
 * Copy ui ent focus model into module-owned storage so callers keep ownership of their
 * input values.
 */
UmiStatus umi_ui_ent_focus_model_set(UmiUiEntFocusModel *m,size_t r,size_t c,size_t rows,size_t cols){/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(!m||r>=rows||c>=cols)return UMI_STATUS_INVALID_ARGUMENT;m->row=r;m->column=c;m->active=1;m->revision++;return UMI_STATUS_OK;}

/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/enterprise/focus_model.c
 *
 * PURPOSE:
 *   Implement clamped logical focus state.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/focus_model.h"
void umi_ui_ent_focus_model_clear(UmiUiEntFocusModel *m){if(m){m->row=0U;m->column=0U;m->active=0;m->revision++;}}
UmiStatus umi_ui_ent_focus_model_set(UmiUiEntFocusModel *m,size_t r,size_t c,size_t rows,size_t cols){if(!m||r>=rows||c>=cols)return UMI_STATUS_INVALID_ARGUMENT;m->row=r;m->column=c;m->active=1;m->revision++;return UMI_STATUS_OK;}

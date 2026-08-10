/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/declarative/event.h
 *
 * PURPOSE:
 *   Represent a semantic component event mapped to a canonical command.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * The declarations below describe semantic application data and behaviour.
 * They deliberately avoid GUI-toolkit types so the same contract can be used
 * by GTK4, web, headless tests and future frontend adapters.
 */

#ifndef UMICOM_DECLARATIVE_EVENT_H
#define UMICOM_DECLARATIVE_EVENT_H
#include "umicom/declarative/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiDeclEvent { char event_name[UMI_DECL_ID_CAPACITY]; char command_id[UMI_DECL_ID_CAPACITY]; } UmiDeclEvent;
UmiStatus umi_decl_event_init(UmiDeclEvent *item,const char *left,const char *right);
int umi_decl_event_is_valid(const UmiDeclEvent *item);
#ifdef __cplusplus
}
#endif
#endif

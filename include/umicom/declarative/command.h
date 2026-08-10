/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/declarative/command.h
 *
 * PURPOSE:
 *   Represent a declarative command reference resolved through the Framework command registry.
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

#ifndef UMICOM_DECLARATIVE_COMMAND_H
#define UMICOM_DECLARATIVE_COMMAND_H
#include "umicom/declarative/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiDeclCommand { char command_id[UMI_DECL_ID_CAPACITY]; char handler_id[UMI_DECL_ID_CAPACITY]; } UmiDeclCommand;
UmiStatus umi_decl_command_init(UmiDeclCommand *item,const char *left,const char *right);
int umi_decl_command_is_valid(const UmiDeclCommand *item);
#ifdef __cplusplus
}
#endif
#endif

/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/declarative/command.h
 *
 * PURPOSE:
 *   Represent a declarative command reference resolved through the Framework command registry.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
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
/**
 * Represent the decl command data shared with callers of this public contract.
 */
typedef struct UmiDeclCommand { char command_id[UMI_DECL_ID_CAPACITY]; char handler_id[UMI_DECL_ID_CAPACITY]; } UmiDeclCommand;
/**
 * Initialise decl command from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_decl_command_init(UmiDeclCommand *item,const char *left,const char *right);
/**
 * Check that decl command satisfies its contract before another service relies on it.
 */
int umi_decl_command_is_valid(const UmiDeclCommand *item);
#ifdef __cplusplus
}
#endif
#endif

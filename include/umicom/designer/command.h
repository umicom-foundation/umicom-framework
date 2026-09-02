/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/command.h
 *
 * PURPOSE:
 *   Register common designer commands against a semantic document and its undo/redo history.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This public contract describes reusable visual-designer state and operations.
 * Product-specific windows remain outside the Framework engine.
 */

#ifndef UMICOM_DESIGNER_COMMAND_H
#define UMICOM_DESIGNER_COMMAND_H
#include "umicom/designer/history.h"
#include "umicom/runtime/command_registry.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Provide the designer register commands operation used by this module and its client
 * applications.
 */
UmiStatus umi_designer_register_commands(UmiCommandRegistry *commands,UmiDesignerHistory *history);
#ifdef __cplusplus
}
#endif
#endif

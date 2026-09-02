/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/components/command.h
 *
 * PURPOSE:
 *   Bind semantic UI components to named Framework/product commands.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This file keeps one responsibility small and explicit. Read the public
 * structure/function declarations first, then follow the implementation in
 * the matching source file.
 */

#ifndef UMICOM_UI_COMPONENTS_COMMAND_H
#define UMICOM_UI_COMPONENTS_COMMAND_H
#include "umicom/base/status.h"
#include "umicom/ui/components/component.h"
#define UMI_UI_COMPONENT_COMMAND_CAPACITY 128U
/**
 * Represent the ui component command binding data shared with callers of this public
 * contract.
 */
typedef struct UmiUiComponentCommandBinding { char component_id[UMI_UI_COMPONENT_ID_CAPACITY]; char command_id[UMI_UI_COMPONENT_COMMAND_CAPACITY]; } UmiUiComponentCommandBinding;
/**
 * Provide the ui component command bind operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_component_command_bind(UmiUiComponentCommandBinding *binding,const char *component_id,const char *command_id);
#endif

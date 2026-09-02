/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: templates/workspace/tools/umicom/src/wizard.h
 *
 * PURPOSE:
 *   Declare the wizard contract shared by Framework services and thin
 *   applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TOOL_WIZARD_H
#define UMICOM_TOOL_WIZARD_H
/**
 * Perform wizard through the module contract so client applications do not duplicate its
 * policy.
 */
int umi_wizard_run(int argc, char **argv, const char *template_root);
#endif

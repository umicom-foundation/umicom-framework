/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_project/templates/c23_assembly.h
 *
 * PURPOSE:
 *   Publish the built-in C23 + Assembly Application project template.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_PROJECT_TEMPLATE_C23_ASSEMBLY_H
#define UMICOM_DEVELOPER_PROJECT_TEMPLATE_C23_ASSEMBLY_H

#include "umicom/developer_project/template.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the developer project template c23 assembly operation used by this module and
 * its client applications.
 */
const UmiDeveloperProjectTemplate *umi_developer_project_template_c23_assembly(void);

#ifdef __cplusplus
}
#endif

#endif

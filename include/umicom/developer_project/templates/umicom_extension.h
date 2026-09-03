/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_project/templates/umicom_extension.h
 *
 * PURPOSE:
 *   Publish the built-in Umicom Extension project template used by Studio and
 *   other Framework-based development tools.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_PROJECT_TEMPLATE_UMICOM_EXTENSION_H
#define UMICOM_DEVELOPER_PROJECT_TEMPLATE_UMICOM_EXTENSION_H

#include "umicom/developer_project/template.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Return the community extension starter without transferring its ownership. */
const UmiDeveloperProjectTemplate *
umi_developer_project_template_umicom_extension(void);

#ifdef __cplusplus
}
#endif

#endif

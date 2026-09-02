/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_project/templates/thin_desktop_application.h
 *
 * PURPOSE:
 *   Publish the built-in Umicom Thin Desktop Application project template.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_PROJECT_TEMPLATE_THIN_DESKTOP_APPLICATION_H
#define UMICOM_DEVELOPER_PROJECT_TEMPLATE_THIN_DESKTOP_APPLICATION_H

#include "umicom/developer_project/template.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the developer project template thin desktop application operation used by this
 * module and its client applications.
 */
const UmiDeveloperProjectTemplate *umi_developer_project_template_thin_desktop_application(void);

#ifdef __cplusplus
}
#endif

#endif

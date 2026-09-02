/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_project/languages/html.h
 *
 * PURPOSE:
 *   Publish the built-in HTML language pack for project models, generation,
 *   build/run planning and future language-service adapters.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_PROJECT_LANGUAGE_HTML_H
#define UMICOM_DEVELOPER_PROJECT_LANGUAGE_HTML_H

#include "umicom/developer_project/language_pack.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the developer project language html operation used by this module and its client
 * applications.
 */
const UmiDeveloperProjectLanguagePack *umi_developer_project_language_html(void);

#ifdef __cplusplus
}
#endif

#endif

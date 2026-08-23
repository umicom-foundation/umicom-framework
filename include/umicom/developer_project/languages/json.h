/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_project/languages/json.h
 *
 * PURPOSE:
 *   Publish the built-in JSON language pack for project models, generation,
 *   build/run planning and future language-service adapters.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_PROJECT_LANGUAGE_JSON_H
#define UMICOM_DEVELOPER_PROJECT_LANGUAGE_JSON_H

#include "umicom/developer_project/language_pack.h"

#ifdef __cplusplus
extern "C" {
#endif

const UmiDeveloperProjectLanguagePack *umi_developer_project_language_json(void);

#ifdef __cplusplus
}
#endif

#endif

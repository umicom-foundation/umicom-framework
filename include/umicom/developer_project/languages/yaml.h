/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_project/languages/yaml.h
 *
 * PURPOSE:
 *   Publish the built-in YAML language pack for project models, generation,
 *   build/run planning and future language-service adapters.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_PROJECT_LANGUAGE_YAML_H
#define UMICOM_DEVELOPER_PROJECT_LANGUAGE_YAML_H

#include "umicom/developer_project/language_pack.h"

#ifdef __cplusplus
extern "C" {
#endif

const UmiDeveloperProjectLanguagePack *umi_developer_project_language_yaml(void);

#ifdef __cplusplus
}
#endif

#endif

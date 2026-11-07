/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_project/languages/vhdl.h
 *
 * PURPOSE:
 *   Expose the shared VHDL language metadata for project and editor clients.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_PROJECT_LANGUAGE_VHDL_H
#define UMICOM_DEVELOPER_PROJECT_LANGUAGE_VHDL_H
#include "umicom/developer_project/language_pack.h"
#ifdef __cplusplus
extern "C" {
#endif
/* The returned metadata has process lifetime; callers do not free it. */
const UmiDeveloperProjectLanguagePack *umi_developer_project_language_vhdl(void);
#ifdef __cplusplus
}
#endif
#endif

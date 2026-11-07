/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_project/languages/umicom_c.h
 *
 * PURPOSE:
 *   Expose the arithmetic-profile Umicom C language contract to Studio and other hosts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_DEVELOPER_PROJECT_LANGUAGE_UMICOM_C_H
#define UMICOM_DEVELOPER_PROJECT_LANGUAGE_UMICOM_C_H
#include "umicom/developer_project/language_pack.h"
#ifdef __cplusplus
extern "C" {
#endif
/* Immutable process-lifetime metadata; the caller does not free it. */
const UmiDeveloperProjectLanguagePack *umi_developer_project_language_umicom_c(void);
#ifdef __cplusplus
}
#endif
#endif

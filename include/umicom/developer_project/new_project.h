/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_project/new_project.h
 *
 * PURPOSE:
 *   Create a fresh project without merging into an existing folder.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_PROJECT_NEW_PROJECT_H
#define UMICOM_DEVELOPER_PROJECT_NEW_PROJECT_H
#include "umicom/developer_project/service.h"
#include "umicom/build/project_profile.h"
#ifdef __cplusplus
extern "C" {
#endif
/** Create using the canonical template service. The destination must be an
 * absolute, new path. Existing destinations are rejected, never overwritten.
 * No compiler, Git command, or network request is run. On an I/O failure new
 * partial files may remain for inspection; existing user files are not removed.
 * The caller must not concurrently modify the selected destination. */
UmiStatus UmiDeveloperProjectCreateNew(UmiDeveloperProjectService *service,
    const UmiDeveloperProjectGenerationRequest *request,
    UmiDeveloperProjectGeneratorReport *outReport,
    UmiDeveloperProjectModel *outModel, UmiBuildProfile *outProfile);
#ifdef __cplusplus
}
#endif
#endif

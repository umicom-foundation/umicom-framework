/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/build/project_profile.h
 *
 * PURPOSE:
 *   Prepare project-relative build settings for any developer frontend.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_BUILD_PROJECT_PROFILE_H
#define UMICOM_BUILD_PROJECT_PROFILE_H
#include "umicom/build/profile.h"
#include "umicom/developer_project/model.h"
#ifdef __cplusplus
extern "C" {
#endif
/** Prepare defaults without reading or executing CMake. The output is assigned
 * only on success. No executable is guessed for an imported folder. */
UmiStatus UmiBuildProfileForWorkspace(const char *root, UmiBuildProfile *outProfile);
/** Copy a generated model into the same profile contract used by the runner. */
UmiStatus UmiBuildProfileFromProject(const UmiDeveloperProjectModel *model,
    UmiBuildProfile *outProfile);
#ifdef __cplusplus
}
#endif
#endif

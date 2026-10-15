/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_project/application_scaffold.h
 * PURPOSE: Adapt a reviewed application scaffold to the shared IDE project and build settings.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/


#ifndef UMICOM_DEVELOPER_PROJECT_APPLICATION_SCAFFOLD_H
#define UMICOM_DEVELOPER_PROJECT_APPLICATION_SCAFFOLD_H
#include "umicom/scaffold/application_module.h"
#include "umicom/developer_project/model.h"
#include "umicom/build/profile.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Prepare project settings from an owned scaffold before applying its files.
 * No filesystem access, source generation, execution trust or compiler action
 * occurs here. Both outputs remain unchanged on failure and must not overlap.
 * Run remains unset because the chosen generator determines executable paths;
 * the IDE's build-target catalogue can select the built program afterwards.
 * Callers mark the project generated only after successful scaffold application.
 */
UmiStatus UmiDeveloperApplicationScaffoldProject(const UmiApplicationScaffoldPlan *plan,
    UmiDeveloperProjectModel *outModel, UmiBuildProfile *outProfile);
#ifdef __cplusplus
}
#endif
#endif

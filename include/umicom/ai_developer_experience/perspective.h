/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_developer_experience/perspective.h
 *
 * PURPOSE:
 *   Publish the reusable AI Developer workbench perspective using the existing
 *   developer-workbench perspective contract. It specifies which AI surfaces
 *   belong together without hard-coding physical docking coordinates.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_DEVELOPER_EXPERIENCE_PERSPECTIVE_H
#define UMICOM_AI_DEVELOPER_EXPERIENCE_PERSPECTIVE_H

#include "umicom/developer_workbench/perspective.h"

#ifdef __cplusplus
extern "C" {
#endif

const UmiDeveloperWorkbenchPerspectiveDefinition *
umi_ai_developer_perspective_definition(void);

#ifdef __cplusplus
}
#endif
#endif

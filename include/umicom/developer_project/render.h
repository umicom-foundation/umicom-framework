/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_project/render.h
 *
 * PURPOSE:
 *   Render {{NAME}} project-template variables using a bounded deterministic
 *   engine that does not execute code or evaluate expressions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_PROJECT_RENDER_H
#define UMICOM_DEVELOPER_PROJECT_RENDER_H

#include "umicom/developer_project/variable_set.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_developer_project_render_text(
    const char *template_text,
    const UmiDeveloperProjectVariableSet *variables,
    char *out_text,
    size_t capacity);

#ifdef __cplusplus
}
#endif

#endif

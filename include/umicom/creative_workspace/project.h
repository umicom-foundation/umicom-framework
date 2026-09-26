/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/creative_workspace/project.h
 * PURPOSE: Edit and validate caller-owned creative documents without storage or GUI dependencies.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_CREATIVE_WORKSPACE_PROJECT_H
#define UMICOM_CREATIVE_WORKSPACE_PROJECT_H
#include "umicom/creative_workspace/types.h"
#ifdef __cplusplus
extern "C" {
#endif
    /* All input text is copied. IDs contain ASCII letters, digits, dot, underscore
 * or hyphen, begin with a letter/digit, and are case-sensitive. Labels are
 * single-line UTF-8. Initialisation clears output on failure. */
    UmiStatus UmiCreativeProjectInit(UmiCreativeProject * outProject, const char * id,
    const UmiCreativeSettings * settings);
    UmiStatus UmiCreativeProjectValidate(const UmiCreativeProject * project);
    UmiStatus UmiCreativeElementInit(UmiCreativeElement * outElement, const char * id,
    const char * label, UmiCreativeElementKind kind, int32_t x, int32_t y,
    int32_t width, int32_t height, uint32_t colourRgb);
    /* Draft operations leave the entire document unchanged on failure. Their
 * revision remains unchanged; only a successful workspace commit advances it. */
    UmiStatus UmiCreativeProjectConfigure(UmiCreativeProject * project,
    const UmiCreativeSettings * settings);
    UmiStatus UmiCreativeProjectPutElement(UmiCreativeProject * project,
    const UmiCreativeElement * element, bool replaceExisting);
    UmiStatus UmiCreativeProjectRemoveElement(UmiCreativeProject * project, const char * id);
    UmiStatus UmiCreativeProjectMoveLayer(UmiCreativeProject * project, const char * id, size_t newIndex);
    UmiStatus UmiCreativeProjectFindElement(const UmiCreativeProject * project, const char * id,
    UmiCreativeElement * outElement);
    UmiStatus UmiCreativeProjectPutKey(UmiCreativeProject * project, const char * elementId,
    const UmiCreativeKey * key);
    UmiStatus UmiCreativeProjectRemoveKey(UmiCreativeProject * project, const char * elementId, uint32_t timeMs);
    UmiStatus UmiCreativeProjectPutNote(UmiCreativeProject * project, const UmiCreativeNote * note, bool replaceExisting);
    UmiStatus UmiCreativeProjectRemoveNote(UmiCreativeProject * project, const char * id);
    /* Add a bounded starter only to an empty project. Accepted profile names are
 * media, music, cad, kitchen, games, web-studio and mobile-studio. */
    UmiStatus UmiCreativeProjectStarter(UmiCreativeProject * project, const char * profile);
#ifdef __cplusplus
}

#endif
#endif

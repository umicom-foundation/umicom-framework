/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/project.h
 *
 * PURPOSE:
 *   Manage multiple designer documents inside one product project without tying them to Studio windows or tabs.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This public contract describes reusable visual-designer state and operations.
 * Product-specific windows remain outside the Framework engine.
 */

#ifndef UMICOM_DESIGNER_PROJECT_H
#define UMICOM_DESIGNER_PROJECT_H
#include "umicom/designer/document.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the designer project data shared with callers of this public contract.
 */
typedef struct UmiDesignerProject UmiDesignerProject;
/**
 * Initialise designer project from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_designer_project_create(const char *project_id,UmiDesignerProject **out_project);
/**
 * Release or reset state held by designer project so the same storage can be reused
 * safely.
 */
void umi_designer_project_destroy(UmiDesignerProject *project);
/**
 * Add designer project only after its inputs and available capacity have been checked.
 */
UmiStatus umi_designer_project_add(UmiDesignerProject *project,const char *document_id,UmiDesignerDocument *document,int take_ownership);
/**
 * Find designer project while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_designer_project_find(UmiDesignerProject *project,const char *document_id,UmiDesignerDocument **out_document);
/**
 * Return the number of records represented by designer project without changing their
 * state.
 */
size_t umi_designer_project_count(const UmiDesignerProject *project);
#ifdef __cplusplus
}
#endif
#endif

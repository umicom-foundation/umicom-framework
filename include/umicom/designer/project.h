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
typedef struct UmiDesignerProject UmiDesignerProject;
UmiStatus umi_designer_project_create(const char *project_id,UmiDesignerProject **out_project);
void umi_designer_project_destroy(UmiDesignerProject *project);
UmiStatus umi_designer_project_add(UmiDesignerProject *project,const char *document_id,UmiDesignerDocument *document,int take_ownership);
UmiStatus umi_designer_project_find(UmiDesignerProject *project,const char *document_id,UmiDesignerDocument **out_document);
size_t umi_designer_project_count(const UmiDesignerProject *project);
#ifdef __cplusplus
}
#endif
#endif

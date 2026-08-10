/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/inspector.h
 *
 * PURPOSE:
 *   Build a property-inspector snapshot for one selected semantic component.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This public contract describes reusable visual-designer state and operations.
 * Product-specific windows remain outside the Framework engine.
 */

#ifndef UMICOM_DESIGNER_INSPECTOR_H
#define UMICOM_DESIGNER_INSPECTOR_H
#include "umicom/designer/document.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiDesignerInspectorSnapshot { char node_id[UMI_DECL_ID_CAPACITY]; char component_type[UMI_DECL_ID_CAPACITY]; UmiDeclAttribute attributes[UMI_DECL_MAX_ATTRIBUTES]; size_t attribute_count; } UmiDesignerInspectorSnapshot;
UmiStatus umi_designer_inspect(const UmiDesignerDocument *document,const char *node_id,UmiDesignerInspectorSnapshot *out_snapshot);
#ifdef __cplusplus
}
#endif
#endif

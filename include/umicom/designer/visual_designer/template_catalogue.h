/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/visual_designer/template_catalogue.h
 *
 * PURPOSE:
 *   Maintain a bounded searchable catalogue of page and form templates.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESIGNER_VISUAL_DESIGNER_TEMPLATE_CATALOGUE_H
#define UMICOM_DESIGNER_VISUAL_DESIGNER_TEMPLATE_CATALOGUE_H
#include "umicom/designer/visual_designer/page_template.h"
#include "umicom/designer/visual_designer/form_template.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the visual designer template catalogue data shared with callers of this public contract.
 */
typedef struct UmiRadTemplateCatalogue { UmiRadPageTemplate pages[UMI_RAD_MAX_ITEMS]; size_t page_count; UmiRadFormTemplate forms[UMI_RAD_MAX_ITEMS]; size_t form_count; } UmiRadTemplateCatalogue;
/**
 * Initialise visual designer template catalogue from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_rad_template_catalogue_init(UmiRadTemplateCatalogue *catalogue);
/**
 * Provide the visual designer template catalogue add page operation used by this module and its client
 * applications.
 */
UmiStatus umi_rad_template_catalogue_add_page(UmiRadTemplateCatalogue *catalogue,const UmiRadPageTemplate *item);
/**
 * Provide the visual designer template catalogue add form operation used by this module and its client
 * applications.
 */
UmiStatus umi_rad_template_catalogue_add_form(UmiRadTemplateCatalogue *catalogue,const UmiRadFormTemplate *item);
#ifdef __cplusplus
}
#endif
#endif

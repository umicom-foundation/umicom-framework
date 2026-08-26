/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/rad/template_catalogue.h
 *
 * PURPOSE:
 *   Maintain a bounded searchable catalogue of page and form templates.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESIGNER_RAD_TEMPLATE_CATALOGUE_H
#define UMICOM_DESIGNER_RAD_TEMPLATE_CATALOGUE_H
#include "umicom/designer/rad/page_template.h"
#include "umicom/designer/rad/form_template.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiRadTemplateCatalogue { UmiRadPageTemplate pages[UMI_RAD_MAX_ITEMS]; size_t page_count; UmiRadFormTemplate forms[UMI_RAD_MAX_ITEMS]; size_t form_count; } UmiRadTemplateCatalogue;
UmiStatus umi_rad_template_catalogue_init(UmiRadTemplateCatalogue *catalogue);
UmiStatus umi_rad_template_catalogue_add_page(UmiRadTemplateCatalogue *catalogue,const UmiRadPageTemplate *item);
UmiStatus umi_rad_template_catalogue_add_form(UmiRadTemplateCatalogue *catalogue,const UmiRadFormTemplate *item);
#ifdef __cplusplus
}
#endif
#endif

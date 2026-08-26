/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/rad/template_catalogue.c
 *
 * PURPOSE:
 *   Maintain a bounded searchable catalogue of page and form templates.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/rad/template_catalogue.h"
#include <string.h>
UmiStatus umi_rad_template_catalogue_init(UmiRadTemplateCatalogue *c){if(c==NULL)return UMI_STATUS_INVALID_ARGUMENT;memset(c,0,sizeof *c);return UMI_STATUS_OK;}
UmiStatus umi_rad_template_catalogue_add_page(UmiRadTemplateCatalogue *c,const UmiRadPageTemplate *i){if(c==NULL||i==NULL||!umi_rad_page_template_is_valid(i))return UMI_STATUS_INVALID_ARGUMENT;if(c->page_count>=UMI_RAD_MAX_ITEMS)return UMI_STATUS_CAPACITY_EXCEEDED;c->pages[c->page_count++]=*i;return UMI_STATUS_OK;}
UmiStatus umi_rad_template_catalogue_add_form(UmiRadTemplateCatalogue *c,const UmiRadFormTemplate *i){if(c==NULL||i==NULL||!umi_rad_form_template_is_valid(i))return UMI_STATUS_INVALID_ARGUMENT;if(c->form_count>=UMI_RAD_MAX_ITEMS)return UMI_STATUS_CAPACITY_EXCEEDED;c->forms[c->form_count++]=*i;return UMI_STATUS_OK;}

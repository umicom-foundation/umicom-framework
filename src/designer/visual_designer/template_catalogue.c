/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/visual_designer/template_catalogue.c
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
#include "umicom/designer/visual_designer/template_catalogue.h"
#include <string.h>
/*
 * Initialise visual designer template catalogue from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_rad_template_catalogue_init(UmiRadTemplateCatalogue *c){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(c==NULL)return UMI_STATUS_INVALID_ARGUMENT;memset(c,0,sizeof *c);return UMI_STATUS_OK;}
/*
 * Provide the visual designer template catalogue add page operation used by this module and its client
 * applications.
 */
UmiStatus umi_rad_template_catalogue_add_page(UmiRadTemplateCatalogue *c,const UmiRadPageTemplate *i){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(c==NULL||i==NULL||!umi_rad_page_template_is_valid(i))return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(c->page_count>=UMI_RAD_MAX_ITEMS)return UMI_STATUS_CAPACITY_EXCEEDED;c->pages[c->page_count++]=*i;return UMI_STATUS_OK;}
/*
 * Provide the visual designer template catalogue add form operation used by this module and its client
 * applications.
 */
UmiStatus umi_rad_template_catalogue_add_form(UmiRadTemplateCatalogue *c,const UmiRadFormTemplate *i){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(c==NULL||i==NULL||!umi_rad_form_template_is_valid(i))return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(c->form_count>=UMI_RAD_MAX_ITEMS)return UMI_STATUS_CAPACITY_EXCEEDED;c->forms[c->form_count++]=*i;return UMI_STATUS_OK;}

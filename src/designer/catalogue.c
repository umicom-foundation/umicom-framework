/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/catalogue.c
 *
 * PURPOSE:
 *   Compose the standard declarative component catalogue with the active schema for visual authoring.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * The functions below modify semantic designer state so undo, preview and
 * generation behave the same in Studio, headless tests and future hosts.
 */

#include "umicom/designer/catalogue.h"
#include "umicom/declarative/catalogue.h"
/*
 * Initialise designer catalogue from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_designer_catalogue_create(UmiDeclComponentRegistry **out,UmiDeclSchema *schema){UmiStatus s;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(out==NULL||schema==NULL)return UMI_STATUS_INVALID_ARGUMENT;s=umi_decl_component_registry_create(out);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==UMI_STATUS_OK)s=umi_decl_catalogue_register_defaults(*out);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==UMI_STATUS_OK)s=umi_decl_schema_init(schema,*out);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK){umi_decl_component_registry_destroy(*out);*out=NULL;}return s;}

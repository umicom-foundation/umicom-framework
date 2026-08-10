/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/catalogue.c
 *
 * PURPOSE:
 *   Compose the standard declarative component catalogue with the active schema for visual authoring.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * The functions below modify semantic designer state so undo, preview and
 * generation behave the same in Studio, headless tests and future hosts.
 */

#include "umicom/designer/catalogue.h"
#include "umicom/declarative/catalogue.h"
UmiStatus umi_designer_catalogue_create(UmiDeclComponentRegistry **out,UmiDeclSchema *schema){UmiStatus s;if(out==NULL||schema==NULL)return UMI_STATUS_INVALID_ARGUMENT;s=umi_decl_component_registry_create(out);if(s==UMI_STATUS_OK)s=umi_decl_catalogue_register_defaults(*out);if(s==UMI_STATUS_OK)s=umi_decl_schema_init(schema,*out);if(s!=UMI_STATUS_OK){umi_decl_component_registry_destroy(*out);*out=NULL;}return s;}

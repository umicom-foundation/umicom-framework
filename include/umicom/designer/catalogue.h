/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/catalogue.h
 *
 * PURPOSE:
 *   Create the default designer component registry and schema used by Studio and standalone Umicom Designer hosts.
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

#ifndef UMICOM_DESIGNER_CATALOGUE_H
#define UMICOM_DESIGNER_CATALOGUE_H
#include "umicom/designer/palette.h"
#include "umicom/declarative/schema.h"
#ifdef __cplusplus
extern "C" {
#endif
UmiStatus umi_designer_catalogue_create(UmiDeclComponentRegistry **out_registry,UmiDeclSchema *out_schema);
#ifdef __cplusplus
}
#endif
#endif

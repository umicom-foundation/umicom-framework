/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/declarative/attribute.c
 *
 * PURPOSE:
 *   Implement declarative attribute construction and name matching.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This implementation works on the semantic .umiapp model.  It keeps parsing,
 * validation and generation independent of any particular graphical toolkit.
 */

#include "umicom/declarative/attribute.h"

#include <string.h>

UmiStatus umi_decl_attribute_init(UmiDeclAttribute *attribute, const char *name, UmiDeclValueKind kind, const char *value_text)
{
    UmiStatus status;
    if (attribute == NULL || name == NULL || name[0] == '\0' || value_text == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(attribute, 0, sizeof(*attribute));
    status = umi_decl_copy_text(attribute->name, sizeof(attribute->name), name);
    if (status != UMI_STATUS_OK) return status;
    return umi_decl_value_from_text(kind, value_text, &attribute->value);
}

int umi_decl_attribute_name_equal(const UmiDeclAttribute *attribute, const char *name)
{
    return attribute != NULL && name != NULL && strcmp(attribute->name, name) == 0;
}

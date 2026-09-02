/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/reflection/types.c
 *
 * PURPOSE:
 *   Validate reflection metadata before inspectors, serializers, designers or
 *   application modules are allowed to rely on its memory descriptions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/reflection/types.h"

#include <ctype.h>
#include <string.h>

/* Confirm that a fixed-capacity string contains a terminator in its storage. */
static int text_terminated(const char *text, size_t capacity)
{
    return text != NULL && memchr(text, '\0', capacity) != NULL;
}

/* Reflection identifiers use a portable subset that is safe in logs and JSON. */
static int identifier_valid(const char *identifier, size_t capacity)
{
    size_t index;

    if (!text_terminated(identifier, capacity) || identifier[0] == '\0') {
        return 0;
    }
    for (index = 0U; identifier[index] != '\0'; ++index) {
        unsigned char value = (unsigned char)identifier[index];
        if (!isalnum(value) && value != (unsigned char)'.' &&
            value != (unsigned char)'_' && value != (unsigned char)'-') {
            return 0;
        }
    }
    return 1;
}

/* Alignment values supplied by C are zero or a power of two. */
static int alignment_valid(size_t alignment)
{
    return alignment == 0U || (alignment & (alignment - 1U)) == 0U;
}

UmiReflectionQuery umi_reflection_query_default(void)
{
    UmiReflectionQuery query;

    (void)memset(&query, 0, sizeof(query));
    query.structure_size = (uint32_t)sizeof(query);
    query.limit = UMI_REFLECTION_MAX_QUERY_RESULTS;
    return query;
}

UmiStatus umi_reflection_type_descriptor_validate(
    const UmiReflectionTypeDescriptor *descriptor)
{
    size_t field_index;
    size_t attribute_index;

    if (descriptor == NULL ||
        descriptor->structure_size < sizeof(*descriptor) ||
        descriptor->schema_version == 0U ||
        !identifier_valid(descriptor->type_id, sizeof(descriptor->type_id)) ||
        !identifier_valid(descriptor->module_id, sizeof(descriptor->module_id)) ||
        !text_terminated(descriptor->display_name,
                         sizeof(descriptor->display_name)) ||
        descriptor->display_name[0] == '\0' ||
        !text_terminated(descriptor->description,
                         sizeof(descriptor->description)) ||
        descriptor->kind < UMI_REFLECTION_TYPE_PRIMITIVE ||
        descriptor->kind > UMI_REFLECTION_TYPE_OPAQUE ||
        !alignment_valid(descriptor->alignment) ||
        descriptor->field_count > UMI_REFLECTION_MAX_FIELDS ||
        descriptor->attribute_count > UMI_REFLECTION_MAX_ATTRIBUTES ||
        (descriptor->field_count > 0U && descriptor->fields == NULL) ||
        (descriptor->attribute_count > 0U && descriptor->attributes == NULL)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Concrete C representations must describe at least one byte. */
    if (descriptor->kind != UMI_REFLECTION_TYPE_OPAQUE &&
        descriptor->size == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (descriptor->kind != UMI_REFLECTION_TYPE_STRUCT &&
        descriptor->kind != UMI_REFLECTION_TYPE_UNION &&
        descriptor->field_count != 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    for (field_index = 0U;
         field_index < descriptor->field_count;
         ++field_index) {
        const UmiReflectionFieldDescriptor *field =
            &descriptor->fields[field_index];
        size_t previous_index;

        if (!identifier_valid(field->field_id, sizeof(field->field_id)) ||
            !identifier_valid(field->type_id, sizeof(field->type_id)) ||
            !text_terminated(field->display_name,
                             sizeof(field->display_name)) ||
            field->display_name[0] == '\0' ||
            !text_terminated(field->description,
                             sizeof(field->description)) ||
            field->value_kind < UMI_REFLECTION_VALUE_BOOLEAN ||
            field->value_kind > UMI_REFLECTION_VALUE_CUSTOM ||
            field->size == 0U || field->offset > descriptor->size ||
            field->size > descriptor->size - field->offset) {
            return UMI_STATUS_INVALID_ARGUMENT;
        }
        if ((field->flags & (uint32_t)UMI_REFLECTION_FIELD_ARRAY) != 0U &&
            field->element_count == 0U) {
            return UMI_STATUS_INVALID_ARGUMENT;
        }
        for (previous_index = 0U;
             previous_index < field_index;
             ++previous_index) {
            if (strcmp(descriptor->fields[previous_index].field_id,
                       field->field_id) == 0) {
                return UMI_STATUS_ALREADY_EXISTS;
            }
        }
    }

    for (attribute_index = 0U;
         attribute_index < descriptor->attribute_count;
         ++attribute_index) {
        const UmiReflectionAttribute *attribute =
            &descriptor->attributes[attribute_index];
        if (!identifier_valid(attribute->key, sizeof(attribute->key)) ||
            !text_terminated(attribute->value, sizeof(attribute->value))) {
            return UMI_STATUS_INVALID_ARGUMENT;
        }
    }
    return UMI_STATUS_OK;
}

UmiStatus umi_reflection_module_descriptor_validate(
    const UmiReflectionModuleDescriptor *descriptor)
{
    size_t index;

    if (descriptor == NULL ||
        descriptor->structure_size < sizeof(*descriptor) ||
        !identifier_valid(descriptor->module_id,
                          sizeof(descriptor->module_id)) ||
        !text_terminated(descriptor->display_name,
                         sizeof(descriptor->display_name)) ||
        descriptor->display_name[0] == '\0' ||
        !text_terminated(descriptor->description,
                         sizeof(descriptor->description)) ||
        descriptor->abi_version == 0U ||
        descriptor->module_kind == 0U ||
        descriptor->capability_count > UMI_REFLECTION_MAX_CAPABILITIES) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    for (index = 0U; index < descriptor->capability_count; ++index) {
        if (!identifier_valid(descriptor->capabilities[index],
                              sizeof(descriptor->capabilities[index]))) {
            return UMI_STATUS_INVALID_ARGUMENT;
        }
    }
    return UMI_STATUS_OK;
}

UmiStatus umi_reflection_resource_descriptor_validate(
    const UmiReflectionResourceDescriptor *descriptor)
{
    if (descriptor == NULL ||
        descriptor->structure_size < sizeof(*descriptor) ||
        !identifier_valid(descriptor->resource_id,
                          sizeof(descriptor->resource_id)) ||
        !identifier_valid(descriptor->module_id,
                          sizeof(descriptor->module_id)) ||
        !text_terminated(descriptor->display_name,
                         sizeof(descriptor->display_name)) ||
        descriptor->display_name[0] == '\0' ||
        !text_terminated(descriptor->description,
                         sizeof(descriptor->description)) ||
        !text_terminated(descriptor->resource_uri,
                         sizeof(descriptor->resource_uri)) ||
        descriptor->resource_uri[0] == '\0' ||
        !identifier_valid(descriptor->media_type,
                          sizeof(descriptor->media_type)) ||
        descriptor->kind < UMI_REFLECTION_RESOURCE_LAYOUT ||
        descriptor->kind > UMI_REFLECTION_RESOURCE_DOCUMENTATION) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

const char *umi_reflection_type_kind_text(UmiReflectionTypeKind kind)
{
    switch (kind) {
    case UMI_REFLECTION_TYPE_PRIMITIVE: return "primitive";
    case UMI_REFLECTION_TYPE_ENUM: return "enum";
    case UMI_REFLECTION_TYPE_STRUCT: return "struct";
    case UMI_REFLECTION_TYPE_UNION: return "union";
    case UMI_REFLECTION_TYPE_OPAQUE: return "opaque";
    default: return "unknown";
    }
}

const char *umi_reflection_resource_kind_text(UmiReflectionResourceKind kind)
{
    switch (kind) {
    case UMI_REFLECTION_RESOURCE_LAYOUT: return "layout";
    case UMI_REFLECTION_RESOURCE_PANEL: return "panel";
    case UMI_REFLECTION_RESOURCE_COMMAND: return "command";
    case UMI_REFLECTION_RESOURCE_SERVICE: return "service";
    case UMI_REFLECTION_RESOURCE_SCHEMA: return "schema";
    case UMI_REFLECTION_RESOURCE_THEME: return "theme";
    case UMI_REFLECTION_RESOURCE_DOCUMENTATION: return "documentation";
    default: return "unknown";
    }
}

/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/reflection/types.h
 *
 * PURPOSE:
 *   Define portable metadata for C types, fields, modules and runtime resources
 *   so Framework applications can inspect capabilities without guessing memory.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_REFLECTION_TYPES_H
#define UMICOM_REFLECTION_TYPES_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/base/version.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_REFLECTION_ID_CAPACITY 128U
#define UMI_REFLECTION_NAME_CAPACITY 160U
#define UMI_REFLECTION_DESCRIPTION_CAPACITY 512U
#define UMI_REFLECTION_URI_CAPACITY 512U
#define UMI_REFLECTION_ATTRIBUTE_KEY_CAPACITY 96U
#define UMI_REFLECTION_ATTRIBUTE_VALUE_CAPACITY 256U
#define UMI_REFLECTION_MAX_FIELDS 64U
#define UMI_REFLECTION_MAX_ATTRIBUTES 16U
#define UMI_REFLECTION_MAX_CAPABILITIES 32U
#define UMI_REFLECTION_MAX_QUERY_RESULTS 256U

/** Describe the broad C representation of a registered type. */
typedef enum UmiReflectionTypeKind {
    UMI_REFLECTION_TYPE_PRIMITIVE = 1,
    UMI_REFLECTION_TYPE_ENUM = 2,
    UMI_REFLECTION_TYPE_STRUCT = 3,
    UMI_REFLECTION_TYPE_UNION = 4,
    UMI_REFLECTION_TYPE_OPAQUE = 5
} UmiReflectionTypeKind;

/** Describe the value stored by one reflected field. */
typedef enum UmiReflectionValueKind {
    UMI_REFLECTION_VALUE_BOOLEAN = 1,
    UMI_REFLECTION_VALUE_SIGNED_INTEGER = 2,
    UMI_REFLECTION_VALUE_UNSIGNED_INTEGER = 3,
    UMI_REFLECTION_VALUE_REAL = 4,
    UMI_REFLECTION_VALUE_TEXT = 5,
    UMI_REFLECTION_VALUE_BYTES = 6,
    UMI_REFLECTION_VALUE_ENUM = 7,
    UMI_REFLECTION_VALUE_STRUCT = 8,
    UMI_REFLECTION_VALUE_POINTER = 9,
    UMI_REFLECTION_VALUE_CUSTOM = 10
} UmiReflectionValueKind;

/** Explain how tools may safely display or modify a reflected field. */
typedef enum UmiReflectionFieldFlags {
    UMI_REFLECTION_FIELD_NONE = 0U,
    UMI_REFLECTION_FIELD_READ_ONLY = 1U << 0U,
    UMI_REFLECTION_FIELD_REQUIRED = 1U << 1U,
    UMI_REFLECTION_FIELD_NULLABLE = 1U << 2U,
    UMI_REFLECTION_FIELD_ARRAY = 1U << 3U,
    UMI_REFLECTION_FIELD_TRANSIENT = 1U << 4U,
    UMI_REFLECTION_FIELD_SENSITIVE = 1U << 5U,
    UMI_REFLECTION_FIELD_DEPRECATED = 1U << 6U
} UmiReflectionFieldFlags;

/** Identify data-driven resources that can appear without recompiling a host. */
typedef enum UmiReflectionResourceKind {
    UMI_REFLECTION_RESOURCE_LAYOUT = 1,
    UMI_REFLECTION_RESOURCE_PANEL = 2,
    UMI_REFLECTION_RESOURCE_COMMAND = 3,
    UMI_REFLECTION_RESOURCE_SERVICE = 4,
    UMI_REFLECTION_RESOURCE_SCHEMA = 5,
    UMI_REFLECTION_RESOURCE_THEME = 6,
    UMI_REFLECTION_RESOURCE_DOCUMENTATION = 7
} UmiReflectionResourceKind;

/** Describe one searchable piece of additional metadata. */
typedef struct UmiReflectionAttribute {
    char key[UMI_REFLECTION_ATTRIBUTE_KEY_CAPACITY];
    char value[UMI_REFLECTION_ATTRIBUTE_VALUE_CAPACITY];
} UmiReflectionAttribute;

/**
 * Describe a field without exposing a live pointer to an object.  Offset and
 * size are used only after the owning type has passed validation.
 */
typedef struct UmiReflectionFieldDescriptor {
    char field_id[UMI_REFLECTION_ID_CAPACITY];
    char display_name[UMI_REFLECTION_NAME_CAPACITY];
    char type_id[UMI_REFLECTION_ID_CAPACITY];
    char description[UMI_REFLECTION_DESCRIPTION_CAPACITY];
    UmiReflectionValueKind value_kind;
    size_t offset;
    size_t size;
    size_t element_count;
    uint32_t flags;
} UmiReflectionFieldDescriptor;

/** Describe a C type supplied by generated code or an explicit declaration. */
typedef struct UmiReflectionTypeDescriptor {
    uint32_t structure_size;
    uint32_t schema_version;
    char type_id[UMI_REFLECTION_ID_CAPACITY];
    char display_name[UMI_REFLECTION_NAME_CAPACITY];
    char module_id[UMI_REFLECTION_ID_CAPACITY];
    char description[UMI_REFLECTION_DESCRIPTION_CAPACITY];
    UmiReflectionTypeKind kind;
    size_t size;
    size_t alignment;
    const UmiReflectionFieldDescriptor *fields;
    size_t field_count;
    const UmiReflectionAttribute *attributes;
    size_t attribute_count;
} UmiReflectionTypeDescriptor;

/** Return type metadata in caller-owned storage that remains valid after mutations. */
typedef struct UmiReflectionTypeSnapshot {
    UmiReflectionTypeDescriptor descriptor;
    UmiReflectionFieldDescriptor fields[UMI_REFLECTION_MAX_FIELDS];
    UmiReflectionAttribute attributes[UMI_REFLECTION_MAX_ATTRIBUTES];
} UmiReflectionTypeSnapshot;

/** Describe a discoverable compiled or data-only module. */
typedef struct UmiReflectionModuleDescriptor {
    uint32_t structure_size;
    char module_id[UMI_REFLECTION_ID_CAPACITY];
    char display_name[UMI_REFLECTION_NAME_CAPACITY];
    char description[UMI_REFLECTION_DESCRIPTION_CAPACITY];
    UmiVersion version;
    uint32_t abi_version;
    uint32_t module_kind;
    uint32_t flags;
    char capabilities[UMI_REFLECTION_MAX_CAPABILITIES]
                     [UMI_REFLECTION_ID_CAPACITY];
    size_t capability_count;
} UmiReflectionModuleDescriptor;

/** Describe one resource contributed by a module or a user directory. */
typedef struct UmiReflectionResourceDescriptor {
    uint32_t structure_size;
    char resource_id[UMI_REFLECTION_ID_CAPACITY];
    char module_id[UMI_REFLECTION_ID_CAPACITY];
    char display_name[UMI_REFLECTION_NAME_CAPACITY];
    char description[UMI_REFLECTION_DESCRIPTION_CAPACITY];
    char resource_uri[UMI_REFLECTION_URI_CAPACITY];
    char media_type[UMI_REFLECTION_ID_CAPACITY];
    UmiReflectionResourceKind kind;
    uint32_t flags;
    uint64_t content_hash;
    uint64_t revision;
} UmiReflectionResourceDescriptor;

/** Select a bounded subset of registered metadata. Empty text fields mean any. */
typedef struct UmiReflectionQuery {
    uint32_t structure_size;
    char module_id[UMI_REFLECTION_ID_CAPACITY];
    char text[UMI_REFLECTION_NAME_CAPACITY];
    UmiReflectionTypeKind type_kind;
    UmiReflectionResourceKind resource_kind;
    size_t offset;
    size_t limit;
} UmiReflectionQuery;

/** Hold stable identifiers returned by a bounded registry query. */
typedef struct UmiReflectionQueryResult {
    uint32_t structure_size;
    char ids[UMI_REFLECTION_MAX_QUERY_RESULTS][UMI_REFLECTION_ID_CAPACITY];
    size_t count;
    size_t total_available;
    int truncated;
} UmiReflectionQueryResult;

/** Return sensible query defaults with no filters and a bounded result size. */
UmiReflectionQuery umi_reflection_query_default(void);

/** Check type metadata, field bounds, identifiers and duplicate field names. */
UmiStatus umi_reflection_type_descriptor_validate(
    const UmiReflectionTypeDescriptor *descriptor);

/** Check module identity, capability counts and fixed-string termination. */
UmiStatus umi_reflection_module_descriptor_validate(
    const UmiReflectionModuleDescriptor *descriptor);

/** Check a data-driven resource before it enters the shared catalogue. */
UmiStatus umi_reflection_resource_descriptor_validate(
    const UmiReflectionResourceDescriptor *descriptor);

/** Return a stable name for logs, inspectors and generated documentation. */
const char *umi_reflection_type_kind_text(UmiReflectionTypeKind kind);

/** Return a stable resource name for catalogues and change-event payloads. */
const char *umi_reflection_resource_kind_text(UmiReflectionResourceKind kind);

#ifdef __cplusplus
}
#endif

#endif

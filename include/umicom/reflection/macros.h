/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/reflection/macros.h
 *
 * PURPOSE:
 *   Reduce repetitive C reflection declarations while keeping every offset,
 *   size, value kind and safety flag visible in normal source code.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_REFLECTION_MACROS_H
#define UMICOM_REFLECTION_MACROS_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/reflection/types.h"

/** Count records in a compile-time metadata array. */
#define UMI_REFLECTION_ARRAY_COUNT(ARRAY) \
    (sizeof(ARRAY) / sizeof((ARRAY)[0]))

/**
 * Describe one ordinary structure member. The compiler calculates the offset
 * and size, avoiding hand-written numbers that can become incorrect later.
 */
#define UMI_REFLECTION_FIELD(TYPE, MEMBER, TYPE_ID, VALUE_KIND, FLAGS, TEXT) \
    {                                                                        \
        .field_id = #MEMBER,                                                 \
        .display_name = #MEMBER,                                             \
        .type_id = TYPE_ID,                                                  \
        .description = TEXT,                                                 \
        .value_kind = (VALUE_KIND),                                          \
        .offset = offsetof(TYPE, MEMBER),                                    \
        .size = sizeof(((TYPE *)0)->MEMBER),                                 \
        .element_count = 1U,                                                 \
        .flags = (uint32_t)(FLAGS)                                           \
    }

/**
 * Describe a fixed C array and record its element count for inspectors and
 * serializers that need to stay inside the declared storage.
 */
#define UMI_REFLECTION_ARRAY_FIELD(                                          \
    TYPE, MEMBER, TYPE_ID, VALUE_KIND, FLAGS, TEXT)                          \
    {                                                                        \
        .field_id = #MEMBER,                                                 \
        .display_name = #MEMBER,                                             \
        .type_id = TYPE_ID,                                                  \
        .description = TEXT,                                                 \
        .value_kind = (VALUE_KIND),                                          \
        .offset = offsetof(TYPE, MEMBER),                                    \
        .size = sizeof(((TYPE *)0)->MEMBER),                                 \
        .element_count = sizeof(((TYPE *)0)->MEMBER) /                       \
                         sizeof(((TYPE *)0)->MEMBER[0]),                     \
        .flags = (uint32_t)(FLAGS) |                                        \
                 (uint32_t)UMI_REFLECTION_FIELD_ARRAY                       \
    }

#endif

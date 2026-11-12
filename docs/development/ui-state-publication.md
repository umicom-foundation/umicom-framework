![Umicom](../../resources/brand/umicom-logo.svg)

# Keep UI state intact when an edit is refused

A failed edit should not leave a view half changed. A common mistake is to update the first field, discover that the second is too long, and return an error. The caller sees failure, but part of its previous state has already disappeared.

The reusable component, cell and theme services prepare a candidate value first. They publish it only after all required inputs fit. This rule also handles borrowed text: an input pointer may refer to a field in the object being edited.

## Give each value one owner

These APIs do not add a lock. Keep mutation on the owning thread or provide application synchronization for the whole operation. A complete publication on one thread does not make concurrent unsynchronized access valid.

~~~c
#include "umicom/ui/components/accessibility.h"

UmiStatus swap_accessible_fields(UmiUiComponentAccessibility *value)
{
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return umi_ui_component_accessibility_set(
        value, value->description, value->name);
}
~~~

The service captures both fields before writing either destination field. If either input exceeds its byte capacity, the complete old value remains unchanged. A null name or description retains the established meaning of an empty field.

## Reuse bounded text replacement

Component ID/text setters and enterprise text copies use **umi_text_replace** from Framework's base service. They inspect at most the destination capacity, support overlapping input and preserve the destination when the input does not fit. Supply readable storage through the terminator or that capacity; a pointer alone cannot prove how much memory is accessible.

The enterprise cell text setter stages the complete typed value. A borrowed suffix such as **cell.text + 1** is captured before the old cell is replaced. A refused text edit preserves a previous integer, real, boolean or text value.

Cell validation checks only the active kind. A text value needs a terminator within its fixed field. A boolean needs zero or one. Numeric and empty values ignore inactive text storage. Equality rejects malformed values before a string comparison; ordinary floating-point equality is retained, including NaN comparing unequal.

## Admit registries before using them

A registry is a public bounded record. Validate its count and every active component before searching. Otherwise, a malformed later row may remain hidden behind an early successful match, or a count may cause traversal beyond the allocated array.

**umi_ui_component_registry_validate** checks the active specifications and unique IDs. It ignores inactive storage. Add and find use that same admission rule. A valid full registry still reports an existing ID before reporting capacity for a new ID.

Find requires output storage separate from the registry because a const lookup must not secretly overwrite a registry row. Its query may borrow the separate output's ID: the query is captured before a successful result is copied. Refused lookups preserve output.

## Publish canonical themes as complete values

Theme initialization prepares both labels, mode, density, initial revision, font scale and the shared brand palette in staging storage. It commits after the palette and labels are ready. It does not redraw a window, select a font or save preferences; those are separate owner workflows.

The existing palette remains authoritative. Applications should use Framework's brand resources and typography settings instead of adding another colour or logo implementation.

## Verify the failure paths

1. Initialize a valid value and save a byte-for-byte snapshot with **memcpy**.
2. Submit a nonterminated field with exactly the documented capacity of readable bytes.
3. Check the returned capacity error and compare the unchanged destination with the snapshot.
4. Repeat with text borrowed from the destination, including swapped fields and suffixes.
5. For registries, include excessive counts, malformed later rows, duplicate IDs and full-capacity cases.
6. Keep memory checks separate from native UI rendering, input and accessibility qualification.

The regression sources are [registry admission](../../tests/ui_components/test_ui_component_registry.c), [accessibility publication](../../tests/ui_components/test_accessibility_publication.c), [cell values](../../tests/ui_enterprise/test_cell_value.c) and [theme publication](../../tests/ui_components/test_theme_publication.c). They use explicit checks, so required calls still execute when **NDEBUG** disables standard assertions.

The [C++ UI consumer example](../../examples/ui_cpp_consumer/README.md) exercises the same shared C services. No application-specific copy of this logic is needed.

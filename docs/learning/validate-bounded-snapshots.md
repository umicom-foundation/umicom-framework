# Check snapshot text before storing it

A snapshot is a C structure that describes one item at a particular moment. A
bookmark snapshot, for example, holds an identifier, a location and a label.
The registry stores its own copy of that structure. Changing your input later
does not edit the saved bookmark.

Many Framework snapshots keep text in fixed-size `char` arrays. A field with
128 bytes can hold at most 127 bytes of text followed by `\0`, the byte that
marks the end of a C string. That limit counts bytes, not displayed characters.
Some characters need more than one byte in UTF-8.

The snapshot validators check these boundaries before the registry looks up an
identifier or copies an item. They also tell you which field needs attention.

## 1. Start with a complete, initialised structure

Include the header for the service you use. This example uses bookmarks:

```c
#include "umicom/platform/bookmarks.h"
#include <stdio.h>

UmiBookmarkSnapshot item = {0};
(void)snprintf(item.id, sizeof(item.id), "%s", "manual");
(void)snprintf(item.label, sizeof(item.label), "%s", "Beginner manual");
(void)snprintf(item.uri, sizeof(item.uri), "%s", "file:///example/manual.html");
```

These short, fixed example strings fit the destination arrays. For input of an
unknown length, check `snprintf`'s result: a negative result reports an error;
a result greater than or equal to the array capacity means the text did not
fit. Decide how your application should handle that input before storing it.
A validator cannot recover text that a producer already truncated.

Always provide an actual readable structure of the declared type. The
`struct_size` member is metadata, not permission to pass a smaller allocation.
Existing upsert functions still write the current structure size and API
version into the registry's stored copy.

## 2. Validate and explain a rejected field

```c
UmiSnapshotValidation detail;
UmiStatus status = umi_platform_bookmarks_snapshot_validate(&item, &detail);
if (status != UMI_STATUS_OK) {
    fprintf(stderr, "%s: %s\n",
        detail.field != NULL ? detail.field : "snapshot",
        UmiSnapshotIssueText(detail.issue));
    /* Correct the producer's input before attempting to store it. */
}
```

The identifier must contain at least one byte before its terminator. Other
text fields may be empty, but every fixed text array needs a terminator within
its own capacity. A zero byte in the next field does not count.

The diagnostic contains a field name and a reason. It does not copy the
record's text. Typed validators return field names with static lifetime, so
you do not free them. The zero-based `field_index` follows the snapshot's
text-field declaration order. `SIZE_MAX` means no individual field applies.
Passing `NULL` for the diagnostic is allowed when you only need the status.

Validation does not allocate memory or modify the input. A successful check
clears earlier diagnostic details. `UmiSnapshotIssueText` returns a static
explanation that you do not free.

## 3. Store only accepted input

Create a registry with `umi_platform_bookmarks_registry_create`, check its
return status, then call `umi_platform_bookmarks_registry_upsert`. Finally,
release the registry with `umi_platform_bookmarks_registry_destroy`.

The upsert function performs the same text checks itself. Calling the validator
first is useful for displaying a field-level message; it is not a requirement
for safe upsert. Keep the input stable during each call and keep registry
access on its owning thread.

Previously, some registries checked only the first identifier byte, and some
forced the last byte of copied text to zero. An unterminated array is now
rejected with `UMI_STATUS_INVALID_ARGUMENT` before lookup or mutation. Fix the
producer by writing a complete, terminated value. The registry does not silently
truncate malformed input. Existing signatures, valid-input storage rules,
insertion order and size/version normalisation remain available.

## 4. Understand what the check means

This check establishes text boundaries. It does not establish that a URL exists,
a path is permitted, an enum value is supported, UTF-8 is well formed, or a
debugger or source-control operation is authorised. Those rules belong to the
service that interprets the value. Optional fields can still be required by a
particular higher-level workflow.

The new validation applies to snapshot upserts in the registries listed in
[Import several snapshots together](import-registry-batches.md#available-registries).
Other APIs that accept `const char *`, such as identifier lookup, still require
ordinary readable, terminated C strings. This feature does not change those
caller obligations.

For your own fixed-array record, `umicom/base/snapshot_validation.h` also exposes
`UmiSnapshotValidateTextFields`. Describe each array with `offsetof`, its actual
`sizeof` and a required flag of zero or one. Supply the actual readable record
size and descriptor count. The function checks every descriptor's range before
reading any field. Descriptor labels are borrowed, so keep them alive for as
long as you use the returned diagnostic. Keep the output separate from the
record and descriptor storage. Do not describe a `char *` as an embedded array.

## 5. Try a complete example

[Import several snapshots together](import-registry-batches.md) walks through
the runnable bookmark lesson. It deliberately creates one unterminated label,
checks that nothing was imported, fixes the label and retries successfully.

If validation fails unexpectedly, inspect the named field's producer. Check its
capacity, its terminator and whether an earlier copy reported truncation. Do not
make the array appear valid by discarding the user's last byte without telling
them; choose an explicit input policy in the calling application.

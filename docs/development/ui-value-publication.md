![Umicom](../../resources/brand/umicom-logo.svg)

# Update UI values without losing the previous state

A UI value is a small owned record. It stores a kind, such as INTEGER or STRING, and the corresponding data. A property bag copies these records so an application can change its own input later without changing a published property.

The shared Framework value service defines how to copy, compare and replace these records. Studio, Trader and other clients should use that service rather than adding their own fixed-buffer string handling.

## 1. Prepare a value and check each result

~~~c
#include "umicom/ui/value.h"

UmiUiValue title;
UmiStatus status = umi_ui_value_set_string(&title, "Umicom Studio");
if (status != UMI_STATUS_OK) {
    /* Report the refusal before publishing anything. */
    return status;
}
~~~

The string capacity is 512 bytes, including the terminating zero byte. This is a byte limit, not a count of displayed characters. A successful string replacement clears the inactive scalar fields and unused string tail.

A null argument returns INVALID_ARGUMENT. Text without a terminator in the supported range returns CAPACITY_EXCEEDED. A refused replacement preserves the entire destination, including its previous kind. The caller still owns the last accepted value.

## 2. Understand why staging matters

The following update is supported:

~~~c
UmiUiValue path;
UmiStatus status = umi_ui_value_set_string(&path, "prefix/Umicom");
if (status != UMI_STATUS_OK) return status;

status = umi_ui_value_set_string(&path, path.string_value + 7U);
if (status != UMI_STATUS_OK) return status;
/* path now owns the string "Umicom". */
~~~

The input points into the destination record. Clearing the destination first would erase that input before copying it. Framework instead prepares a temporary value, checks the bounded terminator and publishes the complete candidate only after success.

The caller must supply readable text through its terminator or through the capacity limit. This API cannot determine whether an arbitrary pointer names readable memory. Use a complete value's string field, a normal C string or a suitably sized input buffer. Do not pass a pointer near the end of an unterminated allocation.

The operation copies bytes. It does not validate UTF-8 or normalize Unicode. Applications accepting user text should apply their documented encoding policy before publication.

## 3. Validate records received from another component

A plug-in or restored object can construct the public record directly. Check its active payload before treating it as a value:

~~~c
if (!umi_ui_value_valid(&received)) {
    /* Keep the previous display and report malformed component data. */
    return UMI_STATUS_INVALID_ARGUMENT;
}
UmiStatus status = umi_ui_value_copy(&current, &received);
if (status != UMI_STATUS_OK) return status;
~~~

Validation accepts the five defined kinds. STRING requires a terminator inside its fixed field. Unknown kinds are rejected. NONE has no active payload.

Scalar domains remain compatible with existing behavior: INTEGER uses its signed range; BOOLEAN comparison uses the stored integer; REAL keeps ordinary floating-point semantics, including NaN and infinity. A financial quantity or dimension may need a stricter application rule. Generic UI admission does not replace that domain validation.

Only the active payload is inspected. An INTEGER with unused string bytes is a valid UI value. This distinction matters for archives, which may serialize more fields than the active UI payload.

## 4. Compare and publish through their owners

~~~c
if (!umi_ui_value_equal(&current, &proposed)) {
    UmiStatus status = umi_ui_property_bag_set(properties, "title", &proposed);
    if (status != UMI_STATUS_OK) return status;
}
~~~

Comparison validates both records before reading their string contents. Invalid records compare unequal, including an invalid record compared with itself. Two NONE values compare equal. NaN remains unequal to itself; ordinary equality is therefore not a universal identity test for every REAL value.

For strings, equality compares bytes through the terminator. It does not compare inactive fields or perform locale-aware text matching.

The property bag validates and copies the value before changing its catalogue. A malformed value cannot add a new key, replace an existing property, change its count or advance its revision. After a successful update, changing the caller's source record does not change the stored property.

The bag serializes its own mutations. The caller must still serialize access to its source record while another thread copies it. A property snapshot is an observation, not permission to mutate a different service.

## 5. Keep archive validation distinct

Reactive observable and computed records reuse the shared active-value contract. Their archive format also includes the fixed string field, so they continue to require its terminator even when the active kind is INTEGER.

Do not remove this archive check because the generic value validator accepts inactive text bytes. The two contracts serve different purposes: one admits a current UI payload; the other ensures every serialized field is readable and bounded.

Unknown embedded kinds are refused before archive encoding changes output bytes or the reported size. Existing valid archive schemas and record layouts remain unchanged.

## 6. Qualify the behavior

Run these commands separately and stop on errors:

~~~powershell
Set-Location "C:\umicom\Umicom-Applications\framework"
cmake --preset windows-ucrt64-debug
cmake --build --preset windows-ucrt64-debug --target umicom-ui-value-publication-test
ctest --test-dir build/windows-ucrt64-debug --output-on-failure -R "^framework[.]ui[.]value_publication[.]"
~~~

The authored cases inspect bounded and aliased strings, all defined kinds, unknown kinds, malformed fixed text, refusal preservation, copy independence and reactive archive admission. They use the actual shared library and do not open a window.

These commands are owner-run qualification steps. Their presence in this guide is not a claim that the current source has compiled or passed. If a case fails, retain the command, source revision and complete diagnostic. Verify that the test executable was rebuilt successfully before interpreting its result.

For values spanning the full unsigned range, continue with [exact unsigned observations](unsigned-ui-values.md). For application-specific numeric policy, validate the domain value before converting it into a display record.

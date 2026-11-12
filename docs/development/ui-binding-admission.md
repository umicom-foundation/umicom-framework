![Umicom](../../resources/brand/umicom-logo.svg)

# Admit a binding before publishing its transformed value

A property binding connects a source view model to a target view model. Framework copies the binding descriptor into a registry, but the registry borrows the two view pointers and any transform context. Keep those objects alive until registration and any in-progress propagation have ended.

The existing view model already advances its revision only when its property owner accepts an update. Binding admission builds on that behavior instead of duplicating property storage.

## 1. Prepare a bounded descriptor

Initialize the complete descriptor before filling it. Its binding identifier and both property names are fixed fields.

~~~c
UmiUiBindingDescriptor binding = {0};
/* Copy reviewed bounded names into binding_id, source_property and
 * target_property, then assign live source_view and target_view pointers. */
binding.enabled = 1;
UmiStatus status = umi_ui_binding_register(registry, &binding);
if (status != UMI_STATUS_OK) {
    /* Keep the existing registry and report the refused registration. */
}
~~~

The identifier uses the existing shared character rules. Identifier validation reads at most UMI_UI_ID_CAPACITY bytes, stopping at a terminator. Empty, overlong and unterminated identifiers are invalid. Property names must be nonempty and terminate inside their own fields.

These checks run before the registry changes. A malformed descriptor cannot consume a slot. Duplicate registration still reports ALREADY_EXISTS; a full registry reports CAPACITY_EXCEEDED for another valid identifier.

## 2. Assign a complete transform result

A transform receives a copied source value and temporary output storage. Returning OK means the callback assigned a complete valid target. Use shared value setters or copying functions.

~~~c
static UmiStatus EnabledLabel(const UmiUiValue *source,
    UmiUiValue *target, void *context)
{
    (void)context;
    if (source->kind != UMI_UI_VALUE_BOOLEAN)
        return UMI_STATUS_INVALID_ARGUMENT;
    return umi_ui_value_set_string(target,
        source->boolean_value ? "Enabled" : "Disabled");
}
~~~

Framework starts transform output as deterministic invalid scratch. A callback that returns OK without assigning a result is refused with INVALID_ARGUMENT. Unsupported kinds and unterminated STRING results are refused too. A callback can intentionally publish NONE by calling umi_ui_value_clear.

Do not treat the scratch record as an initial copy of the source. If no transformation is needed, leave the transform pointer null; Framework copies the source through the shared value contract.

## 3. Preserve the target when a callback refuses

A callback's non-OK status is returned without publishing its partial output. If it writes text and then returns IO_ERROR, the previous target property remains intact. The target view and property revisions remain unchanged.

This is separate from a callback that returns OK with a malformed value. That callback has not satisfied its success contract, so Framework reports INVALID_ARGUMENT before the target owner is called.

The source observation is temporary and read-only. Do not retain either source or target pointer after the callback returns.

## 4. Respect borrowed lifetimes and callback scope

Callbacks execute inside registry propagation. Do not reenter or mutate that registry, destroy its source or target views, or wait for work that requires the same registry. This restriction protects the existing borrowed-lifetime design; the current implementation does not provide a general callback reentrancy service.

Registry destruction precedes destruction of borrowed views and context, after in-progress operations have completed. A mutex protects registry storage; it does not automatically extend the lifetime of objects stored as borrowed pointers.

Native UI callbacks, cross-thread shutdown and arbitrary reentrant transforms require separate qualification. The admission changes do not claim those workflows are safe or newly implemented.

## 5. Interpret propagation of all bindings correctly

Propagation of all bindings runs in registration order and continues after a failure. It returns the first failure. Its count includes OK results, including disabled bindings that perform no work.

This is not an atomic transaction across several views. A later binding can succeed after an earlier one fails, and accepted updates remain visible. Handle the returned failure and inspect the relevant owners rather than assuming every target changed together.

For a single binding, an absent source property returns NOT_FOUND without calling the transform. A disabled binding remains an OK no-op. Existing identity bindings continue copying their source through Framework.

## 6. Qualify the concrete boundaries

Run each command separately and stop on errors:

~~~powershell
Set-Location "C:\umicom\Umicom-Applications\framework"
cmake --preset windows-ucrt64-debug
cmake --build --preset windows-ucrt64-debug --target umicom-ui-binding-admission-test
ctest --test-dir build/windows-ucrt64-debug --output-on-failure -R "^framework[.]ui[.]binding_admission[.]"
~~~

The 44 authored cases cover bounded identifiers, fixed descriptor text, registry limits, successful transforms, no-write callbacks, malformed values, callback errors, disabled and missing-source behavior, and mixed propagation outcomes. They inspect target property and view revisions through the real owners.

These checks have not been executed for the current source. Compiler, CTest and native application behavior remain owner-run and unverified. Retain complete diagnostics and tested source identity if qualification fails.

Continue with [owned value publication](ui-value-publication.md) and [reactive state admission](reactive-value-admission.md) for the shared contracts used by this binding boundary.

![Umicom](../../resources/brand/umicom-logo.svg)

# Validate a theme before presenting it

Umicom Framework keeps semantic colours and typography separate from native widgets. An appearance profile describes the application, a theme profile carries a smaller palette, and an editor snapshot describes the source editor. A valid-looking record is not necessarily safe for a renderer to consume.

Use the canonical Umicom palette, SVG resources, ICO and font defaults. Validation checks the record's structure and supported values; it does not establish that a font is installed or that a logo is visible in a particular native window.

## Follow the ownership sequence

1. Create an appearance or theme through its initializer. The initializer supplies the canonical palette and defaults.
2. Keep proposed edits separate from the last accepted value.
3. Validate all fixed fields and numeric values before publishing a proposal.
4. Resolve an editor snapshot through **umi_ui_editor_theme_resolve**. The resolver stages and validates the complete result before replacing its output.
5. Apply an accepted snapshot through the appropriate toolkit adapter, then qualify the native presentation separately.

~~~c
UmiUiEditorThemeSnapshot next;
UmiStatus status = umi_ui_editor_theme_resolve(&appearance, &next);
if (status == UMI_STATUS_OK) {
    /* Publish next through the existing editor adapter. */
}
~~~

If resolution fails, its destination remains unchanged. The resolver captures input before publication, so readable appearance fields may borrow an overlapping writable destination allocation. The caller must provide both complete object spans and serialize access. Staging is not a synchronization mechanism.

## Check finiteness before a numeric range

A comparison such as **size < 6.0 || size > 96.0** does not reject NaN, the floating-point value meaning "not a number." Both comparisons are false. An explicit finiteness check must come before treating the range as admitted.

The existing general appearance validator already rejects nonfinite sizes and scale. The theme profile and editor validator now apply the same rule. Theme scale remains in [0.75, 2.0], and editor snapshot size remains in [6.0, 96.0]. The appearance profile retains its separately documented ranges before scaling.

An admitted number still does not guarantee readable text. Native DPI scaling, available fonts, glyph coverage and accessibility behavior require presentation checks.

## Bound every text field

The shared internal helper accepts exactly **#RRGGBB**, with six hexadecimal digits and a terminating zero. It examines only the bytes required by that format within the supplied field capacity. It does not accept a lone hash, short CSS colour, named colour or alpha suffix.

Theme profiles now validate all seven semantic colour fields, their nonempty bounded identity/name, declared mode/density and finite scale. Editor snapshots validate all colours, font lists and every active preferred scheme identifier before borrowing one.

The font-list grammar is the existing appearance grammar: ASCII letters, digits, spaces, commas, hyphens, underscores and periods, with an alphanumeric name in each comma-separated family. Empty families are refused. The change reuses that shared rule rather than letting the editor invent a different syntax.

Hexadecimal letter case does not change a colour. An editor background **#abcdef** and current line **#ABCDEF** are the same value and fail the existing distinguishable-line rule. A dark editor's prohibition on opaque white current-line highlighting also applies to **#ffffff**. These rules do not calculate a contrast ratio or claim general accessibility compliance.

## Keep explanations outside the inspected object

A validator accepts a reason buffer so the application can explain refusal. Passing a field of the inspected theme as that buffer would overwrite the theme during what appears to be a read-only operation.

~~~c
char reason[256];
UmiStatus status = umi_ui_theme_profile_validate(&theme, reason, sizeof(reason));
/* Keep reason separate from theme. */
~~~

All three validators reject an overlapping diagnostic buffer with **INVALID_ARGUMENT** and no write. A separate buffer that is too short retains the existing behavior: a terminated truncated explanation and **CAPACITY_EXCEEDED**. That status describes insufficient explanation storage, so use a sufficiently sized separate buffer when you need the underlying validation result.

Preferred-scheme lookup validates the complete editor snapshot before indexing its scheme array. For admitted separate output storage, failure sets the output pointer to null. Missing arguments or an output pointer overlapping the snapshot are refused without a write. A successful borrowed name expires when the snapshot changes or its storage is released.

## Qualify the changes

Run each command separately from the Framework source root in the documented native toolchain environment. Stop on errors.

~~~powershell
cmake -S . -B build/theme-admission -G Ninja -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON
cmake --build build/theme-admission --target umicom-ui-theme_profile_admission-test umicom-ui-editor_theme_admission-test umicom-ui-editor_theme_resolution-test
ctest --test-dir build/theme-admission -R "^framework[.]ui[.]validation[.]" --output-on-failure
~~~

The three authored memory-only checks cover nonfinite sizes, range endpoints, malformed colours, bounded labels/fonts/schemes, case-equivalent colours, oversized scheme counts, overlapping diagnostic output, unchanged failed resolution and borrowed input during successful resolution.

Then run the existing appearance catalogue, persistence, editor-theme, theme publication and native adapter checks as part of broader qualification. A passing source-level test does not establish Windows ICO appearance, GTK/Qt text rendering, installed font fallback or successful application launch. No execution results are claimed by these expected steps.

## Practice diagnosis

Set a copied theme's scale to NaN in a controlled fixture. Predict the old range-only result and the new finite-range result. Next, provide a reason buffer inside the copy and explain why even a valid theme must refuse that output location.

Finally, give two identical RGB colours different letter case. Explain why comparing strings is insufficient for a rule about visible colour. Preserve each failing input and its observed status when reporting a regression; do not replace the shared validator in application code.

## Related token storage lesson

The [semantic theme-token storage lesson](semantic-theme-token-storage.md) explains
bounded text, stable identity, value-copy ownership and catalogue refusal. Token
storage admits complete text; the profile and editor rules in this guide add
presentation-specific meaning.

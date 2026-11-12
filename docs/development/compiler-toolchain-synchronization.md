![Umicom](../../resources/brand/umicom-logo.svg)

# Keep compiler selection and profiles together

A compiler provider records a driver's identity and argument mapping. A compiler profile adds a target, language standard and policy. Changing one while keeping the other stale can make a preview name an unexpected compiler or target. Framework supplies an explicit publication operation for this pair.

## Replace a selected configuration

1. Keep a caller-owned **UmiCompilerCentre** on the heap. It contains large inline collections; do not put it on a small thread stack.
2. Construct complete provider and profile values through their Framework factories.
3. Call **umi_compiler_centre_configure_profile** with the providers to add or replace, the profile, and whether it should become active.
4. Check the returned status before refreshing your view or starting a separate planning operation.
5. Keep all readers and writers excluded during publication. This API does not provide a lock or a lock-free snapshot.

~~~c
UmiCompilerProvider provider =
    umi_compiler_gcc_provider("reviewed-gcc", "x86_64-unknown-linux-gnu");
UmiCompilerTarget target;
UmiCompilerProfile *profile = malloc(sizeof(*profile));
if (profile == NULL) return UMI_STATUS_OUT_OF_MEMORY;
UmiStatus status =
    umi_compiler_target_parse("x86_64-unknown-linux-gnu", &target);
if (status == UMI_STATUS_OK)
    status = umi_compiler_profile_init(profile, "project-debug", "Project Debug",
        "gcc", UMI_COMPILER_LANGUAGE_C, &target);
if (status == UMI_STATUS_OK)
    status = umi_compiler_centre_configure_profile(centre, &provider, 1U,
                                                  profile, true);
free(profile);
return status;
~~~

This is a function-body fragment using **umicom/compiler/compiler.h** and **stdlib.h**; **centre** must already be initialized and owned by the caller. The string **reviewed-gcc** is metadata. The call neither searches for it nor runs it.

The operation stages a registry copy and profile on the heap. It validates the profile against that proposed registry before changing the centre. A late policy, capacity or revision refusal therefore leaves the previous provider, profile and selection intact. Existing identifiers can be replaced when a collection is full; inserting another identifier cannot exceed its capacity. Exhausted revisions refuse mutation instead of wrapping.

Provider rows not named by the operation remain. Other profiles, manifests, artifacts, cache records and previously planned commands also remain. These retained commands describe earlier plans: replacement does not silently regenerate them. Plan a unit again when a consumer needs a command for the new selection.

## Convert a toolchain selection

A **UmiToolchainProfile** identifies an explicitly selected C compiler. Call **umi_compiler_centre_sync_toolchain(centre, toolchain, "selected")** to convert it, replace the provider and named profile together, and activate that profile. Repeating the operation with another path in the same family replaces the prior path. Changing from GCC to Clang updates the named profile to Clang while retaining the old GCC provider for other profiles.

The bridge checks the public record's size/version and the selected slot. Only GCC, Clang and MSVC C selections have mappings. The selected row's kind must agree with the selection. Required display text, target text, executable path and version storage must be terminated within their bounds. A path that cannot fit the compiler record is refused, never shortened.

**FOUND** and **VALIDATED** are admitted metadata states. **MISSING** and **FAILED** return **UMI_STATUS_UNAVAILABLE**. A reported state does not establish freshness or perform a new probe. An unrelated tool such as CMake cannot become GCC through a default branch.

Conversion uses Framework's debug policy defaults. Use the general publication operation when you want an explicitly customized policy. The older registry import remains add-only and returns **ALREADY_EXISTS** on duplicates; it does not perform replacement. Its count output is zero after ordinary admission on failure and must be separate from the input/registry storage.

## Review and recover

On refusal, retain the previous selection and display the actual status. Correct malformed metadata, unsupported selection, capacity or policy first; retrying without a correction is not a repair. A saturated revision requires an explicit owner-managed lifecycle decision, not resetting a live record's counter.

Use **umi_compiler_centre_plan_unit_in_directory** when the intended project directory is known. It records the directory in both the returned plan and compilation database. It does not change process directory or open the path. See [compiler publication](compiler-centre-publication.md) and [native CLI previews](compiler-plan-cli.md).

Authored regressions cover **framework.compiler.profile_publication**, **framework.compiler.toolchain_admission** and **framework.compiler.toolchain_sync**. They cover replacement, malformed and failed tools, borrowed inputs, full collections, revision exhaustion and unchanged owners on failure. Their execution and actual toolchain/platform qualification remain unverified.

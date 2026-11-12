![Umicom](../../resources/brand/umicom-logo.svg)

# Build compiler argument plans with a clear publication boundary

A compiler request describes what a client wants to compile. A plan stores the command and arguments selected by a provider. Both are bounded C records, but a plan also contains pointers back into its own argument array.

That distinction matters when copying a plan: copying the bytes of a pointer does not make it point into the new object.

## 1. Separate draft storage from execution metadata

Use umi_compiler_request_storage_valid to check bounded text, array counts and active option entries. It accepts empty scalar fields so a client can compose a draft. Use umi_compiler_request_validate to require project, unit, source and output metadata, a named language and a named action.

Similarly, umi_compiler_plan_storage_valid checks the plan's inline storage. The existing plan validator additionally requires provider, executable and at least one argument. Neither validator starts a compiler or checks that the command exists.

~~~c
UmiCompilerRequest request;
UmiStatus status = umi_compiler_request_init(
    &request, "demo", "main", UMI_COMPILER_LANGUAGE_C,
    "main.c", "main.o");
if (status != UMI_STATUS_OK)
    return status;

status = umi_compiler_request_add_include(&request, "include");
if (status != UMI_STATUS_OK)
    return status;
status = umi_compiler_request_add_definition(&request, "DEMO=1");
if (status != UMI_STATUS_OK)
    return status;
~~~

Incremental additions inspect the existing request before mutation. They reject a corrupt count or unterminated active field with INVALID_STATE. Missing arguments remain INVALID_ARGUMENT; lack of room remains CAPACITY_EXCEEDED. Every refusal preserves the complete owner, including unused array slots.

## 2. Keep each text boundary explicit

A source path can fit the request's path array but exceed the size of one plan argument. This is a legitimate later capacity failure. Do not silently truncate it.

The provider now builds into a temporary heap-allocated plan. A failure while formatting a later option discards that candidate and leaves the caller's previous plan unchanged. Allocation failure also preserves the destination.

~~~c
UmiCompilerProvider provider =
    umi_compiler_gcc_provider("gcc", "x86_64-unknown-linux-gnu");
UmiCompilerPlan plan;
umi_compiler_plan_init(&plan);

status = umi_compiler_provider_plan(&provider, &request, &plan);
if (status != UMI_STATUS_OK)
    return status;
/* Only a complete accepted plan has been published. */
~~~

The provider-specific argument mapping is retained. Recognizing an action enumeration value does not mean every provider implements that action. Linking, archiving, syntax checking and cross-toolchain behavior still need their specific implementation and qualification; this boundary change does not add those capabilities.

## 3. Rebuild borrowed pointers after a copy

The argument_view field is a cache of pointers into arguments. Storage validation deliberately does not dereference those pointers because a copied plan can contain pointers into its original owner.

~~~c
UmiCompilerPlan copied = plan;
const char *const *arguments = umi_compiler_plan_arguments(&copied);
if (arguments == NULL)
    return UMI_STATUS_INVALID_STATE;

for (size_t index = 0; index < copied.argument_count; ++index)
    consume_argument(arguments[index]);
~~~

The example's consume_argument stands for your own read-only consumer; it is not a Framework API. Read exactly argument_count entries. The array does not promise a trailing NULL sentinel, including when it is full.

Successful provider publication rebuilds the views in the destination before releasing the temporary plan. A successful incremental argument append also rebuilds active views. Keep the receiving plan alive and serialize access while a caller uses those pointers.

## 4. Treat revisions as finite identities

An argument append advances the plan revision once. At UINT64_MAX it returns CAPACITY_EXCEEDED without changing the plan. Reading or rebuilding the derived pointer view remains available at that revision.

Do not force the revision back to one to continue a live owner. If a higher-level service replaces an exhausted plan, it must establish and communicate a new owner lifetime.

Validation diagnostics use caller-owned text buffers. Keep those buffers separate from the request or plan. A too-small diagnostic buffer returns CAPACITY_EXCEEDED; it does not establish that the inspected record is valid.

## 5. Qualify failure preservation and pointer ownership

Run each command separately and stop on errors:

~~~powershell
Set-Location "C:\umicom\Umicom-Applications\framework"
cmake --preset windows-ucrt64-debug
cmake --build --preset windows-ucrt64-debug --target umicom-compiler-request-plan-admission-test umicom-compiler-request-plan-test umicom-compiler-record-admission-test
ctest --test-dir build/windows-ucrt64-debug --output-on-failure -R "^framework[.]compiler[.](request_plan_admission|request_plan|record_admission)$"
~~~

The new regression covers every request text field, active option rows, all three request counts, plan text and count faults, aliased additions, full arrays, exhausted revisions, copied-plan pointer rebasing, and provider failures after earlier arguments were prepared. It compares complete owner bytes after refusal.

These checks are authored and statically reviewed, not executed. The fixture does not launch a compiler. A passing in-memory test would not establish SDK installation, successful compilation, linking or Studio behavior. Allocation-failure injection is not part of this fixture and remains separate coverage.

Continue with [direct compiler records](compiler-record-admission.md) and [catalogue admission](compiler-catalogue-admission.md) to see the preceding boundaries.

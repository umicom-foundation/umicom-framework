![Umicom](../../resources/brand/umicom-logo.svg)

# Keep linker profiles intact when an edit is refused

A linker profile describes a driver, target, library search paths and library names. It is metadata used to prepare a later linking workflow. Creating or validating the profile does not invoke a linker or establish that its libraries exist.

## 1. Construct a complete replacement

~~~c
UmiCompilerTarget target;
UmiStatus status =
    umi_compiler_target_parse("x86_64-unknown-linux-gnu", &target);
if (status != UMI_STATUS_OK)
    return status;

UmiLinkerProfile profile;
status = umi_linker_profile_init(&profile, "release", "cc", &target);
if (status != UMI_STATUS_OK)
    return status;
~~~

Framework uses its existing staged-value constructor for this large inline record. The candidate is allocated away from the thread stack. Any rejected input or allocation failure leaves the previous destination unchanged.

Inputs may be borrowed from the destination:

~~~c
status = umi_linker_profile_init(
    &profile, profile.profile_id, profile.driver, &profile.target);
~~~

The previous identifier, driver and target remain readable until the new candidate is complete. Successful initialization resets the profile to its documented defaults, empty path/library collections and revision one. It is an explicit replacement, not an incremental edit.

## 2. Separate safe storage from a complete profile

umi_linker_profile_storage_valid checks bounded scalar and target text, both collection counts and every active path/library entry. It accepts empty scalar fields for draft storage. Unused slots are ignored.

umi_linker_profile_validate then applies the existing completeness requirements: identifier, driver, target triple and a non-unknown object format. This does not expand target semantics or establish binary compatibility.

Provide a diagnostic buffer separate from the profile. A missing or zero-sized buffer is INVALID_ARGUMENT; a buffer too small for the diagnostic is CAPACITY_EXCEEDED. Do not interpret that diagnostic-capacity result as a successful profile check.

## 3. Add paths and libraries through the owner

~~~c
status = umi_linker_profile_add_path(&profile, "/sdk/lib");
if (status != UMI_STATUS_OK)
    return status;
status = umi_linker_profile_add_library(&profile, "m");
if (status != UMI_STATUS_OK)
    return status;
~~~

Each addition checks the whole existing profile before publication. A malformed count or active string returns INVALID_STATE. Missing input returns INVALID_ARGUMENT. A full collection, oversized string or exhausted revision returns CAPACITY_EXCEEDED.

Every refused addition preserves all owner bytes, including unused slots. Accepted additions copy overlapping or borrowed strings safely, advance the relevant count once and advance the revision once. Existing entries are not deduplicated or replaced.

## 4. Keep exhausted profiles readable

At UINT64_MAX, adding a path or library would reuse an old revision after unsigned wrap. Framework refuses that new observation. Storage and completeness validation still work for the retained profile.

Do not reset a live revision to bypass the boundary. A higher-level replacement policy must communicate a new owner lifetime. Callers must also serialize access; temporary construction and bounded scans do not supply synchronization.

## 5. Run focused qualification

Run each command separately and stop on errors:

~~~powershell
Set-Location "C:\umicom\Umicom-Applications\framework"
cmake --preset windows-ucrt64-debug
cmake --build --preset windows-ucrt64-debug --target umicom-compiler-linker-profile-admission-test
ctest --test-dir build/windows-ucrt64-debug --output-on-failure -R "^framework[.]compiler[.]linker_profile_admission$"
~~~

The authored regression checks constructor refusal and aliases, every fixed text field, both count boundaries, inactive slots, full collections, borrowed additions and revision exhaustion. Whole-owner byte comparisons verify the intended preservation contract.

The tests have not been executed. Allocation-failure injection, actual library discovery, linker invocation and installed application integration remain separate qualification. Continue with [compiler plan publication](compiler-request-plan-admission.md) for the related argument-owner boundary.

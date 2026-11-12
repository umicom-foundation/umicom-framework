![Umicom](../../resources/brand/umicom-logo.svg)

# Validate native link plans before hashing or extending them

A native link plan owns an output name, an output kind, ordered object inputs, libraries and search paths. It describes intended inputs to a later linker workflow. It does not run a linker or prove that files and libraries exist.

## 1. Initialize the plan through Framework

~~~c
UmiNativeLinkPlan plan;
UmiStatus status =
    umi_nc_link_plan_init(&plan, "app", UMI_NC_LINK_EXECUTABLE);
if (status != UMI_STATUS_OK)
    return status;
~~~

Use a named output kind: executable, shared or static. An initialized plan can have no object inputs yet. An all-zero record is not initialized because it lacks an output and a named kind.

The existing staged constructor remains. It allocates the large candidate away from the thread stack and publishes only on success. Inputs may refer to the previous output field. Rejected kinds, missing text, oversized output and allocation failure preserve the previous destination.

## 2. Compose every collection through its API

~~~c
status = umi_nc_link_plan_add_input(&plan, "main.o");
if (status != UMI_STATUS_OK)
    return status;
status = umi_nc_link_plan_add_library(&plan, "m");
if (status != UMI_STATUS_OK)
    return status;
status = umi_nc_link_plan_add_search_path(&plan, "/sdk/lib");
if (status != UMI_STATUS_OK)
    return status;
~~~

Each addition validates the complete existing plan before using its count. A malformed active row or oversized count returns INVALID_STATE. Missing text is INVALID_ARGUMENT; insufficient slot or text capacity is CAPACITY_EXCEEDED.

All refusal paths preserve complete owner bytes, including unused slots. The new search-path operation owns a copy just like the existing input and library operations. Ordering is retained because a later resolver can search roots in that order.

## 3. Admit storage before using a fingerprint

umi_nc_link_plan_valid checks the output, named kind, all three counts and every active string. Unused slots are ignored. Call it directly when you need a storage-admission result.

The fingerprint now refuses malformed storage before hashing. It covers output, kind, ordered inputs, libraries and search roots. Existing calculations remain unchanged for plans with no search roots; a nonempty search-root section contributes to the identity.

This is a noncryptographic fingerprint. Different valid inputs can collide, and a valid result can equal zero. Invalid records return zero too, so do not use a nonzero test as a substitute for explicit validation. The value does not attest to file content, SDK identity or successful linking.

## 4. Reuse bounded native text copying

The shared umi_nc_copy_text helper now scans at most the destination capacity before writing. A missing terminator within that limit returns CAPACITY_EXCEEDED with the destination unchanged. Empty strings are permitted by this general helper; the link-plan APIs separately require nonempty entries.

Successful self, suffix and shifted overlapping copies use memmove. Source storage must remain readable until its terminator or the stated boundary. This does not validate arbitrary pointers.

umi_nc_hash_text remains a convenience function for a terminated C string. Fixed records must be admitted before passing their fields to it. The link-plan validator provides that boundary for this owner; other native compiler owners require their own review.

## 5. Qualify the contract

Run each command separately and stop on errors:

~~~powershell
Set-Location "C:\umicom\Umicom-Applications\framework"
cmake --preset windows-ucrt64-debug
cmake --build --preset windows-ucrt64-debug --target umicom-compiler-native-link-admission-test umicom-native-compiler-link-plan-test umicom-native-compiler-types-test
ctest --test-dir build/windows-ucrt64-debug --output-on-failure -R "^framework[.]compiler[.](native_link_admission|native[.](link[.]plan|types))$"
~~~

The authored regression covers overlapping copies, unchanged refusal bytes, constructor aliases, named kinds, every active collection, oversized counts, full capacities and ordered search-root identity. A golden no-search-root fixture checks the established fingerprint result.

No compiler or test execution has been performed for this change. Allocation-failure injection, broader native compiler consumers, actual linking and operating-system/application integration remain unverified. Continue with [linker profiles](linker-profile-admission.md) for the related driver and library metadata owner.

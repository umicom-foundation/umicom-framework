![Umicom](../../resources/brand/umicom-logo.svg)

# Construct compiler records without damaging the previous value

Compiler metadata is shared between the native CLI, project services and application clients. A target describes an architecture and operating system. An ABI profile describes a calling and representation convention. A provider descriptor identifies a compiler command and its advertised capabilities.

These records use inline arrays. They are values, not running tools. Constructing one does not establish that its command or SDK is installed.

## 1. Publish only a complete result

Consider an editor that stores a target while a user types a replacement. A failed parse should leave the previously accepted target intact:

~~~c
UmiCompilerTarget target;
UmiStatus status =
    umi_compiler_target_parse("x86_64-unknown-linux-gnu", &target);
if (status != UMI_STATUS_OK)
    return status;

status = umi_compiler_target_parse("unfinished", &target);
/* PARSE_ERROR; target still describes the accepted Linux target. */
~~~

The parser builds a temporary record, applies the existing classification rules, and publishes it only on success. Null, empty and unterminated bounded inputs are rejected before parsing. The parser's existing component format and classification policy remain; this change does not introduce a full target-triple specification.

## 2. Understand aliasing during replacement

Aliasing means that two references point into the same storage. Passing a field from an object back to its initializer is common:

~~~c
status = umi_compiler_target_parse(target.triple, &target);
~~~

Clearing target first would also clear the input string. Staging avoids that error. The ABI initializer similarly accepts an identifier and target borrowed from the destination profile:

~~~c
status = umi_compiler_abi_init(
    &profile, profile.abi_id, UMI_COMPILER_ABI_CPP, &profile.target);
~~~

Both inputs are consumed before profile is replaced. Existing defaults remain: C and system profiles are marked stable; C++ profiles enable their existing exceptions and RTTI flags. These flags describe metadata policy, not demonstrated binary interoperability.

Provider initialization supports the same pattern:

~~~c
status = umi_compiler_provider_init(&provider, &provider.descriptor);
~~~

A successful initialization resets the provider revision to its established initial value. Do this during explicit initialization or replacement, not as a way to disguise changes in an already published owner.

## 3. Distinguish bounded storage from compatibility

A fixed text field must contain a terminator inside its array. Direct target and ABI comparisons now reject records with unterminated text before calling string comparison. Provider initialization and planning share the same descriptor storage checks used by the catalogues.

After those checks, target compatibility still compares architecture, operating system, object format and pointer width. ABI compatibility retains its existing target, width and kind comparisons. It does not newly compare every vendor, environment or toolchain property.

A true compatibility result is therefore a result of that metadata policy. It is not proof that two binaries link or call each other correctly. Qualify actual toolchains and interfaces separately.

## 4. Preserve failure results

Target parsing leaves its entire output unchanged on INVALID_ARGUMENT or PARSE_ERROR. ABI initialization leaves its output unchanged on rejection, including CAPACITY_EXCEEDED for an identifier without a terminator inside the identifier capacity. Provider initialization preserves its output for rejected descriptors.

Provider planning rejects malformed descriptor storage before touching the plan. Its request validation and subsequent argument construction are separate boundaries. Do not infer that every later planning failure is atomic from this descriptor check.

Keep inputs readable and serialize access throughout the operation. These functions do not validate arbitrary addresses or protect against another thread changing a record during inspection.

## 5. Run focused owner qualification

Run the commands separately and stop on errors:

~~~powershell
Set-Location "C:\umicom\Umicom-Applications\framework"
cmake --preset windows-ucrt64-debug
cmake --build --preset windows-ucrt64-debug --target umicom-compiler-record-admission-test umicom-compiler-catalogue-admission-test umicom-compiler-language-target-test umicom-compiler-request-plan-test
ctest --test-dir build/windows-ucrt64-debug --output-on-failure -R "^framework[.]compiler[.](record_admission|catalogue_admission|language_target|request_plan)$"
~~~

The new regression checks malformed fixed fields, null inputs, failed parse preservation, maximum-length ABI identifiers, self-borrowed initialization, provider descriptor refusals and unchanged classification/compatibility examples. It does not invoke a compiler.

The test is authored and statically reviewed; execution is pending. Continue with [catalogue admission](compiler-catalogue-admission.md) for the owner-level count and revision boundary. Installed consumers and Studio workflows still require separate execution evidence.

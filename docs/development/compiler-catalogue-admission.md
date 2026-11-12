![Umicom](../../resources/brand/umicom-logo.svg)

# Keep compiler catalogues bounded and observable

A compiler catalogue is a small in-memory directory. The provider registry describes compiler commands; the sysroot catalogue identifies SDK roots; the runtime catalogue identifies language libraries. Studio and command-line clients use these Framework owners to select records.

A public C structure can be changed by its caller. Its count is therefore an assertion that must be checked against the array's actual capacity. A non-null pointer alone does not establish safe storage.

## 1. Initialize and register through the owner

Use the registry initializer and zero-initialize the other two catalogues:

~~~c
UmiCompilerRegistry providers;
UmiCompilerSysrootCatalogue sysroots = {0};
UmiCompilerRuntimeCatalogue runtimes = {0};
umi_compiler_registry_init(&providers);

UmiCompilerProvider gcc =
    umi_compiler_gcc_provider("gcc", "x86_64-unknown-linux-gnu");
UmiStatus status = umi_compiler_registry_register(&providers, &gcc);
if (status != UMI_STATUS_OK)
    return status;
~~~

Registration copies the record. It does not retain a pointer to the caller's input, run the compiler, inspect the filesystem or download an SDK. The availability flag is a supplied observation, not a fresh probe.

## 2. Validate storage before consuming it

The three public storage validators are:

| Owner | Validator |
| --- | --- |
| Providers | umi_compiler_registry_valid |
| Sysroots | umi_compiler_sysroot_catalogue_valid |
| Runtime libraries | umi_compiler_runtime_catalogue_valid |

They check the count before indexing and then check every active row. Fixed text fields must terminate inside their arrays. Identifiers are required; sysroot and runtime paths are required. Runtime languages must be named supported language enumeration values. Unused slots are ignored.

Target and ABI text must also be bounded. These checks intentionally do not invent an ABI compatibility policy, require paths to exist, or assert that a target triple describes a usable SDK. Existing target and ABI compatibility functions still decide selection using their existing fields.

A malformed later row rejects the whole catalogue, even when an earlier row would match. This gives every lookup the same storage contract. Direct calls to other compiler services remain subject to those services' own contracts; this change is not a complete compiler-input audit.

## 3. Handle refusal without losing the owner

Registration distinguishes these outcomes:

| Result | Meaning |
| --- | --- |
| INVALID_ARGUMENT | Required input is absent or the incoming record is malformed. |
| INVALID_STATE | The existing owner's count or active rows are malformed. |
| ALREADY_EXISTS | An admitted owner already contains the identifier. |
| CAPACITY_EXCEEDED | A new row lacks array space or revision capacity. |
| OK | One row was copied and the revision advanced once. |

All refusal paths leave the owner unchanged. Duplicate detection precedes capacity refusal, so registering an existing identifier remains ALREADY_EXISTS even at a full array or the maximum revision. It does not replace the stored row.

A revision is an unsigned observation identity. After UINT64_MAX, adding one would wrap to zero and reuse an old identity. New inserts are refused at that boundary. Reads and duplicate observations remain available. Do not reset a live owner's revision to hide exhaustion; establish a new owner lifetime under an explicit higher-level policy.

## 4. Respect borrowed results

Lookup returns a pointer into the catalogue, or NULL for absent or malformed storage. The pointer is borrowed: keep the owner alive and serialize validation, lookup and use. No internal lock or ownership extension is supplied.

Provider resolution keeps its require_available choice. Sysroot and runtime resolution continue to select the first available compatible row. Storage validation does not change the existing order into ranking or automatic tool discovery.

## 5. Qualify the real boundary

Run each command separately and stop on errors:

~~~powershell
Set-Location "C:\umicom\Umicom-Applications\framework"
cmake --preset windows-ucrt64-debug
cmake --build --preset windows-ucrt64-debug --target umicom-compiler-catalogue-admission-test umicom-compiler-provider-registry-test umicom-compiler-sysroot-runtime-test
ctest --test-dir build/windows-ucrt64-debug --output-on-failure -R "^framework[.]compiler[.](catalogue_admission|provider_registry|sysroot_runtime)$"
~~~

The regression uses real catalogue operations. It covers every stored text field, missing required text, invalid runtime languages, corrupt trailing rows, oversized counts, borrowed duplicate inputs, full arrays, revision exhaustion, malformed lookup inputs and availability selection. Whole-owner byte comparisons check refusal preservation.

These tests are authored and statically reviewed; execution remains pending. A pass would establish these in-memory boundaries. Compiler invocation, SDK usability, linking, installed consumers and Studio workflows require their own qualification.

If registration reports INVALID_STATE, inspect the record producer or restore logic. Silently clamping a count can conceal damaged data. Preserve the rejected owner for diagnosis, correct the producer, and retry with a fully initialized record.

![Umicom](../../resources/brand/umicom-logo.svg)

# Project languages from a bounded manifest

A mixed-language project can show its languages before a compiler runs. Treat that view as metadata. A language bitmask records which named languages occur, not whether their providers, object formats or application binary interfaces can work together.

## Use the shared projection

1. Construct a **UmiCompilerProjectManifest** through the manifest APIs.
2. Keep the complete manifest alive and exclude concurrent writes.
3. Call **umi_compiler_manifest_languages** with a separate **uint32_t** output.
4. Use the mask only after an **OK** result.

~~~c
uint32_t languages = 0U;
UmiStatus status = umi_compiler_manifest_languages(manifest, &languages);
if (status != UMI_STATUS_OK) return status;
bool contains_c = (languages & UMI_COMPILER_LANGUAGE_BIT(UMI_COMPILER_LANGUAGE_C)) != 0U;
~~~

This is a function-body fragment using **umicom/compiler/manifest.h**. The manifest pointer must refer to a complete readable object. Do not use the macro with unchecked external numbers: a shift outside its integer width is invalid C behavior.

The projection checks the entire active manifest storage before traversing it. Counts, fixed text and each known language must satisfy the shared admission contract. Repeated units in one language contribute one bit. A valid empty draft produces zero, while malformed storage returns **INVALID_STATE** and preserves the output. Null pointers or an output overlapping manifest storage return **INVALID_ARGUMENT**.

Storage admission does not require a runnable project. Use **umi_compiler_manifest_validate** for the established project/target and entry-point rules, then perform separate provider, toolchain and actual build qualification. The language mask does not certify interoperability.

## Verify refusals as well as successful views

The authored **framework.compiler.manifest_languages** regression checks empty drafts, repeated languages, invalid later rows, excessive counts, malformed text and overlapping output. It also checks that failures preserve the previous mask and source owner. Execution remains unverified until the matching Framework build and test run are recorded.

When extending the public language enum, review storage admission, mask width and the provider mappings together. A new visible label alone does not implement a new compiler backend.

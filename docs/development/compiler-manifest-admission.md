![Umicom](../../resources/brand/umicom-logo.svg)

# Admit a complete compiler manifest

A project manifest owns an array of compilation-unit records. Each unit names its source, proposed output, language and optional provider/ABI metadata. The public structure is useful to native clients, but a count field alone cannot prove that the array is safe to traverse.

The shared manifest service now checks every active record before lookup or insertion. A malformed later row prevents an earlier matching row from being returned. This keeps different clients from accepting inconsistent views of the same manifest.

1. Construct the manifest with complete project, display-name and target strings. Over-capacity strings are refused, never shortened.
2. Compose each unit with a nonempty identifier, source, output and known language. Provider and exported ABI strings may be empty, but still need termination within their arrays.
3. Add the unit through the shared service. It checks the incoming row and the complete stored manifest before searching for duplicate identifier or source text.
4. Validate the project separately when it is ready for use. Draft storage can contain zero units; project validation still requires at least one and permits no more than one marked entry point.

~~~c
if (!umi_compiler_unit_manifest_valid(&unit))
    return UMI_STATUS_INVALID_ARGUMENT;
UmiStatus status = umi_compiler_manifest_add_unit(&manifest, &unit);
/* On refusal, all previous manifest bytes remain unchanged. */
~~~

Construction uses temporary heap storage because the inline unit array is large. The caller may pass the old manifest's own strings as constructor inputs. They are consumed before the new empty manifest replaces the destination. Allocation or input failure leaves the previous value intact.

Insertion preserves the established duplicate-before-capacity result. A duplicate is still reported when the array is full or its revision is exhausted. A new unit is refused when either limit is reached. Valid lookup remains available at the maximum revision. Returned unit pointers are borrowed: keep the owner alive and serialize reads and mutations.

Storage admission does not verify files, compiler availability, target compatibility, ABI interoperability or source correctness. The existing project validator's optional reason buffer receives a terminated prefix when small; the semantic status remains authoritative. Keep that buffer separate from the manifest.

The owner regression **framework.compiler.manifest_admission** covers scalar and unit text, late corrupt rows, excessive counts, aliased construction, an incoming destination slot, full capacity, revision exhaustion and entry-point rules. It also compares complete owner bytes after refusals. This regression is authored and awaits execution. Four previous implementations remain disabled for review. Higher-level compiler-centre publication needs its own review; this boundary does not prove that every composite operation is atomic.

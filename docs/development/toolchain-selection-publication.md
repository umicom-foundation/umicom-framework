![Umicom](../../resources/brand/umicom-logo.svg)

# Select a toolchain with its child environment

A selected profile and its child-process environment describe one intended execution context. If the profile changes before environment preparation succeeds, a consumer can see a new compiler name with old **CC**, **CXX** or path values. Framework provides **umi_toolchain_catalogue_select_with_environment** to publish the pair after preparation.

## Prepare before publishing

1. Create a catalogue and register complete **UmiToolchainProfile** records.
2. Allocate separate receiving profile and environment owners. The environment contains a large inline value matrix; prefer heap or suitably owned long-lived storage.
3. Call the shared selection function with the intended profile identifier.
4. On success, use the receiving values while keeping their owners alive and stable.
5. On refusal, retain the previous selection and report the status. Both receiving owners and the catalogue's explicit selection remain unchanged.

~~~c
UmiStatus status = umi_toolchain_catalogue_select_with_environment(
    catalogue, "project-gcc", selected_profile, child_environment);
if (status != UMI_STATUS_OK) return status;

const UmiEnvironmentVariable *variables =
    umi_environment_plan_variables(child_environment);
~~~

This fragment uses **umicom/toolchain/catalogue.h**. The three owner pointers must already be valid; the two output objects must not overlap each other or the catalogue. The identifier may borrow text from the receiving profile because it is consumed before publication.

The service stages a profile copy and environment on the heap. It prepares the environment through the existing Framework service, then changes the catalogue selection, then copies the prepared owners. Process-entry name/value pointers are rebound into the receiving environment. Copying an environment without rebinding would leave those pointers referring to temporary storage.

Callers serialize access for the complete operation. This is coherent publication under that ownership rule, not a lock, crash-recovery transaction or protection against unsynchronized concurrent mutation.

## Distinguish storage from usability

**umi_toolchain_profile_storage_valid** checks the public size/version contract, fixed strings and declared known tool slots. It accepts empty draft fields, missing tools and explicitly failed tools as representable metadata. It does not claim a complete kit, an executable path, supported target or valid installed compiler. Reserved slots preserve the existing ABI capacity and are not interpreted as new known tool kinds.

Catalogue upsert requires valid storage and a nonempty profile identifier. It refuses malformed input before lookup or mutation. Selection and upsert refuse revision exhaustion when a mutation is needed; selecting the same identifier or upserting identical bytes remains a no-op. Project bindings require terminated identifiers and database-path storage before publication.

Environment preparation has stricter requirements. A selected compiler must supply usable validated metadata for **CC**; malformed environment text, such as a newline in a tool path, is refused. When a bin directory is supplied, preparation reads inherited **PATH** and can refuse an overlong combined value. It never modifies the parent environment or executes a compiler.

## Keep discovery separate

Discovery can probe tools and update catalogue entries incrementally. The new selection API does not turn discovery into an atomic operation, undo previous discovery observations or authorize probe execution. A missing explicit-root array is refused before traversal, but an otherwise valid discovery request still has its existing side effects and qualification requirements.

The authored regressions **framework.toolchain.profile_storage** and **framework.toolchain.selection_publication** cover bounded record admission, failed environment preparation, unchanged outputs/selection, receiving pointer ownership and overlap refusal. They also check malformed bindings and missing discovery-array admission. Their execution remains unverified. Actual discovery, installed tool compatibility and child-process launches require separate qualification.

For the next layer, see [compiler provider/profile synchronization](compiler-toolchain-synchronization.md). A prepared toolchain environment and a compiler argument plan are related metadata, but neither alone proves a successful build.

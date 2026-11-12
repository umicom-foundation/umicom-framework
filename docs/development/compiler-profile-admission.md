![Umicom](../../resources/brand/umicom-logo.svg)

# Build a complete compiler profile

A compiler profile combines a provider choice, primary language, target, standard, linker configuration and build policy. These nested values have different responsibilities. Bounded text proves that a field can be read; it does not prove that a provider supports a language or that a policy is valid.

The shared constructor now prepares the whole profile in temporary heap storage. It accepts only complete identifiers and a known language, reuses bounded target admission, and publishes after the nested linker initializer succeeds. The old profile stays intact when any step refuses. Inputs may refer to that old profile, including its own identifiers and target.

1. Obtain a bounded target through the target service.
2. Initialize the profile with complete project-chosen strings.
3. Register the intended provider in a valid provider registry.
4. Call profile validation before using the configuration.
5. Handle the returned status and show its reason without claiming toolchain execution.

~~~c
UmiStatus status = umi_compiler_profile_init(
    &profile, "debug", "Debug", "gcc",
    UMI_COMPILER_LANGUAGE_C, &target);
if (status != UMI_STATUS_OK) return status;
status = umi_compiler_profile_validate(
    &profile, &registry, reason, sizeof(reason));
~~~

The storage validator checks every scalar string, the target and the nested linker arrays. It permits an empty draft; semantic validation separately checks required identity/target fields, a known language, provider registration, language support and the established build policy. All active provider records are admitted before a matching provider is used. A malformed unrelated record cannot hide behind an early match.

Policy refusal now carries the policy's actual diagnostic. For example, the existing policy rejects combining thread and address sanitizers. Reporting that refusal with the text “ok” would make a troubleshooting interface misleading. Small optional reason buffers still receive a terminated prefix; the return status retains its original meaning. The reason buffer must not overlap either input owner.

The source retains both previous implementations disabled for review. The owner regression **framework.compiler.profile_admission** exercises aliased construction, over-capacity identifiers, malformed target/linker/provider storage, exhausted but readable revisions, provider selection and policy diagnostics. It compares complete input bytes after refusal. These cases are authored and have not been executed.

This change does not discover compilers, qualify standards or targets, or repair composite compiler-centre publication. Policy option compatibility and later process execution need their own validation.

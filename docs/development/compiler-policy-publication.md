![Umicom](../../resources/brand/umicom-logo.svg)

# Apply a compiler policy as one change

A compiler policy contributes several arguments to a plan. Debug configuration, for example, can add optimisation, debugging and stack-protection switches. A plan may have room for the first switch but not the remaining ones. Returning a failure after changing the live plan leaves a misleading half-applied configuration.

Framework now composes those arguments in a temporary plan. The caller receives the complete result only when every append succeeds. Capacity exhaustion, revision exhaustion, invalid input and allocation failure leave all destination bytes unchanged. Successful publication rebases argument pointers into the receiving plan before releasing temporary storage.

1. Initialize or obtain a plan whose inline fields are valid.
2. Select a named compiler family and a policy.
3. Call umi_compiler_policy_append_arguments and check its returned status.
4. On success, consume exactly argument_count entries from the plan argument service.
5. On failure, retain or display the previous plan and report the refusal. Do not treat it as a partially completed policy.

Policy validation now rejects negative or unknown optimisation values and unknown sanitizer bits. The existing address/thread sanitizer conflict remains a refusal. Optional diagnostic buffers can receive a truncated reason and must not overlap the policy.

Publication and storage ownership are checked alongside the [explicit compiler dialect contract](compiler-policy-dialects.md). The service supports C/C++ compile policies for GCC, Clang and MSVC and refuses unsupported intents before mutation. Accepted switch spelling still requires compiler-version, target, linker and runtime qualification. Memory-only fixtures do not provide that evidence.

The regression target framework.compiler.policy_publication exercises failure after a private append, exact capacity and revision boundaries, malformed storage, enum/bit admission and receiving-owner pointer identity. Its expected GCC/Clang and MSVC debug strings are checked as data. The test does not execute a driver. All compiler and runtime qualification remains outstanding.

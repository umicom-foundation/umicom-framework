![Umicom](../../resources/brand/umicom-logo.svg)

# Select a compiler policy dialect explicitly

A policy describes requested behaviour, while a compiler dialect determines how to spell its switches. Framework now admits C and C++ compile plans for GCC, Clang and MSVC before appending policy arguments. Other languages, families and actions return NOT_IMPLEMENTED without changing the plan. Syntax checking still has its separate provider planning path; this policy service does not silently apply code-generation policy to that action.

1. Set the plan language and action, or obtain a complete plan from a provider.
2. Select a compiler family that matches the intended driver mode.
3. Call umi_compiler_policy_validate_for_plan if you need a readable admission reason.
4. Append the policy and check the status. Refusal preserves the previous complete plan.
5. Qualify the driver version, target, runtime libraries and linker separately.

## Supported mappings

| Intent | GCC / Clang dialect | MSVC dialect |
| --- | --- | --- |
| Debug optimisation | -Og | /Od |
| Debug information | -g | /Zi |
| Warnings as errors | -Werror | /WX |
| Link-time optimisation | -flto | /GL |
| Stack protection | -fstack-protector-strong | /GS |
| Control-flow protection | -fcf-protection=full | /guard:cf |
| Address sanitizer | -fsanitize=address | /fsanitize=address |

MSVC address instrumentation uses its documented slash-prefixed spelling. Other sanitizer bits in this API are refused for MSVC. This is a mapping limit, not a statement about every present or future Microsoft sanitizer feature. See the [MSVC sanitizer option reference](https://learn.microsoft.com/en-us/cpp/build/reference/fsanitize).

GCC memory sanitizer intent is refused; Clang has an explicit memory-sanitizer mapping. Address, thread and memory sanitizer intents are mutually exclusive, while supported undefined-behaviour checks can be combined with one of them. These admission rules follow the [Clang sanitizer guidance](https://clang.llvm.org/docs/UsersManual.html#controlling-code-generation).

Accepted flag spelling is not target verification. GNU control-flow instrumentation is target dependent; consult the [GCC instrumentation reference](https://gcc.gnu.org/onlinedocs/gcc/Instrumentation-Options.html). MSVC Control Flow Guard also needs the matching linker configuration, as described in the [Microsoft CFG reference](https://learn.microsoft.com/en-us/cpp/build/reference/guard-enable-control-flow-guard). This compile-plan service does not add linker flags or probe either toolchain.

## Reproducibility is a separate contract

The reproducible field remains an explicit intent, but policy append now refuses it with NOT_IMPLEMENTED. The previous identity path map and /Brepro approximation did not establish stable paths, timestamps, environment, dependencies or toolchain identity. Retaining a success result would overstate what the service can provide.

The release-policy factory retains its original requested intent. Select reproducible = false only when the operation genuinely does not require this service to provide that guarantee; do not silently clear the field to hide a refusal. A reproducible release workflow needs its own concrete environment and output-comparison evidence.

The exact argument matrix and refusal tests are authored. The earlier atomic-composition test now declares language/action metadata, and the release-profile fixture checks reproducibility refusal before exercising supported options. Older mappings and fixtures remain disabled for review. No native driver, sanitizer runtime, linker or application was executed for these changes.

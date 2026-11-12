![Umicom](../../resources/brand/umicom-logo.svg)

# Plan the requested compiler action explicitly

A compiler request contains both a language and an action. The language describes the source; the action describes the requested stage. Copying an action label into plan metadata is not enough: the generated arguments must select the corresponding operation.

Framework now exposes umi_compiler_provider_can_plan_action so clients can inspect the planner's implemented mappings before requesting a plan. This query does not inspect executable files, run a compiler or qualify a toolchain installation.

## 1. Distinguish a mapping from an available tool

~~~c
UmiCompilerProvider provider =
    umi_compiler_clang_provider("clang", "x86_64-unknown-linux-gnu");
if (!umi_compiler_provider_can_plan_action(
        &provider, UMI_COMPILER_LANGUAGE_C, UMI_COMPILER_ACTION_CHECK))
    return UMI_STATUS_NOT_IMPLEMENTED;
~~~

The query checks bounded provider storage, its declared family and language mask, and the action mapping. It is independent of the available flag. A provider can have a known mapping while its executable is unavailable. The planning operation still reports UNAVAILABLE for an unavailable provider.

Current mappings are:

| Requested action | Mapping |
| --- | --- |
| Compile | Existing provider-specific mapping retained |
| Assemble | Existing mapping retained when the declared language is Assembly |
| Check | C or C++ through GCC, Clang or MSVC |
| Link executable, link shared, archive | Unmapped; NOT_IMPLEMENTED |
| Other language checks | Unmapped; NOT_IMPLEMENTED |

Existing compile and assembly mappings are preserved for compatibility. Their flags, source dialects, SDKs, artifacts and actual execution still need provider-specific qualification. In particular, a generic assembler family does not identify every assembler dialect.

## 2. Compose a syntax-check request

~~~c
UmiCompilerRequest request;
UmiCompilerPlan plan;
UmiStatus status = umi_compiler_request_init(
    &request, "example", "main", UMI_COMPILER_LANGUAGE_C,
    "main.c", "unused-output.o");
if (status != UMI_STATUS_OK)
    return status;
request.action = UMI_COMPILER_ACTION_CHECK;
status = umi_compiler_provider_plan(&provider, &request, &plan);
if (status != UMI_STATUS_OK)
    return status;
~~~

The existing request validator still requires output_path, although the CHECK mapping does not emit that path. Use a valid placeholder as shown. Select a standard and target supported by the actual compiler installation; metadata construction alone cannot establish that support.

The generated syntax-check stage uses -fsyntax-only for GCC and Clang, and /Zs for MSVC. Those options are documented by [GCC](https://gcc.gnu.org/onlinedocs/gcc/Warning-Options.html), [Clang](https://clang.llvm.org/docs/CommandGuide/clang.html) and [Microsoft](https://learn.microsoft.com/en-us/cpp/build/reference/zs-syntax-check-only?view=msvc-170).

The planner retains the language standard, Clang target, include directories, definitions, warning policy and explicit options. It omits generated object-output switches and debug-artifact switches for CHECK. It does not run the compiler.

## 3. Review explicit options before execution

Raw options remain caller-owned policy inputs. They are copied into the plan in their original order and can alter driver behavior, including the effective stage or outputs. The capability query describes the generated core mapping; it is not a sandbox or a complete parser for every compiler flag.

A native UI should show the actual argument vector and apply its existing execution policy. The planner preserves argument boundaries, including paths containing spaces. The process runner must consume the counted argument view rather than reconstructing an unquoted shell command.

## 4. Preserve the previous plan on refusal

A link or archive request no longer receives a compile command carrying a different metadata label. It returns NOT_IMPLEMENTED until a dedicated mapping is available. The previous output plan remains unchanged.

The same preservation rule applies to invalid inputs, unavailable providers, argument capacity failures and allocation failures. Successful publication rebases the plan's borrowed argument pointers into the receiving owner. See [request and plan admission](compiler-request-plan-admission.md) for that lifetime contract.

A native link plan describes intended linking inputs separately. Its presence does not mean this provider command planner has an executable link adapter.

## 5. Qualify plans and toolchains separately

Run commands individually and stop on errors:

~~~powershell
Set-Location "C:\umicom\Umicom-Applications\framework"
cmake --preset windows-ucrt64-debug
cmake --build --preset windows-ucrt64-debug --target umicom-compiler-action-planning-test umicom-compiler-request-plan-test umicom-compiler-request-plan-admission-test
ctest --test-dir build/windows-ucrt64-debug --output-on-failure -R "^framework[.]compiler[.](action_planning|request_plan|request_plan_admission)$"
~~~

The authored test inspects real plan tokens for six C/C++ syntax-check combinations, verifies 24 link/archive refusals across eight provider factories, retains existing compile/assembly entry tokens, and checks availability and late-capacity preservation.

No compiler or test execution has been performed for this change. A passing plan-token test would still not prove that GCC, Clang, MSVC, a linker, an archiver or a target SDK accepts the generated command on the owner's machine.

# Native C23 LLM — Batch 91–92: production-tape acceptance gates

**Project:** Umicom Framework | **Lead:** Sammy Hegab | **Organisation:** Umicom Foundation  
**Code:** C23 | **Licence:** MIT | **Stage:** research and integration qualification

## Executive result and status

This batch adds **no alternative model and no new neural mathematics**. It
registers three narrowly defined tests that link the **real production
`Umicom::AiTraining` target** in the established isolated prefix laboratory.
The targets use `tape.c`, `rotary_tape.c` and their existing Framework owners.
They are not built with substitute training tapes. The new tests are acceptance
checks for the already implemented rotary-training bridge and are intended to
run before implementing a specialised fused tape operator.

**Evidence boundary:** compilation and execution against a user's Windows
UCRT64 checkout are not represented as complete until the included CTest
commands have actually succeeded on that checkout. A passing calculation
is insufficient if graph lifetime, failure atomicity or memory accounting fail.

## Why production integration must come before optimisation

The previous training experiment exercised a short three-token, two-head
rotary attention operation, using tape-owned matrix multiplication, attention,
loss and SGD. Prior qualification compared 32 weight updates with an independent
numerical reference and repeatedly checked positional translation. Its original
CMake tests were wired to `Umicom::AiTraining`; separate release-environment
checks had used API-compatible stand-ins. This release addresses the resulting
integration evidence gap, directly at the existing operator boundary.

The current rotary tape composition uses `EmbeddingLookup`, a per-position
rotation matrix, `Matmul` and `Add`. It is intentionally expensive because
its aim is to preserve the existing automatic-differentiation rules. No claims
of production throughput, low latency, RoPE checkpoint compatibility or full
multi-layer decoder training follow from the new tests.

## 1. Direct projected-query/key gradient tests

New test: `framework.ai.training.rotary_real_tape_gradients`

- Use three tokens, four channels and two attention heads.
- Construct a new **real Framework tape** for each calculation.
- Compare all 24 query/key leaf-tensor gradients with central differences.
- Repeat at absolute positions 0, 17 and 513: **72 total estimates**.
- Verify that an equal position translation preserves loss approximately.
- Require a declared per-value absolute tolerance of `0.0020` for the
  particular bounded FP32 teaching problem.

A numerical derivative of a scalar loss `L` with respect to one parameter
`w` is approximated by

`(L(w + h) - L(w - h)) / ((w + h)_fp32 - (w - h)_fp32)`.

The actual FP32-rounded denominator matters: the computer's two test weights
may not be separated by *exactly* `2*h`. The numerical test is independent
of the tape's backward graph, but it reuses the same trusted forward operator.
The older separate oracle remains the stronger independent-forward reference.

## 2. Failure atomicity under real tape memory budgets

New test: `framework.ai.training.rotary_real_tape_budget`

- Preserve the three original leaf handles.
- Use the **public Framework** `TapeSetMemoryBudget` API to trigger capacity
  failures after some composed nodes may already have been created.
- Confirm no incomplete returned output, no surviving temporary nodes, and
  the exact original tracked committed-byte count.
- Lift the budget and prove that a subsequent valid computation succeeds.
- Check that rollback releases the new graph and preserves the retained leaves.
- Reject an out-of-range absolute starting position without mutating the tape.

`peakBytes` is a high-water mark and may legitimately grow during a failed
operation **before rollback**. Tests compare the current allocation, not an
incorrect assumption that the historic peak must fall back.

## 3. Repeated real-tape forward, backward and rewind

New test: `framework.ai.training.rotary_real_tape_rewind`

The test reuses one tape for **48 complete rounds**. Each round creates fresh
intermediates, computes a forward loss, backpropagates into the original leaves,
and rewinds to the same initial four leaves. It verifies:

- finite loss and gradients;
- the same number of retained graph nodes after each rewind;
- exactly the same tracked committed-byte count after each rewind;
- no increase in *tracked peak* for this fixed graph shape after the first run;
- numerical stability under the tested uniform position translations.

This is **not** proof of whole-process leak freedom. AddressSanitizer,
LeakSanitizer where supported, UndefinedBehaviourSanitizer, production load
cases, allocator instrumentation and platform-specific checks remain necessary.

## Build and test — isolated native LLM prefix project

PowerShell, from `C:\umicom\Umicom-Applications`:

```powershell
$env:Path = 'C:\msys64\ucrt64\bin;' + $env:Path
cmake -S '.\framework\learning\native-llm\prefix-work' `
  -B '.\build\windows-ucrt64-native-llm-prefix-debug' `
  -G Ninja -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON
if ($LASTEXITCODE -ne 0) { throw 'Native LLM configuration failed.' }

cmake --build '.\build\windows-ucrt64-native-llm-prefix-debug' `
  --target umicom-ai-training-rotary-real-tape-gradients-test `
           umicom-ai-training-rotary-real-tape-budget-test `
           umicom-ai-training-rotary-real-tape-rewind-test `
           umicom-native-llm-real-rotary-tape `
  --parallel 2
if ($LASTEXITCODE -ne 0) { throw 'Native tape acceptance build failed.' }

ctest --test-dir '.\build\windows-ucrt64-native-llm-prefix-debug' `
  -R '^framework\.ai\.training\.rotary_real_tape_' `
  --output-on-failure --no-tests=error
if ($LASTEXITCODE -ne 0) { throw 'Production-tape acceptance failed.' }
```

After those focused tests pass, also run the previous rotary qualification
and optimisation regressions, and then the full laboratory. The exact CI
results, compiler and configuration must be recorded rather than inferred.

## Compatibility and source preservation

- The canonical source remains `src/ai/training/tape.c`.
- Existing C ABI, pretrained learned-absolute-position decoder and checkpoint
  bytes are unchanged.
- No existing source, comments, functions or behaviour is deleted.
- Tests and demonstration are registered via a new additive CMake file.
- No PowerShell/Python source is introduced as project functionality.
- The next specialised operator should be considered only if these acceptance
  gates pass on the real production tape and ASan/UBSan qualification is clean.

## Education

Read Bits2Banking LLM Lessons 91 and 92 alongside Maths for AI Units 16–18.
Each formula is explained in words and numbers. This separation lets students
from any academic or professional background revisit fractions, matrices,
limits and differentiation without losing their place in the engineering course.

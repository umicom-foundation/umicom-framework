# Native LLM — Independent RoPE Training-Tape Qualification

**Umicom Framework · Bits to Banking · Batch 89–90**  
**Author:** Sammy Hegab · Umicom Foundation  
**Implementation preference:** Native C23; no scripts in production or tests.

## Why this batch exists

Batch 87–88 implemented an eight-step training demonstration based on the
canonical Umicom Framework training tape. Earlier isolated validation used an
API-compatible *test-only tape substitute*. Passing that test does not by
itself prove the behaviour of the production `src/ai/training/tape.c`.

Batch 89–90 adds an independent, C23 numerical acceptance test and a separate
repeatability/lifetime acceptance test. The normal Framework CMake composition
links both directly against `Umicom::AiTraining`. **There is no substitute
training implementation in the delivered repository files.** The independent
oracle is confined to `tests/ai_training/` and cannot be linked as an
application inference engine.

The full production tape and Windows UCRT64 integration must be built and run
by the contributor before claiming the production acceptance gate passed. The
release developer's isolated local tests are explicitly reported separately.

## Source tree ownership

| Path | Responsibility |
|---|---|
| `tests/ai_training/test_training_rotary_qualification_oracle.c` | Independent loss and 32-weight finite-difference check; never Framework runtime code |
| `tests/ai_training/test_training_rotary_qualification_lifecycle.c` | Repeatable position-shift and graph-lifetime contract checks |
| `learning/native-llm/prefix-work/audit_rotary_training.c` | Thin native educational CLI that invokes existing Framework APIs |
| `cmake/UmicomNativeLlmRotaryQualification.cmake` | Additive CMake registration for real Framework targets |
| `learning/native-llm/prefix-work/CMakeLists.txt` | One appended include; all older CMake lines retained |

No existing `tape.c`, `rotary_tape.c`, `rotary_optimisation.c`, `decoder.c`,
checkpoint format, RNG, or generation behaviour is changed. A future faster
operator needs its own separate design and independent qualification.

## What the independent numerical oracle calculates

The input has three rows, four channels, two attention heads and two 4 × 4
trainable projection matrices. The reference deliberately rebuilds the
mathematics without calling any tape, RoPE or sampler function. It performs:

1. Q = X × Wq and K = X × Wk using explicit finite FP32 row-major projections.
2. Adjacent-pair query/key rotations at the declared absolute positions.
3. Per-head dot products divided by √2, with future rows excluded.
4. Stable softmax and the weighted combination of the fixed value vectors.
5. Mean squared error over 12 output components.

The oracle verifies the initial loss and, after one real Framework SGD step,
the independently recomputed final loss.

### Explain the central gradient comparison

For a particular weight *w*, estimate its gradient by taking two loss samples
around the original value:

`referenceGradient ≈ (L(w+epsilon) - L(w-epsilon)) / (storedPlus - storedMinus)`

- **L**: independently calculated mean squared error.
- **w**: one of the 32 query/key matrix weights.
- **epsilon**: a small positive perturbation, 0.002 in this experiment.
- **storedPlus/storedMinus**: the actual FP32 weights after perturbation.
- **≈**: finite differences estimate a derivative; they are not exact.

The gradient used by the real Framework optimisation step can be recovered:

`observedGradient ≈ (weightBefore - weightAfter) / learningRate`

The test checks all 32 elements separately, with fixed **absolute error
< 0.0008**. It identifies the exact matrix and element when a check fails.
This is an acceptance threshold for this small FP32 exercise only. Do not
apply it universally or relax it merely to silence a failure.

Two accessible concrete calculations are in **Maths for AI Unit 14** and
**LLM Lesson 89** in `Umicom-Bits2Banking`.

## Lifecycle and absolute-position qualification

A second test performs a full eight-step run at the baseline position, then
sixteen full runs with both query/key positions shifted equally. It checks:

- unchanged original problem values after every call;
- identical completed step count and tracked peak tape allocation;
- loss traces and updated weights within declared FP32 tolerances;
- one-, two-, ..., eight-step runs reproducing the initial prefix of the
  eight-step loss trace;
- an invalid NaN parameter being rejected without changing an output sentinel;
- a fresh valid run remaining reproducible after the failed call.

For rotary positions, the ideal dot product is unchanged when query and key
positions receive the same additive offset: (5−3)=(15−13)=2. Rounding causes
small numerical differences, which the test measures instead of assuming
bitwise equality across different positions.

**Memory limitation:** a stable `peakTapeBytes` measurement is a statement
about tracked persistent tape allocations for this graph. It is NOT process
RSS and does not prove the absence of memory leaks. Run the same tests with
AddressSanitizer/UndefinedBehaviourSanitizer for additional evidence.

## Windows UCRT64 — isolated real-Framework acceptance

From PowerShell:

```powershell
Set-Location 'C:\umicom\Umicom-Applications'
$env:Path = 'C:\msys64\ucrt64\bin;' + $env:Path

cmake -S '.\framework\learning\native-llm\prefix-work' `
  -B '.\build\windows-ucrt64-native-llm-prefix-debug' `
  -G Ninja -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON
if ($LASTEXITCODE -ne 0) { throw 'Native LLM configuration failed.' }

cmake --build '.\build\windows-ucrt64-native-llm-prefix-debug' `
  --target umicom-ai-training-rotary-qualification-oracle-test `
           umicom-ai-training-rotary-qualification-lifecycle-test `
           umicom-native-llm-rotary-qualification --parallel 2
if ($LASTEXITCODE -ne 0) { throw 'Native LLM qualification build failed.' }

ctest --test-dir '.\build\windows-ucrt64-native-llm-prefix-debug' `
  -R '^framework\.ai\.training\.rotary_qualification_' `
  --output-on-failure --no-tests=error
if ($LASTEXITCODE -ne 0) { throw 'Production tape qualification failed.' }
```

Run the complete prefix laboratory only **after building all targets**;
`ctest` alone does not compile executables. See the separate private guide for
full build, merged checkout validation, source baseline and Git workflow.

## What has not been proved

- No local execution using the original production `tape.c` has been claimed.
- No general trainable rotary decoder or checkpoint family is introduced.
- No production inference speedup, multi-layer KV correctness, GPU performance,
  generalisation, or trading safety is claimed.
- The test-only mathematical reference must not become a Framework inference
  implementation. Reusable production operators continue to belong to the
  original native Framework and must preserve the stable ABI.

**Education:** see Bits2Banking Mathematics for AI Units 13–15, its symbol
reference page and LLM Lessons 89–90. These lessons explain every new
mathematical symbol in ordinary language with worked answers and illustrations.

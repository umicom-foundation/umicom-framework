# Compare Native Decoder Models on the Same Held-Out Data

## What the quality gate measures

A decoder can complete training and still be a poor candidate for a software release. The native model quality gate provides a repeatable comparison of **two explicitly reviewed combined AdamW checkpoints** using exactly the same held-out source windows.

The Framework performs each evaluation through `UmiAiTrainingModelAuditEvaluate`. That established evaluator calculates held-out cross-entropy, perplexity, top-one accuracy and top-five accuracy. The new quality gate compares those measurements. It does not create another model, select a different dataset or modify weights.

A held-out *target token* is a byte-token the model was asked to predict. A top-one prediction is correct when the highest-ranked token matches the target. Top-five accuracy counts a target when it appears among the five highest-ranked tokens. Smaller cross-entropy is better; smaller perplexity is generally better too.

## The comparison contract

Both models must be evaluated using **one approved experiment plan and one immutable loaded dataset**. The plan includes the evaluation seed, context size, evaluation batch count and evaluation batch size. The Framework rejects a comparison if the recorded plan identity, dataset identity, document summary or evaluated-token count differs.

The default educational selection policy requires a **strictly positive cross-entropy improvement**, no decrease in measured top-one or top-five accuracy, and at least 32 evaluated target tokens. A matching checkpoint or a loss tie does not pass.

The default is a learning example, **not a statistically defensible production threshold**. Before real product releases, use independent human-reviewed source datasets, multiple task suites and a much larger, explicitly documented sample.

### Worked example

Suppose both candidates are evaluated on 100 target tokens from the same held-out files:

| Metric | Baseline | Candidate |
|---|---:|---:|
| Cross-entropy | 3.00 | 2.50 |
| Correct top-one predictions | 30 | 35 |
| Correct top-five predictions | 70 | 75 |

Cross-entropy reduction = 3.00 − 2.50 = **0.50**. Top-one and top-five accuracy each improved by **5 percentage points**. The candidate passes the default *educational* criteria if the reports are valid, the checkpoint identities differ, and their evaluation contracts match.

If the candidate instead returned top-one 25/100, it would fail by default even though its cross-entropy was smaller. The policy can explicitly allow a small accuracy regression, but this is a reviewer decision, not an automatic override.

## Windows — build and inspect two approved combined checkpoints

Use PowerShell in the Umicom Applications checkout. The standalone laboratory is built from Framework-owned C23 source files and does not require the complete desktop application:

```powershell
Set-Location 'C:\umicom\Umicom-Applications'
$env:Path = 'C:\msys64\ucrt64\bin;' + $env:Path

cmake -S .\framework\learning\native-llm `
  -B .\build\windows-ucrt64-native-llm-debug -G Ninja `
  -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON `
  -DUMICOM_AI_TRAINING_ENABLE_SIGNATURES=ON

cmake --build .\build\windows-ucrt64-native-llm-debug `
  --target umicom-native-llm-quality-compare --parallel 2

ctest --test-dir .\build\windows-ucrt64-native-llm-debug `
  -R '^framework\.ai\.training\.model_quality_' `
  --output-on-failure --no-tests=error
```

When comparing two **separately reviewed local** combined AdamW checkpoint files:

```powershell
& '.\build\windows-ucrt64-native-llm-debug\umicom-native-llm-quality-compare.exe' `
  'C:\umicom\reviewed\evaluation-plan.conf' `
  'C:\umicom\reviewed\baseline.umi' `
  'C:\umicom\reviewed\candidate.umi'
```

The plan must have a total step count **at least as large as both checkpoints' recorded optimiser-step counts**. The tool accepts the existing opt-in AdamW plan grammar, including the `maximum_gradient_norm` setting. Clipping policy affects earlier training but is not applied during this read-only comparison.

Exit status is zero only when the candidate passes; one indicates an evaluated but rejected candidate; two indicates invalid usage or a failed comparison. As usual, confirm executable return codes in PowerShell.

## C23 reusable API

```c
#include "umicom/ai/training/model_quality_gate.h"

UmiAiTrainingModelQualityPolicy policy;
UmiAiTrainingModelQualityReport report;
UmiStatus status = UmiAiTrainingModelQualityDefaults(&policy);
if (status == UMI_STATUS_OK) {
    status = UmiAiTrainingModelQualityEvaluate(
        baselineModel, candidateModel, approvedDataset,
        &approvedEvaluationPlan, &policy, &report);
}
if (status == UMI_STATUS_OK && report.accepted != 0) {
    /* The model improved on this sampled held-out evaluation only. */
}
```

All inputs remain owned by the caller. A rejection or error never partially overwrites the output report. The direct `CompareReports` API is useful for inspecting already calculated reports, but those reports must be trusted by the caller and the function enforces matching plan/dataset identities.

## Readiness and trust are separate decisions

A model that passes the quality gate is **not automatically approved for distribution**. In particular, sampled next-token prediction does not test whether generated C23 code compiles, is safe, or satisfies a programming task. Both checkpoint files must come from reviewed sources; this demonstration does not authenticate signatures. Where signed native publications are used, verify them independently before opening checkpoints. The release approval policy and the training quality gate answer different questions and should not be conflated.

Exercises: (1) Show why a change in the evaluation seed makes scores incomparable. (2) Adjust the cross-entropy and top-one values in the worked example and decide whether the candidate passes. (3) Explain why passing a 32-token sample cannot establish general model quality. (4) Compare the meaning of a model audit fingerprint with a cryptographically signed training publication.

## Deterministic quality evidence in native C23

A quality assessment is useful for automated tools only if its output can be
interpreted without relying on terminal text or the computer's numeric locale.
`UmiAiTrainingModelQualityEvidenceJson` is a Framework API that renders an
already computed `UmiAiTrainingModelQualityReport` as compact JSON. It
recalculates the decision from the original audit snapshots and policy before
serialisation; inconsistent copied results are rejected without modifying the
caller's output buffer.

The native read-only comparison application supports an optional JSON output:

```powershell
Set-Location 'C:\umicom\Umicom-Applications'
& '.\build\windows-ucrt64-native-llm-debug\umicom-native-llm-quality-compare.exe' `
  'C:\umicom\reviewed\evaluation-plan.conf' `
  'C:\umicom\reviewed\baseline.umi' `
  'C:\umicom\reviewed\candidate.umi' --evidence-json
```

The three original positional arguments still work and retain their previous
human-readable output. In JSON mode, stdout contains a single JSON document.
A passing quality policy returns exit code 0; a valid measured comparison
that fails the policy returns exit code 1 and still emits honest JSON.
Malformed inputs and evaluation failures return a nonzero exit code and do
not publish successful comparison evidence.

### Reading exact floating-point values

The JSON stores cross-entropy, perplexity, deltas and thresholds using
`*_binary64` fields. Each contains exactly 16 lower-case hexadecimal digits
representing the IEEE 754 binary64 pattern of the original `double` value.
For example, the exact bits for 0.5 are `3fe0000000000000`. These strings
are not decimal JSON numbers. An IEEE 754-compatible consumer can recover
the original number from its bit pattern rather than guessing at rounding
or depending on a machine's decimal separator.

FNV-1a64 checkpoint, dataset and plan fields are explicitly labelled
`*_fnv1a64`. They are reproducibility indicators, not digital signatures.
The JSON's assurance fields remain `false` for publisher verification,
independent release approval and general programming competence. Use the
separate signed-publication and release-approval services for those tasks.

### Build before running tests

```powershell
Set-Location 'C:\umicom\Umicom-Applications'
$env:Path = 'C:\msys64\ucrt64\bin;' + $env:Path
cmake -S .\framework\learning\native-llm `
  -B .\build\windows-ucrt64-native-llm-debug -G Ninja `
  -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON `
  -DUMICOM_AI_TRAINING_ENABLE_SIGNATURES=ON
if ($LASTEXITCODE -ne 0) { throw 'CMake configure failed.' }
cmake --build .\build\windows-ucrt64-native-llm-debug --parallel 2
if ($LASTEXITCODE -ne 0) { throw 'Native LLM build failed. Do not run CTest yet.' }
ctest --test-dir .\build\windows-ucrt64-native-llm-debug `
  -R '^framework\.ai\.training\.model_quality_' `
  --output-on-failure --no-tests=error
```

The real integration regression calls `UmiAiTrainingDecoderInspect` with its
correct `UmiAiTrainingDecoderConfig *` output argument. It is compiled with
incompatible-pointer-type diagnostics treated as errors, preventing the
previous Windows build failure from returning unnoticed. A missing CTest
executable after a failed compilation means **not run**, not that the model
failed its quality assessment. Resolve the compilation failure and rebuild
all selected test targets before trusting a test result.

# Reproducible AdamW Source Experiments

**Author:** Sammy Hegab · **Organisation:** Umicom Foundation · **Licence:** MIT  
**Source owner:** Umicom Framework · **Implementation:** ISO C23

## What the native runner does

A reviewed local experiment plan selects a source manifest, model dimensions,
training and evaluation random seeds, a bounded step target, and held-out
scoring cadence. The new Framework-owned `experiment_adamw` service combines
that established machinery with the AdamW optimiser and the combined decoder
plus optimiser checkpoint codec. The original SGD experiment APIs are unchanged.

The entry points are:

```c
UmiAiTrainingExperimentAdamWValidate(...);
UmiAiTrainingExperimentRunAdamW(...);
UmiAiTrainingExperimentResumeAdamW(...);
```

Each run returns a **decoder and a bound AdamW optimiser**. The caller must
first call `UmiAiTrainingAdamWDestroy(optimizer)`, then
`UmiAiTrainingDecoderDestroy(model)`. Errors or cancelled observations publish
neither handle and do not partially fill the result.

## Absolute-step learning-rate scheduling

For optimiser step `s`, the caller's unchanged schedule is evaluated with
`UmiAiTrainingLearningRateForStep(schedule, s, &rate)`. The source sampler
uses `s - 1` as its absolute batch identifier. The first portion of an
interrupted run may stop at step 3 while the schedule remains defined over
8 steps; continuation starts at step 4 with the SAME 8-step schedule. A newly
constructed 5-step schedule would give different rates and invalidate an
exact-recovery claim.

`experimentConfig.learningRate` must equal the scheduled peak; this is an
explicit guard against ambiguous training plans. The native CLI uses the
reviewed plan's learning rate for the peak, a positive 10%-of-peak minimum,
cosine decay and two warmup steps for schedules longer than four steps.

## Exact model and optimiser continuation

AdamW has persistent first- and second-moment vectors. A decoder-only
checkpoint cannot restore them. The combined AdamW format is decoded as a
pair, and a resumed experiment validates the model architecture, optimiser
hyperparameters, completed-step identity and schedule capacity. The caller's
pair is cloned through the existing combined checkpoint codec before training;
it is never altered in place.

No training source, random seed, schedule or evaluation policy is bound into
the combined checkpoint. This is an important provenance limitation. A
successful decode or checksum only proves a structurally consistent file,
not that the input files and plan are the same, nor that the file is trusted.
Verify the source manifest and the source fingerprints with the existing
model-audit facilities and preserve the original schedule text out of band.

## Hands-on Windows laboratory

Configure and build the native training laboratory from an ordinary PowerShell
session. Keep the security software enabled and do not execute quarantined
files until the security provider has reviewed them.

```powershell
Set-Location 'C:\umicom\Umicom-Applications'
$env:Path = "C:\msys64\ucrt64\bin;$env:Path"
cmake -S '.\framework\learning\native-llm' `
  -B '.\build\windows-ucrt64-native-llm-debug' -G Ninja `
  -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON
cmake --build '.\build\windows-ucrt64-native-llm-debug' --parallel 2
ctest --test-dir '.\build\windows-ucrt64-native-llm-debug' `
  -R '^framework\.ai\.training\.adamw_experiment_(resume|contract|cancel|demo)$' `
  --output-on-failure
```

If the antivirus blocks any test executable, CTest may record `BAD_COMMAND`
or `Not Run`; this is neither a passing test nor a model-mathematics failure.
Collect the details, obtain the vendor's verdict, then rerun the affected
executable and complete suite. Never rename executables to evade scanning.

A source-only demonstration with **no file writes**:

```powershell
& '.\build\windows-ucrt64-native-llm-debug\umicom-native-llm-adamw-experiment.exe' `
  '.\framework\learning\native-llm\fixtures\experiment.conf' 32
```

With a separately approved new checkpoint destination:

```powershell
& '.\build\windows-ucrt64-native-llm-debug\umicom-native-llm-adamw-experiment.exe' `
  '.\framework\learning\native-llm\fixtures\experiment.conf' 32 `
  '.\build\windows-ucrt64-native-llm-debug\approved-adamw.umi'
```

The destination must not exist. For a staged resume use a reviewed plan with a
smaller `steps` value, but pass the SAME `schedule-total-steps` argument as
will be used for completion. Provide the existing combined checkpoint as the
last argument; new output still requires a fresh destination:

```powershell
& '.\build\windows-ucrt64-native-llm-debug\umicom-native-llm-adamw-experiment.exe' `
  '.\framework\learning\native-llm\fixtures\experiment.conf' 32 `
  '.\build\windows-ucrt64-native-llm-debug\continuation.umi' `
  '.\build\windows-ucrt64-native-llm-debug\approved-adamw.umi'
```

A resume is scientifically comparable only when the checkpoint was created
with the same approved training source bytes, seeds, optimiser parameters and
full schedule. A full experiment's final report contains sampled held-out
metrics, not proof that the tiny decoder understands real banking operations.

## Known limits

- This runner supports the canonical bounded training batch, not AdamW
  gradient accumulation. The existing SGD accumulated runner is retained.
- Combined checkpoints are versioned, bounded and checksummed for accidental
  corruption, but not digitally signed and not a cryptographic proof of origin.
- The schedule and manifest are external commitments; exact resume requires
  the caller to preserve and verify them.
- No process, script, network request or arbitrary source execution occurs
  in the Framework experiment runner.

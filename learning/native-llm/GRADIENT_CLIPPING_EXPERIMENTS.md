# Reproducible Gradient Clipping in Native AdamW Experiments

Umicom Framework's reviewed text plans can express a global gradient L2 norm
limit for the **opt-in** native AdamW training laboratory. This educational
facility performs token-weighted gradient accumulation, calculates the norm of
the combined gradient, clips it once when necessary, and then invokes the
canonical AdamW update. Model tensors, optimiser moments and the existing
binary checkpoint format retain their previous implementations.

## Prepare a plan

A small fixture is available at
`learning/native-llm/fixtures/clipped-adamw-experiment.conf`:

```ini
UMICOM-AI-EXPERIMENT-1
manifest=manifest.txt
steps=4
eval_interval=2
eval_batches=1
batch_sequences=2
effective_sequences=2
microbatch_sequences=1
learning_rate=0.025
maximum_gradient_norm=1.0
```

`maximum_gradient_norm` must be a positive, finite, non-exponent decimal. The
value is a global L2 threshold, not an individual tensor threshold. Invalid or
duplicate keys are rejected before training. The legacy plan interfaces still
reject this new key; only `UmiAiTrainingExperimentPlanLoadAdamW` accepts it.

For this first CLI, `effective_sequences` (when present) must equal
`batch_sequences`. `microbatch_sequences` controls the graph chunk size and
must be positive. The Framework's underlying clipping and accumulation APIs
can be reused independently by application and Studio modules.

## Build on Windows

From PowerShell in `C:\umicom\Umicom-Applications`:

```powershell
$env:Path = 'C:\msys64\ucrt64\bin;' + $env:Path
cmake -S .\framework\learning\native-llm -B .\build\windows-ucrt64-native-llm-debug `
  -G Ninja -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON
cmake --build .\build\windows-ucrt64-native-llm-debug `
  --target umicom-native-llm-adamw-clipped-experiment --parallel 2
ctest --test-dir .\build\windows-ucrt64-native-llm-debug `
  -R 'gradient_policy|adamw_clipped_experiment_resume' --output-on-failure
```

Only execute a binary that has passed your local security review; builds and
regression results alone do not resolve an antivirus classification.

## Train and resume

```powershell
$exe = '.\build\windows-ucrt64-native-llm-debug\umicom-native-llm-adamw-clipped-experiment.exe'
$plan = '.\framework\learning\native-llm\fixtures\clipped-adamw-experiment.conf'
& $exe $plan 8 'C:\umicom\_local\models\clipped-first.umi'
```

A successful checkpoint write creates two independent files:
`clipped-first.umi` and `clipped-first.umi.clip-policy`.
The latter stores the precise float threshold, batch and microbatch counts,
and an SHA-256 identity for the **combined** checkpoint bytes.

Resuming requires the approved input checkpoint and its sidecar, as well as a
**different, unused** output filename. It rejects altered or missing sidecars
and unequal clipping policies. The supplied fixture
`learning/native-llm/fixtures/clipped-adamw-continuation.conf` changes the
requested `steps` to eight, leaving every other plan value unchanged. Use the
**same** eight-step absolute schedule when training the first four steps and
continuing from step five:

```powershell
$nextPlan = '.\framework\learning\native-llm\fixtures\clipped-adamw-continuation.conf'
& $exe $nextPlan 8 'C:\umicom\_local\models\clipped-continued.umi' 'C:\umicom\_local\models\clipped-first.umi'
``` The established `umicom-native-llm-adamw-experiment`
command remains available for previous un-clipped experiments.

The output pair is not an atomic two-file transaction. If saving its record
fails after creating the checkpoint, that checkpoint is incomplete and must
not be resumed by the clipped command. Review both files and any OS-level
interruption before retrying with a new filename.

## Interpretation and trust limits

This record is an integrity and continuity aid, not cryptographic signer
authentication. It does **not** attest training-machine integrity, dataset
contents, source licence, held-out quality, or model capability. Reproducible
continuation still requires an unchanged reviewed dataset, seed, optimiser
configuration and absolute learning-rate schedule. Compare held-out metrics
and test full vs checkpoint-resumed runs for model **and** moment identity.

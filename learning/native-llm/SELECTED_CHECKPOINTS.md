![Umicom](../../resources/brand/umicom-logo.svg)

# Keep the checkpoint selected by held-out loss

Training updates the decoder using one group of source files. Evaluation uses
different files and can reveal that a later training step did worse on the
held-out sample. A checkpoint stores the decoder's weights and step number.
Keeping the last checkpoint and keeping the one selected by evaluation are
two distinct choices.

## Compare the two runs

1. Configure the native LLM laboratory with testing enabled and build the
   `umicom-native-llm-experiment` target. Use the same settings and reviewed
   dataset for both runs.
2. In a new work directory, set `$tool` to the absolute path of the built
   experiment executable and `$plan` to the absolute path of
   `learning/native-llm/fixtures/experiment.conf`. The plan names its
   corpus relative to the file.
3. Run both commands. Use unused checkpoint paths.

~~~powershell
& $tool $plan (Join-Path $PWD 'final.umckpt')
& $tool --select-held-out 2 $plan (Join-Path $PWD 'selected.umckpt')
~~~

The first command trains to the plan's requested step and saves that
decoder. The second checks the same held-out windows after the same step
intervals. A patience of two means training can stop after two consecutive
evaluations without an improvement. Set patience to zero to run every step
and still select a checkpoint.

Check the terminal step and the selected step printed by the program. They
can differ. Step zero represents the initialized decoder before training,
and it is a valid selection. A held-out improvement describes these
sampled windows under this experiment's corpus and seeds.

The command prints a failure if either checkpoint path already exists.
Re-run with a new filename after checking the completed and selected steps.
The selection mode uses a from-scratch, constant-rate training plan with
one batch per optimiser step. Resume and gradient accumulation have their
own experiment modes.

## Use the same policy from C

Include `umicom/ai/training/experiment.h` and call
`UmiAiTrainingExperimentRunSelected`. For example, the policy
`{.minimumImprovement = 0.001, .patience = 2}` replaces the retained model
when the measured loss falls by at least 0.001. The returned report carries
both the best raw observation and the measured loss of the returned model.
Use the existing checkpoint API if the caller explicitly requests a file.
Release the returned decoder with `UmiAiTrainingDecoderDestroy`.

Run `framework.ai.training.experiment_selection` in CTest to observe both
a worsening held-out sample and an improving one. The test also compares
the retained weights with the initialized checkpoint and checks that a
cancelled experiment leaves the caller's prior results intact.

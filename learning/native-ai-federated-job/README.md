# Native federated job admission and deterministic aggregation

**Umicom Foundation · Sammy Hegab · MIT**

The bounded C23 experiment composes the pre-existing `UmiAiTrainingFederatedRound` with a new round-scoped job admission layer. The numeric operation is sample-weighted FedAvg over **weight-space deltas** tied to a shared base checkpoint. Existing Framework training, checkpoint, message, cryptographic and governance facilities remain separate authorities.

## Build and run

From this directory, with a C23 compiler and CMake 3.24 or newer:

```text
cmake -S . -B build -G Ninja -DBUILD_TESTING=ON
cmake --build build --parallel 2
ctest --test-dir build --output-on-failure
```

The target `umicom-native-federated-job-demo` accepts no live data and makes no network connections. Its three workers are *synthetic contributions*, not trained models.

## Control boundary

1. A controller verifies the intended worker and dataset permissions **outside** this module.
2. `JobAdmit` compares immutable byte identities and reserves a forecast transfer budget. A digest's presence alone is not verification of rights or provenance.
3. `JobComplete` forwards the actual float delta to the established `FederatedRoundSubmit` implementation. Failed submissions leave the assignment pending and the FedAvg round unchanged.
4. A cancelled assignment releases its forecast reservation; its worker ID stays consumed for this round.
5. `JobFinalise` refuses outstanding work and returns only a **candidate** mean delta. It neither updates a model nor guarantees held-out quality.

### Explicit limitations

This is a local, serialised coordinator. Tickets are **process-local** counters, not cryptographic credentials; the reported transfer sizes are claimed forecasts, not measured bytes. It implements no training worker, network protocol, TLS, signing, remote proof-of-learning, secure aggregation, confidential-computing attestation or blockchain consensus. The round result is *not* a released model checkpoint.

## Learning references

- McMahan et al., [Federated Averaging](https://proceedings.mlr.press/v54/mcmahan17a.html), AISTATS 2017.
- Hu et al., [Low-Rank Adaptation](https://arxiv.org/abs/2106.09685), 2021.
- Douillard et al., [DiLoCo](https://arxiv.org/abs/2311.08105), 2023.

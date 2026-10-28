# Canonical federated wire format (schema 1)

**Umicom Foundation · Sammy Hegab · MIT**

All integer fields are unsigned and encoded in **network/big-endian order**.
The first 184 bytes form a fixed header. A result adds `deltaCount × 4` bytes
containing IEEE-754 binary32 weight-space deltas in big-endian bit order.
All frames end with 32 bytes of SHA-256 over the domain prefix
`umicom.ai.federated-wire.schema-1` followed by the preceding frame bytes.

| Byte offset | Width | Meaning |
| ---: | ---: | --- |
| 0 | 4 | ASCII `UMFW` |
| 4 | 2 | Schema revision (1) |
| 6 | 2 | Kind (1=assignment, 2=result) |
| 8 | 4 | Exact total frame length |
| 12 | 8 | Round ID, nonzero |
| 20 | 8 | Public sequence ID, nonzero |
| 28 | 32 | Contributor ID commitment |
| 60 | 32 | Shared base checkpoint SHA-256 |
| 92 | 32 | Dataset-shard commitment |
| 124 | 32 | Data-policy commitment |
| 156 | 8 | Claimed distinct example count |
| 164 | 8 | Claimed local optimisation steps |
| 172 | 8 | Declared transfer-reservation bytes |
| 180 | 4 | Delta scalar count |
| 184 | variable | FP32 values (results only) |
| 184 + 4×count | 32 | SHA-256 trailer |

A complete assignment is 216 bytes. One-scalar result is 220 bytes. The
maximum result holds 4,096 scalars and occupies 16,600 bytes. No partial
frame is published. A receiver must reject short, overlong, unknown-version,
checksum-invalid and non-finite result data.

An unkeyed checksum, even SHA-256, does **not** prove who originated the
message. Nor does a valid signature (if added by a separately approved
transport) prove the worker ran the claimed training job. Data permissions,
participant authorisation, replay/expiry policy and held-out model evaluation
are distinct responsibilities. Keep raw datasets, full checkpoints and
private gradients out of any public ledger.

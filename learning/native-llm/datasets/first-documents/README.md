# First document dataset

This small original dataset introduces document preparation for the native byte-model laboratory. It contains two training documents and one evaluation document. No account or external data download is needed.

1. Open each text file and read it before training.
2. Open `dataset.txt`. Each line after the header selects a whole file and its partition.
3. Build `umicom-native-llm-dataset` from the [native learning laboratory](../../README.md), then pass the full path of `dataset.txt` to it.
4. Expect three documents, two training files and one evaluation file. The printed SHA-256 identifies the imported snapshot. Line-ending changes alter this identity.
5. Run the inspector again without editing the files. The digest and sampled token arrays should be identical.

Eight input tokens are paired with the next eight byte tokens. Tokens are numbers; a UTF-8 character can occupy several bytes. These short examples teach mechanics and cannot establish general language ability.

The reader opens the files named by a trusted local manifest. It is not a sandbox for downloaded manifests or symbolic links. Use a private working copy to try changes. Never place passwords, customer records or unlicensed material in a learning dataset.

Keep the evaluation document out of training. A lower loss on training text alone does not show that the model handles new text. Similar copies across the two partitions can also make evaluation misleading.

# ReadMe

## List of dependencies

The following is the complete list of dependencies and the version that have been tested:
- RapidJSON (v1.1.0)
- Bison (v3.0.4)
- Flex  (v2.6.4)
- Boost (v1.66.0)

## Buldind Instruction

To configure and build the project run the following commands from the root of the project:

```shell
./waf configure
./waf buid
```

## Usage of the Alignment Tool

To execute the alignment tool from the root of this repository:

```shell
./build/tools/alignment_tool
```

The previous command should print the help message of the tool.  Example of usage are:
- Local alignment with the score table in config.json
  ```shell
  ./build/tools/alignment_tool -i ./tests/Human-Mouse-BRCA2-cds.fasta -l -c ./tests/config.json
  ```
- Global alignment with the score table in config.json
  ```shell
  ./build/tools/alignment_tool -i ./tests/Human-Mouse-BRCA2-cds.fasta -c ./tests/config.json
  ```
- Global alignment using default scores ( +1 -1 -1 -5 )
  ```shell
  ./build/tools/alignment_tool -i ./tests/Human-Mouse-BRCA2-cds.fasta
  ```

## Usage of the SuffixTree Tool

To execute the alignment tool from the root of this repository:

```shell
./build/tools/suffixtree_tool
```

The previous command should print the help message of the tool.  Examples of usage are:
- Build and Print the SuffixTree as Dot file:
  ```shell
  ./build/tools/suffixtree_tool -i tests/s1.fasta --dot banana.dot
  ```
- Build the SuffixTree and save to file the BWT and the Post Order
  ```shell
  ./build/tools/suffixtree_tool -i ./tests/s1.fasta --bwt banana.bwt --post banana.post
  ```

The directory output-files contains dumps of the outputs requested.  The experiment folder contains the scripts used to generate them.

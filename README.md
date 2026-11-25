# v4zip

v4zip is a lossless FASTA and FASTQ compressor built on Klein four-group symmetry.
It restores the original file byte for byte, including headers, letter case,
ambiguity symbols, line endings, and FASTQ quality scores.

## Try it

After [building v4zip](#build-and-install), run these commands from the source
directory with unused example filenames. This FASTA includes lowercase bases
and an ambiguity symbol.

```sh
printf '>example\nAcGTNN\n' > example.fa
./build/v4zip compress example.fa example.v4z
./build/v4zip decompress example.v4z restored.fa
cmp example.fa restored.fa
```

For FASTQ, add `fastq` before the command. Each record must have four lines,
with one quality character per sequence character.

```sh
printf '@read1\nAcGT\n+read1\n!I~#\n' > example.fastq
./build/v4zip fastq compress example.fastq example.v4zq
./build/v4zip fastq decompress example.v4zq restored.fastq
cmp example.fastq restored.fastq
```

Both `cmp` commands exit silently when the files match. v4zip requires a new
output filename for each compression or decompression.

## Build and install

Use a 64-bit POSIX system with a C++17 compiler, CMake 3.16 or later, and
the Zstandard development library. Unpack the source archive and run the
commands from the directory containing `CMakeLists.txt`. [v3.0 source archive](https://github.com/michelyamagishi/v4zip/archive/refs/tags/v3.0.tar.gz)

**Linux (Debian or Ubuntu)**

```sh
sudo apt-get update
sudo apt-get install build-essential cmake libzstd-dev
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
```

**macOS** with Apple's Command Line Tools (`xcode-select --install`) and Homebrew

```sh
brew install cmake zstd
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH="$(brew --prefix zstd)"
```

On either platform, build the executable and check its version.

```sh
cmake --build build --parallel 2
./build/v4zip --version
```

The examples use `./build/v4zip`. To install a copy at `./install/bin/v4zip`,
run `cmake --install build --prefix "$PWD/install"`.

## Compression results

In a Linux comparison with v4zip 2.1, v3.0 produced smaller archives for all
four inputs. The largest reduction was on the FASTQ sample, where quality
scores accounted for 52,979 of the 59,468 bytes saved.

| Input | Input bytes | v2.1 archive bytes | v3.0 archive bytes | Size reduction | Compression time, v2.1 → v3.0 |
|---|---:|---:|---:|---:|---:|
| *M. genitalium* FASTA | 587,389 | 130,851 | 130,000 | 0.65% | 2.18 → 1.54 s |
| *E. coli* K-12 FASTA | 4,699,745 | 1,098,231 | 1,094,427 | 0.35% | 10.66 → 12.10 s |
| *S. cerevisiae* FASTA | 12,310,392 | 2,949,821 | 2,919,045 | 1.04% | 27.43 → 32.21 s |
| Illumina MT147 FASTQ, first 10,000 reads | 3,904,484 | 489,323 | 429,855 | 12.15% | 4.06 → 5.52 s |

Both versions restored all four inputs byte for byte. Each timing is from one
run per version and input. Peak process memory during compression ranged from
**0.48 to 0.54 GiB for v3.0** and from **3.95 to 3.98 GiB for v2.1** on these
inputs.

<details>
<summary>Linux test conditions, decompression times, memory use, and input hashes</summary>

On November 25, 2026, v4zip 3.0.0 and 2.1.0 were built in CMake Release
mode with GCC 15.2.0 and libzstd 1.5.7 on an Intel Core i7-10700 running
x86-64 Linux. The comparison used v3.0 commit `85a5692c` and the local
2.1.0 tree, whose production sources matched the public v2.1.0 tag.
[v3.0 source](https://github.com/michelyamagishi/v4zip/tree/v3.0) · [v2.1.0 source](https://github.com/michelyamagishi/v4zip/tree/v2.1.0)

Each version compressed and decompressed each input once, one command at a
time. GNU `time` recorded wall time and maximum resident set size (RSS) for
each process. `cmp` verified every restored file against its input. Archive
bytes include the container, sequence, headers, formatting, and FASTQ
qualities. Size reduction is the difference in archive bytes
divided by the v2.1 archive size.

| Input | Decompression, v2.1 / v3.0 (s) | Peak compression RSS, v2.1 / v3.0 (KiB) | Peak decompression RSS, v2.1 / v3.0 (KiB) |
|---|---:|---:|---:|
| *M. genitalium* G37 | 2.19 / 1.51 | 4,140,768 / 502,704 | 4,140,496 / 501,656 |
| *E. coli* K-12 MG1655 | 10.70 / 12.21 | 4,152,516 / 516,564 | 4,147,008 / 508,940 |
| *S. cerevisiae* S288C | 27.54 / 32.81 | 4,172,636 / 564,632 | 4,159,204 / 522,896 |
| Illumina MT147, first 10,000 reads | 3.90 / 5.03 | 4,148,584 / 567,456 | 4,144,956 / 521,256 |

The FASTA inputs were complete files. The FASTQ sample was the first 40,000
lines (10,000 complete records) of `MT147_S26_L002_R1_001.fastq`. Its quality
stream took 148,864 bytes in v2.1 and 95,885 bytes in v3.0. The following
SHA-256 hashes identify the exact uncompressed inputs, including their original
headers and line endings.

| Input | SHA-256 |
|---|---|
| *M. genitalium* G37 | `f392aff8c52ea376e68fbc3b1e9f3c2601c264d7dc4ac0186e2e0e05e82ab098` |
| *E. coli* K-12 MG1655 | `53bb6a51b6e92139ced1e38f74b7938781027c52200922ff03718c2237d23bb4` |
| *S. cerevisiae* S288C | `fe42735d3e6242f8c40b457a232940e34ef15eb4502717e54a34ed118face4c1` |
| Illumina MT147, first 10,000 reads | `3425ec495dc91ecaa3b4d67671aa14277ab82624ecdf9588c909e08b90f70d5b` |

</details>

<details>
<summary>Apple M5 results from the v3.0 release</summary>

The release reports these measurements on an Apple M5 with macOS and
AppleClang 21, with SHA-256 verification of restored files. Its FASTQ sample
is ERR000589. The v2.1 size for the five-strain dataset is approximate, so its
reported size ratio is also approximate. [v3.0 release measurements](https://github.com/michelyamagishi/v4zip/blob/v3.0/README.md#benchmarks)

| Input | Input bytes | v2.1 archive bytes | v3.0 archive bytes | Reduction relative to v2.1 | v3.0 compression / decompression (s) |
|---|---:|---:|---:|---:|---:|
| `m_genitalium.fna` | 587,389 | 130,851 | 130,000 | 0.65% | 1.12 / 1.10 |
| `e_coli_k12.fna` | 4,699,745 | 1,098,231 | 1,094,427 | 0.35% | 9.89 / 10.27 |
| `err000589_10k.fastq` | 1,457,148 | 437,403 | 405,047 | 7.40% | 0.85 / 0.75 |
| `mg_5x_snp1.fna` (five strains, 1% SNPs) | 2,936,980 | ≈654,000 | 191,532 | ≈71% | 1.51 / 1.34 |

</details>

<details>
<summary>Historical v2.1 comparisons with other compressors</summary>

On November 10, 2026, v2.1 restored all seven complete inputs byte for byte
in three serial repetitions per input. The runs used one fixed CPU on an Intel
Core i7-10700 with x86-64 Linux, GCC 15.2.0, and libzstd 1.5.7. The other
compressors were measured on November 5 using the same inputs. Across the
seven inputs, total archive size fell from 117,686,700 bytes with the v2.0
rewrite to 100,126,639 bytes with v2.1, a 14.92% reduction. The highest
recorded v2.1 process RSS was 4.54 GiB. [v2.1 release measurements](https://github.com/michelyamagishi/v4zip/blob/v2.1.0/README.md#benchmarks)

### FASTA

Values are bits per ACGT base. For v4zip, gzip, and xz the numerator includes
the full archive. GeCo3 level 9 encodes only the uppercase ACGT projection.
The runs used gzip 1.14 and xz 5.8.3.

| Genome | v4zip 2.1 | GeCo3 level 9 (DNA only) | gzip -9 | xz -9 |
|---|---:|---:|---:|---:|
| *M. genitalium* | 1.8046 | 1.7942 | 2.2858 | 2.1136 |
| *E. coli* K-12 | 1.8928 | 1.8831 | 2.3769 | 2.1745 |
| *S. cerevisiae* | 1.9411 | 1.8166 | 2.5278 | 2.2000 |
| *H. sapiens* chr22 | 1.6669 | 1.6008 | 2.2792 | 1.9666 |
| *C. elegans* | 1.8960 | 1.7017 | 2.5284 | 2.1404 |
| *H. sapiens* chr1 | 1.6244 | 1.5684 | 2.2695 | 1.9018 |

### FASTQ

This comparison used the complete Illumina MT147 file (621,630 reads of 159
bases, 231.5 MiB). Each entry restored the full FASTQ. Compression ratio is
the original size divided by the archive size. The runs used gzip 1.14,
bzip2 1.0.8, xz 5.8.3, and zstd 1.5.7.

| Compressor | Archive size (MiB) | Compression ratio |
|---|---:|---:|
| v4zip 2.1 | 17.50 | 13.23× |
| gzip -9 | 39.89 | 5.80× |
| bzip2 -9 | 33.57 | 6.90× |
| xz -9 | 26.16 | 8.85× |
| zstd -19 | 30.70 | 7.54× |

</details>

## Compatibility and limits

- v3.0 writes archive format 3 and reads formats 1, 2, and 3 of the rewrite.
  Anyone receiving a format 3 archive needs a format 3 reader, such as v3.0.
  v2.1 reads rewrite formats 1 and 2.
- Archives from the original v4zip implementation predate the rewrite and
  require the original executable.
- v4zip reads the entire input into memory. Allow additional RAM for the model,
  intermediate buffers, and output. The default limit for a restored file is
  1 GiB. For a larger file, pass `--max-output BYTES` to `decompress`, with the
  limit in bytes. Use `--help` for command syntax and `--verbose` for sizes
  and elapsed time.

## How compression works

A DNA word, its reverse, its complement, and its reverse complement form a
set of related sequences under the Klein four-group. v4zip uses these
relationships to share prediction statistics and find repeated segments.
Version 3 selects a DNA model using sequence length, runs of identical bases,
sampled repeat frequency, and, for FASTQ, average read length. Repetitive
inputs can use matches to earlier segments in any of the four orientations.
[DNA strategy selection](src/detail/genome_profiler.hpp) · [Repeat coding](src/detail/repeat_codec.hpp)

The encoder processes DNA, identifiers, and qualities in parallel. For
eligible FASTQ quality strings, it tries both an adaptive quality model and
Zstandard, then stores the smaller result. Headers, formatting, and quality
scores are included in the archive sizes above. [Archive encoder](src/archive2.cpp)

## Citation

If you use v4zip in research, please cite the book describing its mathematical
basis. [Publisher record](https://doi.org/10.1007/978-3-319-62689-5)

```bibtex
@book{yamagishi2017mathematical,
  title     = {Mathematical Grammar of Biology},
  author    = {Yamagishi, Michel Eduardo Beleza},
  year      = {2017},
  publisher = {Springer International Publishing},
  series    = {SpringerBriefs in Mathematics},
  doi       = {10.1007/978-3-319-62689-5},
  isbn      = {978-3-319-62689-5}
}
```

## License

MIT

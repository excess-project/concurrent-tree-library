Tree Libraries
==== 

How to run concurrent search trees benchmarks:

1. Clone the repository
2. Go to `./bench` directory
3. Generate the search tree binaries. Run `./make-bins.sh`
4. Run `qsub ./bench-short` for short benchmark OR `qsub ./bench-long` for complete benchmark (>2 hours).
5. Combined data in CSV format will be available in `./bench/combined` directory
6. Comparison chart in PDF format will be generated in `./bench/charts` directory
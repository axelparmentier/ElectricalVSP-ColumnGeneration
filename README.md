# Electric Vehicle Fleets: Scalable Route and Recharge Scheduling through Column Generation

This repository provides the code necessary to reproduce the numerical experiments of the paper

> Parmentier, Axel, Martinelli, Rafael, and Vidal, Thibaut, [Electric Vehicle Fleets: Scalable Route and Recharge Scheduling through Column Generation](https://arxiv.org/abs/2104.03823).

If you use the `latticeRCSP` package for resource constrainted shortest path problems included, please cite the paper below.

> Parmentier, Axel, [Algorithms for non-linear and stochastic resource constrained shortest path](https://link.springer.com/article/10.1007/s00186-018-0649-x)

## Installation

The code is written in `C++17`. It has been compiled on `Windows` and `Linux`. The instructions below are for `Linux`. 

### Dependencies

The code depends on the following libraries: `CPLEX`, `Boost`. An academic version of the [CPLEX](https://www.ibm.com/analytics/cplex-optimizer) library can be obtained [here](http://ibm.biz/CPLEXonAI).

Boost can be installed using the package manager of your distribution. For example, on `Ubuntu`:

```bash
sudo apt install libboost-all-dev
```

### Compilation

The code can be compiled using `Make`. Before compiling, you need to set the environment variables to find `CPLEX`. To that purpose, update the file `cplex_conf.sh` with the path to your `CPLEX` installation. Then, run the following commands:

```bash
source cplex_conf.sh
```

The code can then be compiled using:

```bash
make
```

The resulting executable is `relsease/evsp`. A debug version can be compiled using:

```bash
make debug
```

## Usage

Execute the program using:

```bash
./release/evsp
```

This will give you a list of options and parameters. 
Options `-evsp`, `-cg-rf`, and `cg-pr mo` should always be specified.
Here are a few examples. In order to run a non-sparsfied column generation on instance `instances/ropke/D2_S4_C100_01.txt`, run:

```bash
./release/evsp instances/ropke/D2_S4_C100_01.txt -evsp -cg -cg-rf -cg-pr mo
```

In order to run a sparsified column generation, run:

```bash
./release/evsp instances/ropke/D2_S4_C100_01.txt -evsp -cg -cg-rf -cg-pr mo -spar
```

In order to run a branch-and-price on the sparsified instance, run

```bash
./release/evsp instances/ropke/D2_S4_C100_01.txt -evsp -bp -cg-rf -cg-pr mo -spar
```

In order to run the diving heuristic on the sparsified instance, run

```bash
./release/evsp instances/ropke/D2_S4_C100_01.txt -evsp -bp -cg-rf -cg-pr mo -spar -bb-d
```

## Reproducing the numerical experiments

The numerical experiments of the paper can be reproduced using the following scripts.

```bash
./scripts/cg.sh     # To calibrate the column generation
./scripts/bp.sh     # Branch and price experiments
./scripts/dive.sh   # Diving heuristic experiments
```

Remark that you may have to install `sem` using `sudo apt install parallel` to rune these scripts.
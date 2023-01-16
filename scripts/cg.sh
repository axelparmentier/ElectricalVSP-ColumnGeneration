#!/bin/bash

./scripts/run.sh instances/ropke/large.txt "-cg -evsp -cg-pr mo -cg-rf -tl 21600 -cg-bo sc -spar -cg-mr 50" cg-spar
./scripts/run.sh instances/ropke/large.txt "-cg -evsp -cg-pr mo -cg-rf -tl 21600 -cg-bo sc -spar -cg-mr 50 -seq" cg-spar-seq
./scripts/run.sh instances/ropke/large.txt "-cg -evsp -cg-pr mo -cg-rf -tl 21600 -cg-bo sc -cg-mr 50" cg


# FRAIG (Functionally Reduced And-Inverter Graph)
Final project of DSNP

## Problem Description
- Implement a special circuit representation, FRAIG (Functionally Reduced And-Inverter Graph), from a circuit description file.

## Overview
1. Using hash to detect structurally equivalent signals in a circuit.
2. Performing Boolean logic simulations and designing your own data structure to
identify functionally equivalent candidate pairs in the circuit.
3. Being able to call Boolean Satisfiability (SAT) solver to prove the functional
equivalence.

## Functionalities (Step by step)
1. Parse the circuit description file in the AIGER format
2. Sweep out the gates that cannot be reached from POs (remove unused gates)
3. Perform trivial circuit optimization (e.g. 1 & x, the and gate could be removed)
4. Provide a function to perform “structural hash” (strash) on the circuit netlist (used to merge the structurally equivalent signals)
5. Perform parallel-pattern simulation to detect the FEC (functionally equivalent candidate) pairs
6. Prove or disprove the signals in a FEC pair by a Boolean Satisfiability (SAT) solver
7. Merge two signals in the circuit netlist if they are proven to be functionally equivalent. 
8. After all of the steps, the resultant circuit will then be simplified as a **functionally reduced And-Inverter graph** (FRAIG)

## Commands Supported (Correspond to the functionalities above)
- CIRRead: read in a circuit and construct the netlist
- CIRPrint: print circuit
- CIRGate: report a gate
- CIRWrite: write the netlist to an ASCII AIG file (.aag)
- CIRSWeep: remove unused gates
- CIROPTimize: perform trivial optimizations
- CIRSTRash: perform structural hash on the circuit netlist
- CIRSIMulate: perform Boolean logic simulation on the circuit
- CIRFraig: perform FRAIG operation on the circuit

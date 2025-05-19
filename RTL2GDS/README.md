#  32-bit ALU RTL to GDSII ASIC Flow using OpenLane and Sky130

This project implements a 32-bit parameterized Arithmetic Logic Unit (ALU) in Verilog and carries it through the full ASIC physical design flow using [OpenLane](https://github.com/The-OpenROAD-Project/OpenLane) and the [Sky130](https://github.com/google/skywater-pdk) open-source PDK.

---

##  Overview

- **Design:** 32-bit ALU supporting configurable logical and arithmetic operations
- **Language:** Verilog (synthesizable RTL)
- **PDK:** Sky130A via Volare
- **Flow Tool:** OpenLane (v1.1.1)
- **Final Output:** GDSII layout, LEF, powered netlist, and signoff reports

---

##  Flow Steps Completed

1. **RTL Design**  
   Wrote a fully synthesizable 32-bit ALU in Verilog with parameterized inputs, inversion flags, and operation select.

2. **Yosys Flattening (Pre-step)**  
   Run in Yosys shell:

    ```bash
    yosys
    > read_verilog alufinal.v
    > synth -top n_bit_alu
    > flatten
    > write_verilog -noattr alufinal_flat.v
    > exit
    ```

3. **OpenLane Flow Execution**  
   Full RTL-to-GDSII flow:

    ```bash
    ./flow.tcl -design openlane/n_bit_alu -tag run_11
    ```

4. **Steps Performed**  
   - RTL parsing & synthesis (Yosys)  
   - Floorplanning and IO placement  
   - Power grid generation (multi-layer PDN)  
   - Global + detailed placement (RePlAce + OpenDP)  
   - Clock tree synthesis (CTS)  
   - Routing (FastRoute + TritonRoute)  
   - SPEF parasitic extraction  
   - Multi-corner static timing analysis (STA)  
   - GDSII & LEF generation  
   - Magic DRC and Netgen LVS  
   - KLayout DRC (16 minor violations)

---

## Verification and Constraints

- **Clock Port:** `fake_clk`
- **Clock Period:** `10.0` ns
- **SDC files:** Used `base.sdc` for synthesis and signoff constraints; `pnr.sdc` optionally for placement/routing
- **DRC Checks:** Passed Magic DRC; minor KLayout violations present
- **LVS Checks:** Netlist successfully matched layout using Netgen
- **STA:** Multi-corner timing analysis completed

---

## Layout Visualization

To view the final GDSII layout:

```bash
klayout openlane/n_bit_alu/runs/run_11/results/final/gds/n_bit_alu.gds


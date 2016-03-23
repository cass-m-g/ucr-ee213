hring5 (optional)
.lib 'SAED90nm.lib' TT_12 
*.MODEL NCH n12.1 
*.MODEL PCH p12.1 
.GLOBAL VDD GND

Vsupply VDD 0 1
Vground GND 0 0

.subckt inverter in out
M1 out in GND GND n12 L=0.1u W=2.0u 
M2 out in VDD VDD p12 L=0.1u W=4.0u
Cload out GND 100f
.ends

X1 a5 a1 inverter
X2 a1 a2 inverter
X3 a2 a3 inverter
X4 a3 a4 inverter
X5 a4 a5 inverter

.IC v(a1)=1 

.TRAN 5p 5n 

.END

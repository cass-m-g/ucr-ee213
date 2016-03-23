hring5
.lib 'SAED90nm.lib' TT_12 
*.MODEL NCH n12.1 
*.MODEL PCH p12.1 
.GLOBAL VDD GND

.param agauss_l=unif(100n,5)
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
*sweep monte=20 

.model p1_ra mosra level=1
+tit0=3e-7 titfd=7.5e-10 tittd=1.45e-20
+tn=0.25

.param month=2.628e6
.appendmodel p1_ra mosra p12 pmos
.mosra reltotaltime = 3*month

.OPTION LIST POST PROBE 
.PROBE V(a1) V(a2) V(a3) V(a4) V(a5)
.measure tran tcycle trig V(a2) val=0.5 rise=2
+		 targ V(a2) val=0.5 rise=3

******* Alter cases
.alter case 2:  6 months
.mosra reltotaltime = 6*month

.alter case 3:  9 months
.mosra reltotaltime = 9*month 

.alter case 4:  12 months
.mosra reltotaltime = 12*month

.alter case 5:  15 months
.mosra reltotaltime = 15*month

.alter case 6:  18 months
.mosra reltotaltime = 18*month

.alter case 7:  21 months
.mosra reltotaltime = 21*month

.alter case 8:  24 months
.mosra reltotaltime = 24*month

.alter case 9:  27 months
.mosra reltotaltime = 27*month

.alter case 10:  30 months
.mosra reltotaltime = 30*month

.alter case 11:  60 months
.mosra reltotaltime = 60*month

.alter case 12:  90 months
.mosra reltotaltime = 90*month

*.alter case 11:  120 months
*.mosra reltotaltime = 120*month
.END

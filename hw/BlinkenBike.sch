EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title "BlinkenBike"
Date "2020-10-12"
Rev "r1.x"
Comp "PhiSch.org"
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L Switch:SW_Push SW3
U 1 1 5F7C251F
P 1900 2800
F 0 "SW3" H 1900 3085 50  0000 C CNN
F 1 "SW_Push" H 1900 2994 50  0000 C CNN
F 2 "" H 1900 3000 50  0001 C CNN
F 3 "~" H 1900 3000 50  0001 C CNN
	1    1900 2800
	0    -1   -1   0   
$EndComp
$Comp
L Device:R_Variable R3
U 1 1 5F7C5647
P 1900 2000
F 0 "R3" H 2028 2046 50  0000 L CNN
F 1 "> 5k" H 2028 1955 50  0000 L CNN
F 2 "" V 1830 2000 50  0001 C CNN
F 3 "~" H 1900 2000 50  0001 C CNN
	1    1900 2000
	1    0    0    -1  
$EndComp
$Comp
L Device:Q_NMOS_DGS Q1
U 1 1 5F7CB1D0
P 4500 2400
F 0 "Q1" H 4704 2446 50  0000 L CNN
F 1 "BS170" H 4704 2355 50  0000 L CNN
F 2 "" H 4700 2500 50  0001 C CNN
F 3 "~" H 4500 2400 50  0001 C CNN
	1    4500 2400
	1    0    0    -1  
$EndComp
$Comp
L Device:R R5
U 1 1 5F7D5C43
P 4600 1900
F 0 "R5" H 4670 1946 50  0000 L CNN
F 1 "1k" H 4670 1855 50  0000 L CNN
F 2 "" V 4530 1900 50  0001 C CNN
F 3 "~" H 4600 1900 50  0001 C CNN
	1    4600 1900
	1    0    0    -1  
$EndComp
Text Label 1900 1750 0    50   ~ 0
+
$Comp
L Device:R R4
U 1 1 5F7DBE94
P 4200 1900
F 0 "R4" H 4270 1946 50  0000 L CNN
F 1 "1M" H 4270 1855 50  0000 L CNN
F 2 "" V 4130 1900 50  0001 C CNN
F 3 "~" H 4200 1900 50  0001 C CNN
	1    4200 1900
	1    0    0    -1  
$EndComp
Text Label 2750 2400 0    50   ~ 0
SW
Text Notes 850  1350 0    50   ~ 0
KY-040\nRotary Encoder
Wire Notes Line
	1550 3250 1550 1800
$Comp
L Device:R R1
U 1 1 5F83D7F3
P 1650 5500
F 0 "R1" H 1720 5546 50  0000 L CNN
F 1 "10k" H 1720 5455 50  0000 L CNN
F 2 "" V 1580 5500 50  0001 C CNN
F 3 "~" H 1650 5500 50  0001 C CNN
	1    1650 5500
	0    1    1    0   
$EndComp
$Comp
L Device:R R2
U 1 1 5F83DC9A
P 1700 4050
F 0 "R2" H 1770 4096 50  0000 L CNN
F 1 "10k" H 1770 4005 50  0000 L CNN
F 2 "" V 1630 4050 50  0001 C CNN
F 3 "~" H 1700 4050 50  0001 C CNN
	1    1700 4050
	0    -1   -1   0   
$EndComp
$Comp
L Switch:SW_Push SW2
U 1 1 5F8417CE
P 1950 5150
F 0 "SW2" V 1996 5102 50  0000 R CNN
F 1 "SW_Push" V 1905 5102 50  0000 R CNN
F 2 "" H 1950 5350 50  0001 C CNN
F 3 "~" H 1950 5350 50  0001 C CNN
	1    1950 5150
	0    -1   -1   0   
$EndComp
$Comp
L Switch:SW_Push SW1
U 1 1 5F8424AD
P 1950 4400
F 0 "SW1" V 1904 4548 50  0000 L CNN
F 1 "SW_Push" V 1995 4548 50  0000 L CNN
F 2 "" H 1950 4600 50  0001 C CNN
F 3 "~" H 1950 4600 50  0001 C CNN
	1    1950 4400
	0    1    1    0   
$EndComp
$Comp
L power:+5V #PWR07
U 1 1 5F845BB7
P 6800 850
F 0 "#PWR07" H 6800 700 50  0001 C CNN
F 1 "+5V" H 6815 1023 50  0000 C CNN
F 2 "" H 6800 850 50  0001 C CNN
F 3 "" H 6800 850 50  0001 C CNN
	1    6800 850 
	1    0    0    -1  
$EndComp
$Comp
L Transistor_FET:BS170 Q2
U 1 1 5F846F9C
P 6700 2150
F 0 "Q2" H 6904 2196 50  0000 L CNN
F 1 "BS170" H 6904 2105 50  0000 L CNN
F 2 "Package_TO_SOT_THT:TO-92_Inline" H 6900 2075 50  0001 L CIN
F 3 "http://www.fairchildsemi.com/ds/BS/BS170.pdf" H 6700 2150 50  0001 L CNN
	1    6700 2150
	1    0    0    -1  
$EndComp
$Comp
L Device:R R7
U 1 1 5F8485D1
P 6800 1650
F 0 "R7" H 6870 1696 50  0000 L CNN
F 1 "2k2" H 6870 1605 50  0000 L CNN
F 2 "" V 6730 1650 50  0001 C CNN
F 3 "~" H 6800 1650 50  0001 C CNN
	1    6800 1650
	1    0    0    -1  
$EndComp
$Comp
L Device:R R8
U 1 1 5F84BBB2
P 7150 1900
F 0 "R8" V 6943 1900 50  0000 C CNN
F 1 "330R" V 7034 1900 50  0000 C CNN
F 2 "" V 7080 1900 50  0001 C CNN
F 3 "~" H 7150 1900 50  0001 C CNN
	1    7150 1900
	0    1    1    0   
$EndComp
Wire Wire Line
	6800 1800 6800 1900
Wire Wire Line
	7000 1900 6800 1900
$Comp
L power:GND #PWR08
U 1 1 5F84FD03
P 6800 3200
F 0 "#PWR08" H 6800 2950 50  0001 C CNN
F 1 "GND" H 6805 3027 50  0000 C CNN
F 2 "" H 6800 3200 50  0001 C CNN
F 3 "" H 6800 3200 50  0001 C CNN
	1    6800 3200
	1    0    0    -1  
$EndComp
Text Label 7950 1900 2    50   ~ 0
D_In
Text Label 6250 3450 1    50   ~ 0
PA7
Wire Notes Line
	6400 1400 7350 1400
Text Notes 6400 1500 0    50   ~ 0
WS2812b Level Shift
Text Notes 6800 1100 1    50   ~ 0
[gn]
Text Notes 7750 1900 2    50   ~ 0
[ws]
Text Notes 6800 2900 0    50   ~ 0
[sw]
Text Notes 6250 2900 0    50   ~ 0
[br]
Wire Notes Line
	3700 3500 3700 5900
Text Notes 3750 3700 0    50   ~ 0
STM32F103C8T6\n(„STM32 BluePill“)\n
NoConn ~ 6250 3800
NoConn ~ 3900 4050
$Comp
L Device:R R6
U 1 1 5F86CEC3
P 5850 4050
F 0 "R6" H 5920 4096 50  0000 L CNN
F 1 "40k" H 5920 4005 50  0000 L CNN
F 2 "" V 5780 4050 50  0001 C CNN
F 3 "~" H 5850 4050 50  0001 C CNN
	1    5850 4050
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR09
U 1 1 5F870DE5
P 7500 6200
F 0 "#PWR09" H 7500 5950 50  0001 C CNN
F 1 "GND" H 7505 6027 50  0000 C CNN
F 2 "" H 7500 6200 50  0001 C CNN
F 3 "" H 7500 6200 50  0001 C CNN
	1    7500 6200
	1    0    0    -1  
$EndComp
Text Notes 7500 5650 0    50   ~ 0
[sw]
Text Notes 5850 3450 2    50   ~ 0
[rt]
Wire Wire Line
	4600 1650 4600 1750
$Comp
L power:GND #PWR05
U 1 1 5F8CBAAA
P 4600 3200
F 0 "#PWR05" H 4600 2950 50  0001 C CNN
F 1 "GND" H 4605 3027 50  0000 C CNN
F 2 "" H 4600 3200 50  0001 C CNN
F 3 "" H 4600 3200 50  0001 C CNN
	1    4600 3200
	1    0    0    -1  
$EndComp
Text Notes 4200 1100 1    50   ~ 0
[rt]
Text Notes 5500 3450 0    50   ~ 0
[or]
Wire Wire Line
	4600 2050 4600 2150
Connection ~ 4600 2150
Wire Wire Line
	4600 2150 4600 2200
Wire Notes Line
	5300 1400 5300 2650
Wire Notes Line
	5300 2650 3800 2650
Wire Notes Line
	3800 2650 3800 1400
Wire Notes Line
	3800 1400 5300 1400
Wire Wire Line
	4200 1650 4600 1650
Wire Wire Line
	4200 1650 4200 1750
Wire Wire Line
	4200 2050 4200 2400
Wire Wire Line
	4200 2400 4300 2400
Text Notes 3800 1500 0    50   ~ 0
Inverter for WKUP Pin
Wire Wire Line
	1900 2150 1900 2400
Wire Notes Line
	6400 2650 7350 2650
Wire Notes Line
	7350 1400 7350 2650
Wire Notes Line
	6400 1400 6400 2650
Text Notes 3700 1250 0    50   ~ 0
Breadboard Adapter
Wire Notes Line
	3700 1150 7450 1150
Wire Notes Line
	7450 1150 7450 2800
Wire Notes Line
	7450 2800 3700 2800
Wire Notes Line
	3700 2800 3700 1150
Wire Wire Line
	4200 1650 4200 850 
Connection ~ 4200 1650
Wire Wire Line
	6800 1500 6800 850 
Wire Notes Line
	8000 1150 8000 2800
Text Notes 8000 1350 0    50   ~ 0
WS2812b Strip\n(1+9+9 = 19 LEDs)
$Comp
L power:GND #PWR013
U 1 1 5F9037BB
P 7800 3200
F 0 "#PWR013" H 7800 2950 50  0001 C CNN
F 1 "GND" H 7805 3027 50  0000 C CNN
F 2 "" H 7800 3200 50  0001 C CNN
F 3 "" H 7800 3200 50  0001 C CNN
	1    7800 3200
	1    0    0    -1  
$EndComp
$Comp
L power:+5V #PWR012
U 1 1 5F904962
P 7800 850
F 0 "#PWR012" H 7800 700 50  0001 C CNN
F 1 "+5V" H 7815 1023 50  0000 C CNN
F 2 "" H 7800 850 50  0001 C CNN
F 3 "" H 7800 850 50  0001 C CNN
	1    7800 850 
	1    0    0    -1  
$EndComp
Wire Notes Line
	7450 3500 7450 5900
Wire Notes Line
	3700 3500 7450 3500
Wire Notes Line
	3700 5900 7450 5900
Wire Wire Line
	4600 2150 5500 2150
Text Label 5500 3450 1    50   ~ 0
PA0
Wire Wire Line
	6500 2150 6250 2150
Wire Wire Line
	6250 2150 6250 3800
$Comp
L power:VDD #PWR06
U 1 1 5F91ED58
P 5850 3200
F 0 "#PWR06" H 5850 3050 50  0001 C CNN
F 1 "VDD" H 5865 3373 50  0000 C CNN
F 2 "" H 5850 3200 50  0001 C CNN
F 3 "" H 5850 3200 50  0001 C CNN
	1    5850 3200
	1    0    0    -1  
$EndComp
Wire Notes Line
	6100 3750 6100 4550
Wire Notes Line
	6100 4550 5200 4550
Wire Notes Line
	5200 4550 5200 3750
Wire Notes Line
	5200 3750 6100 3750
Text Notes 5200 4550 0    50   ~ 0
WKUP Pin w/\ninternal 40k\nPull-down
Connection ~ 1900 2400
Wire Wire Line
	1900 2400 1900 2600
Wire Wire Line
	1900 850  1900 1600
Wire Wire Line
	1800 5500 1950 5500
Wire Wire Line
	1950 5500 1950 5350
Wire Wire Line
	1850 4050 1950 4050
Wire Wire Line
	1950 4050 1950 4200
$Comp
L power:GND #PWR03
U 1 1 5F93E977
P 3350 4750
F 0 "#PWR03" H 3350 4500 50  0001 C CNN
F 1 "GND" V 3355 4622 50  0000 R CNN
F 2 "" H 3350 4750 50  0001 C CNN
F 3 "" H 3350 4750 50  0001 C CNN
	1    3350 4750
	0    -1   -1   0   
$EndComp
Wire Wire Line
	1550 4050 1350 4050
Wire Wire Line
	1350 4050 1350 4750
Wire Wire Line
	1350 5500 1500 5500
Wire Wire Line
	1350 4750 1200 4750
Connection ~ 1350 4750
Wire Wire Line
	1350 4750 1350 5500
Wire Wire Line
	1900 3000 1900 3400
$Comp
L power:GND #PWR02
U 1 1 5F9445D7
P 1900 3400
F 0 "#PWR02" H 1900 3150 50  0001 C CNN
F 1 "GND" H 1905 3227 50  0000 C CNN
F 2 "" H 1900 3400 50  0001 C CNN
F 3 "" H 1900 3400 50  0001 C CNN
	1    1900 3400
	1    0    0    -1  
$EndComp
Connection ~ 1950 4050
Connection ~ 1950 5500
NoConn ~ 3900 5500
Wire Notes Line
	2500 3750 1000 3750
Wire Notes Line
	1000 3750 1000 5800
Wire Notes Line
	1000 5800 2500 5800
Wire Notes Line
	2500 3750 2500 5800
Wire Notes Line
	2500 1800 2500 3250
Wire Notes Line
	1550 3250 2500 3250
Wire Notes Line
	1550 1800 2500 1800
Wire Wire Line
	1950 4050 3900 4050
Wire Wire Line
	1950 5500 3900 5500
Text Label 3650 4050 2    50   ~ 0
PB7
Text Label 3650 5500 2    50   ~ 0
PB6
Wire Wire Line
	1900 2400 4200 2400
Connection ~ 4200 2400
Text Label 2750 4050 0    50   ~ 0
CLK
Text Label 2750 5500 0    50   ~ 0
DT
Text Notes 3100 2400 0    50   ~ 0
[ge]
Text Notes 3100 4050 0    50   ~ 0
[ws]
Text Notes 3100 5500 0    50   ~ 0
[bl]
Wire Notes Line
	2700 5900 2700 1150
Wire Notes Line
	2700 1150 800  1150
Wire Notes Line
	800  1150 800  5900
Wire Notes Line
	800  5900 2700 5900
Wire Wire Line
	1950 4600 1950 4750
Wire Wire Line
	3350 4750 1950 4750
Connection ~ 1950 4750
Wire Wire Line
	1950 4750 1950 4950
Wire Wire Line
	1200 4750 1200 1600
Wire Wire Line
	1200 1600 1900 1600
Connection ~ 1900 1600
Wire Wire Line
	1900 1600 1900 1850
Text Notes 3100 4750 0    50   ~ 0
[sw]
Text Notes 1900 1050 0    50   ~ 0
[rt]
Wire Wire Line
	5850 3200 5850 3850
Wire Wire Line
	5850 3850 5500 3850
Wire Wire Line
	5500 2150 5500 3850
Connection ~ 5850 3850
Wire Wire Line
	5850 3850 5850 3900
NoConn ~ 7250 3800
NoConn ~ 7250 4700
NoConn ~ 7250 5650
Wire Wire Line
	7250 4700 7750 4700
Wire Wire Line
	7250 3800 7750 3800
Wire Wire Line
	7500 5650 7500 6200
Wire Wire Line
	4600 2600 4600 3200
Text Notes 5500 2900 0    50   ~ 0
[or]
Text Notes 7750 1750 2    50   ~ 0
[gn]
Text Notes 7750 2050 2    50   ~ 0
[sw]
Text Notes 2450 5800 2    50   ~ 0
Turning Knob
Text Notes 2450 3250 2    50   ~ 0
Push Button
$Comp
L power:+5V #PWR010
U 1 1 5F9DBBB2
P 7750 3800
F 0 "#PWR010" H 7750 3650 50  0001 C CNN
F 1 "+5V" V 7765 3928 50  0000 L CNN
F 2 "" H 7750 3800 50  0001 C CNN
F 3 "" H 7750 3800 50  0001 C CNN
	1    7750 3800
	0    1    1    0   
$EndComp
Text Notes 5950 5350 0    50   Italic 0
Note: STM32 Board is supposed\nto be supplied w/ 5V. It generates\n3.3V from a Richtek RT9193 LDO\nRegulator.
Wire Notes Line
	5900 5000 7350 5000
Wire Notes Line
	7350 5000 7350 5400
Wire Notes Line
	7350 5400 5900 5400
Wire Notes Line
	5900 5000 5900 5400
Wire Wire Line
	5850 4200 5850 5650
$Comp
L power:+3.3V #PWR01
U 1 1 5F9E63AA
P 1900 850
F 0 "#PWR01" H 1900 700 50  0001 C CNN
F 1 "+3.3V" H 1915 1023 50  0000 C CNN
F 2 "" H 1900 850 50  0001 C CNN
F 3 "" H 1900 850 50  0001 C CNN
	1    1900 850 
	1    0    0    -1  
$EndComp
$Comp
L power:+3.3V #PWR04
U 1 1 5F9E7217
P 4200 850
F 0 "#PWR04" H 4200 700 50  0001 C CNN
F 1 "+3.3V" H 4215 1023 50  0000 C CNN
F 2 "" H 4200 850 50  0001 C CNN
F 3 "" H 4200 850 50  0001 C CNN
	1    4200 850 
	1    0    0    -1  
$EndComp
$Comp
L power:+3.3V #PWR011
U 1 1 5F9E81D9
P 7750 4700
F 0 "#PWR011" H 7750 4550 50  0001 C CNN
F 1 "+3.3V" V 7765 4828 50  0000 L CNN
F 2 "" H 7750 4700 50  0001 C CNN
F 3 "" H 7750 4700 50  0001 C CNN
	1    7750 4700
	0    1    1    0   
$EndComp
Text Notes 7500 4700 0    50   ~ 0
[rt]
Text Notes 7500 3800 0    50   ~ 0
[gn]
Wire Wire Line
	7500 5650 5850 5650
Text Label 2750 4750 0    50   ~ 0
GND
Text Notes 6250 3450 0    50   ~ 0
[br]
Wire Notes Line
	8250 1700 8250 2050
Text Notes 8300 2000 0    50   ~ 0
User\nInterface\nLED
Wire Notes Line
	8700 2050 8700 1700
Wire Notes Line
	8250 2050 8700 2050
Wire Notes Line
	8250 1700 8700 1700
Wire Wire Line
	8100 2050 7800 2050
NoConn ~ 8100 2050
NoConn ~ 8100 1750
Wire Wire Line
	7300 1900 8100 1900
NoConn ~ 8100 1900
NoConn ~ 8850 1750
NoConn ~ 8850 1900
NoConn ~ 8850 2050
Wire Wire Line
	7800 1750 8100 1750
Wire Wire Line
	7800 850  7800 1750
Wire Notes Line
	8000 2800 8950 2800
Wire Notes Line
	9500 2800 11000 2800
Wire Notes Line
	11000 2800 11000 1150
Wire Wire Line
	9400 900  9400 1750
$Comp
L power:+5V #PWR014
U 1 1 5FA07852
P 9400 900
F 0 "#PWR014" H 9400 750 50  0001 C CNN
F 1 "+5V" H 9415 1073 50  0000 C CNN
F 2 "" H 9400 900 50  0001 C CNN
F 3 "" H 9400 900 50  0001 C CNN
	1    9400 900 
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR015
U 1 1 5FA09B25
P 9400 3200
F 0 "#PWR015" H 9400 2950 50  0001 C CNN
F 1 "GND" H 9405 3027 50  0000 C CNN
F 2 "" H 9400 3200 50  0001 C CNN
F 3 "" H 9400 3200 50  0001 C CNN
	1    9400 3200
	1    0    0    -1  
$EndComp
Text Notes 9400 1750 2    50   ~ 0
[gn]
Text Notes 9400 2050 2    50   ~ 0
[sw]
Text Notes 9400 1900 2    50   ~ 0
[ws]
Text Label 9550 1900 0    50   ~ 0
D_Out
Wire Wire Line
	9400 1750 9850 1750
Wire Wire Line
	9400 2050 9850 2050
Wire Wire Line
	8950 1900 9850 1900
NoConn ~ 9850 2050
NoConn ~ 9850 1900
NoConn ~ 9850 1750
Wire Notes Line
	9950 1700 9950 2050
Wire Notes Line
	9950 2050 10850 2050
Wire Notes Line
	10850 2050 10850 1700
Wire Notes Line
	10850 1700 9950 1700
Text Notes 10000 2000 0    50   ~ 0
2x\n9 LED\nBars
Wire Notes Line
	11000 1150 9500 1150
Wire Notes Line
	8000 1150 8950 1150
Wire Notes Line
	9500 1150 9500 2800
Wire Notes Line
	8950 1150 8950 2800
Wire Wire Line
	6800 2350 6800 3250
Wire Wire Line
	7800 2050 7800 3200
Wire Wire Line
	9400 2050 9400 3200
Text Notes 800  6900 0    50   Italic 0
Note: The Rotary Encoder works as follows:\n- When the Knob is turned to a latching position, both\n  Switches (SW1 and SW2) will be closed or opened.\n- When the knob is turned clock-wise, then SW1\n  will close / open first, then SW2 will close / open\n  right after, but before the next latching position is\n  reached.\n- When the knob is turned counter-clockwise, then\n  the SW2 will open / close first, then SW1 will follow\n  before the next latching position is needed.
Text Notes 3750 5100 0    50   Italic 0
Note: The wiring shown here is\nbuggy; it reverses the turning\ndirection information in the µC.\n\nThis is compensated in S/W.\n\nCorrect Mapping should be:\nCLK —> PB7 —> TIM4_CH2\nDT —> PB6 —> TIM4_CH1
$EndSCHEMATC

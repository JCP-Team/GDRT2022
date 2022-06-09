EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L Schematic-rescue:Arduino_Nano_Every-MCU_Module A?
U 1 1 6274C323
P 4400 3550
F 0 "A?" H 4400 2461 50  0000 C CNN
F 1 "Arduino_Nano_Every" H 4400 2370 50  0000 C CNN
F 2 "Module:Arduino_Nano" H 4400 3550 50  0001 C CIN
F 3 "https://content.arduino.cc/assets/NANOEveryV3.0_sch.pdf" H 4400 3550 50  0001 C CNN
	1    4400 3550
	1    0    0    -1  
$EndComp
$Comp
L Device:Solar_Cell SC?
U 1 1 62788412
P 6000 2000
F 0 "SC?" V 5695 2050 50  0000 C CNN
F 1 "Solar_Cell" V 5786 2050 50  0000 C CNN
F 2 "" V 6000 2060 50  0001 C CNN
F 3 "~" V 6000 2060 50  0001 C CNN
	1    6000 2000
	0    1    1    0   
$EndComp
$Comp
L power:GND #PWR?
U 1 1 6278B20D
P 6400 2000
F 0 "#PWR?" H 6400 1750 50  0001 C CNN
F 1 "GND" H 6405 1827 50  0000 C CNN
F 2 "" H 6400 2000 50  0001 C CNN
F 3 "" H 6400 2000 50  0001 C CNN
	1    6400 2000
	1    0    0    -1  
$EndComp
Wire Wire Line
	6400 2050 6400 2000
Wire Wire Line
	6200 2000 6400 2000
Connection ~ 6400 2000
$Comp
L SparkFun:I2C_Standard_PTH_Connector J?
U 1 1 6279FBF7
P 6000 3350
F 0 "J?" H 6057 3717 50  0000 C CNN
F 1 "I2C_Standard_PTH_Connector" H 6057 3626 50  0000 C CNN
F 2 "" H 5980 3450 20  0001 C CNN
F 3 "" H 5950 3300 60  0001 C CNN
	1    6000 3350
	-1   0    0    1   
$EndComp
$Comp
L Zander-Components:VoltBooster U?
U 1 1 62752287
P 3950 2350
F 0 "U?" H 3875 2425 50  0000 C CNN
F 1 "VoltBooster" H 3875 2334 50  0000 C CNN
F 2 "" H 3950 2350 50  0001 C CNN
F 3 "" H 3950 2350 50  0001 C CNN
	1    3950 2350
	-1   0    0    1   
$EndComp
$Comp
L Zander-Components:SolarCharger U?
U 1 1 6274FAA1
P 5000 2350
F 0 "U?" H 4925 2465 50  0000 C CNN
F 1 "SolarCharger" H 4925 2374 50  0000 C CNN
F 2 "" H 5000 2350 50  0001 C CNN
F 3 "" H 5000 2350 50  0001 C CNN
	1    5000 2350
	-1   0    0    1   
$EndComp
$Comp
L Device:Battery_Cell BT?
U 1 1 62A5A8E4
P 3950 1750
F 0 "BT?" H 3832 1754 50  0000 R CNN
F 1 "Battery_Cell" H 3832 1845 50  0000 R CNN
F 2 "" V 3950 1810 50  0001 C CNN
F 3 "~" V 3950 1810 50  0001 C CNN
	1    3950 1750
	-1   0    0    1   
$EndComp
Wire Wire Line
	3950 1650 4900 1650
Wire Wire Line
	4900 1650 4900 1800
Wire Wire Line
	4900 1800 5000 1800
Wire Wire Line
	5000 1900 4900 1900
Wire Wire Line
	4900 1900 4900 2000
Wire Wire Line
	4900 2000 3950 2000
Wire Wire Line
	5000 2100 4250 2100
Wire Wire Line
	4250 2100 4250 2150
Wire Wire Line
	4250 2150 4200 2150
Wire Wire Line
	4200 2150 4200 2200
Connection ~ 4200 2150
Wire Wire Line
	4200 2200 5000 2200
Connection ~ 4200 2200
$Comp
L Zander-Components:SIM800L U?
U 1 1 62A5F83D
P 2550 2700
F 0 "U?" V 3315 2392 50  0000 C CNN
F 1 "SIM800L" V 3224 2392 50  0000 C CNN
F 2 "" H 2550 2700 50  0001 C CNN
F 3 "" H 2550 2700 50  0001 C CNN
	1    2550 2700
	0    -1   -1   0   
$EndComp
Wire Wire Line
	3800 2200 3800 2150
Wire Wire Line
	3800 2200 3650 2200
Connection ~ 3800 2200
Wire Wire Line
	3650 2200 3650 2350
Wire Wire Line
	3650 2550 4300 2550
Wire Wire Line
	3400 2150 3400 2300
Wire Wire Line
	3400 4550 4400 4550
Wire Wire Line
	4400 4550 5300 4550
Wire Wire Line
	5300 4550 5300 3200
Wire Wire Line
	5300 3200 5800 3200
Connection ~ 4400 4550
Wire Wire Line
	3050 2600 3500 2600
Wire Wire Line
	3500 2600 3500 3050
Wire Wire Line
	3500 3050 3900 3050
Wire Wire Line
	3900 3150 3300 3150
Wire Wire Line
	3300 3150 3300 2700
Wire Wire Line
	3300 2700 3050 2700
Wire Wire Line
	3050 2800 3200 2800
Wire Wire Line
	3200 2800 3200 3350
Wire Wire Line
	3200 3350 3400 3350
Connection ~ 3400 3350
Wire Wire Line
	3400 3350 3400 4550
Wire Wire Line
	3050 2900 3050 3250
Wire Wire Line
	3050 3250 3900 3250
Wire Wire Line
	4900 3550 5000 3550
Wire Wire Line
	5000 3550 5000 4850
Wire Wire Line
	5000 4850 2550 4850
Wire Wire Line
	2550 4850 2550 1850
Wire Wire Line
	2550 1850 3950 1850
Wire Wire Line
	3950 1850 3950 1950
Connection ~ 3950 1950
Wire Wire Line
	3950 1950 3950 2000
Wire Wire Line
	4900 3950 5100 3950
Wire Wire Line
	5100 3950 5100 3400
Wire Wire Line
	5100 3400 5800 3400
Wire Wire Line
	5800 3500 5200 3500
Wire Wire Line
	5200 3500 5200 4050
Wire Wire Line
	5200 4050 4900 4050
Wire Wire Line
	5800 3300 5100 3300
Wire Wire Line
	5100 3300 5100 2550
Wire Wire Line
	5100 2550 4600 2550
Wire Wire Line
	5900 2000 5300 2000
$Comp
L Device:CP_Small C?
U 1 1 62A943B5
P 3250 2200
F 0 "C?" H 3338 2246 50  0000 L CNN
F 1 "CP_Small" H 3338 2155 50  0000 L CNN
F 2 "" H 3250 2200 50  0001 C CNN
F 3 "~" H 3250 2200 50  0001 C CNN
	1    3250 2200
	1    0    0    -1  
$EndComp
Wire Wire Line
	3750 2150 3400 2150
Wire Wire Line
	3650 2350 3500 2350
Wire Wire Line
	3500 2350 3500 2100
Wire Wire Line
	3500 2100 3250 2100
Connection ~ 3650 2350
Wire Wire Line
	3650 2350 3650 2550
Wire Wire Line
	3250 2100 3100 2100
Wire Wire Line
	3100 2100 3100 2200
Wire Wire Line
	3100 2200 3050 2200
Connection ~ 3250 2100
Wire Wire Line
	3400 2300 3250 2300
Connection ~ 3400 2300
Wire Wire Line
	3400 2300 3400 3350
Connection ~ 3250 2300
Wire Wire Line
	3250 2300 3050 2300
$EndSCHEMATC

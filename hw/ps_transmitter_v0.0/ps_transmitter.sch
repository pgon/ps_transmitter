EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title "Transmisor de presión WiFi"
Date "2021-08-21"
Rev "0"
Comp "IAP"
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L MCU_Module:WeMos_D1_mini U1
U 1 1 61219EB9
P 5900 3350
F 0 "U1" H 5900 2461 50  0000 C CNN
F 1 "WeMos_D1_mini" H 5900 2370 50  0000 C CNN
F 2 "Module:WEMOS_D1_mini_light" H 5900 2200 50  0001 C CNN
F 3 "https://wiki.wemos.cc/products:d1:d1_mini#documentation" H 4050 2200 50  0001 C CNN
	1    5900 3350
	1    0    0    -1  
$EndComp
$Comp
L wemos_d1_mini_batery_shield:WeMos_D1_mini U2
U 1 1 6121BD52
P 3750 3350
F 0 "U2" H 3750 2461 50  0000 C CNN
F 1 "WeMos_Batery_shield" H 3750 2370 50  0000 C CNN
F 2 "Module:WEMOS_D1_mini_light" H 3750 2200 50  0001 C CNN
F 3 "https://wiki.wemos.cc/products:d1:d1_mini#documentation" H 1900 2200 50  0001 C CNN
	1    3750 3350
	1    0    0    -1  
$EndComp
$Comp
L Device:Battery_Cell BT1
U 1 1 6121D2BE
P 2500 3700
F 0 "BT1" H 2618 3796 50  0000 L CNN
F 1 "Battery_Cell" H 2618 3705 50  0000 L CNN
F 2 "" V 2500 3760 50  0001 C CNN
F 3 "~" V 2500 3760 50  0001 C CNN
	1    2500 3700
	1    0    0    -1  
$EndComp
Wire Wire Line
	2500 3500 2500 3450
Wire Wire Line
	2500 3450 3350 3450
Wire Wire Line
	3350 3450 3350 3700
Wire Wire Line
	2500 3800 3350 3800
$Comp
L power:VDD #PWR?
U 1 1 61224759
P 3850 2450
F 0 "#PWR?" H 3850 2300 50  0001 C CNN
F 1 "VDD" H 3867 2623 50  0000 C CNN
F 2 "" H 3850 2450 50  0001 C CNN
F 3 "" H 3850 2450 50  0001 C CNN
	1    3850 2450
	1    0    0    -1  
$EndComp
$Comp
L power:VDD #PWR?
U 1 1 612251DF
P 6000 2450
F 0 "#PWR?" H 6000 2300 50  0001 C CNN
F 1 "VDD" H 6017 2623 50  0000 C CNN
F 2 "" H 6000 2450 50  0001 C CNN
F 3 "" H 6000 2450 50  0001 C CNN
	1    6000 2450
	1    0    0    -1  
$EndComp
Wire Wire Line
	3850 2450 3850 2550
Wire Wire Line
	6000 2450 6000 2550
$Comp
L power:GND #PWR?
U 1 1 61225C69
P 3750 4550
F 0 "#PWR?" H 3750 4300 50  0001 C CNN
F 1 "GND" H 3755 4377 50  0000 C CNN
F 2 "" H 3750 4550 50  0001 C CNN
F 3 "" H 3750 4550 50  0001 C CNN
	1    3750 4550
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR?
U 1 1 61226A0E
P 5900 4550
F 0 "#PWR?" H 5900 4300 50  0001 C CNN
F 1 "GND" H 5905 4377 50  0000 C CNN
F 2 "" H 5900 4550 50  0001 C CNN
F 3 "" H 5900 4550 50  0001 C CNN
	1    5900 4550
	1    0    0    -1  
$EndComp
Wire Wire Line
	3750 4550 3750 4150
Wire Wire Line
	5900 4150 5900 4550
$Comp
L Device:Jumper_NO_Small JP1
U 1 1 612296C6
P 7050 3550
F 0 "JP1" H 7050 3735 50  0000 C CNN
F 1 "serial log" H 7050 3644 50  0000 C CNN
F 2 "" H 7050 3550 50  0001 C CNN
F 3 "~" H 7050 3550 50  0001 C CNN
	1    7050 3550
	1    0    0    -1  
$EndComp
$Comp
L Device:Jumper_NO_Small JP2
U 1 1 6122C0A2
P 7350 3750
F 0 "JP2" H 7350 3935 50  0000 C CNN
F 1 "modem-sleep" H 7350 3844 50  0000 C CNN
F 2 "" H 7350 3750 50  0001 C CNN
F 3 "~" H 7350 3750 50  0001 C CNN
	1    7350 3750
	1    0    0    -1  
$EndComp
Wire Wire Line
	6300 3750 7250 3750
Wire Wire Line
	6300 3550 6950 3550
$Comp
L power:GND #PWR?
U 1 1 6122D6D2
P 7800 3850
F 0 "#PWR?" H 7800 3600 50  0001 C CNN
F 1 "GND" H 7805 3677 50  0000 C CNN
F 2 "" H 7800 3850 50  0001 C CNN
F 3 "" H 7800 3850 50  0001 C CNN
	1    7800 3850
	1    0    0    -1  
$EndComp
Wire Wire Line
	7450 3750 7800 3750
Wire Wire Line
	7800 3750 7800 3850
Wire Wire Line
	7150 3550 7800 3550
Wire Wire Line
	7800 3550 7800 3750
Connection ~ 7800 3750
Wire Wire Line
	5500 2950 5150 2950
Wire Wire Line
	5150 2950 5150 2100
Wire Wire Line
	5150 2100 6600 2100
Wire Wire Line
	6600 2100 6600 2950
Wire Wire Line
	6600 2950 6300 2950
Text Label 5450 2100 0    50   ~ 0
deep_sleep_reset
$Comp
L intrasense_board:Intrasense_calib_board U3
U 1 1 61230523
P 9050 3400
F 0 "U3" H 9380 3446 50  0000 L CNN
F 1 "Intrasense_calib_board" H 9380 3355 50  0000 L CNN
F 2 "Module:Intrasense_calib_board" H 9050 2250 50  0001 C CNN
F 3 "https://wiki.wemos.cc/products:d1:d1_mini#documentation" H 7200 2250 50  0001 C CNN
	1    9050 3400
	1    0    0    -1  
$EndComp
Wire Wire Line
	6300 3250 7400 3250
Wire Wire Line
	7400 3250 7400 3000
Wire Wire Line
	7400 3000 8650 3000
$Comp
L power:GND #PWR?
U 1 1 61231446
P 8300 3150
F 0 "#PWR?" H 8300 2900 50  0001 C CNN
F 1 "GND" H 8305 2977 50  0000 C CNN
F 2 "" H 8300 3150 50  0001 C CNN
F 3 "" H 8300 3150 50  0001 C CNN
	1    8300 3150
	1    0    0    -1  
$EndComp
Wire Wire Line
	8650 3100 8300 3100
Wire Wire Line
	8300 3100 8300 3150
Wire Wire Line
	6300 3150 7850 3150
Wire Wire Line
	7850 3150 7850 3400
Wire Wire Line
	7850 3400 8650 3400
Wire Wire Line
	8650 3300 8000 3300
Wire Wire Line
	8000 3300 8000 3050
Wire Wire Line
	8000 3050 6300 3050
$EndSCHEMATC

/*
 ============================================================================
 Name        : Tec.c
 Author      : Armel
 Version     :
 Copyright   : Your copyright notice
 Description :
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

typedef struct measured{
	float temperatur;  //in Kelvin
	float voltage;
	float resistance;
}measured_t;


//Steinhart koeffizient
float a0, a1, a3;

#define MEASURED_COUNT		18
//das sind gemessene Werte
measured_t measure[MEASURED_COUNT] = {
		{20,  1.41,  0},
		{25,  1.28,  0},
		{30,  1.15,  0},
		{35,  1.03,  0},
		{40,  0.9,   0},
		{45,  0.79,  0},
		{50,  0.7,   0},
		{55,  0.62,  0},
		{60,  0.56,  0},
		{65,  0.47,  0},
		{70,  0.43,  0},
		{75,  0.36,  0},
		{80,  0.32,  0},
		{85,  0.28,  0},
		{90,  0.26,  0},
		{95,  0.24,  0},
		{100, 0.21,  0},
		{105, 0.17,  0},
};

//NTC widerstand anhang de gemessene Spannung berechnen
float NTC_Widerstand(float UT){
	float Uref = 2.7;
	float R7 = 10000; // 10K
	float RT = R7 * (UT/(Uref - UT));  // SAPANNUGSTEILER

	return RT;
}

void Calculate_NTC_Resistance(measured_t* values){

	int i = 0;
	if(values == (measured_t *)0) return;

	for(i=0; i<MEASURED_COUNT; i++){
		values[i].resistance = NTC_Widerstand(values[i].voltage);
	}

}

void Calibrate(float Rlow, float Rmiddle, float Rhigh,
				 float Tlow, float Tmiddle, float Thigh){


  //Steinhart - Hart Equation 1/T = A+B(LnR)+C(LnR)^3

  //Mit der obere Gleichung kann die Koeffizient A, B und C wie folgt gerechnet.
  //es wird immer für die Berechnung der Koeffiziente immer 3 verschiedene
  //Temperaturwerte
  //zum beispiel bei folgende Temperatur wird die Koeffiziente gerechnet
  //bei Tlow (kleine Temperatur) ergibt Rlow und so bekommen wir 1/Tlow = A+B(LnRlow)+C(LnRlow)^3
  //bei Tmiddle (mittlere Temperatur) ergibt Rmiddle und so bekommen wir 1/Tmiddle = A+B(LnRmiddle)+C(LnRmiddle)^3
  //bei Thigh (großere Temperatur) ergibt Rhigh und so bekommen wir 1/Thigh = A+B(LnRhigh)+C(LnRhigh)^3

  // Für 3 Gleichungen
  // 1.   => 1/Tlow = A+B(LnRlow)+C(LnRlow)^3
  // 2.   => 1/Tmiddle = A+B(LnRmiddle)+C(LnRmiddle)^3
  // 3.   => 1/Thigh = A+B(LnRhigh)+C(LnRhigh)^3

  float Y1 = 1/(Tlow + 273.15);  //in kelvin umrechnen
  float Y2 = 1/(Tmiddle + 273.15);
  float Y3 = 1/(Thigh + 273.15);

  float X1 = log(Rlow);
  float X2 = log(Rmiddle);
  float X3 = log(Rhigh);

  float Z1 = (log(Rlow)*log(Rlow)*log(Rlow));
  float Z2 = (log(Rmiddle)*log(Rmiddle)*log(Rmiddle));
  float Z3 = (log(Rhigh)*log(Rhigh)*log(Rhigh));

  float C = ((Y3-Y1)*(X1-X2) + (X1-X3)*(Y1-Y2))/((Z1-Z2)*(X1-X3) - (Z1-Z3)*(X1-X2));
  float B = ((Y1-Y2) - (C*(Z1-Z2)))/(X1-X2);
  float A = Y1 - B*X1 - C*Z1;

  a0 = A;
  a1 = B;
  a3 = C;

  //lgR = log(R_current/Rlow);
  //Result = 1.0/(a0 + a1*lgR + a3*lgR*lgR*lgR) - 273.15; // in °Celsius umrechnen

}

float mittelwert(float werte[], int length){
	if(werte == (float *)0) return 0.0;

	int i=0;
	int count = 0;
	float result = 0.0;
	for(i=0; i<length; i++){
		result += werte[i];
		count++;
	}

	if(count > 0)
		result = result / count;

	return result;
}

float Coeff_a0[20];
float Coeff_a1[20];
float Coeff_a3[20];
void Coeff_Calculate(measured_t* values){
	int count = 0, i;


	if(values == (measured_t *)0) return;

	for(i=0; i<MEASURED_COUNT; i++){
		Calibrate(values[i].resistance, values[i+1].resistance, values[i+2].resistance,
				values[i].temperatur, values[i+1].temperatur, values[i+2].temperatur);

		Coeff_a0[i] = a0;
		Coeff_a1[i] = a1;
		Coeff_a3[i] = a3;
		count++;
		if(values[i].temperatur == 95) break;
	}

	a0 = mittelwert(Coeff_a0, count);
	a1 = mittelwert(Coeff_a1, count);
	a3 = mittelwert(Coeff_a3, count);

}

int main(void) {

	Calculate_NTC_Resistance(&measure);

	Coeff_Calculate(&measure);

	return 0;
}

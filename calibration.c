//Mit gemessene Widerstand berechnen
//Diese Berechnung kommt von der Steinhart-Hart-Gleichung
//https://de.wikipedia.org/wiki/Steinhart-Hart-Gleichung
//Rlow ist der gemessene Widerstand bei der Temperatur Tlow
//Rmiddle ist der gemessene Widerstand bei der Temperatur Tmiddle
//Rhigh ist der gemessene Widerstand bei der Temperatur Thigh

float Temperatur(float Rlow, float Rmiddle, float Rhigh,
				 float Tlow, float Tmiddle, float Thigh, float R_current){

  float Divisor;
  float expTl, expTm, expTh;
  float lnRm, lnRh;
  float ln3Rm, ln3Rh;
  float Result;
  float lgR;

  expTl = 1/(Tlow + 273.15);
  expTm = 1/(Tmiddle + 273.15);
  expTh = 1/(Thigh + 273.15);
  lnRm  = log(Rmiddle);
  ln3Rm = lnRm*lnRm*lnRm;
  lnRh  = log(Rhigh);
  ln3Rh = lnRh*lnRh*lnRh;
  Divisor = lnRm*ln3Rh - lnRh*ln3Rm;
  float a0 = 1/(Tlow + 273.15);
  float a1 = ((expTm - expTl)*ln3Rh - (expTh - expTl)*ln3Rm) / Divisor;
  float a3 = ((expTh - expTl)*lnRm - (expTm - expTl)*lnRh) / Divisor;

  lgR = log(R_current/Rlow);
  Result = 1.0/(a0 + a1*lgR + a3*lgR*lgR*lgR) - 273.15; // in °Celsius

  return Result;
}

/*
 * A program to automate watering plants on my sun-parched deck.
 * 
 * The system will draw water from a reservoir using a small 12 V fountain pump,
 * which will be triggered by measuring soil moisture.  The system will adjust
 * watering frequency by also considering sunlight, temperature, and rain.
 * 
 * Designed around a state machine, with the various sensor readings providing
 * adjustments to the watering timing depending on their results.
 * 
 * Include a small 12 V fan to keep system enclosure from getting too hot.
 * Will need mosfets to control the fan and pump
 * 
 * Reset button, 'water now' button, power switch
 */

const int soilmoist = A0;
const int sunlight = A1;
const int temp = A2;
const int moistpwr = 3;
const int pump = 4;
const int fan = 5;





void setup() {
  pinMode(soilmoist, INPUT);
  pinMode(sunlight, INPUT);
  pinMode(temp, INPUT);
  pinMode(moistpwr, OUTPUT);
  pinMode(pump, OUTPUT);
  pinMode(fan, OUTPUT);
  

}

void loop() {
  // put your main code here, to run repeatedly:

}

boolean checksoil() {
  int watercheck1;
  int watercheck2;
  int watercheck3;
  int avewater;
  digitalWrite(moistpwr,HIGH);
  delay(3000);
  watercheck1 = analogRead(soilmoist);
  delay(1000);
  watercheck2= analogRead(soilmoist);
  delay(1000);
  watercheck3 = analogRead(soilmoist);
  delay(500);
  digitalWrite(moistpwr,LOW);
  avewater = (watercheck1+watercheck2+watercheck3)/3;
  if (avewater <= 300) {
    return true;
  } else {
    return false;
  }
}



























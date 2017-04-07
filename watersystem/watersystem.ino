/*
 * A program to automate watering plants on my sun-parched deck.
 * 
 * The system will draw water from a reservoir using a 12 V fountain pump,
 * which will be triggered by measuring soil moisture.  The system will adjust
 * watering frequency by also considering sunlight, temperature, and rain.
 * 
 * Designed around a state machine, with the various sensor readings providing
 * adjustments to the watering timing depending on their results.
 * 
 * Include a small 12 V fan to keep system enclosure from getting too hot.
 * Will need mosfets to control the fan and pump
 * 
 * status led, that shows when power is on, plus blinking during sensor readings,
 * and slower blinking during watering.
 * 
 * Reset button, 'water now' button, power switch
 * 
 * The analogRead values have to be calibrated/mapped.
 */

const int soilmoist = A0;
const int sunlight = A1;
const int temp = A2;
const int rain = A3;
const int moistpwr = 3;
const int pump = 4;
const int fan = 5;
const int waternow = 6;
const int statusLED = 7;

// state variables for LED
boolean ledstate = LOW;
unsigned long prevLEDmillis = 0;

int checkinterval = 1200000;  //20 minnutes between soil moisture checks
int sensorinterval = 300000;  //5 minutes between sun/rain/temp checks
int watertime = 90000;        //2 minutes watering time

unsigned long prevmainMillis; //main loop state checking
unsigned long prevsensormillis;
unsigned long currentMillis;


void setup() {
  pinMode(soilmoist, INPUT);
  pinMode(sunlight, INPUT);
  pinMode(temp, INPUT);
  pinMode(rain, INPUT);
  pinMode(waternow, INPUT_PULLUP);
  pinMode(moistpwr, OUTPUT);
  pinMode(pump, OUTPUT);
  pinMode(fan, OUTPUT);
  pinMode(statusLED, OUTPUT);

  digitalWrite(statusLED,HIGH);
  ledstate = HIGH;
  
  if (checksoil()) {
    watering();
  }

}

void loop() {
  currentMillis = millis();
  if (currentMillis - prevmainMillis > checkinterval) {
    prevmainMillis = currentMillis;
    if (checksoil()) {
      watering();
    }
  }
  if (currentMillis - prevsensormillis > sensorinterval) {
    prevsensormillis = currentMillis;
    sensors();
  }
  if (waternow == LOW) {    //user initiates watering, plus sensor adjustment
    watering();
    sensors();
    prevmainMillis = currentMillis;
    prevsensormillis = currentMillis;
  }
}

// function that measures soil moisture three times over ten seconds and returns TRUE if
// the average moisture reading is below a threshold.
boolean checksoil() {
  int watercheck1;
  int watercheck2;
  int watercheck3;
  int avewater;
  boolean firstdone = false;
  boolean seconddone = false;
  boolean thirddone = false;  
  unsigned long now_test;
  unsigned long start_test = millis();
  long test_time = start_test + 10000;
  long first_test = start_test + 2500;
  long second_test = start_test + 5000;
  long third_test = start_test + 7500;
  long ontime = 250;
  long offtime = 250;
  
  digitalWrite(moistpwr,HIGH);
  while (now_test <= test_time){
    if (now_test > first_test && firstdone == false){
      watercheck1 = analogRead(soilmoist);
      firstdone = true;
    }
    if (now_test > second_test && seconddone == false) {
      watercheck2 = analogRead(soilmoist);
      seconddone = true;
    }
    if (now_test > third_test && thirddone == false) {
      watercheck3 = analogRead(soilmoist);
      thirddone = true;
    }
    if (ledstate == HIGH && now_test - prevLEDmillis >= ontime) {
      ledstate = LOW;
      prevLEDmillis = now_test;
      digitalWrite(statusLED,ledstate);
    } else if (ledstate == LOW && now_test - prevLEDmillis >= offtime) {
      ledstate = HIGH;
      prevLEDmillis = now_test;
      digitalWrite(statusLED,ledstate);
    }
    now_test = millis();
   }  //end while loop
  digitalWrite(moistpwr,LOW);
  digitalWrite(statusLED, HIGH);
  ledstate = HIGH;
  avewater = (watercheck1+watercheck2+watercheck3)/3;
  if (avewater <= 300) {
    return true;
  } else {
    return false;
  }
}

// function that runs the watering pump
void watering() {
  unsigned long start_watering = millis();
  unsigned long end_watering = start_watering + watertime;
  unsigned long wateringprogress = start_watering;
  digitalWrite(pump, HIGH);
  digitalWrite(statusLED, HIGH);
  long ontime = 1000;
  long offtime = 1000;
  prevLEDmillis = start_watering;
  
  while (wateringprogress <= end_watering) { //while loop blinks status LED while watering
    if (ledstate == HIGH && wateringprogress - prevLEDmillis >= ontime) {
      ledstate = LOW;
      prevLEDmillis = wateringprogress;
      digitalWrite(statusLED,ledstate);
    } else if (ledstate == LOW && wateringprogress - prevLEDmillis >= offtime) {
      ledstate = HIGH;
      prevLEDmillis = wateringprogress;
      digitalWrite(statusLED,ledstate);
    }
    wateringprogress = millis();
  } //end while loop
  digitalWrite(pump, LOW);
  digitalWrite(statusLED, HIGH);
  ledstate = HIGH;
}

// function that checks the rain/sun/temp sensors and adjusts the watering time accordingly
void sensors() {
  if (analogRead(temp) >= 20) {   //high air temp --> turn the case fan on
    digitalWrite(fan,HIGH);
  } else {
    digitalWrite(fan,LOW);
  }
  
  if (analogRead(rain) > 300) {    //raining --> extend the interval between waterings, and shorten watering time
    checkinterval = 2400000;
    watertime = 30000;
  } else {
    checkinterval = 1200000;
    watertime = 90000;
  }

  if (analogRead(sunlight) < 300) { //sun gone down? extend watering cycle from 20 minutes to 4 hours
    if (checksoil())
      watering();
    checkinterval = 14400000;
    watertime = 30000;
  } else {
    checkinterval = 1200000;
    watertime = 90000;
  }
}

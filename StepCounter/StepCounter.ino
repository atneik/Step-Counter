#include <LiquidCrystal.h>

/* LCD connections */
LiquidCrystal lcd(12, 11, 5, 4, 7, 6);
/*Pressure sensor pin*/
int fsrAnalogPin = 0;
/*Current and previous analog value of pressure sensor*/
int fsrReading;
int fsrReadingPrev = 0;
/*default Calibration*/
int caliReading = 0;
/*init previous value */
int valuePrev = 7;
/*init number of steps*/
int nSteps = -1;
/*for managing the step state, hence count*/ 
boolean flag = true;
/*For interrupt state 0 and 1 respectively*/
volatile int syncState= LOW;
volatile int caliState= LOW;

/* HEX coding for the bar */
uint8_t bar[8][8] = { { 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 },  
                    { 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x3f },
                    { 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x3f, 0x3f },
                    { 0x0, 0x0, 0x0, 0x0, 0x0, 0x3f, 0x3f, 0x3f },
                    { 0x0, 0x0, 0x0, 0x0, 0x3f, 0x3f, 0x3f, 0x3f },
                    { 0x0, 0x0, 0x0, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f },
                    { 0x0, 0x0, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f },
                    { 0x0, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f }

};


void setup() {
  Serial.begin(9600);
  /*create byte char for*/
  for(int i=0; i<8; i++){
    lcd.createChar(i, bar[i]);
  }
  lcd.begin(16, 2);
  /*attach interrupts for syncing and calibrating*/
  attachInterrupt(0, sync, RISING);
  attachInterrupt(1, cali, RISING);
}

void loop() {
 lcd.clear();
 /*read current pressure sensor value*/
 fsrReading = analogRead(fsrAnalogPin);
 /*map to plot the bars*/
 int value = map(fsrReading, caliReading, 1023, 0, 7);
 
 /*print only if the value is greater than zero*/
 if(valuePrev>=0)
   lcd.write(byte(valuePrev));
 else
    lcd.write(byte(0));
    
 if(value>=0)
   lcd.write(byte(value));
 else
    lcd.write(byte(0));
 
 /*increment step count in case of rising pressue value*/
 if(valuePrev >= value){
   if(flag == true && value == 0){
     nSteps++;
     flag = false;
   }
 }else{
   flag = true;
 }
 valuePrev = value;
 
 //Serial.print("Steps: ");
 //Serial.println(nSteps);
 lcd.print(" Steps: ");
 lcd.print(nSteps);

 /*If sync interrupt was called then send the value to serial port*/
 
  if(syncState == HIGH){
    Serial.println("save: ");
    Serial.println(nSteps);
    syncState = LOW;
    nSteps = 0;
    lcd.clear();
    lcd.print("SYNCING..");
    delay(3000);
  }
  
 /*If caliberation interrupt was called then reset its value*/
  
  if(caliState == HIGH){
    caliState = LOW;
    caliReading = fsrReading;
    lcd.clear();
    lcd.print("CALIBRATING..");
    delay(3000);
  }
  
 delay(10);
}

void sync(){
 syncState = HIGH;
}

void cali(){
 caliState = HIGH;
}

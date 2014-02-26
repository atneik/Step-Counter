#include <LiquidCrystal.h>

class PressureSensor {
  
  private:
    /*Pressure sensor pin*/
    int fsrAnalogPin;
    /*Current and previous analog value of pressure sensor*/
    int fsrReading;
    int fsrReadingPrev;
    /*prev value for bar*/
    int valuePrev;
    /*prev value for bar*/
    int value;
    /*default Calibration*/
    int caliReading;
    /*init number of steps*/
    int nSteps;
    /*for managing the step state, hence count*/ 
    boolean flag;
    
  public:
    PressureSensor(){}
    PressureSensor(int pin, int cali){
      fsrAnalogPin = pin;
      fsrReadingPrev = 0;
      caliReading = cali;
      nSteps = -1;
      int valuePrev = 7;
      flag = true;
    }
    
    long getSteps(){
      return nSteps;
    }
    
    void calibrate(int cali){
      caliReading = cali;
    }
    
    void calibrateCurr(){
      caliReading = fsrReading;
    }
    
    void readValue(){
       /*read current pressure sensor value*/
       fsrReading = analogRead(fsrAnalogPin);
        /*map to plot the bars*/
       value = map(fsrReading, caliReading, 1023, 0, 7);
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
 
    }
    
    int getValuePrev() {
      return valuePrev;
    }
    
    int getValue() {
      return value;
    }
    
    void resetSteps(){
      nSteps = 0;
    }
    
    

};

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
    
/* LCD connections */
LiquidCrystal lcd(12, 11, 5, 4, 7, 6);
/*For interrupt state 0 and 1 respectively*/
volatile int syncState= LOW;
volatile int caliState= LOW;

PressureSensor sensor;

void setup() {
  Serial.begin(9600);
  //sensor = new PressureSensor(0, 0);
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
  sensor.readValue();
  lcd.clear();
 /*print only if the value is greater than zero*/
 if(sensor.getValuePrev()>=0)
   lcd.write(byte(sensor.getValuePrev()));
 else
    lcd.write(byte(0));
    
 if(sensor.getValue()>=0)
   lcd.write(byte(sensor.getValue()));
 else
    lcd.write(byte(0));
 

 //Serial.print("Steps: ");
 //Serial.println(nSteps);
 lcd.print(" Steps: ");
 lcd.print(sensor.getSteps());

 /*If sync interrupt was called then send the value to serial port*/
 
  if(syncState == HIGH){
    Serial.println("save: ");
    Serial.println(sensor.getSteps());
    syncState = LOW;
    sensor.resetSteps();
    lcd.clear();
    lcd.print("SYNCING..");
    delay(3000);
  }
  
 /*If caliberation interrupt was called then reset its value*/
  
  if(caliState == HIGH){
    caliState = LOW;
    sensor.calibrateCurr();
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

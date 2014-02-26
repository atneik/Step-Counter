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

//(pin, cali)
PressureSensor sensor1 = PressureSensor(0, 0);
PressureSensor sensor2 = PressureSensor(1, 0);

//indicator LED
int indicatePin = 9;

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
  pinMode(indicatePin, OUTPUT);
}

void loop() {
  sensor1.readValue();
  sensor2.readValue();
  lcd.clear();
 /*print only if the value is greater than zero*/
 if(sensor1.getValuePrev()>=0)
   lcd.write(byte(sensor1.getValuePrev()));
 else
    lcd.write(byte(0));
    
 if(sensor1.getValue()>=0)
   lcd.write(byte(sensor1.getValue()));
 else
    lcd.write(byte(0));
 
lcd.setCursor(0, 1);
 //Serial.print("Steps: ");
 //Serial.println(nSteps);
 lcd.print("Steps: ");
 lcd.print(sensor1.getSteps()+sensor2.getSteps());
 
 lcd.setCursor(14, 0);
  /*print only if the value is greater than zero*/
 if(sensor2.getValuePrev()>=0)
   lcd.write(byte(sensor2.getValuePrev()));
 else
    lcd.write(byte(0));
    
 if(sensor2.getValue()>=0)
   lcd.write(byte(sensor2.getValue()));
 else
    lcd.write(byte(0));

 /*If sync interrupt was called then send the value to serial port*/
 
  if(syncState == HIGH){
    Serial.println("save: ");
    Serial.println(sensor1.getSteps()+sensor2.getSteps());
    syncState = LOW;
    sensor1.resetSteps();
    sensor2.resetSteps();
    lcd.clear();
    lcd.print("SYNCING..");
    blinkWait(4);
  }
  
 /*If caliberation interrupt was called then reset its value*/
  
  if(caliState == HIGH){
    caliState = LOW;
    sensor1.calibrateCurr();
    sensor2.calibrateCurr();
    lcd.clear();
    lcd.print("CALIBRATING..");
    blinkWait(4);
  }
  
 delay(10);
}

void sync(){
 syncState = HIGH;
}

void cali(){
 caliState = HIGH;
}

void blinkWait(int times) {
  for(int i = 0; i<times; i++){
    digitalWrite(indicatePin, HIGH);
    delay(100);
    digitalWrite(indicatePin, LOW);
    delay(200);
  }
}

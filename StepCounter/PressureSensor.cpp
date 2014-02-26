#include <Arduino.h>

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

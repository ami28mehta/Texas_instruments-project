#include <Servo.h>
#include "I2Cdev.h"
#include "HMC5883L.h"

//PA_6 SCL
//PA_7 SDA

HMC5883L mag;

int calibratePin = PUSH1;
int count = 0,count2 = 0,cnt;

int16_t mx, my, mz;
int  zlim1,zlim2,xlim1,xlim2;
float Z_REF = 0,X_REF = 0;

//johnson direction pins
int dir1 = A5;    //PD_2
int dir2 = A4;    //PD_3


Servo myservo;

void initPins() {
//  attachInterrupt(PUSH1,ISR,FALLING);
  pinMode(calibratePin,INPUT_PULLUP);
  pinMode(BLUE_LED,OUTPUT);
  }

 int movServo() {

    count2 ++;                      
    myservo.write(180);              // tell servo to go to position in variable 'pos' 
    delay(1000);
    myservo.write(0);
    delay(1000);
     
    return count2;       
  }


void initMagneto(){
    // join I2C bus (I2Cdev library doesn't do this automatically)
    Wire.begin();
    mag.getHeading(&mx, &my, &mz);
   
    // initialize serial communication
    // (38400 chosen because it works as well at 8MHz as it does at 16MHz, but
    // it's really up to you depending on your project)
    Serial.begin(38400);

    // initialize device
    Serial.println("Initializing I2C devices...");
    mag.initialize();

    // verify connection
    Serial.println("Testing device connections...");
    Serial.println(mag.testConnection() ? "HMC5883L connection successful" : "HMC5883L connection failed");

}


void calibrate(){
   Z_REF = mz;
   X_REF = mx; 
   zlim2 = Z_REF + 100;
   zlim1 = Z_REF - 50;
   xlim2 = X_REF + 100;
   xlim1 = X_REF - 100;
  myservo.attach(PD_0);
     
}


void readMagneto(){
    // read raw heading measurements from device
    mag.getHeading(&mx, &my, &mz);
    }

void stopJohnson(){
  digitalWrite(dir1,LOW);
  digitalWrite(dir2,LOW);
}
  
void movJohnsonCW(){
  Serial.print(count2);
   count2 = 0;      
  digitalWrite(dir1,HIGH);
  digitalWrite(dir2,LOW);
  digitalWrite(GREEN_LED,HIGH);
  delay(3000);  
}

void movJohnsonACW(){
  count2 = 0;
  Serial.print("ACW");
  digitalWrite(dir1,LOW);
  digitalWrite(dir2,HIGH);
  delay(3000);
}


void setup() {
  initMagneto();
  
  attachInterrupt(PUSH1,ISR,FALLING);
  pinMode(calibratePin,INPUT_PULLUP);
  pinMode(BLUE_LED,OUTPUT);
  pinMode(RED_LED,OUTPUT);
  pinMode(dir1,OUTPUT);
  pinMode(dir2,OUTPUT);
  pinMode(GREEN_LED,OUTPUT);
  digitalWrite(GREEN_LED,LOW);
  
}

void loop() {
    readMagneto();  
    Serial.print("welcome");
    /*Serial.print("Z axis :");
    Serial.print(mz);
    Serial.print("\t X axis ");
    Serial.println(mx);
    Serial.print("\tCalibrated Value: ");
    Serial.println(Z_REF);
    delay(100);*/
    
   if((mz <zlim1) || (mz > zlim2)||(mx <xlim1) || (mx > xlim2)){
      digitalWrite(BLUE_LED,LOW);
      digitalWrite(RED_LED,HIGH);
      digitalWrite(GREEN_LED,LOW);
      movServo();
      if (count2 >= 3){
        movJohnsonCW();
        delay(1000);
        movJohnsonACW();
        stopJohnson();
        }
       
}
  else
  { count2 = 0;   
    digitalWrite(BLUE_LED,HIGH);
       digitalWrite(RED_LED,LOW);
      digitalWrite(GREEN_LED,LOW);
  }
}

void ISR(){
   if(!digitalRead(calibratePin))
  count ++;
  if(count >1){
  calibrate();
  count = 0;
  
}
}


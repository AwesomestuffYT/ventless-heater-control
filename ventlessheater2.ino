
/*

in the irlib2 examples you will need to open record 
then push the buttens you want to use then write them down
it looks like this"Received NEC Value:0xFF609F"
you will want the 0xFF609F hex number it will probably be different 
number for you then place them in the correct defined variable 

*/
#include <IRLibAll.h>
IRdecode myDecoder;
#include "DHT.h"

#define DHTPIN 2   // pin number for the temp sensor
#define UP_ARROW    0xFFA05F // hex number sent from remote 
#define DOWN_ARROW  0xFF20DF // hex number sent from remote 
#define SLEEP       0xFF609F // hex number sent from remote 
#define NORMAL      0xFFE01F // hex number sent from remote 
#define DHTTYPE DHT22 
IRrecv myReceiver(3); //pin number for the receiver
DHT dht(DHTPIN, DHTTYPE);

bool heat = false;
bool received = false;
unsigned long  counter = 500;
unsigned long  counterTwo = 4000;
unsigned long  counterThree = 500; 
uint32_t codeValue;
float temp = 71;
float retemp = 0;
void setup() {
  pinMode(A0,INPUT);
  pinMode(8,OUTPUT);
  pinMode(10,OUTPUT);
  pinMode(11,OUTPUT);
  pinMode(13,OUTPUT);
  shut_down(); // makes sure the pins on the h bridge are low so it wont short on start up
  Serial.begin(9600);
  
  //myReceiver.blink13(true); // for debugging 
  myReceiver.enableIRIn();
  dht.begin();
  retemp = dht.readTemperature(true);
  turn_off();
}

void loop() {
  //counter = millis();
  //counterTwo = millis();
  //counterThree = millis();
  
  
  
  if((long)(millis() - counter) >= 0){
    
    received = false;
    myReceiver.enableIRIn();
    counter += 500;
  }

  // unlocks the ir receiver when it gets locked up
  if((long)(millis() - counterTwo) >= 0){
    //Serial.println("ir reset");
    myReceiver.disableIRIn();
    myReceiver.enableIRIn();
    counterTwo += 4000;
    
  }

  // what to do when the ir receiver well receives lol
  if (myReceiver.getResults()) {
    if(myDecoder.decode()) {
     if (myDecoder.value == REPEAT_CODE) {
            // Don't record a NEC repeat value as that's useless.
            //Serial.println(F("repeat; ignoring."));
        } else {
              codeValue = myDecoder.value;
         
        }
        //Serial.println(codeValue,HEX);
      switch(codeValue) {
        
        case UP_ARROW:      temp += .5; received = true; counter = millis()+500; break;
        case DOWN_ARROW:    temp -= .5; received = true; counter = millis()+500; break;
        case SLEEP:         temp = 60;  received = true; counter = millis()+500; break;
        case NORMAL:        temp = 70;  received = true; counter = millis()+500; break;
        
      }
       
    }
    
  }

  // sets pin 8 high or low 
  if(received == true){

    digitalWrite(8,HIGH);
    
  }else{

    digitalWrite(8,LOW);
  }
    Serial.println(retemp);
    Serial.println(temp);
   if((long)(millis() - counterThree) >= 0){
    retemp = dht.readTemperature(true);
    Serial.println("checked temp");
    counterThree += 5000;
   }
  // checks the temperature then compares it to the variable temp
  if( (retemp < (temp - .4)) && heat == false ){
    digitalWrite(13,HIGH);
    
    //Serial.println("on");
    turn_on();
  }
  
  // checks the temperature then compares it to the variable temp
  if( (retemp > (temp + .4)) && heat == true ){
    digitalWrite(13,LOW);
    
    //Serial.println("off");
    turn_off();
  }
  

}

void turn_off(){
  heat = false;
  shut_down();
  // delay is so the h bridge has time to shut off
  delayMicroseconds(1);
  digitalWrite(11,HIGH);
  
}


void turn_on(){
  heat = true;
  shut_down();
  // delay is so the h bridge has time to shut off
  delayMicroseconds(1);
  digitalWrite(10,HIGH);
  
}

void shut_down(){
  
  digitalWrite(10,LOW);
  digitalWrite(11,LOW);

  
}


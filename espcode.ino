#include <ESP8266WiFi.h>                                               
#include <FirebaseArduino.h>                                        
 
#define FIREBASE_HOST "smartsocketsfit.firebaseio.com"              // the project name address from firebase id
#define FIREBASE_AUTH "FnFwngTm2yRI4bbDrLm0EljA2kwTy4lKGxwLaFx7"       // the secret key generated from firebase
#define WIFI_SSID "jerome"                                          //WIFI NAME
#define WIFI_PASSWORD "shambala"                                    //WIFI PASSKEY

String fireStatus = "";  
int nos=3;
int relay[]={16,5,4};
String text;
int ldr_flag=0;
int ldr_init_flag=0;
float normal=0;
int state[]={0,0,0,0};
String socket[]={"SOCK1","SOCK2","SOCK3","SOCK4"};
int i;



void setup(){
  for(i=0;i<nos;i++){//INIT RELAYS
    pinMode(relay[i],OUTPUT);
  }
  Serial.begin(9600);
  delay(1000);                   
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);                               
  Serial.print("Connecting to ");
  Serial.print(WIFI_SSID);
  while (WiFi.status() != WL_CONNECTED) 
  {       
    Serial.print("*");
    delay(500);
  }
  Serial.println();
  Serial.print("Connected to ");
  Serial.println(WIFI_SSID);
  Serial.println("Connecting to firebase....");
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH); // connect to firebase
  Serial.println("Connected to firebase...!");
  for(i=0;i<nos;i++){
    Firebase.setString(socket[i], "OFF");     
    digitalWrite(relay[i],LOW);
  }  
  Firebase.setString("LDRINIT", "OFF"); 
  fireStatus=Firebase.getString("LDRVAL");
  normal=fireStatus.toFloat();

}

void ldr_update(){
  int sensorValue = analogRead(A0);   // read the input on analog pin 0

  float voltage = sensorValue * (50.0 / 1023.0);   // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V)

  //Serial.println(voltage);   // print out the value you read
}

void update_relays(){
  
  for(i=0;i<nos;i++){
    if(state[i]==1){
      digitalWrite(relay[i],HIGH);
    }
    else{
      digitalWrite(relay[i],LOW);
    }
  }

}

void update_firebase(){
  
  for(i=0;i<nos;i++){
    fireStatus = Firebase.getString(socket[i]);
    if (fireStatus == "ON"){
      state[i]=1;      
    }
    if (fireStatus == "OFF"){
      state[i]=0;      
    }
    fireStatus=Firebase.getString("LDR");
    if (fireStatus == "ON"){
      ldr_flag=1;      
    }
    if (fireStatus == "OFF"){
      ldr_flag=0;      
    }
    fireStatus=Firebase.getString("LDRINIT");
    if (fireStatus == "ON"){
      ldr_init_flag=1;
      ldr_initx();      
    }    
          
  }
}

void update_ldr(){
  if(ldr_flag==1){
    int sensorValue = analogRead(A0);   
    float voltage = sensorValue * (50.0 / 1023.0);   
    //Serial.println(voltage);
    if(voltage>normal){
      Firebase.setString("SOCK1", "OFF");           
    }
    else{
      Firebase.setString("SOCK1", "ON");       
    }    
  }
}

void ldr_initx(){
  while(ldr_init_flag!=0){
    int sensorValue = analogRead(A0);   
    float voltage = sensorValue * (50.0 / 1023.0); 
    //Serial.println("SENDING VOLTAGE TO FIREBASE");
    //Serial.println(voltage);  
    Firebase.setFloat("LDRVAL", voltage);         
    fireStatus=Firebase.getString("LDRINIT");
    if (fireStatus == "OFF"){
      ldr_init_flag=0;    
      normal=voltage+1;  
    }    
  }
}




void loop(){
  //Serial.println("Updating from firebase...");
  update_firebase();
  //Serial.println("Updating the relays...");
  update_relays();
  //Serial.println("Updating the LDR...");
  update_ldr();
  
  
  

}

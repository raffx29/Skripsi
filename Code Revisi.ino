/*Comment this out to disable prints and save space*/
  #define BLYNK_PRINT Serial

/*Fill in information from Blynk Device Info here*/
  #define BLYNK_TEMPLATE_ID "TMPL6UZ6TU4yi"
  #define BLYNK_TEMPLATE_NAME "Controll Kandang"
  #define BLYNK_AUTH_TOKEN "vD1EYpOBTKm88imTifn4pTJ3wzkQS44X"

/*Library*/
  #include <WiFi.h>
  #include <BlynkSimpleEsp32.h>
  #include <DHT.h>
  #include <ESP32Servo.h>
  #include "HX711.h"
  #include <Wire.h>
  #include <LiquidCrystal_I2C.h>

/*Your WiFi credentials*/
  char ssid[] = "EVERYDAY 2.4G";
  char pass[] = "xxxxxxx";

/*PIN Sensor*/
  #define DHTPIN 32
  #define DHTTYPE DHT22
  #define WLPIN 35

/*PIN Relay*/
  #define RelayPin1 5
  #define RelayPin2 17
  #define RelayPin3 16

/*Relay Variable*/
  bool toggleState_1 = LOW;
  bool toggleState_2 = LOW;
  bool toggleState_3 = LOW;

/*Push Button*/
  const int buttonPin1 = 33;
  const int buttonPin2 = 25;
  const int buttonPin3 = 26;
  const int buttonPin4 = 27;
  int buttonState1 = 0;
  int buttonState2 = 0;
  int buttonState3 = 0;
  int buttonState4 = 0;

/*Buzzer*/
  const int buzzer = 2;

/*DHT22*/
  float temp, hum;

/*Load Cell Variable*/
  long reading;
  const int LOADCELL_DOUT_PIN = 18;
  const int LOADCELL_SCK_PIN = 19;

/*Water Level Variable*/
  int sensorValue, tinggiAir;
  float sensorVoltage = 0;
  int nilaiMax = 1023;
  float panjangSensor = 4.0;

/*Servo*/
  int pos;
  int pinValue = 0;

/*Class*/
  LiquidCrystal_I2C lcd(0x27, 16, 2);
  DHT dht(DHTPIN, DHTTYPE);
  BlynkTimer timer;
  HX711 scale;
  Servo myservo;

/*Blynk Relay*/
  BLYNK_CONNECTED(){
    Blynk.syncAll();
  }

  BLYNK_WRITE(V4){
    toggleState_1 = param.asInt();
    if(toggleState_1 == 1){
      digitalWrite(RelayPin1, LOW);
    }
    else{ 
      digitalWrite(RelayPin1, HIGH);
    }
  }

  BLYNK_WRITE(V5) {
    toggleState_2 = param.asInt();
    if(toggleState_2 == 1){
      digitalWrite(RelayPin2, LOW);
    }
    else{ 
      digitalWrite(RelayPin2, HIGH);
    }
  }

  BLYNK_WRITE(V6){
    toggleState_3 = param.asInt();
    if(toggleState_3 == 1){
      digitalWrite(RelayPin3, LOW);
    }
    else{ 
      digitalWrite(RelayPin3, HIGH);
    }
  }

/*Blynk Servo*/
  BLYNK_WRITE(V7){
    int pinValue = param.asInt();
    if (pinValue == 1){
      for (pos = 0; pos <= 180; pos += 1){ 
        myservo.write(pos);
        delay(25);
      }
      for (pos = 180; pos >= 0; pos -= 1){
        myservo.write(pos);
        delay(15);
      }
    }
  }

void setup(){
/*Buzzer*/
  pinMode(buzzer, OUTPUT);

/*Push Button*/
  pinMode(buttonPin1, INPUT);
  pinMode(buttonPin2, INPUT);
  pinMode(buttonPin3, INPUT);
  pinMode(buttonPin4, INPUT);  

/*Relay*/
  pinMode(RelayPin1, OUTPUT);
  pinMode(RelayPin2, OUTPUT);
  pinMode(RelayPin3, OUTPUT);

  digitalWrite(RelayPin1, HIGH);
  digitalWrite(RelayPin2, HIGH);
  digitalWrite(RelayPin3, HIGH);

  Serial.begin(9600);

/*Mulai WiFi on LCD*/
  lcd.begin(16,2);
  lcd.init();                 
  lcd.backlight();
  
  lcd.setCursor(0, 0);
  lcd.print("Connecting to :");
  lcd.setCursor(0, 1);
  lcd.print(ssid);

  WiFi.begin(ssid, pass);
  delay(2000);
  lcd.clear();
  
  for (int a = 0; a <= 15; a++){
    lcd.setCursor(a, 1);
    lcd.print(".");
    delay(300);
  }
  
  if (WiFi.status() == WL_CONNECTED){
    lcd.clear();
    lcd.setCursor(1, 0);
    lcd.print("WiFi connected");
    delay(2000);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("IP address :");
    lcd.setCursor(0, 1);
    lcd.print(WiFi.localIP());
    delay(2000);
  }
    
  else {
    tone(buzzer, 1000);
    lcd.clear();
    lcd.setCursor(1, 0);
    lcd.print("Failed connect");
    lcd.setCursor(1, 1);
    lcd.print("Do Manual Mode");
    delay(2000);
    WiFi.disconnect();
  }

  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("System Loading");
  for (int a = 0; a <= 15; a++){
    lcd.setCursor(a, 1);
    lcd.print(".");
    delay(300);
  }
  lcd.clear();

/*Blynk*/
  Blynk.config(BLYNK_AUTH_TOKEN);
  timer.setInterval(1000L, sendSensor);

/*Servo*/
  myservo.attach(23);

/*DHT22*/
  dht.begin();

/*LoadCell*/
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale(395.8);
  scale.tare();

  noTone(buzzer);
}

void loop(){
/*LCD Read Sensor*/
  lcd.setCursor(0,0);
  lcd.print("Temp : ");
  lcd.print(temp);
  lcd.print(" C");
  lcd.setCursor(0,1);
  lcd.print("Hum : ");
  lcd.print(hum);
  lcd.print(" %");
  delay(2000);
  lcd.clear();
      
  lcd.setCursor(0,0);
  lcd.print("Water : ");
  lcd.print(tinggiAir);
  lcd.print(" %");
  lcd.setCursor(0,1);
  lcd.print("Voer : ");
  lcd.print(reading);
  lcd.print(" g");
  delay(2000);
  lcd.clear();

/*Push Button*/
  int buttonState1 = digitalRead(buttonPin1);
    if (buttonState1 == HIGH){
        digitalWrite(RelayPin1, LOW);
      }
      else{ 
      digitalWrite(RelayPin1, HIGH);
    }
    
  int buttonState2 = digitalRead(buttonPin2);
    if (buttonState3 == HIGH){
      digitalWrite(RelayPin2, LOW);
    }
    else{ 
      digitalWrite(RelayPin2, HIGH);
    }
    
  int buttonState3 = digitalRead(buttonPin3);
    if (buttonState4 == HIGH){
      digitalWrite(RelayPin3, LOW);
    }
    else{ 
      digitalWrite(RelayPin3, HIGH);
    }
  int buttonState4 = digitalRead(buttonPin4);
    if (buttonState4 == HIGH){
      for (pos = 0; pos <= 180; pos += 1){  
        myservo.write(pos);
        delay(25);
      }
      for (pos = 180; pos >= 0; pos -= 1){
        myservo.write(pos);
        delay(15);
      }
    }

/*Blynk Run*/
  Blynk.run();
  timer.run();
}

void sendSensor(){
/*DHT22*/
  hum = dht.readHumidity();
  temp = dht.readTemperature();
    if (isnan(hum) || isnan(temp)) {
    tone(buzzer, 1000);
    return;
  }
  Blynk.virtualWrite(V0, temp);
  Blynk.virtualWrite(V1, hum);

/*Water Level*/
  sensorValue = analogRead(WLPIN);
  tinggiAir = sensorValue*panjangSensor/nilaiMax;
  if (isnan(sensorValue)) {
    tone(buzzer, 1000);
    return;
  }
  Blynk.virtualWrite(V2, tinggiAir);

/*Load Cell*/
  reading = scale.get_units();
  if (isnan(reading)) {
    tone(buzzer, 1000);
    return;
  Blynk.virtualWrite(V3, reading);
  }
}
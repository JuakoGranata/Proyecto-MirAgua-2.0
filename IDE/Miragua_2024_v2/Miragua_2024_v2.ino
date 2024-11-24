#include <I2C_LCD.h>
#include <DHT.h>
#include <Arduino.h>
#include <Wire.h>
#include <stdlib.h>
#include <hardware/adc.h>

//defino el GPIO del DHT
#define DHTPIN 15
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

//defino el GPIO del sensor de gases GPIO28
float sensorgas = 28;

//defino el GPIO del sensor de humedad GPIO26
float sensorhum = 26;

//defino varibles para usar con el DHT
float ht = 0;
float hmed;

//defino el GPIO del sensor de luz GPIO27
#define LDR_PIN 27

//defino las salidas
#define LED_AZUL 19
#define LED_VERDE 20

//I=Definimos los 2 LCDs
// Primer LCD (I2C0)
I2C_LCD lcd(0x27);
// Segundo LCD (I2C1)
I2C_LCD lcd2(0x27, &Wire1);

void setup()
{
//Habilito los canales I2C
Wire.begin(); // Inicia I2C0
Wire1.setSDA(6); // Configura SDA para I2C1
Wire1.setSCL(7); // Configura SCL para I2C1
Wire1.begin();   // Inicia I2C1

//Inicializo el DHT
dht.begin();

//Defino e inicializo los 2 LCDs
lcd.begin(20,4); //LCD de datos
lcd.backlight();

lcd2.begin(16,2); //LCD de status
lcd2.backlight();

//Inicializo el puerto serial
Serial.begin(115200);

// defino los GPIOS de salida
pinMode(LED_AZUL, OUTPUT); // ventilacion
pinMode(LED_VERDE, OUTPUT); //riego
}

void loop()
{
delay(1000);
lcd.clear();
lcd2.clear();

//leemos la temp y humedad en el DHT
float h = dht.readHumidity();
float t = dht.readTemperature();

//leemos la cantidad de luz
float luz = analogRead(LDR_PIN);
float l=map(luz, 140, 1023, 100, 0); //l = map(valor, mín orig, max orig, mín a escalar, max a escalar );

//leemos los gases
float gas = analogRead(sensorgas);
float sg = map(gas, 350, 700, 0, 100); // sg = map(valor, mín orig, max orig, mín a escalar, max a escalar );

//leemos la humedad de la tierra
hmed = analogRead(sensorhum);
ht=map(hmed, 430, 1023, 100, 0); //ht = map(valor, mín orig, max orig, mín a escalar, max a escalar );

if (isnan(h) || isnan(t)) //control del modulo DHT
  {
    //Serial.println(F("Failed to read from DHT sensor..."));
    lcd.setCursor(0,1);
    lcd.print("DHT Error...");
    return;
  }
 else
  {
  // Implrimo resultados de las lecturas en el LCD 20x4
  lcd.setCursor(4,0);
  lcd.print("Miragua v2.0");
 
  lcd.setCursor(0,1);
  lcd.print("TA:");
  lcd.print(t,1);
  lcd.print(" ");
  lcd.print("HA:");
  lcd.print(h,1);
  
  lcd.setCursor(0,2);
  lcd.print("HT:");
  lcd.print(ht,1);
  lcd.print(" ");
  lcd.print("L:");
  lcd.print(l,1);
  lcd.setCursor(5,3);
  lcd.print("Gas:");
  lcd.print(sg,1);

  //Imprimo los valores de los sensores en el serial para que se grafiquen por python
  //Imprimo "DHT_Temp:"
  Serial.println(t);
  //Imprimo "DHT_Hum:"
  Serial.println(h);
  //Imprimo "Humedad Terreno:"
  Serial.println(ht);
  //Imprimo "Lux:"
  Serial.println(l);
  //Imprimo "Gases:"
  Serial.println(sg);

  //Seteo a zero las salidas de respuesta
  digitalWrite(LED_AZUL, LOW);
  digitalWrite(LED_VERDE, LOW);
  
  //Tomo acciones segun los valores medidos
  lcd2.setCursor(4,0);
  lcd2.print("Accion: ");  

  if (ht > 80 && t > 32) 
  {
    digitalWrite(LED_AZUL, HIGH); // prendo ventilación
    lcd2.setCursor(0,1);
    lcd2.print("Inicia Ventilac");
    return;
  }
  else if (ht > 80 && t < 26) 
  {
    digitalWrite(LED_AZUL, HIGH); // prendo ventilación
    lcd2.setCursor(0,1);
    lcd2.print("Inicia Calef y Vent");
    return;
  } 
  else if (ht < 39 && t < 32 && l < 30) 
  {
    digitalWrite(LED_VERDE, HIGH); // prendo riego
    lcd2.setCursor(0,1);
    lcd2.print("Inicia Riego");
    return;
  } 
  else if (sg > 20) 
  {
    digitalWrite(LED_AZUL, HIGH); // prendo ventilacion
    lcd2.setCursor(0,1);
    lcd2.print("Inicia Ventilac.");
  }
 }
}

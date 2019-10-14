
#include <WiFi.h>
#include "FirebaseESP32.h"

#define FIREBASE_HOST "interruptor-esp32.firebaseio.com"  // o endereço do nome do projeto do firebase id
#define FIREBASE_AUTH "TVuI7ZVdyKNdTyOVCjgJmL6SrQV2u2UTRAHBol3u" // a chave secreta gerada a partir do firebase
#define WIFI_SSID "OLA : D"                                      // insira seu nome de wifi em casa ou público
#define WIFI_PASSWORD "renangui99"                               // senha do wifi ssid

#define ledred1 15
#define ledgreen1 2
#define ledblue1 4
#define ledred2 16
#define ledgreen2 17
#define ledblue2 5
#define ledred3 18
#define ledgreen3 19
#define ledblue3 21
#define rele1 3
#define rele2 1
#define sensor_touch1 22
#define sensor_touch2 23

int valor;
volatile int estado_rele1 = 0;
volatile int estado_rele2 = 0;~
volatile int trava_touch1 = 0;
volatile int trava_touch2 = 0;

FirebaseData firebaseData;

void setup()
{
  Serial.begin(115200);
  pinMode(ledred1, OUTPUT);
  pinMode(ledgreen1, OUTPUT);
  pinMode(ledblue1, OUTPUT);
  pinMode(ledred2, OUTPUT);
  pinMode(ledgreen2, OUTPUT);
  pinMode(ledblue2, OUTPUT);
  pinMode(ledred3, OUTPUT);
  pinMode(ledgreen3, OUTPUT);
  pinMode(ledblue3, OUTPUT);
  pinMode(rele1, OUTPUT);
  pinMode(rele2, OUTPUT);
  pinMode(sensor_touch1, INPUT);
  pinMode(sensor_touch1, INPUT);
  digitalWrite(rele1,LOW);
  digitalWrite(rele2,LOW);

  ledcAttachPin(ledred1, 0);
  ledcSetup(0, 1000, 8);
  ledcAttachPin(ledgreen1, 1);
  ledcSetup(1, 1000, 8);
  ledcAttachPin(ledblue1, 2);
  ledcSetup(2, 1000, 8);
  ledcAttachPin(ledred2, 3);
  ledcSetup(3, 1000, 8);
  ledcAttachPin(ledgreen2, 4);
  ledcSetup(4, 1000, 8);
  ledcAttachPin(ledblue2, 5);
  ledcSetup(5, 1000, 8);
  ledcAttachPin(ledred3, 6);
  ledcSetup(6, 1000, 8);
  ledcAttachPin(ledgreen3, 7);
  ledcSetup(7, 1000, 8);
  ledcAttachPin(ledblue3, 8);
  ledcSetup(8, 1000, 8);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Conectado AO Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
  }
  Serial.println("Contectado com Sucesso");
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);

  Firebase.setInt(firebaseData, "/rele1", 0);
  Firebase.setInt(firebaseData, "/rele2", 0);
  attachInterrupt(digitalPinToInterrupt(sensor_touch1), touch1, RISING);
  attachInterrupt(digitalPinToInterrupt(sensor_touch2), touch2, RISING);
}

void RGB()
{
  Firebase.getInt(firebaseData, "/ledred1");
  valor = firebaseData.intData();
  Serial.print("Valor LEDred1=");
  Serial.println(valor);
  ledcWrite(0, valor);
  Firebase.getInt(firebaseData, "/ledgreen1");
  valor = firebaseData.intData();
  Serial.print("Valor LEDgreen1=");
  Serial.println(valor);
  ledcWrite(1, valor);
  Firebase.getInt(firebaseData, "/ledblue1");
  valor = firebaseData.intData();
  Serial.print("Valor LEDblue1=");
  Serial.println(valor);
  ledcWrite(2, valor);
  Firebase.getInt(firebaseData, "/ledred2");
  valor = firebaseData.intData();
  Serial.print("Valor LEDred2=");
  Serial.println(valor);
  ledcWrite(3, valor);
  Firebase.getInt(firebaseData, "/ledgreen2");
  valor = firebaseData.intData();
  Serial.print("Valor LEDgreen2=");
  Serial.println(valor);
  ledcWrite(4, valor);
  Firebase.getInt(firebaseData, "/ledblue2");
  valor = firebaseData.intData();
  Serial.print("Valor LEDblue2=");
  Serial.println(valor);
  ledcWrite(5, valor);
  Firebase.getInt(firebaseData, "/ledred3");
  valor = firebaseData.intData();
  Serial.print("Valor LEDred3=");
  Serial.println(valor);
  ledcWrite(6, valor);
  Firebase.getInt(firebaseData, "/ledgreen3");
  valor = firebaseData.intData();
  Serial.print("Valor LEDgreen3=");
  Serial.println(valor);
  ledcWrite(7, valor);
  Firebase.getInt(firebaseData, "/ledblue3");
  valor = firebaseData.intData();
  Serial.print("Valor LEDblue3=");
  Serial.println(valor);
  ledcWrite(8, valor); 
}

void touch1()
{
  if (estado_rele1 == 0)
  {
    estado_rele1 = 1;
  }else if (estado_rele1 == 1)
  {
    estado_rele1 = 0;
  }
  Serial.print("Rele1 =");
  Serial.println(estado_rele1);
  digitalWrite(rele1, estado_rele1);
  trava_touch1 =0;
  
}
void touch2()
{
  
  if (estado_rele2 == 0)
  {
    estado_rele2 = 1;
  }else if (estado_rele2 == 1)
  {
    estado_rele2 = 0;
  }
  
  Serial.print("Rele2 =");
  Serial.println(estado_rele2);
  digitalWrite(rele2, estado_rele2);
  trava_touch2 = 1;
  
}

void touch1_envio(){
if(trava_touch1==1){
  Firebase.setInt(firebaseData,"/rele1",estado_rele1);
  Serial.println("Enviado");
  trava_touch1 = 0;
}
}

void touch2_envio(){
  if(trava_touch2==1){
  Firebase.setInt(firebaseData,"/rele2",estado_rele2);
  Serial.println("Enviado");
  trava_touch2 =0;
  }
}

void rele_firebase(){
  Firebase.getInt(firebaseData,"/rele1");
  if(firebaseData.intData() != estado_rele1){
    if(estado_rele1 == 0){
      estado_rele1 = 1;
    }else if(estado_rele1 == 1){
      estado_rele1=0;
    }
    Serial.print("Rele1 =");
    Serial.println(estado_rele1);
    digitalWrite(rele1, estado_rele1);
  }
  Firebase.getInt(firebaseData,"/rele2");
  if(firebaseData.intData() != estado_rele2){
    if(estado_rele2 == 0){
      estado_rele2 = 1;
    }else if(estado_rele2 == 1){
      estado_rele2=0;
    }
    Serial.print("Rele2 =");
    Serial.println(estado_rele2);
    digitalWrite(rele2, estado_rele2);
  }
}

void loop()
{ 
  Serial.println("OLA");
  RGB(); 
  rele_firebase();
  touch1_envio();
  touch2_envio();
}
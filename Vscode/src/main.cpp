
#include <WiFi.h>
#include "FirebaseESP32.h"

#define FIREBASE_HOST "interruptor-esp32.firebaseio.com"  // o endereço do nome do projeto do firebase id
#define FIREBASE_AUTH "TVuI7ZVdyKNdTyOVCjgJmL6SrQV2u2UTRAHBol3u" // a chave secreta gerada a partir do firebase
#define WIFI_SSID "FamGDLink"                                      // insira seu nome de wifi em casa ou público
#define WIFI_PASSWORD "nc67bg92rg99"                               // senha do wifi ssid

#define ledred1 15
#define ledgreen1 2
#define ledblue1 4
#define ledred2 16
#define ledgreen2 17
#define ledblue2 5
#define ledred3 18
#define ledgreen3 19
#define ledblue3 21
#define rele1 13
#define rele2 12
#define sensor_touch1 22
#define sensor_touch2 23

int valor;
String svalor;
volatile int estado_rele1 = 0;
volatile int estado_rele2 = 0;
volatile int trava_touch1 = 0;
volatile int trava_touch2 = 0;
String path = "/quarto1";
int trava_puxar1= 0;
int trava_puxar2= 0;
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
  pinMode(sensor_touch2, INPUT);
  digitalWrite(rele1, HIGH);
  digitalWrite(rele2  , HIGH);

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

  Firebase.setInt(firebaseData, path +"/rele1", 0);
  Firebase.setInt(firebaseData, path +"/rele2", 0);

  if (!Firebase.beginStream(firebaseData,"/quarto1"))
  {
    Serial.println("> FALHO");
    Serial.println("> ERRO: " + firebaseData.errorReason());
    Serial.println();
  }
  else
  {
    Serial.println("> PASSOU");
    Serial.println("------------------------------------");
    Serial.println();
  }

  Firebase.setStreamCallback(firebaseData, streamCallback, streamTimeoutCallback);

  attachInterrupt(digitalPinToInterrupt(sensor_touch1), touch1, RISING);
  attachInterrupt(digitalPinToInterrupt(sensor_touch2), touch2, RISING);
  RGB();
}

void streamTimeoutCallback(bool timeout)
{
  if (timeout)
  {
    Serial.println();
    Serial.println("> Stream 1 acabou o tempo, resumindo o streaming...");
    Serial.println();
  }
}

void streamCallback(StreamData data)
{
  RGB();
  //rele_firebase();
}

void RGB()
{
  Firebase.getString(firebaseData, path +"/ledred");
  Serial.println(firebaseData.stringData());
  
  valor = firebaseData.stringData().toInt();
  valor = map(valor,0,255,255,0);
  Serial.print("Valor LEDred=");
  Serial.println(valor);
  ledcWrite(0, valor);
  ledcWrite(3, valor);
  ledcWrite(6, valor);
  Firebase.getString(firebaseData, path +"/ledgreen");
  
  valor = firebaseData.stringData().toInt();
  valor = map(valor,0,255,255,0);
  Serial.print("Valor LEDgreen=");
  Serial.println(valor);
  ledcWrite(1, valor);
  ledcWrite(4, valor);
  ledcWrite(7, valor);
  Firebase.getString(firebaseData, path +"/ledblue");
  
  valor = firebaseData.stringData().toInt();
  valor = map(valor,0,255,255,0);
  Serial.print("Valor LEDblue=");
  Serial.println(valor);
  ledcWrite(2, valor);
  ledcWrite(5, valor);
  ledcWrite(8, valor);
  
  
  
}

void touch1()
{
  if (estado_rele1 == 0)
  {
    estado_rele1 = 1;
    desliga_led1();
  } else if (estado_rele1 == 1)
  {
    estado_rele1 = 0;
    liga_led1();
  }
  Serial.print("Rele1 =");
  Serial.println(estado_rele1);
  digitalWrite(rele1, estado_rele1);
  trava_touch1 = 1;
  trava_puxar1 = 1;
}
void touch2()
{
  if (estado_rele2 == 0)
  {
    estado_rele2 = 1;
    desliga_led2();
  } else if (estado_rele2 == 1)
  {
    estado_rele2 = 0;
    liga_led2();
  }

  Serial.print("Rele2 =");
  Serial.println(estado_rele2);
  digitalWrite(rele2, estado_rele2);
  trava_touch2 = 1;
  trava_puxar2 = 1;
}

void touch1_envio(){
  if (trava_touch1 == 1) {
    Firebase.setString(firebaseData, path +"/rele1", String(estado_rele1));
    Serial.println("Enviado");
    trava_touch1 = 0;
    trava_puxar1 = 0;
  }
}

void touch2_envio() {
  if (trava_touch2 == 1) {
    Firebase.setString(firebaseData, path +"/rele2", String(estado_rele2));
    Serial.println("Enviado");
    trava_touch2 = 0;
    trava_puxar2 = 0;
  }
}

void liga_led1(){
  ledcWrite(0,255);
  ledcWrite(1,0);
  ledcWrite(2,255);
  }
void desliga_led1(){
  ledcWrite(0,0);
  ledcWrite(1,255);
  ledcWrite(2,255);
  }
void liga_led2(){
  ledcWrite(6,255);
  ledcWrite(7,0);
  ledcWrite(8,255);
  }
  
void desliga_led2(){
  ledcWrite(6,0);
  ledcWrite(7,255);
  ledcWrite(8,255);
  }

void rele_firebase() {
  Firebase.getString(firebaseData, path +"/rele1");
    if (firebaseData.stringData().toInt() != estado_rele1) {
    if (estado_rele1 == 0) {
      estado_rele1 = 1;
      desliga_led1();
    } else if (estado_rele1 == 1) {
      estado_rele1 = 0;
      liga_led1();
    }
    Serial.print("Rele1 =");
    Serial.println(estado_rele1);
    digitalWrite(rele1, estado_rele1);
  }
  Firebase.getString(firebaseData,path + "/rele2");
  if (firebaseData.stringData().toInt() != estado_rele2) {
    if (estado_rele2 == 0) {
      desliga_led2();
      estado_rele2 = 1;
    } else if (estado_rele2 == 1) {
      estado_rele2 = 0;
      liga_led2();
    }
    Serial.print("Rele2 =");
    Serial.println(estado_rele2);
    digitalWrite(rele2, estado_rele2);
  }
}

void loop()
{
  //Serial.println("OLA");
 

  touch1_envio();
  touch2_envio();
  if(trava_puxar1 != 1 && trava_puxar2){
  rele_firebase();
  }
}
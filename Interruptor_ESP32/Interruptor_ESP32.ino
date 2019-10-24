
#include <WiFi.h>
#include "FirebaseESP32.h"

//-----------------------Config do Firebase e WiFi----------------------------------------
#define FIREBASE_HOST "*****************************"  // o endereço do nome do projeto do firebase id
#define FIREBASE_AUTH "******************************" // a chave secreta gerada a partir do firebase
#define WIFI_SSID "***********************"                                      // insira seu nome de wifi em casa ou público
#define WIFI_PASSWORD "******************************"                               // senha do wifi ssid
//----------------------Pinos-----------------------------------------
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
//-----------------------Variavei----------------------------------------
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
//---------------------------------------------------------------
void setup()
{
  //Seto o estado dos pinos
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
  // Seto o PWN no ESP32
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
  //Inicio o WiFi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Conectado AO Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
  }
  Serial.println("Contectado com Sucesso");
  //Inicializo o Firebase
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);
  //e peço para inicializar os valores no estado 0
  Firebase.setInt(firebaseData, path +"/rele1", 0);
  Firebase.setInt(firebaseData, path +"/rele2", 0);
  //Aki começo o Stream do quarto no firebase
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
  // E aki eu coloco um callback para toda vez q haver uma alteracao no local selecionado
  Firebase.setStreamCallback(firebaseData, streamCallback, streamTimeoutCallback);
  //Aki coloco uma interrupçao para os sensores touch para so quando o sensor der uma borda de subida fazer a acao
  attachInterrupt(digitalPinToInterrupt(sensor_touch1), touch1, RISING);
  attachInterrupt(digitalPinToInterrupt(sensor_touch2), touch2, RISING);
  RGB();
}
// Essa função so serve para avisao quando o Stream terminar
void streamTimeoutCallback(bool timeout)
{
  if (timeout)
  {
    Serial.println();
    Serial.println("> Stream 1 acabou o tempo, resumindo o streaming...");
    Serial.println();
  }
}
// Aki e oque vai acontecer quando o Callback for acionado
void streamCallback(StreamData data)
{
  RGB();
  //rele_firebase();
}

void RGB()
{
  // Pega o valor do led vermelho e aplico o valor na porta PWN dos leds vermelhos
  Firebase.getString(firebaseData, path +"/ledred");
  
  valor = firebaseData.stringData().toInt();
  valor = map(valor,0,255,255,0);
  Serial.print("Valor LEDred=");
  Serial.println(valor);
  ledcWrite(0, valor);
  ledcWrite(3, valor);
  ledcWrite(6, valor);
  //Mesma coisa para os leds Verdes
  Firebase.getString(firebaseData, path +"/ledgreen");
  
  valor = firebaseData.stringData().toInt();
  valor = map(valor,0,255,255,0);
  Serial.print("Valor LEDgreen=");
  Serial.println(valor);
  ledcWrite(1, valor);
  ledcWrite(4, valor);
  ledcWrite(7, valor);
   //Mesma coisa para os leds Azuis
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
  //Aki ele somente vai ver qual o estado do rele atual e mudar para o oposto quando o touch for acionado
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
// So umas funçoes para deixar os leds verde e vermelho quando o interruptor for acionado
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
// Esta função e para checar quando o estado do rele for mudado no FireBase
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
  
 

  touch1_envio();
  touch2_envio();
  if(trava_puxar1 != 1 && trava_puxar2){
  rele_firebase();
  }
}

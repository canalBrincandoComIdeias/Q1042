#define pinMotor1A     10 //az
#define pinMotor1B     11 //vd
#define pinMotor2A     9  //br
#define pinMotor2B     6  //cz

#define pinSensorLinD  4  //cz 
#define pinSensorLinE  5  //br

#define pinDistEcho    2  //am
#define pinDistTrigger 3  //lj 

#define pinBtRx        8  //vd
#define pinBtTx        7  //az

#include <SoftwareSerial.h>

SoftwareSerial bluetooth(pinBtRx, pinBtTx); // RX, TX

void enviaPulso();
void medeDistancia();

//Variaveis para Sensor de Distancia
volatile unsigned long  inicioPulso = 0;
volatile float          distancia   = 0;
volatile int            modo        = -1;

//Variaveis para Motor
int velocidade = 0;
bool sentido = 1;
int estado = 0;

void setup() {
  pinMode(pinMotor1A, OUTPUT);
  pinMode(pinMotor1B, OUTPUT);
  pinMode(pinMotor2A, OUTPUT);
  pinMode(pinMotor2B, OUTPUT);

  pinMode(pinSensorLinD, INPUT);
  pinMode(pinSensorLinE, INPUT);

  pinMode(pinDistEcho, INPUT);
  pinMode(pinDistTrigger, OUTPUT);
  digitalWrite(pinDistTrigger, LOW);

  Serial.begin(9600);
  bluetooth.begin(9600);

  // CONFIGURA A INTERRUPÇÃO PARA SENSOR DE DISTANCIA
  attachInterrupt(digitalPinToInterrupt(pinDistEcho), medeDistancia, CHANGE); 
}

void loop() {

  //Teste do Bluetooth
  if (bluetooth.available()) {
    Serial.write(bluetooth.read());
    Serial.print("- ");
  } else {
    Serial.print(" - ");
  }

  //Teste dos Sensores de Linha
  Serial.print("SD:");
  Serial.print(digitalRead(pinSensorLinD));
  
  Serial.print(" SE:");
  Serial.print(digitalRead(pinSensorLinE));

  //Teste do Sensor de Distancia
  // ENVIA O COMANDO PARA O MÓDULO LER A DISTANCIA
  enviaPulso();
  // A RESPOSTA DA DISTANCIA VEM POR INTERRUPÇÃO, SÓ PRECISA ESPERAR ALGUNS MILISSEGUNDOS
  
  delay(25); // TEMPO DE RESPOSTA APÓS A LEITURA
  
  Serial.print(" Dist:");
  Serial.print(distancia);
  Serial.println("cm");

  //Teste dos Motores
  if (sentido) {
    velocidade += 2;
    if (velocidade >= 255) {
       velocidade = 255;
       sentido = 0;
       estado++;
       if (estado > 5) estado = 0;
    }
  } else {
    velocidade -= 2;
    if (velocidade <= 0) {
       velocidade = 0;
       sentido = 1;
       estado++;
       if (estado > 5) estado = 0;
    }
  }

  switch (estado) {
    case 0:
       analogWrite(pinMotor1A, velocidade);
       analogWrite(pinMotor1B, 0);
       analogWrite(pinMotor2A, velocidade);
       analogWrite(pinMotor2B, 0);
       break;
    case 1:
       analogWrite(pinMotor1A, 0);
       analogWrite(pinMotor1B, velocidade);
       analogWrite(pinMotor2A, 0);
       analogWrite(pinMotor2B, velocidade);
       break;
    case 2:
       analogWrite(pinMotor1A, velocidade);
       analogWrite(pinMotor1B, 0);
       analogWrite(pinMotor2A, 0);
       analogWrite(pinMotor2B, 0);
       break;
    case 3:
       analogWrite(pinMotor1A, 0);
       analogWrite(pinMotor1B, 0);
       analogWrite(pinMotor2A, velocidade);
       analogWrite(pinMotor2B, 0);
       break;
    case 4:
       digitalWrite(pinMotor1A, LOW);
       digitalWrite(pinMotor1B, LOW);
       digitalWrite(pinMotor2A, LOW);
       digitalWrite(pinMotor2B, LOW);
       break;
  }

  delay(20);
}

// IMPLEMENTO DE FUNÇÕES
void medeDistancia(){
  switch (modo) {
    case 0: {
        inicioPulso = micros();
        modo = 1;
        break;
      }
    case 1: {
        distancia = (float)(micros() - inicioPulso) / 58.3; // distancia em CM
        inicioPulso = 0;
        modo = -1;
        break;
      }
  }
}

void enviaPulso(){
  // ENVIA O SINAL PARA O MÓDULO INICIAR O FUNCIONAMENTO
  digitalWrite(pinDistTrigger, HIGH);
  // AGUARDAR 10 uS PARA GARANTIR QUE O MÓDULO VAI INICIAR O ENVIO
  delayMicroseconds(10);
  // DESLIGA A PORTA PARA FICAR PRONTO PARA PROXIMA MEDIÇÃO
  digitalWrite(pinDistTrigger, LOW);
  // INDICA O MODO DE FUNCIONAMENTO (AGUARDAR PULSO)
  modo = 0;
}

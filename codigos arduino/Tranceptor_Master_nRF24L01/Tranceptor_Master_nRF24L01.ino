/*  ----------------------------------------------------------------
    SETISAEDU
    Codigo para el uso del modulo NRF2401 para comunicar dos Arduinos.
    Codigo de control del maestro (Transmisor)
  --------------------------------------------------------------------
*/
#include <SPI.h>
#include "nRF24L01.h" //Librerias del modulos nRF24L01+
#include "RF24.h"

RF24 radio(9, 10);//Declaracion de los pines de control CE y CSN para el modulo, se define "radio"

// Definicion de los canales de comunicacion 
byte canal_E[5] = {'l', 'i', 'n', 'e', '1'};//Canal de de Escritura del modulo Maestro
byte canal_L[5] = {'l', 'i', 'n', 'e', '2'};//Canal de de Lectura del modulo Mestro
char fun[3] = {'P', 'L', 'T'};

void setup(void)
{ 
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  Serial.begin(115200);
  
  radio.begin(); //Inicio del modulo nRF24L01+
  radio.setRetries(15, 15);// Cinfiguracion del numero maximo de reintentos
  //Apertura de los canales de comunicacion
  radio.openWritingPipe(canal_E);
  radio.openReadingPipe(1, canal_L);
  Serial.println("Sistema de comunicacion con nRF24L01");
  Serial.println("Digite la letra de la variable a consultar al receptor");
  Serial.println("\"P\"-Potenciometro,\"L\"-LDR, \"T\"-Temperatura");
}

void loop(void)
{
  radio.stopListening(); // Paramos la escucha para poder escribir
  if (Serial.available())
  {
    int data = Serial.read();
    if (data == 'P')
    {
      Serial.print("Leyendo potenciometro  ");
      bool ok = radio.write(&fun[0], sizeof(fun[0])); //Envio de la funcion seleccionada
      if (ok)
        Serial.println("ok...");
      else
        Serial.println("failed");

      radio.startListening(); //Volvemos a la escucha
      unsigned long started_waiting_at = millis();
      bool timeout = false;
      while ( ! radio.available() && ! timeout ) // Esperamos 200ms
        if (millis() - started_waiting_at > 200 )
          timeout = true;
      if ( timeout )
        Serial.println("Error, No ha habido respuesta a tiempo");
      else
      { // Leemos el mensaje recibido
        unsigned int Pot;
        radio.read( &Pot, sizeof(unsigned int)); //Funcion de lectura de datos
        Serial.print("Valor analogico del potenciometro = ");
        Serial.println(Pot);
      }
    } else if (data == 'L')
    {
      Serial.print("Leyendo LDR  ");
      bool ok = radio.write(&fun[1], sizeof(fun[1])); //Envio de la funcion seleccionada
      if (ok)
        Serial.println("ok...");
      else
        Serial.println("failed");

      radio.startListening(); //Volvemos a la escucha
      unsigned long started_waiting_at = millis();
      bool timeout = false;
      while ( ! radio.available() && ! timeout ) // Esperamos 200ms
        if (millis() - started_waiting_at > 200 )
          timeout = true;
      if ( timeout )
        Serial.println("Error, No ha habido respuesta a tiempo");
      else
      { // Leemos el mensaje recibido
        unsigned int LDR;
        radio.read( &LDR, sizeof(unsigned int)); //Funcion de lectura de datos
        Serial.print("Valor analogico del LDR = ");
        Serial.println(LDR);
      }
    } else if (data == 'T')
    {
      Serial.print("Leyendo Temperatura  ");
      bool ok = radio.write(&fun[2], sizeof(fun[2])); //Envio de la funcion seleccionada
      if (ok)
        Serial.println("ok...");
      else
        Serial.println("failed");

      radio.startListening();                      //Volvemos a la escucha
      unsigned long started_waiting_at = millis();
      bool timeout = false;
      while ( ! radio.available() && ! timeout )       // Esperamos 200ms
        if (millis() - started_waiting_at > 200 )
          timeout = true;
      if ( timeout )
        Serial.println("Error, No ha habido respuesta a tiempo");
      else
      { // Leemos el mensaje recibido
        float Temp;
        radio.read( &Temp, sizeof(Temp)); //Funcion de lectura de datos
        Serial.print("Temperatura = ");
        Serial.print(Temp);
        Serial.println(" C");
      }
    }
  }
 
}


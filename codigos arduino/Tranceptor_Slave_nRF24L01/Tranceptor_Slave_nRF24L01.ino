/*  ----------------------------------------------------------------
    SETISAEDU
    Codigo para el uso del modulo NRF2401 para comunicar dos Arduinos.
    Codigo de control del esclavo (Receptor)
  --------------------------------------------------------------------
*/
#include <SPI.h> //libreria para el uso del modulo SPI
#include <Wire.h> //Libreria para el uso del modulo I2C
#include <LiquidCrystal_I2C.h> //Libreria para el control de la LCD por I2C
#include "nRF24L01.h" //Libreria del modulos nRF24L01+
#include "RF24.h"

LiquidCrystal_I2C lcd(0x27, 20, 4); /* Configuracion de la direccion i2c
                                     * y dimeciones del modulo LCD  */

RF24 modulo(9, 10); //Declaracion de los pines de control CE y CSN para el modulo, se define el objeto "modulo"


// Definicion de los canales de comunicacion 
byte canal_L[5] = {'l', 'i', 'n', 'e', '1'}; //Canal de de Lectura del modulo Esclavo
byte canal_E[5] = {'l', 'i', 'n', 'e', '2'}; //Canal de de Escritura del modulo Esclavo
//Declaracion de las variables a usar
unsigned int pot = 0;
unsigned int LDR = 0;
unsigned int fake_temp = 0;
unsigned int t = 0;

float temp = 0;


void setup(void)
{
  pinMode(10, OUTPUT); //Iniciar como salida el pin de control CSN
  modulo.begin(); //Inicio del modulo nRF24L01+
  modulo.setRetries(15, 15); //Configuracion del numero de intentos
  //Apertura de los canales de comunicacion
  modulo.openWritingPipe(canal_E); 
  modulo.openReadingPipe(1, canal_L);

  //Iniciacion de la LCD
  lcd.init();
  lcd.backlight();
  //Mensaje inicial
  lcd.setCursor(0, 0);
  lcd.print("Comunicacion con ");
  lcd.setCursor(0, 1);
  lcd.print("los modulos nRF24L01");
  lcd.setCursor(0, 2);
  lcd.print("SETISA EDUCATION");
  modulo.startListening(); // Activando el modulo en modo de escucha
  delay(1000);
  lcd.setCursor(0, 3);
  lcd.print("modulo iniciado."); 
  lcd.clear();
  Serial.begin(115200);

}

void loop(void)
{
  //Inicio del lazo principal
  //Lectura de los datos
  fake_temp = analogRead(A2); //Primera lectura del sensor de la temperatura para evitar datos erroneos
  t = analogRead(A2);  //Lectura del sensor de temperatura
  pot = analogRead(A0); //Lectura del valor del Potenciometro
  LDR = analogRead(A1); //Lectura del valor de la LDR
  temp = (5.0 * t * 100) / 1023.0; //ajuste a grados centigrados

  //Impresion de los datos leidos en la LCD
  lcd.setCursor(3, 0);
  lcd.print("Datos leidos");
  lcd.setCursor(0, 1);
  lcd.print("Potenciometro=      ");
  lcd.setCursor(16, 1);
  lcd.print(pot);
  lcd.setCursor(0, 2);
  lcd.print("LDR=                "); 
  lcd.setCursor(6, 2);
  lcd.print(LDR);
  lcd.setCursor(0, 3);
  lcd.print("Temperatura=        ");
  lcd.setCursor(14, 3);
  lcd.print(temp);
  
  //Espera de peticiones
  if ( modulo.available() )      // Comprobacion de datos recibidos
  {
    char op; //operacion
    bool done = false;
    while (!done)          // Espera aqui hasta recibir algo
    {
      modulo.read( &op, sizeof(op) );
      if (op > 0)done = true;
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Solicitud Recibido = ");
      lcd.print(String(op));
      delay(20);             // Para dar tiempo al emisor
      lcd.clear();
    }
    modulo.stopListening(); // Paramos el modo de escucha para poder escribir
    if (op == 'P') //Solicitud de valor del Potenciometro
    {
      modulo.write( &pot, sizeof(unsigned int)); //Envio de variable solicitada
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Enviando valor del");
      lcd.setCursor(0,1);
      lcd.print("Potenciometro..");
      delay(300); // Espera para que se puede visualizar en la LCD
      lcd.clear();
    } else if (op == 'L') //Solicitud de valor de LDR
    {
      
      modulo.write( &LDR, sizeof(unsigned int)); //Envio de variable solicitada
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Enviando valor de");
      lcd.setCursor(0,1);
      lcd.print("la LDR..");
      delay(300); // Espera para que se puede visualizar en la LCD
      lcd.clear();
    } else if (op == 'T') //Solicitud de la temperatura
    {
      modulo.write( &temp, sizeof(float)); //Envio de variable solicitada
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Enviando valor de");
      lcd.setCursor(0,1);
      lcd.print("Temperatura..");
      delay(300); // Espera para que se puede visualizar en la LCD
      lcd.clear();
    }else //En el caso de recibir un dato erroneo
    {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Error!");
      lcd.setCursor(0,1);
      lcd.print("operacion no validad!");
      delay(1000); // Espera para que se puede visualizar en la LCD
      lcd.clear();
    }
    // Volvemos a la escucha para recibir mas paquetes
    modulo.startListening();
  } 
}


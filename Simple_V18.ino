//// Librerias Sensor dallas y one wire
#include <OneWire.h>
#include <DallasTemperature.h>

// Data wire is plugged into port 2 on the Arduino
#define ONE_WIRE_BUS 2

////Librerias necesarias para comunicacion ethernet

#include <SPI.h>
#include <Ethernet.h>

////Libreria Modbus
#include "MgsModbus.h"
MgsModbus Mb;

#include <EEPROM.h>



// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);

//Define la variable y el tamaño donde se guardarán las direcciones
DeviceAddress RomID[10];
/*
   The setup function. We only start the sensors here
*/

int dato[18];

void setup(void) {

  ethernet_escribir();
  ethernet_leer();


  //// Configuracion ethernet (depending on MAC and Local network)
  //// La libreria por defecto tiene configurado el puerto 502
  byte mac[] = {0x90, 0xA2, 0xDA, 0x0E, 0x94, 0xB5 };
  IPAddress ip(dato[0], dato[1] , dato[2] , dato[3]);
  IPAddress gateway(dato[4], dato[5] , dato[6], dato[7]);
  IPAddress subnet(dato[8], dato[9] , dato[10], dato[11]);


  Ethernet.begin(mac, ip, gateway, subnet);   // inicia comunicacion ethernet

  // Fill MbData
  //  Mb.SetBit(0,false);
  Mb.MbData[0] = 16;
  /*  Mb.MbData[1] = 0;
    Mb.MbData[2] = 0;
    Mb.MbData[3] = 0;
    Mb.MbData[4] = 0;
    Mb.MbData[5] = 0;
    Mb.MbData[6] = 0;
    Mb.MbData[7] = 0;
    Mb.MbData[8] = 0;
    Mb.MbData[9] = 0;
    Mb.MbData[10] = 0;
    Mb.MbData[600] = 0;*/

  //Inicializa los registros cargando el valor 0
  for (int j = 0; j <= 600; j++) {
    Mb.MbData[j] = (0);
  }

  // start serial port
  Serial.begin(9600);

  // Start up the library
  sensors.begin();
  int i = 0;
  OneWire ow(2);//Pin donde está conectado el bus one wire
  DeviceAddress RomIDD;
  uint8_t address[8];

}

//// funcion para imprimir en el puerto serial las direcciones de los sensores
void printAddress(DeviceAddress deviceAddress)
{
  for (uint8_t i = 0; i < 8; i++)
  {
    // zero pad the address if necessary
    if (deviceAddress[i] < 16) Serial.print("0");
    Serial.print(deviceAddress[i], HEX);
  }
}


void loop() {

  int j = 0;

  //call sensors.requestTemperatures() to issue a global temperature
  // request to all devices on the bus
  Serial.print("Requesting temperatures...");
  sensors.requestTemperatures(); // Send the command to get temperatures
  Serial.println("DONE");
  // After we got the temperatures, we can print them here.
  // We use the function ByIndex, and as an example get the temperature from the first sensor only.

  Mb.MbsRun();

  OneWire ow(2);
  uint8_t address[8];
  uint8_t count = 0;
  uint8_t i = 0;

  while (ow.search(address)) { //Busca sensores presentes en el bus
    sensors.getAddress(RomID[i], i); //Consigue las direcciones
    printAddress(RomID[i]);
    Serial.print('\n');
    //Guarda las direcciones de los sensores en los registros modbus comenzando desde Mb.MbData[1]
    Mb.MbData[4 * i + 1] = RomID[i][0] * 256 + RomID[i][1];
    Mb.MbData[4 * i + 2] = RomID[i][2] * 256 + RomID[i][3];
    Mb.MbData[4 * i + 3] = RomID[i][4] * 256 + RomID[i][5];
    Mb.MbData[4 * i + 4] = RomID[i][6] * 256 + RomID[i][7];
    //dato temperatura, los datos de temperatura comienzan a guardarse desde la direccion 500
    //sensors.getTempCByIndex(i): muestra la temperatura donde i es el numero del sensor en el bus
    Mb.MbData[ i + 500] = (int) (sensors.getTempCByIndex(i)*100);
    i = i + 1;
  }

  //Cantiadad de sensores presentes en el bus
  Serial.print("Cantidad de sensores=");
  Serial.print(i);
  Serial.print('\n');//Terminador de linea
  Mb.MbData[0] = i;

  //imprime las temperaturas en el monitor serial
  Serial.print("temperatura 0: ");
  Serial.println(Mb.MbData[500]);
  Serial.print("temperatura 1: ");
  Serial.println(sensors.getTempCByIndex(1));

}

void ethernet_escribir() {
  //IP];
  dato[0] = 192;
  dato[1] = 168;
  dato[2] = 2;
  dato[3] = 10;
  //getaway
  dato[4] = 192;
  dato[5] = 168;
  dato[6] = 2;
  dato[7] = 1;
  //mask
  dato[8] = 255;
  dato[9] = 255;
  dato[10] = 255;
  dato[11] = 0;

  int j = 0;
  int k = 0;

  for (j = 0; j <= 12; j++) {
    EEPROM.write(k, dato[j]);
    k = k + 1;
  }

}


void ethernet_leer() {
  int j = 0;
  int k = 0;
  for (j = 0; j <= 12; j++) {
    dato[j] = EEPROM.read(k);
    k = k + 1;
  }

}

/*
/Este es un skectch para transmitir la data de un sensor de calidad de AIRE AIQ a un servicio de MQTT en internet 
/debe tomarse en cuenta que deben agregarse la libreria libalgobsec.a en el directorio lib para lograr el funcionamiento 
/de la libreri incluida por el fabricante BOSCH adicionamente debe modificarse el archivo platform.ini e incluir la opcion
/build_flags = -Llib/ -lalgobsec para un correcto funcionamiento del mismo, el sensot debe quemarse durante 24 horas luego de 
/ser encendido por primer vez para lograr una esabilidad en la medicion del VOC  y luego no debra encenderse por mas de 15 minutos 
/para no perder la calibracion del mismo en un corto tiempo, se recomeinda mediciones consecuticvas de un minuto en intervalos de 10 minutos
/de forma optima para preservar el sensor en un estado optimo por un periodo de un año. 
/
*/
//--------Incluir Librerias para el funcionamienot del sensor BME680 de BOSCH
#include <Arduino.h>
#include <bsec.h>
#include <Wire.h>
//--------Incluir librerias para almacenar los ultimos datos leidos como un Json -------
#include <FS.h>                                         //Manejo de sistema de archivos JSON
#include <ArduinoJson.h>                                //serializacion y desearilizacion de docuemntos
#include <SPIFFS.h>
//--------Incluir Librerias para la sinronizacion de hora por medio de servicios de NTP
#include <NTPClient.h>                                  //Liberia de manejode consulta de hora en intenet
#include <WiFi.h>                                       // for WiFi shield
#include <WiFiUdp.h>                                    //Libreria para el manejo de paquetes UDP sobre Wifi
//-------------------------Estrcuturas (struct)-----------------------------------------------------------
#include "prgm_struct.h"                                //Libreria con variables de configuracion

/*-----------------------------------------------------Defeniciones-----------------------------------------------------------*/
const char *JsonConfigurefileName = "/SetupConfig.json"; //Archivo JSON de configuracion
const char *JsonDatafileName = "/BME680data.json";      //Archivo JSON de configuracion
/*-----------------------------------------------------Variables--------------------------------------------------------------*/
WiFiUDP ntpUDP;
WConfig Wificonfig;                                     //Cliente UDP para Wifi
NTPConfig NTPConnection;

// You can specify the time server pool and the offset (in seconds, can be
// changed later with setTimeOffset() ). Additionaly you can specify the
// update interval (in milliseconds, can be changed using setUpdateInterval() ).
NTPClient timeClient(ntpUDP, NTP_Server, NTPConnection.NTP_port, NTPConnection.NTP_interval);

// Helper functions declarations
void checkIaqSensorStatus(void);
void errLeds(void);

// Create an object of the class Bsec
Bsec iaqSensor;

String output;

// Entry point for the example
void setup(void)
{
  Serial.begin(115200);
  Wire.begin();

  iaqSensor.begin(BME680_I2C_ADDR_PRIMARY, Wire);
  output = "\nBSEC library version " + String(iaqSensor.version.major) + "." + String(iaqSensor.version.minor) + "." + String(iaqSensor.version.major_bugfix) + "." + String(iaqSensor.version.minor_bugfix);
  Serial.println(output);
  checkIaqSensorStatus();

  bsec_virtual_sensor_t sensorList[10] = {
      BSEC_OUTPUT_RAW_TEMPERATURE,
      BSEC_OUTPUT_RAW_PRESSURE,
      BSEC_OUTPUT_RAW_HUMIDITY,
      BSEC_OUTPUT_RAW_GAS,
      BSEC_OUTPUT_IAQ,
      BSEC_OUTPUT_STATIC_IAQ,
      BSEC_OUTPUT_CO2_EQUIVALENT,
      BSEC_OUTPUT_BREATH_VOC_EQUIVALENT,
      BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_TEMPERATURE,
      BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_HUMIDITY,
  };

  iaqSensor.updateSubscription(sensorList, 10, BSEC_SAMPLE_RATE_LP);
  checkIaqSensorStatus();

  // Print the header
  output = "Timestamp [ms], raw temperature [°C], pressure [hPa], raw relative humidity [%], gas [Ohm], IAQ, IAQ accuracy, temperature [°C], relative humidity [%], Static IAQ, CO2 equivalent, breath VOC equivalent";
  Serial.println(output);
}

// Function that is looped forever
void loop(void)
{
  unsigned long time_trigger = millis();
  if (iaqSensor.run())
  { // If new data is available
    output = String(time_trigger) + "[ms]";
    output += ", " + String(iaqSensor.rawTemperature) + "[°C]";
    output += ", " + String(iaqSensor.pressure) + "[hPa]";
    output += ", " + String(iaqSensor.rawHumidity) + "[%]";
    output += ", " + String(iaqSensor.gasResistance) + "[Ohm]";
    output += ", " + String(iaqSensor.iaq) + "[IAQ]";
    output += ", " + String(iaqSensor.iaqAccuracy) + "[IAQ accuracy]";
    output += ", " + String(iaqSensor.temperature) + "[°C]";
    output += ", " + String(iaqSensor.humidity) + "[%]";
    output += ", " + String(iaqSensor.staticIaq) + "[ Static IAQ]";
    output += ", " + String(iaqSensor.co2Equivalent) + "[CO2 equivalent]";
    output += ", " + String(iaqSensor.breathVocEquivalent) + "[breath VOC equivalent]";
    Serial.println(output);
  }
  else
  {
    checkIaqSensorStatus();
  }
}

// Helper function definitions
void checkIaqSensorStatus(void)
{
  if (iaqSensor.status != BSEC_OK)
  {
    if (iaqSensor.status < BSEC_OK)
    {
      output = "BSEC error code : " + String(iaqSensor.status);
      Serial.println(output);
      for (;;)
        errLeds(); /* Halt in case of failure */
    }
    else
    {
      output = "BSEC warning code : " + String(iaqSensor.status);
      Serial.println(output);
    }
  }

  if (iaqSensor.bme680Status != BME680_OK)
  {
    if (iaqSensor.bme680Status < BME680_OK)
    {
      output = "BME680 error code : " + String(iaqSensor.bme680Status);
      Serial.println(output);
      for (;;)
        errLeds(); /* Halt in case of failure */
    }
    else
    {
      output = "BME680 warning code : " + String(iaqSensor.bme680Status);
      Serial.println(output);
    }
  }
}

void errLeds(void)
{
  //pinMode(LED_BUILTIN, OUTPUT);  
  //digitalWrite(LED_BUILTIN, HIGH);
  Serial.println("error in sensor");
  delay(100);
  Serial.println("porfavor revise los cables");
  //digitalWrite(LED_BUILTIN, LOW);
  delay(100);
}
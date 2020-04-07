#include <Arduino.h>
#include <bsec.h>
#include <Wire.h>

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
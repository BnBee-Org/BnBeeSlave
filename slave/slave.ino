/*
    Hive code:
    Ostap Kych
    21/09/2022
*/
#include <SoftwareSerial.h>
#include <DHT.h>
//#include <HX711.h>

#define DHTPIN 2
#define DHTTYPE DHT21
//#define DHTTYPE DHT11

#define LOADCELL_DOUT_PIN 5
#define LOADCELL_SCK_PIN 4
#define calibration_factor - 24080

#define RS485_TXEN 3
#define RS485_Rx 8
#define RS485_Tx 9

SoftwareSerial mySerial(RS485_Rx, RS485_Tx);

DHT dht(DHTPIN, DHTTYPE);

HX711 scale;
String SLAVE_ID = "01";
int hum = 0;
float temp = 0;
float weight = 0;

void setup()
{
  pinMode(RS485_Rx, INPUT);
  pinMode(RS485_Tx, OUTPUT);
  pinMode(RS485_TXEN, OUTPUT);
  mySerial.begin(19200);
  Serial.begin(9600);
  digitalWrite(RS485_TXEN, LOW);
  delay(30);
  scale.set_scale(calibration_factor);
  dht.begin();
}

void loop()
{
  temp = dht.readTemperature();
  hum = dht.readHumidity();
  weight = scale.get_units(), 1;  
  String slave_data = "SD;" +
    SLAVE_ID +
    ";H:" + hum +
    ";T:" + temp +
    ";W:" + weight;
  if (mySerial.available())
  {
    //looks for data from master, which are sperated with '\n'
    String request = mySerial.readStringUntil('\n');
    if (request == "D;" + SLAVE_ID + ";")
    {
      Serial.println("Data request received, sending:");
      Serial.println(slave_data);
      send_data(slave_data);
    }
  }
}


void send_data(String request)
{
  digitalWrite(RS485_TXEN, HIGH);
  delay(30);
  mySerial.println(request);
  digitalWrite(RS485_TXEN, LOW);
  delay(30);
}

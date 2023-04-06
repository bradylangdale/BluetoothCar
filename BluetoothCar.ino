#include <BluetoothSerial.h>
#include "camera_interface.h"
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

BluetoothSerial SerialBT;
#define PACKET_SIZE 64

#define IN1 4
#define IN2 2
#define IN3 14
#define IN4 15

byte *frame;

void setup() 
{
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); //disable brownout detector 
  Serial.begin(115200);
  
  SerialBT.begin("BluetoothCar");
  Serial.println("Bluetooth enabled.");

  delay(1000);
  
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  Serial.println("Motor pins enabled.");

  delay(1000);

  ESP_CAMERA::init_camera();
  frame = (byte *)malloc(20000);

  Serial.println("Camera initialized.");

  delay(1000);

  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 1); //enable brownout detector
}

void loop() 
{  
  while (SerialBT.available()) {
      Serial.print("Received: ");
      char command = SerialBT.read();
      Serial.println(command);

      switch(command)
      {
        case 'f':
          forward();
          break;
        case 'b':
          reverse();
          break;
        case 'l':
          left();
          break;
        case 'r':
          right();
          break;
        case 's':
          stop();
          break;
        default:
          stop();
          break;
      }
  }

  int bytes = ESP_CAMERA::get_frame(frame);
  if (bytes > 0)
  {
    writeBytes(frame, bytes);
  }
}

void stop()
{
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
    
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, LOW);
}

void forward()
{
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);

    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
}

void reverse()
{
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
}

void left()
{
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
}

void right()
{
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);

    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
}

void writeBytes(byte message[], int len)
{
  byte msg[PACKET_SIZE] = {0};
  int i = 0;
  if (len > PACKET_SIZE)
  {
    for (i = 0; i < int(len / PACKET_SIZE); i++)
    {
      memcpy(msg, message + (i * PACKET_SIZE), PACKET_SIZE);
      SerialBT.write(msg, PACKET_SIZE);
    }
    
    if (i * PACKET_SIZE < len)
    {
      memcpy(msg, message + (i * PACKET_SIZE), len - (i * PACKET_SIZE));
      SerialBT.write(msg, PACKET_SIZE);
    }
  } else
  {
    memcpy(msg, message, PACKET_SIZE);
    SerialBT.write(msg, PACKET_SIZE);
  }
}

/*
 * HM10Demo: Simple sketch to interact with the HM-10 module.
 *
 * Copyright Chris Dinh 2022
 */

/* INCLUDES */
#include <AltSoftSerial.h>

static AltSoftSerial btSerial;

/* There is data waiting to be read from the HM-10 device. */
static void HandleRxDataIndication(void)
{
   char c = btSerial.read();

   /* Just echo the character for now. */
   Serial.write(c);
}

/* There is data waiting to be sent to the HM-10 device. */
static void HandleTxDataIndication(void)
{
   char c = Serial.read();

   /* Echo the character just been sent. */
   Serial.write(c);

   /* We don't send carriage return or line feed. */
   if (c == 0x0A || c == 0x0D)
   {
      return;
   }

   btSerial.write(c);
}

void setup()
{
   Serial.begin(9600);
   btSerial.begin(9600);
   Serial.println("BTserial started at 9600");
}

void loop()
{
   if (btSerial.available())
   {
      HandleRxDataIndication();
   }

   if (Serial.available())
   {
      HandleTxDataIndication();
   }
}

/*
 * IrReceiverModuleDemo: Simple sketch to interact with the IR receiver module to
 * intercept IR signals.
 */
#include <IRremote.h>

#define RECV_PIN 11 // IR receiver signal pin

void setup()
{
   Serial.begin(115200);
   IrReceiver.begin(RECV_PIN, ENABLE_LED_FEEDBACK);
}

void loop()
{
   if (IrReceiver.decode())
   {
      Serial.println(IrReceiver.decodedIRData.decodedRawData, HEX);
      IrReceiver.resume();
   }
}

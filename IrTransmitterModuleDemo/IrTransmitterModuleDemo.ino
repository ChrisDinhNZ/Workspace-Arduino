/*
 * IrTransmitterModuleDemo: Simple sketch to interact with the IR transmitter module to
 * send IR signals.
 *
 * Copyright Chris Dinh 2022
 */

#include "Arduino.h"
#include <IRremote.h>

/* NEC command consist of 32 raw bits i.e. 8 Hex digit string. */
#define NEC_HEX_STRING_MAX_LENGTH 8U
#define COMMAND_VALUE_MASK 0xFF0000
#define COMMAND_VALUE_OFFSET 16U
#define ADDRESS_VALUE_MASK 0xFFFF

#define LINE_FEED_VALUE 10U
#define CARRIAGE_RETURN_VALUE 13U

#define IR_SEND_PIN 13U

static char hexString[NEC_HEX_STRING_MAX_LENGTH];
static uint8_t hexStringLength;

static void ResetHexString(void)
{
   memset(hexString, 0, sizeof(hexString[0]) * NEC_HEX_STRING_MAX_LENGTH);
   hexStringLength = 0;
}

static void processNecCommand(const char *command)
{
   /* We assumed validation has been done and command is a valid NEC format command.
      Convert Hex string to actual 32 bit representation. */
   uint32_t convertedCommand = strtoul(command, (char **)NULL, 16);
   uint16_t necAddress = (convertedCommand & ADDRESS_VALUE_MASK);
   uint8_t necCommand = (convertedCommand & COMMAND_VALUE_MASK) >> COMMAND_VALUE_OFFSET;
   uint8_t necRepeats = 0;

   if (convertedCommand == 0)
   {
      return;
   }

   IrSender.sendNEC(necAddress, necCommand, necRepeats);
   Serial.println("Command sent");
   ResetHexString();
}

void setup()
{
   ResetHexString();
   IrSender.begin(IR_SEND_PIN, true, LED_BUILTIN);
   Serial.begin(115200); // opens serial port
   Serial.println("Begin IrTransmitterModuleDemo");
}

void loop()
{
   /* If there are data to read, do nothing */
   if (!Serial.available())
   {
      return;
   }

   char c = Serial.read();

   /* When receiving data over the serial connection, we will treat LF and CR as command termination. */
   if (c == LINE_FEED_VALUE || c == CARRIAGE_RETURN_VALUE)
   {
      /* If command received so far is not at the expected length, start over. */
      if (hexStringLength != NEC_HEX_STRING_MAX_LENGTH)
      {
         ResetHexString();
         return;
      }

      processNecCommand(hexString);
      return;
   }

   /* The command should only contains valid Hex digits, otherwise start again.
      Valid chars: [{0 - 9}, {A - F}]
   */
   if (c < '0' || (c > '9' && c < 'A') || c > 'F')
   {
      ResetHexString();
      return;
   }

   /* Check if command's still has valid length. */
   if (hexStringLength >= NEC_HEX_STRING_MAX_LENGTH)
   {
      ResetHexString();
      return;
   }

   /* Carry on adding hex digit to current command. */
   hexString[hexStringLength] = c;
   hexStringLength++;
}

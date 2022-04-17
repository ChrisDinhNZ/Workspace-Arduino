/*
 * IrTransmitterModuleDemo_v2: Simple sketch to interact with the IR transmitter module to
 * send IR signals.
 *
 * Copyright Chris Dinh 2022
 */

/* INCLUDES */
#include "Arduino.h"
#include <IRremote.h>
#include <string.h>

/* DEFINITIONS */
#define COMMAND_STRING_MAX_LENGTH 15U

#define COMMAND_VALUE_MASK 0xFF0000
#define COMMAND_VALUE_OFFSET 16U
#define ADDRESS_VALUE_MASK 0xFFFF

#define LINE_FEED_VALUE 10U
#define CARRIAGE_RETURN_VALUE 13U

#define IR_SEND_PIN 13U

/* Remote Control R0803 Button Code */
#define BUTTON_CODE_POWER_TOGGLE    0x629D47B8  // Toggles power on/off
#define BUTTON_CODE_0               0x7F8047B8  // Numbers can be used to select radio presets
#define BUTTON_CODE_1               0x7E8147B8
#define BUTTON_CODE_2               0x7D8247B8
#define BUTTON_CODE_3               0x7C8347B8
#define BUTTON_CODE_4               0x7B8447B8
#define BUTTON_CODE_5               0x7A8547B8
#define BUTTON_CODE_6               0x798647B8
#define BUTTON_CODE_7               0x788747B8
#define BUTTON_CODE_8               0x778847B8
#define BUTTON_CODE_9               0x768947B8
#define BUTTON_CODE_10X             0xF20D47B8  // Selects radio presets 10 or greater
#define BUTTON_CODE_VOLUME_UP       0x649B47B8
#define BUTTON_CODE_VOLUME_DOWN     0x659A47B8
#define BUTTON_CODE_MUTE            0x639C47B8  // Toggle mute
#define BUTTON_CODE_INPUT           0x33CC01B8  // Iterate through input types (Radio, Phono, CD, Video2, Video1, Tape1)
#define BUTTON_CODE_STEREO          0x28D747B8  // Stereo mode
#define BUTTON_CODE_SURROUND_MODE   0x20DF47B8  // Iterate through surround mode (Dolby Pro Logic, Dolby 3 Stereo, DSP Logic, DSP Arena)
#define BUTTON_CODE_AUDIO           0x639C02B8  // Iterate through audio settings (Bass, Trebble, Sub Woofer, Balance)
#define BUTTON_CODE_SURROUND        0x629D02B8  // Iterate through surround settings (Rear, Delay, Center)
#define BUTTON_CODE_UP              0xAA5501B8  // Used to control audio/surround settings
#define BUTTON_CODE_DOWN            0xAB5401B8  // Used to control audio/surround settings

/* VARIABLES */
static char commandString[COMMAND_STRING_MAX_LENGTH];
static uint8_t commandStringLength;

/* PRIVATE FUNCTIONS */
static void ResetCommandString(void)
{
   memset(commandString, 0, sizeof(commandString[0]) * COMMAND_STRING_MAX_LENGTH);
   commandStringLength = 0;
}

static void sendNecCommand(uint32_t buttonCode, uint8_t repeats)
{
   uint16_t address = (buttonCode & ADDRESS_VALUE_MASK);
   uint8_t command = (buttonCode & COMMAND_VALUE_MASK) >> COMMAND_VALUE_OFFSET;

   IrSender.sendNEC(address, command, repeats);
   Serial.println("Command sent");
}

static void processCommandString(const char *command)
{
   if (strcmp(command, "power") == 0)
   {
      sendNecCommand(BUTTON_CODE_POWER_TOGGLE, 0);
      return;
   }

   if (strcmp(command, "vol+") == 0)
   {
      /* Must be in volume context already to change volume. Otherwise the first
         "vol+" will only put receiver in volume context. */
      sendNecCommand(BUTTON_CODE_VOLUME_UP, 0);
      return;
   }

   if (strcmp(command, "vol-") == 0)
   {
      /* Must be in volume context already to change volume. Otherwise the first
         "vol-" will only put receiver in volume context. */
      sendNecCommand(BUTTON_CODE_VOLUME_DOWN, 0);
      return;
   }

   if (strcmp(command, "mute") == 0)
   {
      sendNecCommand(BUTTON_CODE_MUTE, 0);
      return;
   }
}

/* PUBLIC FUNCTIONS */
void setup()
{
   ResetCommandString();
   IrSender.begin(IR_SEND_PIN, true, LED_BUILTIN);
   Serial.begin(115200); // opens serial port
   Serial.println("Begin IrTransmitterModuleDemo_v2");
}

void loop()
{
   /* If there are no data to read, do nothing */
   if (!Serial.available())
   {
      return;
   }

   char c = Serial.read();

   if (c == LINE_FEED_VALUE || c == CARRIAGE_RETURN_VALUE)
   {
      /* When receiving data over the serial connection, we will treat LF and CR as command termination. */
      processCommandString(commandString);
      ResetCommandString();
   }
   else if (commandStringLength > COMMAND_STRING_MAX_LENGTH)
   {
      /* Command length exceeded, start again. */
      ResetCommandString();
   }
   else
   {
      /* Carry on retrieving current command. */
      commandString[commandStringLength] = c;
      commandStringLength++;
   }
}

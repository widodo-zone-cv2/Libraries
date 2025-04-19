#include <mcp2515-SUN.h>
MCP2515 can(5);

void setup()
{
    Serial.begin(115200);
    if (can.initialize(MCP2515::OPSMOD::REQ_LOOPBACK,
                       MCP2515::IDMOD::IMOD_ALL,
                       MCP2515::SPEED::SPD_8MHz_500K))
    {
        Serial.println("MCP2515 sukses.");
    }
    can.setMaskFilt(0b111110000000)
        .filter0(0b010000000000);
}

void loop()
{
    if (can.available())
    {
        can.readData(&rxId, &len, rxBuf);

        if ((rxId & 0x80000000) == 0x80000000) // Determine if ID is standard (11 bits) or extended (29 bits)
            sprintf(msgString, "Extended ID: 0x%.8lX  DLC: %1d  Data:", (rxId & 0x1FFFFFFF), len);
        else
            sprintf(msgString, "Standard ID: 0x%.3lX       DLC: %1d  Data:", rxId, len);

        Serial.print(msgString);

        if ((rxId & 0x40000000) == 0x40000000)
        { // Determine if message is a remote request frame.
            sprintf(msgString, " REMOTE REQUEST FRAME");
            Serial.print(msgString);
        }
        else
        {
            for (byte i = 0; i < len; i++)
            {
                sprintf(msgString, " 0x%.2X", rxBuf[i]);
                Serial.print(msgString);
            }
        }

        Serial.println();
    }
    delay(100);
}

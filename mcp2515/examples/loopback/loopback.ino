#include "mcp2515.h"

MCP2515 can(5);
uint32_t rxId;
unsigned char len = 0;
unsigned char rxBuf[8];
char msgString[128];

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

int count = 0;
const unsigned long id[9] = {
    0x315, // 001100010101
    0x347, // 001101000111
    0x387, // 001110000111
    0x308, // 001100001000
    0x311, // 001100010001
    0x424, // 010000100100
    0x425, // 010000100101
    0x608, // 011000001000
    0x6F0, // 011011110000
};
byte data[9][8] = {
    {0, 1, 2, 3, 4, 5, 6, 7},        // data untuk id[0]
    {10, 11, 12, 13, 0, 15, 16, 17}, // data untuk id[1]
    {0, 0, 0, 0, 0, 0, 0, 0},        // data untuk id[2]
    {184, 31, 1, 33, 34, 35, 0, 37}, // data untuk id[3]
    {0, 0, 0, 0, 0, 0, 0, 0},        // data untuk id[4]
    {0, 0, 2, 53, 54, 55, 56, 57},   // data untuk id[5]
    {0, 0, 0, 0, 0, 0, 0, 0},        // data untuk id[6]
    {0, 0, 0, 0, 0, 0, 0, 0},        // data untuk id[7]
};

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

    //
    byte cek = can.writeData(id[count], 0, 8, data[count]);
    if (cek == MCP2515::RESPONSE::RSPN_OK)
    {
        count++;
        if (count > 8)
        {
            count = 0;
        }
    }

    delay(100);
}

// END FILE

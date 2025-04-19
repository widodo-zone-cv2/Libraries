/**
 * @file mcp2515-SUN.h
 * @brief MCP2515 library untuk Arduino dan ESP32
 * @version 1.0
 * @date 2025-04-20
 * @note Library ini digunakan untuk berkomunikasi dengan chip MCP2515 melalui SPI.
 * @note Library ini mendukung mode operasi normal, sleep, loopback, listen-only, dan konfigurasi.
 * @note Library ini juga mendukung pengaturan filter dan mask untuk pesan CAN.
 * @note Library ini menggunakan SPI untuk komunikasi dengan MCP2515.
 * @note Library ini ditulis untuk digunakan dengan Arduino dan ESP32.
 * @note Library ini ditulis oleh Sugeng Wahyu Widodo Founder dari PT SUN Corporate.
 */

#ifndef MCP2515_LIB_SUN_H
#define MCP2515_LIB_SUN_H

#include <Arduino.h>
#include <SPI.h>
#include <inttypes.h>

class MCP2515
{
public:
    enum RESPONSE
    {
        RSPN_OK = 0,
        RSPN_FAIL = 1,
        RSPN_FAILTX = 2,
        RSPN_MSGAVAIL = 3,
        RSPN_NOMSG = 4,
        RSPN_GETTXBFTIMEOUT = 6,
        RSPN_SENDMSGTIMEOUT = 7,
        RSPN_ALLTXBUSSY = 10,
    };
    enum IDMOD
    {
        IMOD_ALL = 0, // Standar dan Extended IDs
        IMOD_STD = 1, // hanya Standar IDs
        IMOD_EXT = 2, // hanya Extended IDs
        IMOD_ANY = 3, // Disable Mask dan Filter
    };
    enum OPSMOD
    {
        REQ_NORMAL = 0b00000000,     // 0x00
        REQ_SLEEP = 0b00100000,      // 0x20
        REQ_LOOPBACK = 0b01000000,   // 0x40
        REQ_LISTENONLY = 0b01100000, // 0x60
        REQ_CONFIG = 0b10000000,     // 0x80
    };
    enum SPEED
    {
        SPD_8MHz_1000K = 0b000000001000000010000000,
        SPD_8MHz_500K = 0b000000001101000110000001,
        SPD_8MHz_250K = 0b000000001011000110000101,
        SPD_8MHz_200K = 0b000000001011010010000110,
        SPD_8MHz_125K = 0b000000011011000110000101,
        SPD_8MHz_100K = 0b000000011011010010000110,
        SPD_8MHz_80K = 0b000000011011111110000111,
        SPD_8MHz_50K = 0b000000111011010010000110,
        SPD_8MHz_40K = 0b000000111011111110000111,
        SPD_8MHz_33K3 = 0b010001111110001010000101,
        SPD_8MHz_31K25 = 0b000001111010010010000100,
        SPD_8MHz_20K = 0b000001111011111110000111,
        SPD_8MHz_10K = 0b000011111011111110000111,
        SPD_8MHz_5K = 0b000111111011111110000111,

        SPD_16MHz_1000K = 0b000000001101000010000010, // 00000000 11010000 10000010
        SPD_16MHz_500K = 0b000000001111000010000110,
        SPD_16MHz_250K = 0b010000011111000110000101,
        SPD_16MHz_200K = 0b000000011111101010000111,
        SPD_16MHz_125K = 0b000000111111000010000110,
        SPD_16MHz_100K = 0b000000111111101010000111,
        SPD_16MHz_95K = 0b000000111010110100000111,
        SPD_16MHz_83K3 = 0b000000111011111000000111,
        SPD_16MHz_80K = 0b000000111111111110000111,
        SPD_16MHz_50K = 0b000001111111101010000111,
        SPD_16MHz_40K = 0b000001111111111110000111,
        SPD_16MHz_33K3 = 0b010011101111000110000101,
        SPD_16MHz_20K = 0b000011111111111110000111,
        SPD_16MHz_10K = 0b000111111111111110000111,
        SPD_16MHz_5K = 0b001111111111111110000111,

        SPD_20MHz_1000K = 0b000000001101100110000010,
        SPD_20MHz_500K = 0b000000001111101010000111,
        SPD_20MHz_250K = 0b010000011111101110000110,
        SPD_20MHz_200K = 0b000000011111111110000111,
        SPD_20MHz_125K = 0b000000111111101010000111,
        SPD_20MHz_100K = 0b000001001111101010000111,
        SPD_20MHz_83K3 = 0b000001001111111010000111,
        SPD_20MHz_80K = 0b000001001111111110000111,
        SPD_20MHz_50K = 0b000010011111101010000111,
        SPD_20MHz_40K = 0b000010011111111110000111,
        SPD_20MHz_33K3 = 0b000010111111111110000111,
    };

private:
    SPIClass *_spi;
    SPISettings _spiSettings;
    int8_t _cs; // Chip Select pin number

    byte m_nExtFlg; // Identifier Type
    uint32_t m_nID; // CAN ID
    byte m_nDlc;    // Data Length Code
    byte m_nDta[8]; // Data array
    byte m_nRtr;    // Remote request flag
    byte canError = 0;
    OPSMOD _opsModeUse = REQ_NORMAL;

    enum REGBIT
    {
        BIT_RX0IF = 0x01,
        BIT_RX1IF = 0x02,
    };
    enum TAEK
    {
        MCP_TXB_EXIDE_M = 0b00001000,
        DLC_MASK = 0x0F,
        RTR_MASK = 0x40,
    };
    enum CMDSET
    {
        CMD_RESET = 0b11000000,    // 0xC0
        CMD_READ = 0b00000011,     // 0x03
        CMD_BITMODIF = 0b00000101, // 0x05
        CMD_WRITE = 0b00000010,    // 0x02
        CMD_READ_STATUS = 0xA0,
    };
    enum MASKB
    {
        MASK_BxBFS = 0x30,
        MASK_BxBFE = 0x0C,
    };
    enum RXBRX
    {
        RXB_RX_MASK = 0x60,
        RXB_RX_ANY = 0x60,
        RXB_RX_STDEXT = 0x00,
    };
    enum TXBTX
    {
        TXB_TXREQ_M = 0x08,
    };
    enum REGCANT
    {
        CTR_CANCTRL = 0b00001111, // 0x0F
        CTR_CANSTAT = 0b00001110, // 0x0E
        CTR_CANINTE = 0b00101011, // 0x2B
        CTR_CANINTF = 0b00101100, // 0x2C
        CTR_RXM0SIDH = 0x20,
        CTR_RXM1SIDH = 0x24,
        CTR_RXF5SIDH = 0x18,
        CTR_RXF4SIDH = 0x14,
        CTR_RXF3SIDH = 0x10,
        CTR_RXF2SIDH = 0x08,
        CTR_RXF1SIDH = 0x04,
        CTR_RXF0SIDH = 0x00,
        CTR_TXB0CTRL = 0x30,
        CTR_TXB1CTRL = 0x40,
        CTR_TXB2CTRL = 0x50,
        CTR_RXB0CTRL = 0x60,
        CTR_RXB1CTRL = 0x70,
        CTR_BFPCTRL = 0x0C,
        CTR_TXRTSCTRL = 0x0D,
        CTR_RXB0SIDH = 0x61,
        CTR_RXB1SIDH = 0x71,
    };
    enum CANINTF
    {
        INTF_RX0IF = 0x01,
        INTF_RX1IF = 0x02,
        INTF_TX0IF = 0x04,
        INTF_TX1IF = 0x08,
        INTF_TX2IF = 0x10,
        INTF_ERRIF = 0x20,
        INTF_WAKIF = 0x40,
        INTF_MERRF = 0x80,
    };

    inline void __spi_unSelect() { digitalWrite(_cs, HIGH); }
    inline void __spi_select() { digitalWrite(_cs, LOW); }
    inline uint8_t __spi_readWrite(uint8_t data) { return _spi->transfer(data); }
    inline uint8_t __spi_read() { return __spi_readWrite(0x00); }

    /**
     * @brief __bitModify
     * @param address Alamat register yang akan dimodifikasi
     * @param mask Mask untuk menentukan bit yang akan diubah
     * @param data Data baru yang akan ditulis ke register
     * @note Fungsi ini digunakan untuk memodifikasi bit pada register tertentu.
     */
    inline void __bitModify(const byte address, const byte mask, const byte data)
    {
        _spi->beginTransaction(_spiSettings);
        __spi_select();
        //
        __spi_readWrite(CMD_BITMODIF); // 0x05 00000101 Perintah BIT MODIFY
        __spi_readWrite(address);      // 0x0F 00001111 Alamat register CANCTRL (0x0F)
        __spi_readWrite(mask);         // 0xE0 11100000  Mask: bit yang boleh diubah (REQOP2:0)
        __spi_readWrite(data);         // 0x80 10000000 Data baru: 0x80 → minta Configuration Mode
        //
        __spi_unSelect();
        _spi->endTransaction();
    }

    /**
     * @brief __readRegister
     * @param address Alamat register yang akan dibaca
     * @return Nilai register yang dibaca
     * @note Fungsi ini digunakan untuk membaca nilai dari register tertentu.
     */
    byte __readRegister(const byte address)
    {
        byte ret;
        _spi->beginTransaction(_spiSettings);
        __spi_select();
        __spi_readWrite(CMD_READ);
        __spi_readWrite(address);
        ret = __spi_read();
        __spi_unSelect();
        _spi->endTransaction();

        return ret;
    }

    /**
     * @brief __readRegisters
     * @param address Alamat register yang akan dibaca
     * @param values Array untuk menyimpan nilai yang dibaca
     * @param n Jumlah register yang akan dibaca
     * @return void
     * @note Fungsi ini digunakan untuk membaca beberapa register sekaligus.
     */
    void __readRegisters(const byte address, byte values[], const byte n)
    {
        byte i;
        _spi->beginTransaction(_spiSettings);
        __spi_select();
        __spi_readWrite(CMD_READ);
        __spi_readWrite(address);
        // mcp2515 has auto-increment of address-pointer
        for (i = 0; i < n; i++)
            values[i] = __spi_read();

        __spi_unSelect();
        _spi->endTransaction();
    }

    /**
     * @brief __writeRegister
     * @param address Alamat register yang akan ditulis
     * @param value Nilai yang akan ditulis ke register
     * @return void
     * @note Fungsi ini digunakan untuk menulis nilai ke register tertentu.
     */
    void __writeRegister(const byte address, const byte value)
    {
        _spi->beginTransaction(_spiSettings);
        __spi_select();
        __spi_readWrite(0x02);
        __spi_readWrite(address);
        __spi_readWrite(value);
        __spi_unSelect();
        _spi->endTransaction();
    }

    /**
     * @brief __writeRegisters
     * @param address Alamat register yang akan ditulis
     * @param values Array nilai yang akan ditulis ke register
     * @param n Jumlah nilai yang akan ditulis
     * @return void
     * @note Fungsi ini digunakan untuk menulis beberapa nilai ke register sekaligus.
     */
    void __writeRegisters(const byte address, const byte values[], const byte n)
    {
        byte i;
        _spi->beginTransaction(_spiSettings);
        __spi_select();
        __spi_readWrite(CMD_WRITE);
        __spi_readWrite(address);

        for (i = 0; i < n; i++)
            __spi_readWrite(values[i]);

        __spi_unSelect();
        _spi->endTransaction();
    }

    /**
     * @brief _toRequestMode
     * @param newMod Mode baru yang akan diminta
     * @return Kode status dari permintaan mode baru
     * @note Fungsi ini digunakan untuk meminta mode baru pada MCP2515.
     */
    byte _toRequestMode(const byte newMod)
    {
        byte startTime = millis();

        // Spam new mode request and wait for the operation  to complete
        while (1)
        {
            // Request new mode
            // This is inside the loop as sometimes requesting the new mode once doesn't work (usually when attempting to sleep)
            __bitModify(CTR_CANCTRL, 0xE0, newMod);

            byte statReg = __readRegister(CTR_CANSTAT);
            if ((statReg & 0xE0) == newMod) // We're now in the new mode
                return RSPN_OK;
            else if ((byte)(millis() - startTime) > 200) // Wait no more than 200ms for the operation to complete
                return RSPN_FAIL;
        }
    }

    /**
     * @brief _exitSleepMode
     * @param reqMode Mode baru yang akan diminta
     * @return Kode status dari permintaan mode baru
     * @note Fungsi ini digunakan untuk keluar dari mode tidur (Sleep Mode) pada MCP2515.
     */
    byte _exitSleepMode(const byte reqMode)
    {
        /**
         * periksa apakah chip sedang dalam "Sleep Mode" melalui Register CANSTAT ambil bit ke 5:7
         * dan jika mode Baru adalah Sleep Mode (0x20). maka tidak perlu mengaktifkan interrupt wake-up.
         * Jika chip dalam Sleep Mode dan mode baru bukan Sleep Mode,
         * maka periksa apakah interrupt wake-up sudah aktif atau belum.
         * Jika belum aktif, aktifkan interrupt wake-up (WAKIE) pada Register CANINTE
         * dan Register CANINTF, lalu ubah mode ke mode baru yang diinginkan.
         * Jika tidak dalam Sleep Mode, langsung ubah mode ke mode baru.
         */
        if ((__readRegister(CTR_CANSTAT) & 0xE0) == REQ_SLEEP && reqMode != REQ_SLEEP)
        {
            byte wakeIntEnabled = (__readRegister(CTR_CANINTE) & 0x40);
            if (!wakeIntEnabled)
            {
                __bitModify(CTR_CANINTE, 0x40, 0x40);
            }
            __bitModify(CTR_CANINTF, 0x40, 0x40);

            if (_toRequestMode(REQ_LISTENONLY) != RSPN_OK)
            {
                return RSPN_FAIL;
            }

            if (!wakeIntEnabled)
            {
                __bitModify(CTR_CANINTE, 0x40, 0);
            }
        }
        // Clear wake flag
        __bitModify(CTR_CANINTF, 0x40, 0);

        return RSPN_OK;
    }

    /**
     * @brief _setCANCTRL
     * @param mode Mode baru yang akan diminta
     * @return Kode status dari permintaan mode baru
     * @note Fungsi ini digunakan untuk mengatur mode CAN pada MCP2515.
     */
    byte _setCANCTRL(const byte mode)
    {
        /**
         * menangani kondisi saat MCP2515 sedang dalam Sleep Mode (0x20) dan
         * ingin dipindahkan ke mode lain (bukan Sleep).
         * Agar transisi itu berhasil, ia mengaktifkan sementara interrupt wake-up,
         * lalu masuk ke mode baru.
         */
        if (_exitSleepMode(mode) != RSPN_OK)
        {
            return RSPN_FAIL;
        }
        //
        return _toRequestMode(mode);
    }

    /**
     * @brief _writeMasknFilter
     * @param mcp_addr Alamat MCP2515
     * @param ext Flag ekstensi untuk ID
     * @param id ID yang akan ditulis
     * @note Fungsi ini digunakan untuk menulis nilai mask dan filter pada MCP2515.
     */
    void _writeMasknFilter(const byte mcp_addr, const byte ext, const uint32_t id)
    {
        uint16_t canid;
        byte tbufdata[4];

        canid = (uint16_t)(id & 0x0FFFF);

        if (ext == 1)
        {
            tbufdata[3] = (byte)(canid & 0xFF);
            tbufdata[2] = (byte)(canid >> 8);
            canid = (uint16_t)(id >> 16);
            tbufdata[1] = (byte)(canid & 0x03);
            tbufdata[1] += (byte)((canid & 0x1C) << 3);
            tbufdata[1] |= MCP_TXB_EXIDE_M;
            tbufdata[0] = (byte)(canid >> 5);
        }
        else
        {
            tbufdata[3] = (byte)(canid & 0xFF);
            tbufdata[2] = (byte)(canid >> 8);
            canid = (uint16_t)(id >> 16);
            tbufdata[1] = (byte)((canid & 0x07) << 5);
            tbufdata[0] = (byte)(canid >> 3);
        }

        __writeRegisters(mcp_addr, tbufdata, 4);
    }

    /**
     * @brief _initCANBuffers
     * @note Fungsi ini digunakan untuk menginisialisasi buffer CAN pada MCP2515.
     */
    void _initCANBuffers(void)
    {
        byte i, a1, a2, a3;

        byte std = 0;
        byte ext = 1;
        uint32_t ulMask = 0x00;
        uint32_t ulFilt = 0x00;

        // MASK
        // RXM0SIDH (0x20)	MASK untuk Filter 0 dan Filter 1
        // RXM1SIDH (0x24)	MASK untuk Filter 2 sampai Filter 5

        // 0x20 00100000 Set both masks to 0
        _writeMasknFilter(CTR_RXM0SIDH, ext, ulMask);
        // 0x24 00100100 Mask register ignores ext bit
        _writeMasknFilter(CTR_RXM1SIDH, ext, ulMask);

        /* Set all filters to 0         */
        _writeMasknFilter(CTR_RXF0SIDH, ext, ulFilt); /* RXB0: extended               */
        _writeMasknFilter(CTR_RXF1SIDH, std, ulFilt); /* RXB1: standard               */
        _writeMasknFilter(CTR_RXF2SIDH, ext, ulFilt); /* RXB2: extended               */
        _writeMasknFilter(CTR_RXF3SIDH, std, ulFilt); /* RXB3: standard               */
        _writeMasknFilter(CTR_RXF4SIDH, ext, ulFilt);
        _writeMasknFilter(CTR_RXF5SIDH, std, ulFilt);

        /* Clear, deactivate the three  */
        /* transmit buffers             */
        /* TXBnCTRL -> TXBnD7           */
        a1 = CTR_TXB0CTRL;
        a2 = CTR_TXB1CTRL;
        a3 = CTR_TXB2CTRL;
        for (i = 0; i < 14; i++)
        { /* in-buffer loop               */
            __writeRegister(a1, 0);
            __writeRegister(a2, 0);
            __writeRegister(a3, 0);
            a1++;
            a2++;
            a3++;
        }
        __writeRegister(CTR_RXB0CTRL, 0); // RXB0CTRL    0x60
        __writeRegister(CTR_RXB1CTRL, 0); // RXB1CTRL    0x70
    }

    /**
     * @brief _readReceivMsg
     * @param buffer_sidh_addr Alamat buffer yang akan dibaca
     * @return void
     * @note Fungsi ini digunakan untuk membaca pesan yang diterima dari MCP2515.
     */
    void _readReceivMsg(const byte buffer_sidh_addr)
    {
        byte mcp_addr, ctrl;

        mcp_addr = buffer_sidh_addr;

        _readIDs(mcp_addr, &m_nExtFlg, &m_nID);

        ctrl = __readRegister(mcp_addr - 1);
        m_nDlc = __readRegister(mcp_addr + 4);

        if (m_nDlc > 8)
            m_nDlc = 8;

        if (ctrl & 0x08)
            m_nRtr = 1;
        else
            m_nRtr = 0;

        m_nDlc &= DLC_MASK;
        __readRegisters(mcp_addr + 5, &(m_nDta[0]), m_nDlc);
    }

    /**
     * @brief _readStatus
     * @return Status byte dari MCP2515
     * @note Fungsi ini digunakan untuk membaca status dari MCP2515.
     */
    byte _readStatus(void)
    {
        byte i;
        _spi->beginTransaction(_spiSettings);
        __spi_select();
        __spi_readWrite(CMD_READ_STATUS);
        i = __spi_read();
        __spi_unSelect();
        _spi->endTransaction();
        return i;
    }

    /**
     * @brief _writeIDs
     * @param mcp_addr Alamat MCP2515
     * @param ext Flag ekstensi untuk ID
     * @param id ID yang akan ditulis
     * @note Fungsi ini digunakan untuk menulis ID pada MCP2515.
     */
    void _writeIDs(const byte mcp_addr, const byte ext, const uint32_t id)
    {
        uint16_t canid;
        byte tbufdata[4];
        canid = (uint16_t)(id & 0x0FFFF);
        if (ext == 1)
        {
            tbufdata[3] = (byte)(canid & 0xFF);
            tbufdata[2] = (byte)(canid >> 8);
            canid = (uint16_t)(id >> 16);
            tbufdata[1] = (byte)(canid & 0x03);
            tbufdata[1] += (byte)((canid & 0x1C) << 3);
            tbufdata[1] |= MCP_TXB_EXIDE_M;
            tbufdata[0] = (byte)(canid >> 5);
        }
        else
        {
            tbufdata[0] = (byte)(canid >> 3);
            tbufdata[1] = (byte)((canid & 0x07) << 5);
            tbufdata[3] = 0;
            tbufdata[2] = 0;
        }

        __writeRegisters(mcp_addr, tbufdata, 4);
    }

    /**
     * @brief _readIDs
     * @param mcp_addr Alamat MCP2515
     * @param ext Flag ekstensi untuk ID
     * @param id Pointer ke variabel untuk menyimpan ID
     * @note Fungsi ini digunakan untuk membaca ID dari MCP2515.
     */
    void _readIDs(const byte mcp_addr, byte *ext, uint32_t *id)
    {
        byte tbufdata[4];
        *ext = 0;
        *id = 0;
        __readRegisters(mcp_addr, tbufdata, 4);
        *id = (tbufdata[0] << 3) + (tbufdata[1] >> 5);
        if ((tbufdata[1] & MCP_TXB_EXIDE_M) == MCP_TXB_EXIDE_M)
        {
            /* extended id                  */
            *id = (*id << 2) + (tbufdata[1] & 0x03);
            *id = (*id << 8) + tbufdata[2];
            *id = (*id << 8) + tbufdata[3];
            *ext = 1;
        }
    }

    /**
     * @brief _setStdFilt
     * @param filterNumber Nomor filter (0-5)
     * @param id Nilai ID yang akan diatur
     * @note Fungsi ini digunakan untuk mengatur nilai filter pada MCP2515.
     */
    void _setStdFilt(uint8_t filterNumber, uint16_t id)
    {
        uint8_t sidh = (id >> 3) & 0xFF;
        uint8_t sidl = (id & 0x07) << 5;

        uint8_t baseAddr;

        switch (filterNumber)
        {
        case 0:
            baseAddr = 0x00;
            break; // RXF0SIDH
        case 1:
            baseAddr = 0x04;
            break; // RXF1SIDH
        case 2:
            baseAddr = 0x08;
            break;
        case 3:
            baseAddr = 0x10;
            break;
        case 4:
            baseAddr = 0x14;
            break;
        case 5:
            baseAddr = 0x18;
            break;
        default:
            return;
        }

        __writeRegister(baseAddr, sidh);
        __writeRegister(baseAddr + 1, sidl);
    }

    /**
     * @brief _setStdMaskFilt
     * @param maskNumber Nomor mask (0 atau 1)
     * @param mask Nilai mask yang akan diatur
     * @note Fungsi ini digunakan untuk mengatur nilai mask pada kedua filter RXF0 dan RXF1.
     */
    void _setStdMaskFilt(uint8_t maskNumber, uint16_t mask)
    {
        uint8_t sidh = (mask >> 3) & 0xFF;
        uint8_t sidl = (mask & 0x07) << 5;

        uint8_t baseAddr = (maskNumber == 0) ? 0x20 : 0x24; // RXM0SIDH / RXM1SIDH

        __writeRegister(baseAddr, sidh);
        __writeRegister(baseAddr + 1, sidl);
    }

    template <int Step>

    class Filter
    {
    private:
        MCP2515 &parent;

    public:
        Filter(MCP2515 &par) : parent(par) {}

        // Filter-0 => RXF0 (Register 0b00000000 – RXF0SIDH)
        template <int S = Step, typename std::enable_if<S == 0, int>::type = 0>
        Filter<1> filter0(uint16_t id)
        {
            parent._setStdFilt(0, id);
            return Filter<1>(parent);
        }

        // Filter-1 => RXF1 (Register 0b00000100 – RXF1SIDH)
        template <int S = Step, typename std::enable_if<S == 1, int>::type = 0>
        Filter<2> filter1(uint16_t id)
        {
            parent._setStdFilt(1, id);
            return Filter<2>(parent);
        }
        // Filter-2 => RXF2 (Register 0b00001000 – RXF2SIDH)
        template <int S = Step, typename std::enable_if<S == 2, int>::type = 0>
        Filter<3> filter2(uint16_t id)
        {
            parent._setStdFilt(2, id);
            return Filter<3>(parent);
        }
        // Filter-3 => RXF3 (Register 0b00010000 – RXF3SIDH)
        template <int S = Step, typename std::enable_if<S == 3, int>::type = 0>
        Filter<4> filter3(uint16_t id)
        {
            parent._setStdFilt(3, id);
            return Filter<4>(parent);
        }
        // Filter-4 => RXF4 (Register 0b00011000 – RXF4SIDH)
        template <int S = Step, typename std::enable_if<S == 4, int>::type = 0>
        Filter<5> filter4(uint16_t id)
        {
            parent._setStdFilt(4, id);
            return Filter<5>(parent);
        }

        // Filter-5 => RXF5 (Register 0b00100000 – RXF5SIDH)
        template <int S = Step, typename std::enable_if<S == 5, int>::type = 0>
        void filter5(uint16_t id)
        {
            parent._setStdFilt(5, id);
        }
    };

public:
    /**
     * @brief Set Mask Filter
     * @param mask0 Mask 0 (RXM0SIDH)
     * @param mask1 Mask 1 (RXM1SIDH)
     * @return Gunakan Filter<0> untuk melanjutkan konfigurasi filter.
     * @note Fungsi ini digunakan untuk mengatur nilai mask pada kedua filter RXF0 dan RXF1.
     */
    Filter<0> setMaskFilt(uint16_t mask0, uint16_t mask1 = 0x0000)
    {
        uint8_t result = _setCANCTRL(REQ_CONFIG);
        if (result != RSPN_OK)
        {
            Serial.println("Mode Konfigurasi Gagal dimuat!");
        }

        _setStdMaskFilt(0, mask0);
        if (mask1 == 0x0000)
        {
            _setStdMaskFilt(1, mask0);
        }
        else
        {
            _setStdMaskFilt(1, mask1);
        }
        result = _setCANCTRL(_opsModeUse);
        return Filter<0>(*this);
    }

public:
    MCP2515(uint8_t CS_PIN) : _cs(CS_PIN),
                              _spi(&SPI),
                              _spiSettings(SPISettings(10000000, MSBFIRST, SPI_MODE0)) {}

    /**
     * @brief initialize
     * @param opsMod Mode operasi yang diinginkan (normal, sleep, loopback, listen-only, atau konfigurasi).
     * @param imod Mode ID (standar, extended, atau semua).
     * @param canSpeed Kecepatan CAN yang diinginkan (1000K, 500K, 250K, dll).
     * @return 1 jika berhasil, 0 jika gagal.
     * @note Fungsi ini digunakan untuk menginisialisasi MCP2515 dengan mode operasi,
     * mode ID, dan kecepatan CAN yang diinginkan.
     */
    byte initialize(OPSMOD opsMod, IDMOD imod, SPEED canSpeed)
    {
        pinMode(_cs, OUTPUT);
        __spi_unSelect();
        _spi->begin();
        //

        uint8_t result = _setCANCTRL(REQ_CONFIG);
        if (result != RSPN_OK)
        {
            Serial.println("Mode Konfigurasi Gagal dimuat!");
        }

        /**
         * mengkonfigurasi BitRate
         * mengambil nilai enum dari parameter canSpeed, kemudian
         * memecahnya menjadi 3 byte (bagian)
         */
        byte cfg1 = (canSpeed >> 16) & 0xFF; // Ambil byte paling atas (cfg1)
        byte cfg2 = (canSpeed >> 8) & 0xFF;  // Ambil byte tengah (cfg2)
        byte cfg3 = canSpeed & 0xFF;         // Ambil byte paling bawah (cfg3)

        __writeRegister(0x2A, cfg1); // 00101010 CNF1
        __writeRegister(0x29, cfg2); // 00101001 CNF2
        __writeRegister(0x28, cfg3); // 00101000 CNF3

        if (result == RSPN_OK)
        {
            // initialize Buffers
            _initCANBuffers();
            // interrupt Mode
            __writeRegister(CTR_CANINTE, INTF_RX0IF | INTF_RX1IF);
            // Sets BF pins as GPO
            __writeRegister(CTR_BFPCTRL, MASK_BxBFS | MASK_BxBFE);
            // Sets RTS pins as GPI
            __writeRegister(CTR_TXRTSCTRL, 0x00);

            if (imod == IMOD_ANY)
            {
                __bitModify(CTR_RXB0CTRL, RXB_RX_MASK | (1 << 2), RXB_RX_ANY | (1 << 2));
                __bitModify(CTR_RXB1CTRL, RXB_RX_MASK, RXB_RX_ANY);
            }
            else if (imod == IMOD_STD || imod == IMOD_EXT || imod == IMOD_ALL)
            {
                __bitModify(CTR_RXB0CTRL, RXB_RX_MASK | (1 << 2), RXB_RX_STDEXT | (1 << 2));
                __bitModify(CTR_RXB1CTRL, RXB_RX_MASK, RXB_RX_STDEXT);
            }
            result = _setCANCTRL(opsMod);
        }
        if (result == RSPN_OK)
        {
            _opsModeUse = opsMod;
            return true;
        }
        return false;
    }

    /**
     * @brief writeData
     * @param id ID dari data yang akan dikirimkan
     * @param ext Flag ekstensi untuk ID
     * @param len Panjang data yang akan dikirimkan
     * @param buf Pointer ke buffer data yang akan dikirimkan
     * @return Kode status dari pengiriman data
     * @note Fungsi ini digunakan untuk mengirimkan data ke MCP2515.
     * */
    byte writeData(uint32_t id, byte ext, byte len, byte *buf)
    {
        if (canError)
            return 100;
        byte res;

        // START fungsi -------- setMsg ------------------
        // setMsg(id, 0, ext, len, buf);
        int i = 0;
        m_nID = id;
        m_nRtr = 0;
        m_nExtFlg = ext;
        m_nDlc = len;
        if (m_nDlc > 8)
            m_nDlc = 8;
        for (i = 0; i < m_nDlc; i++)
            m_nDta[i] = *(buf + i);
        // END fungsi ---------- setMsg ------------------

        // START fungsi ---------- sendMsg ------------------
        // res = sendMsg();
        byte res1, txbuf_n;
        uint32_t uiTimeOut, temp;

        temp = micros();
        // 24 * 4 microseconds typical
        do
        {
            // START fungsi -------- mcp2515_getNextFreeTXBuf ------------------
            // res = mcp2515_getNextFreeTXBuf(&txbuf_n); /* info = addr.*/
            byte i, ctrlval;
            byte ctrlregs[3] = {CTR_TXB0CTRL, CTR_TXB1CTRL, CTR_TXB2CTRL};
            res = RSPN_ALLTXBUSSY;
            txbuf_n = 0x00;
            for (i = 0; i < 3; i++)
            {
                ctrlval = __readRegister(ctrlregs[i]);
                if ((ctrlval & TXB_TXREQ_M) == 0)
                {
                    txbuf_n = ctrlregs[i] + 1; /* return SIDH-address of Buffer*/

                    res = RSPN_OK;
                }
            }
            // END fungsi -------- mcp2515_getNextFreeTXBuf ------------------
            uiTimeOut = micros() - temp;
        } while (res == RSPN_ALLTXBUSSY && (uiTimeOut < 2500));

        if (uiTimeOut >= 2500)
        {
            return RSPN_GETTXBFTIMEOUT;
        }
        uiTimeOut = 0;
        // START fungsi -------- mcp2515_write_canMsg ------------------
        // mcp2515_write_canMsg(txbuf_n);
        byte mcp_addr;
        mcp_addr = txbuf_n;
        __writeRegisters(mcp_addr + 5, m_nDta, m_nDlc);

        if (m_nRtr == 1)
            m_nDlc |= RTR_MASK;

        __writeRegister((mcp_addr + 4), m_nDlc);
        _writeIDs(mcp_addr, m_nExtFlg, m_nID);
        // END fungsi -------- mcp2515_write_canMsg ------------------
        __bitModify(txbuf_n - 1, TXB_TXREQ_M, TXB_TXREQ_M);

        temp = micros();
        do
        {
            res1 = __readRegister(txbuf_n - 1); /* read send buff ctrl reg 	*/
            res1 = res1 & 0x08;
            uiTimeOut = micros() - temp;
        } while (res1 && (uiTimeOut < 2500));

        if (uiTimeOut >= 2500) /* send msg timeout             */
            return RSPN_SENDMSGTIMEOUT;

        return RSPN_OK;
        // END fungsi ---------- sendMsg ------------------

        // return res;
    }

    /**
     * @brief readData
     * @param id Pointer ke ID dari data yang diterima
     * @param len Pointer ke panjang data yang diterima
     * @param buf Pointer ke buffer untuk menyimpan data yang diterima
     * @return Kode status dari pembacaan data
     * @note Fungsi ini digunakan untuk membaca data dari MCP2515.
     */
    byte readData(uint32_t *id, byte *len, byte *buf)
    {
        if (canError)
            return 100;
        // START fungsi -------- readMsg ------------------
        // if (readMsg() == 4)
        //     return 4;
        byte stat, res;

        stat = _readStatus();

        if (stat & (1 << 0)) /* Msg in Buffer 0              */
        {
            _readReceivMsg(CTR_RXB0SIDH);
            __bitModify(CTR_CANINTF, BIT_RX0IF, 0);
            res = RSPN_OK;
        }
        else if (stat & (1 << 1)) /* Msg in Buffer 1              */
        {
            _readReceivMsg(CTR_RXB1SIDH);
            __bitModify(CTR_CANINTF, BIT_RX1IF, 0);
            res = RSPN_OK;
        }
        else
            res = RSPN_NOMSG;

        if (res == RSPN_NOMSG)
        {
            return RSPN_NOMSG;
        }
        // END fungsi -------- readMsg ------------------

        if (m_nExtFlg)
            m_nID |= 0x80000000;

        if (m_nRtr)
            m_nID |= 0x40000000;

        if (m_nDlc > 8)
            m_nDlc = 8;

        *id = m_nID;
        *len = m_nDlc;

        for (int i = 0; i < m_nDlc; i++)
            buf[i] = m_nDta[i];

        return RSPN_OK;
    }

    /**
     * @brief available
     * @return 1 jika ada data yang tersedia, 0 jika tidak ada data yang tersedia
     * @note Fungsi ini digunakan untuk memeriksa apakah ada data yang tersedia untuk dibaca dari MCP2515.
     */
    bool available(void)
    {
        if (canError)
            return false;
        byte res;
        res = _readStatus(); /* RXnIF in Bit 1 and 0         */
        if (res & 0x03)
            return true;
        else
            return false;
    }
};
#endif



#include "ftdi_local.h"
#include "measure2.h"
#include "ftd2xx.h"
#include "timing.h"
#include "WinTypes.h"
#include <chrono>
#include <thread>
#include <algorithm> // for std::clamp
#include <cstdio>
#include <iostream>
#include <cstring>
#include "config.h"



using Clock = std::chrono::steady_clock;

// FIXED OLD FTDI LOCKUP
FT_HANDLE ftHandle = nullptr;
std::atomic<bool> g_ftdi_connected{false};
std::atomic<bool> g_ftdi_sync_ready{false};

std::mutex g_ftdi_write_mutex;
std::vector<uint16_t> g_ftdi_write_buffer;
std::atomic<bool> g_ftdi_write_ready{false};

bool Init_FTDI(FT_HANDLE &ftHandle)
{
    FT_STATUS ftStatus;
    UCHAR Mask = 0xFF;
    UCHAR Mode;

    DWORD RxBytes;
    DWORD TxBytes;
    DWORD EventDWord;

    ftStatus = FT_Open(0, &ftHandle);
    if (ftStatus != FT_OK)
    {
        printf("FT_Open FAILED!\n");
        return false;
    }

    Mode = 0x00; // reset
    ftStatus = FT_SetBitMode(ftHandle, Mask, Mode);

    Delay_Msec(300);

    Mode = 0x40; // Sync FIFO
    ftStatus = FT_SetBitMode(ftHandle, Mask, Mode);
    if (ftStatus != FT_OK)
    {
        printf("FT_SetBitMode FAILED!\n");
        FT_Close(ftHandle);
        return false;
    }

    FT_SetLatencyTimer(ftHandle, 2);
    FT_SetUSBParameters(ftHandle, 0x10000, 0x10000);
    FT_SetFlowControl(ftHandle, FT_FLOW_RTS_CTS, 0x0, 0x0);

    FT_Purge(ftHandle, FT_PURGE_RX);

    ftStatus = FT_GetStatus(ftHandle, &RxBytes, &TxBytes, &EventDWord);
    if (ftStatus != FT_OK)
    {
        printf("FT_GetStatus FAILED!\n");
        FT_Close(ftHandle);
        return false;
    }

    return true;
}

bool Check_FT_For_Read(FT_HANDLE ftHandle)
{
    static char RxBuffer[8192];

    DWORD RxBytes = 0;
    DWORD TxBytes = 0;
    DWORD BytesReceived = 0;
    DWORD EventDWord = 0;

    FT_STATUS ftStatus =
        FT_GetStatus(ftHandle, &RxBytes, &TxBytes, &EventDWord);

    if (ftStatus != FT_OK)
    {
        std::cout << "FTDI status failed - disconnected\n";
        g_ftdi_connected = false;
        ::ftHandle = nullptr;
        return false;
    }

    if (RxBytes >= 64)
    {
        DWORD bytes_to_read = std::min<DWORD>(RxBytes, 8192);

        ftStatus = FT_Read(ftHandle, RxBuffer, bytes_to_read, &BytesReceived);

        if (ftStatus != FT_OK)
        {
            std::cout << "FTDI read failed - disconnected\n";
            g_ftdi_connected = false;
            ::ftHandle = nullptr;
            return false;
        }

        if (RxBytes != 64)
            std::cout << "long RxBytes!!!!!!!!!!!!!!! " << RxBytes << std::endl;

        return true;
    }

    return false;
}

bool FTDI_Write_Buffer(
    FT_HANDLE ftHandle,
    uint16_t *buffer,
    DWORD buffer_size_bytes)
{
    FT_STATUS ftStatus = FT_OK;
    DWORD BytesWritten = 0;

    ftStatus = FT_Write(ftHandle, buffer, buffer_size_bytes, &BytesWritten);

    if ((ftStatus != FT_OK) ||
        (BytesWritten != buffer_size_bytes))
    {
        std::printf("FT_Write FAILED\n");
        return false;
    }

    return true;
}

// FIXED OLD FTDI LOCKUP
// FIXED OLD FTDI LOCKUP
void FTDI_Thread()
{
    g_ftdi_write_buffer.resize(SCULPTURE_SEND_SIZE_RGBW_BYTES / sizeof(uint16_t));

    while (g_running)
    {
        if (!g_ftdi_connected)
        {
            FT_HANDLE newHandle = nullptr;

            if (Init_FTDI(newHandle))
            {
                ftHandle = newHandle;
                g_ftdi_connected = true;
            }
            else
            {
                Delay_Msec(1000);
            }
        }
        else
        {
            // Read sync from hardware
            if (Check_FT_For_Read(ftHandle))
                g_ftdi_sync_ready = true;

            // Write latest sculpture buffer
            if (g_ftdi_write_ready)
            {
                std::lock_guard<std::mutex> lock(g_ftdi_write_mutex);

                if (!g_ftdi_write_buffer.empty())
                {
                    bool ok = FTDI_Write_Buffer(
                        ftHandle,
                        g_ftdi_write_buffer.data(),
                        g_ftdi_write_buffer.size() * sizeof(uint16_t));

                    if (!ok)
                    {
                        FT_Close(ftHandle);
                        ftHandle = nullptr;

                        g_ftdi_connected = false;
                        g_ftdi_sync_ready = false;
                    }
                }

                g_ftdi_write_ready = false;
            }

            Delay_Msec(1);
        }
    }
}

void Queue_FTDI_Write(const uint16_t *buffer, DWORD buffer_size_bytes)
{
    std::lock_guard<std::mutex> lock(g_ftdi_write_mutex);

    int count = buffer_size_bytes / sizeof(uint16_t);

    // g_ftdi_write_buffer.assign(buffer, buffer + count);
    memcpy(g_ftdi_write_buffer.data(), buffer, buffer_size_bytes);

    g_ftdi_write_ready = true;
}


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

using Clock = std::chrono::steady_clock;

// FIXED OLD FTDI LOCKUP
FT_HANDLE ftHandle = nullptr;
std::atomic<bool> g_ftdi_connected{false};
std::atomic<bool> g_ftdi_sync_ready{false};

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

    Delay_Msec(1000);

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
void FTDI_Thread()
{
    while (g_running)
    {
        if (!g_ftdi_connected)
        {
            FT_HANDLE newHandle = nullptr;

            if (Init_FTDI(newHandle))
            {
                ftHandle = newHandle;
                g_ftdi_connected = true;
                std::cout << "FTDI connected\n";
            }
            else
            {
                Delay_Msec(1000);
            }
        }
        else
        {
            if (Check_FT_For_Read(ftHandle))
                g_ftdi_sync_ready = true;

            Delay_Msec(1);
        }
    }
}

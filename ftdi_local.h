#pragma once

#include "ftd2xx.h"
#include "WinTypes.h"
#include <cstdint>
#include <atomic>
#include "globals.h"

 // FIXED OLD FTDI LOCKUP
extern FT_HANDLE ftHandle;
extern std::atomic<bool> g_ftdi_connected;
void FTDI_Thread();
 // FIXED OLD FTDI LOCKUP

bool Init_FTDI(FT_HANDLE &ftHandle);

bool Check_FT_For_Read(FT_HANDLE ftHandle);

bool FTDI_Write_Buffer(
    FT_HANDLE ftHandle,
    uint16_t *buffer,
    DWORD buffer_size_bytes);



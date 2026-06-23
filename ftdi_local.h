#pragma once

#include "ftd2xx.h"
#include "WinTypes.h"
#include <cstdint>



bool Init_FTDI(FT_HANDLE &ftHandle);

bool Check_FT_For_Read(FT_HANDLE ftHandle);

bool FTDI_Write_Buffer(
    FT_HANDLE ftHandle,
    uint16_t *buffer,
    DWORD buffer_size_bytes);

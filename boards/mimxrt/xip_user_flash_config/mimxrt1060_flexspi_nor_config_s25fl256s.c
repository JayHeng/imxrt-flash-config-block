/*
 * Copyright 2021 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "evkcmimxrt1060_flexspi_nor_config.h"

/* Component ID definition, used by tools. */
#ifndef FSL_COMPONENT_ID
#define FSL_COMPONENT_ID "platform.drivers.xip_board"
#endif

/*******************************************************************************
 * Code
 ******************************************************************************/
#if defined(XIP_BOOT_HEADER_ENABLE) && (XIP_BOOT_HEADER_ENABLE == 1)
#if defined(__CC_ARM) || defined(__ARMCC_VERSION) || defined(__GNUC__)
__attribute__((section(".boot_hdr.conf"), used))
#elif defined(__ICCARM__)
#pragma location = ".boot_hdr.conf"
#endif

#define S25FL256S_XIP_MODE_1BIT_30MHz_SDR  (0)
#define S25FL256S_XIP_MODE_4BIT_80MHz_SDR  (0)
#define S25FL256S_XIP_MODE_4BIT_100MHz_SDR (0)
#define S25FL256S_XIP_MODE_4BIT_60MHz_DDR  (1)

#if S25FL256S_XIP_MODE_4BIT_80MHz_SDR
#define FLASH_DUMMY_CYCLES 0x06
#define FLASH_DUMMY_QE_VALUE  0x0200
#elif S25FL256S_XIP_MODE_4BIT_100MHz_SDR
#define FLASH_DUMMY_CYCLES 0x07
#define FLASH_DUMMY_QE_VALUE  0x8200
#elif S25FL256S_XIP_MODE_4BIT_60MHz_DDR
#define FLASH_DUMMY_CYCLES 0x0E
#define FLASH_DUMMY_QE_VALUE  0x0200
#endif

const flexspi_nor_config_t qspiflash_config = {
    .memConfig =
        {
            .tag              = FLEXSPI_CFG_BLK_TAG,
            .version          = FLEXSPI_CFG_BLK_VERSION,
#if S25FL256S_XIP_MODE_1BIT_30MHz_SDR
            .readSampleClkSrc = kFlexSPIReadSampleClk_LoopbackInternally,
#elif S25FL256S_XIP_MODE_4BIT_80MHz_SDR | S25FL256S_XIP_MODE_4BIT_100MHz_SDR | S25FL256S_XIP_MODE_4BIT_60MHz_DDR
            .readSampleClkSrc = kFlexSPIReadSampleClk_LoopbackFromDqsPad,
#endif
            .csHoldTime       = 3u,
            .csSetupTime      = 3u,
            .deviceType           = kFlexSpiDeviceType_SerialNOR,
#if S25FL256S_XIP_MODE_1BIT_30MHz_SDR
            // Safe configuration
            .controllerMiscOption = (1u << kFlexSpiMiscOffset_SafeConfigFreqEnable),
            .sflashPadType        = kSerialFlash_1Pad,
            .serialClkFreq        = kFlexSpiSerialClk_30MHz,
#elif S25FL256S_XIP_MODE_4BIT_80MHz_SDR
            // Safe configuration
            .controllerMiscOption = (1u << kFlexSpiMiscOffset_SafeConfigFreqEnable),
            .sflashPadType        = kSerialFlash_4Pads,
            .serialClkFreq        = kFlexSpiSerialClk_80MHz,
#elif S25FL256S_XIP_MODE_4BIT_100MHz_SDR
            // Safe configuration
            .controllerMiscOption = (1u << kFlexSpiMiscOffset_SafeConfigFreqEnable),
            .sflashPadType        = kSerialFlash_4Pads,
            .serialClkFreq        = kFlexSpiSerialClk_100MHz,
#elif S25FL256S_XIP_MODE_4BIT_60MHz_DDR
            // Enable DDR mode, Safe configuration
            .controllerMiscOption = (1u << kFlexSpiMiscOffset_DdrModeEnable) | (1u << kFlexSpiMiscOffset_SafeConfigFreqEnable),
            .sflashPadType        = kSerialFlash_4Pads,
            .serialClkFreq        = kFlexSpiSerialClk_60MHz,
#endif
            .sflashA1Size         = 256u * 1024u * 1024u,
            /* Enable flash configuration feature */
            .configCmdEnable   = 1u,
#if S25FL256S_XIP_MODE_4BIT_80MHz_SDR | S25FL256S_XIP_MODE_4BIT_100MHz_SDR | S25FL256S_XIP_MODE_4BIT_60MHz_DDR
            // Write Status Register-1, Configuration Register-1 to set dummy cycles and QE
            .configModeType[0] = kDeviceConfigCmdType_Generic,
            .configCmdSeqs[0] =
                {
                    .seqNum   = 1,
                    .seqId    = 12,
                    .reserved = 0,
                },
            .configCmdArgs[0] = FLASH_DUMMY_QE_VALUE,
#endif
            .lookupTable =
                {
#if S25FL256S_XIP_MODE_1BIT_30MHz_SDR
                    // Read LUTs
                    [0] = FLEXSPI_LUT_SEQ(CMD_SDR, FLEXSPI_1PAD, 0x13, RADDR_SDR, FLEXSPI_1PAD, 0x20),
                    [1] = FLEXSPI_LUT_SEQ(READ_SDR, FLEXSPI_1PAD, 0x04, STOP, FLEXSPI_1PAD, 0x0),
#elif S25FL256S_XIP_MODE_4BIT_80MHz_SDR | S25FL256S_XIP_MODE_4BIT_100MHz_SDR
                    // Read LUTs
                    [0] = FLEXSPI_LUT_SEQ(CMD_SDR, FLEXSPI_1PAD, 0xEC, RADDR_SDR, FLEXSPI_4PAD, 0x20),
                    [1] = FLEXSPI_LUT_SEQ(DUMMY_SDR, FLEXSPI_4PAD, FLASH_DUMMY_CYCLES, READ_SDR, FLEXSPI_4PAD, 0x04),
#elif S25FL256S_XIP_MODE_4BIT_60MHz_DDR
                    // Read LUTs
                    [0] = FLEXSPI_LUT_SEQ(CMD_SDR, FLEXSPI_1PAD, 0xEE, RADDR_DDR, FLEXSPI_4PAD, 0x20),
                    [1] = FLEXSPI_LUT_SEQ(DUMMY_DDR, FLEXSPI_4PAD, FLASH_DUMMY_CYCLES, READ_DDR, FLEXSPI_4PAD, 0x04),
#endif
                    // Read Status register-1 LUTs
                    [4 * 1 + 0] = FLEXSPI_LUT_SEQ(CMD_SDR, FLEXSPI_1PAD, 0x05, READ_SDR, FLEXSPI_1PAD, 0x04),

                    // Write Enable LUTs
                    [4 * 3 + 0] = FLEXSPI_LUT_SEQ(CMD_SDR, FLEXSPI_1PAD, 0x06, STOP, FLEXSPI_1PAD, 0x0),

                    // Erase Sector LUTs
                    [4 * 5 + 0] = FLEXSPI_LUT_SEQ(CMD_SDR, FLEXSPI_1PAD, 0x21, RADDR_SDR, FLEXSPI_1PAD, 0x20),

                    // 64KB Erase Block LUTs
                    [4 * 8 + 0] = FLEXSPI_LUT_SEQ(CMD_SDR, FLEXSPI_1PAD, 0xDC, RADDR_SDR, FLEXSPI_1PAD, 0x20),

                    // Pape Program LUTs
                    [4 * 9 + 0] = FLEXSPI_LUT_SEQ(CMD_SDR, FLEXSPI_1PAD, 0x12, RADDR_SDR, FLEXSPI_1PAD, 0x20),
                    [4 * 9 + 1] = FLEXSPI_LUT_SEQ(WRITE_SDR, FLEXSPI_1PAD, 0x04, STOP, FLEXSPI_1PAD, 0x0),

                    // Erase Chip LUTs
                    [4 * 11 + 0] = FLEXSPI_LUT_SEQ(CMD_SDR, FLEXSPI_1PAD, 0x60, STOP, FLEXSPI_1PAD, 0x0),

                    // WRR LUTs
                    [4 * 12 + 0] = FLEXSPI_LUT_SEQ(CMD_SDR, FLEXSPI_1PAD, 0x01, WRITE_SDR, FLEXSPI_1PAD, 0x02),
                    [4 * 12 + 1] = FLEXSPI_LUT_SEQ(STOP, FLEXSPI_1PAD, 0x00, 0, 0, 0),
                },
        },
    .pageSize           = 256u,
    .sectorSize         = 4u * 1024u,
    .ipcmdSerialClkFreq = 1u,
    .blockSize          = 64u * 1024u,
    .isUniformBlockSize = false,
};
#endif /* XIP_BOOT_HEADER_ENABLE */

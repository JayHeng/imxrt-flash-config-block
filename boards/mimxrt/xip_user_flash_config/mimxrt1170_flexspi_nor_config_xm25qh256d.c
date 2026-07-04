/*
 * Copyright 2018-2022 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "evkbmimxrt1170_flexspi_nor_config.h"

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

#define XM25QH256D_XIP_MODE_1BIT_30MHz  (1)
#define XM25QH256D_XIP_MODE_4BIT_60MHz  (0)
#define XM25QH256D_XIP_MODE_4BIT_133MHz (0)

#if XM25QH256D_XIP_MODE_4BIT_60MHz
#define FLASH_DUMMY_CYCLES 0x04
#define FLASH_DUMMY_VALUE  0x01
#elif XM25QH256D_XIP_MODE_4BIT_133MHz
#define FLASH_DUMMY_CYCLES 0x08
#define FLASH_DUMMY_VALUE  0x02
#endif

const flexspi_nor_config_t qspiflash_config = {
    .memConfig =
        {
            .tag              = FLEXSPI_CFG_BLK_TAG,
            .version          = FLEXSPI_CFG_BLK_VERSION,
#if XM25QH256D_XIP_MODE_1BIT_30MHz | XM25QH256D_XIP_MODE_4BIT_60MHz
            .readSampleClkSrc = kFlexSPIReadSampleClk_LoopbackInternally,
#elif XM25QH256D_XIP_MODE_4BIT_133MHz
            .readSampleClkSrc = kFlexSPIReadSampleClk_LoopbackFromDqsPad,
#endif
            .csHoldTime       = 3u,
            .csSetupTime      = 3u,
            // Enable DDR mode, Wordaddassable, Safe configuration, Differential clock
            .controllerMiscOption = 0x10,
            .deviceType           = kFlexSpiDeviceType_SerialNOR,
#if XM25QH256D_XIP_MODE_1BIT_30MHz
            .sflashPadType        = kSerialFlash_1Pad,
            .serialClkFreq        = kFlexSpiSerialClk_30MHz,
#elif XM25QH256D_XIP_MODE_4BIT_60MHz
            .sflashPadType        = kSerialFlash_4Pads,
            .serialClkFreq        = kFlexSpiSerialClk_60MHz,
#elif XM25QH256D_XIP_MODE_4BIT_133MHz
            .sflashPadType        = kSerialFlash_4Pads,
            .serialClkFreq        = kFlexSpiSerialClk_133MHz,
#endif
            .sflashA1Size         = 64u * 1024u * 1024u,
            /* Enable flash configuration feature */
            .configCmdEnable   = 1u,
#if XM25QH256D_XIP_MODE_4BIT_60MHz | XM25QH256D_XIP_MODE_4BIT_133MHz
            // Write Status Register-3 to set dummy cycles
            .configModeType[0] = kDeviceConfigCmdType_Generic,
            .configCmdSeqs[0] =
                {
                    .seqNum   = 1,
                    .seqId    = 12,
                    .reserved = 0,
                },
            .configCmdArgs[0] = (FLASH_DUMMY_VALUE << 3) | 0x20,
#endif
            // Write 0x02 into Status Register-2 to enable QE mode
            .configModeType[1] = kDeviceConfigCmdType_Generic,
            .configCmdSeqs[1] =
                {
                    .seqNum   = 1,
                    .seqId    = 13,
                    .reserved = 0,
                },
            .configCmdArgs[1] = 0x02,
            .lookupTable =
                {
#if XM25QH256D_XIP_MODE_1BIT_30MHz
                    // Read LUTs
                    //[0] = FLEXSPI_LUT_SEQ(CMD_SDR, FLEXSPI_1PAD, 0x03, RADDR_SDR, FLEXSPI_1PAD, 0x18),
                    [0] = FLEXSPI_LUT_SEQ(CMD_SDR, FLEXSPI_1PAD, 0x13, RADDR_SDR, FLEXSPI_1PAD, 0x20),
                    [1] = FLEXSPI_LUT_SEQ(READ_SDR, FLEXSPI_1PAD, 0x04, STOP, FLEXSPI_1PAD, 0x0),
#elif XM25QH256D_XIP_MODE_4BIT_60MHz | XM25QH256D_XIP_MODE_4BIT_133MHz
                    // Read LUTs
                    //[0] = FLEXSPI_LUT_SEQ(CMD_SDR, FLEXSPI_1PAD, 0xEB, RADDR_SDR, FLEXSPI_4PAD, 0x18),
                    [0] = FLEXSPI_LUT_SEQ(CMD_SDR, FLEXSPI_1PAD, 0xEC, RADDR_SDR, FLEXSPI_4PAD, 0x20),
                    [1] = FLEXSPI_LUT_SEQ(DUMMY_SDR, FLEXSPI_4PAD, FLASH_DUMMY_CYCLES, READ_SDR, FLEXSPI_4PAD, 0x04),
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

                    // Write Status Register-3 LUTs
                    [4 * 12 + 0] = FLEXSPI_LUT_SEQ(CMD_SDR, FLEXSPI_1PAD, 0x11, WRITE_SDR, FLEXSPI_1PAD, 0x01),
                    [4 * 12 + 1] = FLEXSPI_LUT_SEQ(STOP, FLEXSPI_1PAD, 0x00, 0, 0, 0),

                    // Write Status Register-2 LUTs
                    [4 * 13 + 0] = FLEXSPI_LUT_SEQ(CMD_SDR, FLEXSPI_1PAD, 0x31, WRITE_SDR, FLEXSPI_1PAD, 0x01),
                    [4 * 13 + 1] = FLEXSPI_LUT_SEQ(STOP, FLEXSPI_1PAD, 0x00, 0, 0, 0),
                },
        },
    .pageSize           = 256u,
    .sectorSize         = 4u * 1024u,
    .ipcmdSerialClkFreq = 0x1,
    .blockSize          = 64u * 1024u,
    .isUniformBlockSize = false,
};
#endif /* XIP_BOOT_HEADER_ENABLE */

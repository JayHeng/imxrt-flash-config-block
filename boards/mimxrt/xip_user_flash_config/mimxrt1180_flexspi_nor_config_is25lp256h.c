/*
 * Copyright 2018-2024 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "evkmimxrt1180_flexspi_nor_config.h"

/* Component ID definition, used by tools. */
#ifndef FSL_COMPONENT_ID
#define FSL_COMPONENT_ID "platform.drivers.xip_board"
#endif

/* clang-format off */
#if defined(XIP_EXTERNAL_FLASH) && (XIP_EXTERNAL_FLASH == 1) && \
    defined(XIP_BOOT_HEADER_ENABLE) && (XIP_BOOT_HEADER_ENABLE == 1)
/* clang-format on */

#if defined(USE_HYPERRAM)

#if defined(__CC_ARM) || defined(__ARMCC_VERSION) || defined(__GNUC__)
__attribute__((section(".boot_hdr.xmcd_data"), used))
#elif defined(__ICCARM__)
#pragma location = ".boot_hdr.xmcd_data"
#endif

const uint32_t xmcd_data[] = {
    0xC002000C, /* FlexSPI instance 2 */
    0xC1000800, /* Option words = 2 */
    0x00010000  /* PINMUX Secondary group */
};

#endif

#if defined(USE_SDRAM)

#if defined(__CC_ARM) || defined(__ARMCC_VERSION) || defined(__GNUC__)
__attribute__((section(".boot_hdr.xmcd_data"), used))
#elif defined(__ICCARM__)
#pragma location = ".boot_hdr.xmcd_data"
#endif

const uint32_t xmcd_data[] = {
    0xC010000D, /* SEMC -> SDRAM */
    0xA60001A1, /* SDRAM config */
    0x00008000, /* SDRAM config */
    0X00000001  /* SDRAM config */
};

#endif

#if defined(__CC_ARM) || defined(__ARMCC_VERSION) || defined(__GNUC__)
__attribute__((section(".boot_hdr.conf"), used))
#elif defined(__ICCARM__)
#pragma location = ".boot_hdr.conf"
#endif

/*
 * FlexSPI nor flash configuration block
 * Note:
 *    Below setting is special for EVK board flash, to achieve maximum access performance.
 *    For other boards or flash, may leave it 0 or delete fdcb_data, which means auto probe.
 */

#define IS25LP256H_XIP_MODE_4BIT_80MHz  (0)
#define IS25LP256H_XIP_MODE_4BIT_133MHz (1)

#if IS25LP256H_XIP_MODE_4BIT_80MHz
#define FLASH_DUMMY_CYCLES 0x06
#elif IS25LP256H_XIP_MODE_4BIT_133MHz
#define FLASH_DUMMY_CYCLES 0x0b
#define FLASH_DUMMY_VALUE  0x0b
#endif

const flexspi_nor_config_t qspi_flash_nor_config = {
    .memConfig =
        {
            .tag              = FLEXSPI_CFG_BLK_TAG,
            .version          = FLEXSPI_CFG_BLK_VERSION,
            .readSampleClkSrc = kFlexSPIReadSampleClk_LoopbackFromDqsPad,
            .csHoldTime       = 3u,
            .csSetupTime      = 3u,
            // Enable DDR mode, Wordaddassable, Safe configuration, Differential clock
            .controllerMiscOption = 0x10,
            .deviceType           = kFlexSpiDeviceType_SerialNOR,
            .sflashPadType        = kSerialFlash_4Pads,
#if IS25LP256H_XIP_MODE_4BIT_80MHz
            .serialClkFreq        = kFlexSpiSerialClk_80MHz,
#elif IS25LP256H_XIP_MODE_4BIT_133MHz
            .serialClkFreq        = kFlexSpiSerialClk_133MHz,
#endif
            .sflashA1Size         = 32u * 1024u * 1024u,
            /* Enable flash configuration feature */
            .configCmdEnable   = 1u,
#if IS25LP256H_XIP_MODE_4BIT_133MHz
            // Write Read-Register to set dummy cycles
            .configModeType[0] = kDeviceConfigCmdType_Generic,
            .configCmdSeqs[0] =
                {
                    .seqNum   = 1,
                    .seqId    = 12,
                    .reserved = 0,
                },
             .configCmdArgs[0] = (FLASH_DUMMY_VALUE << 3),
#endif
            // Write 0x40 into Status-Register to enable QE mode
            .configModeType[1] = kDeviceConfigCmdType_Generic,
            .configCmdSeqs[1] =
                {
                    .seqNum   = 1,
                    .seqId    = 13,
                    .reserved = 0,
                },
            .configCmdArgs[1] = 0x40,
            .lookupTable =
                {
                    // Read LUTs
                    [0] = FLEXSPI_LUT_SEQ(CMD_SDR, FLEXSPI_1PAD, 0xEC, RADDR_SDR, FLEXSPI_4PAD, 0x20),
                    [1] = FLEXSPI_LUT_SEQ(DUMMY_SDR, FLEXSPI_4PAD, FLASH_DUMMY_CYCLES, READ_SDR, FLEXSPI_4PAD, 0x04),

                    // Read Status LUTs
                    [4 * 1 + 0] = FLEXSPI_LUT_SEQ(CMD_SDR, FLEXSPI_1PAD, 0x05, READ_SDR, FLEXSPI_1PAD, 0x04),

                    // Write Enable LUTs
                    [4 * 3 + 0] = FLEXSPI_LUT_SEQ(CMD_SDR, FLEXSPI_1PAD, 0x06, STOP, FLEXSPI_1PAD, 0x0),

                    // Erase Sector LUTs
                    [4 * 5 + 0] = FLEXSPI_LUT_SEQ(CMD_SDR, FLEXSPI_1PAD, 0x21, RADDR_SDR, FLEXSPI_1PAD, 0x20),

                    // Erase Block LUTs
                    [4 * 8 + 0] = FLEXSPI_LUT_SEQ(CMD_SDR, FLEXSPI_1PAD, 0xDC, RADDR_SDR, FLEXSPI_1PAD, 0x20),

                    // Pape Program LUTs
                    [4 * 9 + 0] = FLEXSPI_LUT_SEQ(CMD_SDR, FLEXSPI_1PAD, 0x12, RADDR_SDR, FLEXSPI_1PAD, 0x20),
                    [4 * 9 + 1] = FLEXSPI_LUT_SEQ(WRITE_SDR, FLEXSPI_1PAD, 0x04, STOP, FLEXSPI_1PAD, 0x0),

                    // Erase Chip LUTs
                    [4 * 11 + 0] = FLEXSPI_LUT_SEQ(CMD_SDR, FLEXSPI_1PAD, 0x60, STOP, FLEXSPI_1PAD, 0x0),

                    // Write Read-Register LUTs
                    [4 * 12 + 0] = FLEXSPI_LUT_SEQ(CMD_SDR, FLEXSPI_1PAD, 0xC0, WRITE_SDR, FLEXSPI_1PAD, 0x01),
                    [4 * 12 + 1] = FLEXSPI_LUT_SEQ(STOP, FLEXSPI_1PAD, 0x00, 0, 0, 0),

                    // Write Status-Register LUTs
                    [4 * 13 + 0] = FLEXSPI_LUT_SEQ(CMD_SDR, FLEXSPI_1PAD, 0x01, WRITE_SDR, FLEXSPI_1PAD, 0x01),
                    [4 * 13 + 1] = FLEXSPI_LUT_SEQ(STOP, FLEXSPI_1PAD, 0x00, 0, 0, 0),
                },
        },
    .pageSize           = 256u,
    .sectorSize         = 4u * 1024u,
    .ipcmdSerialClkFreq = 0x1,
    .blockSize          = 64u * 1024u,
    .isUniformBlockSize = false,
};
/* clang-format on */

#endif

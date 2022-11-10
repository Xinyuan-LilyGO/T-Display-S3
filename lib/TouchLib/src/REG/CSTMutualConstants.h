#pragma once

#define CTS328_SLAVE_ADDRESS (0x1A)
#define L58_SLAVE_ADDRESS    (0X5A)

/* Version Information Register */
/* MODE_DEBUG_INFO  */

/*
 BYTE 3:KEY_NUM
 BYTE 2:TP_NRX
 BYTE 1:NC
 BYTE 0:TP_NTX
*/
#define CST328_INFO_1_REG    (uint16_t((0XD1F4))

/*
 BYTE 3 ~ BYTE 2:TP_RESY
 BYTE 1 ~ BYTE 0:TP_RESX
 */
#define CST328_INFO_2_REG    (uint16_t((0XD1F8))

/*
 BYTE 3 ~ BYTE 2:0XCACA
 BYTE 1 ~ BYTE 0:BOOT_TIMER
 */
#define CST328_INFO_3_REG    (uint16_t((0XD1FC))

/*
 BYTE 3 ~ BYTE 2:IC_TYPE
 BYTE 1 ~ BYTE 0:PROJECT_ID
 */
#define CST328_INFO_4_REG    (uint16_t((0XD204))

/*
 BYTE 3:FW_MAJOR
 BYTE 2:FW_MINOR
 BYTE 1 ~ BYTE 0:FW_BUILD
*/
#define CST328_INFO_5_REG    (uint16_t((0XD208))

/*
 BYTE 3:CHECKSNM_H
 BYTE 2:CHECKSNM_H
 BYTE 1:CHECKSNM_L
 BYTE 0:CHECKSNM_L
*/
#define CST328_INFO_6_REG    (uint16_t((0XD20C))

#define MODE_DEBUG_INFO_REG        (uint16_t(0xD101))
#define CHIP_SYSTEM_RESET_REG      (uint16_t(0xD102))
#define REDO_CALIBRATION_REG       (uint16_t(0xD104))
#define CHIP_DEEP_SLEEP_REG        (uint16_t(0xD105))
#define MODE_DEBUG_POINT_REG       (uint16_t(0xD108))
#define MODE_NORMAL_REG            (uint16_t(0xD109))
#define MODE_DEBUG_RAWDATA_REG     (uint16_t(0xD10A))
#define MODE_DEBUG_WRITE_REG       (uint16_t(0xD10B))
#define MODE_DEBUG_CALIBRATION_REG (uint16_t(0xD10C))
#define MODE_DEBUG_DIFF_REG        (uint16_t(0xD10D))
#define MODE_FACTORY_REG           (uint16_t(0xD119))

/* touch information register */
/*  MODE_NORMAL  */

/*
    BIT 7 ~ BIT 4: 1st finger ID
    BIT 3 ~ BIT 0: 1st finger state: pressed (0x06) or lifted
 */
#define MODE_NORMAL_0_REG          (uint16_t(0xD000))

/*
    BIT 7 ~ BIT 0: The X coordinate value of the 1st finger is eight high digits: X_Position>>4
 */
#define MODE_NORMAL_1_REG          (uint16_t(0xD001))

/*
    BIT 7 ~ BIT 0: The Y coordinate value of the 1st finger is eight high digits: Y_Position>>4
 */
#define MODE_NORMAL_2_REG          (uint16_t(0xD002))

/*
    BIT 7 ~ BIT 4: The X coordinate value of the 1st finger X_Position&0x0F
    BIT 3 ~ BIT 0: The Y coordinate value of the 1st finger Y_Position&0x0F
 */
#define MODE_NORMAL_3_REG          (uint16_t(0xD003))

/*
    BIT 7 ~ BIT 0: 1st finger pressure value
 */
#define MODE_NORMAL_4_REG          (uint16_t(0xD004))

/*
    BIT 7 ~ BIT 4: Report button flag (0x80)
    BIT 3 ~ BIT 0: Report the number of fingers
 */
#define MODE_NORMAL_5_REG          (uint16_t(0xD005))

/*
    BIT 7 ~ BIT 0: Fixed 0xAB
 */
#define MODE_NORMAL_6_REG          (uint16_t(0xD006))

/*
    BIT 7 ~ BIT 4: 2nd finger ID
    BIT 3 ~ BIT 0: 2nd finger state: pressed (0x06) or lifted
 */
#define MODE_NORMAL_7_REG          (uint16_t(0xD007))

/*
    BIT 7 ~ BIT 0: The X coordinate value of the 2nd finger is eight high digits: X_Position>>4
 */
#define MODE_NORMAL_8_REG          (uint16_t(0xD008))

/*
    BIT 7 ~ BIT 0: The Y coordinate value of the 2nd finger is eight high digits: Y_Position>>4
 */
#define MODE_NORMAL_9_REG          (uint16_t(0xD009))

/*
    BIT 7 ~ BIT 4: The X coordinate value of the 2nd finger X_Position&0x0F
    BIT 3 ~ BIT 0: The Y coordinate value of the 2nd finger Y_Position&0x0F
 */
#define MODE_NORMAL_10_REG         (uint16_t(0xD00A))

/*
    BIT 7 ~ BIT 0: 2nd finger pressure value
 */
#define MODE_NORMAL_11_REG         (uint16_t(0xD00B))

/*
    BIT 7 ~ BIT 4: 3rd finger ID
    BIT 3 ~ BIT 0: 3rd finger state: pressed (0x06) or lifted
 */
#define MODE_NORMAL_12_REG         (uint16_t(0xD00C))

/*
    BIT 7 ~ BIT 0: The X coordinate value of the 3rd finger is eight high digits: X_Position>>4
 */
#define MODE_NORMAL_13_REG         (uint16_t(0xD00D))

/*
    BIT 7 ~ BIT 0: The Y coordinate value of the 3rd finger is eight high digits: Y_Position>>4
 */
#define MODE_NORMAL_14_REG         (uint16_t(0xD00E))

/*
    BIT 7 ~ BIT 4: The X coordinate value of the 3rd finger X_Position&0x0F
    BIT 3 ~ BIT 0: The Y coordinate value of the 3rd finger Y_Position&0x0F
 */
#define MODE_NORMAL_15_REG         (uint16_t(0xD00F))

/*
    BIT 7 ~ BIT 0: 3rd finger pressure value
 */
#define MODE_NORMAL_16_REG         (uint16_t(0xD010))

/*
    BIT 7 ~ BIT 4: 4th finger ID
    BIT 3 ~ BIT 0: 4th finger state: pressed (0x06) or lifted
 */
#define MODE_NORMAL_17_REG         (uint16_t(0xD011))

/*
    BIT 7 ~ BIT 0: The X coordinate value of the 4th finger is eight high digits: X_Position>>4
 */
#define MODE_NORMAL_18_REG         (uint16_t(0xD012))

/*
    BIT 7 ~ BIT 0: The Y coordinate value of the 4th finger is eight high digits: Y_Position>>4
 */
#define MODE_NORMAL_19_REG         (uint16_t(0xD013))

/*
    BIT 7 ~ BIT 4: The X coordinate value of the 4th finger X_Position&0x0F
    BIT 3 ~ BIT 0: The Y coordinate value of the 4th finger Y_Position&0x0F
 */
#define MODE_NORMAL_20_REG         (uint16_t(0xD014))

/*
    BIT 7 ~ BIT 0: 4th finger pressure value
 */
#define MODE_NORMAL_21_REG         (uint16_t(0xD015))

/*
    BIT 7 ~ BIT 4: 5th finger ID
    BIT 3 ~ BIT 0: 5th finger state: pressed (0x06) or lifted
 */
#define MODE_NORMAL_22_REG         (uint16_t(0xD016))

/*
    BIT 7 ~ BIT 0: The X coordinate value of the 5th finger is eight high digits: X_Position>>4
 */
#define MODE_NORMAL_23_REG         (uint16_t(0xD017))

/*
    BIT 7 ~ BIT 0: The Y coordinate value of the 5th finger is eight high digits: Y_Position>>4
 */
#define MODE_NORMAL_24_REG         (uint16_t(0xD018))

/*
    BIT 7 ~ BIT 4: The X coordinate value of the 5th finger X_Position&0x0F
    BIT 3 ~ BIT 0: The Y coordinate value of the 5th finger Y_Position&0x0F
 */
#define MODE_NORMAL_25_REG         (uint16_t(0xD019))

/*
    BIT 7 ~ BIT 0: 5th finger pressure value
 */
#define MODE_NORMAL_26_REG         (uint16_t(0xD01A))

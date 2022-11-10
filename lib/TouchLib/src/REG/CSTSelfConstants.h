#pragma once

#define CTS826_SLAVE_ADDRESS  (0X15)
#define CTS820_SLAVE_ADDRESS  (0X15)
#define CTS816S_SLAVE_ADDRESS (0X15)

/* Working mode switching command */
// Normal reporting and gesture reporting
#define NOMAL_CMD_REG         ((uint16_t)0x0000)
// Factory test data acquisition
#define DBG_IDAC_CMD_REG      ((uint16_t)0x0004)
// Factory test button and coordinate acquisition
#define DBG_POS_CMD_REG       ((uint16_t)0x00E0)
// Get the original value
#define DBG_RAW_CMD_REG       ((uint16_t)0x0006)
// get the differ value
#define DBG_SIG_CMD_REG       ((uint16_t)0x0007)

/* NOMAL register description */
/*
Write:  00: NOMAL
        04: DBG_IDAC
        E0: DBG_POS
        06: DBG_RAW
        07: DBG_SIG
 */
#define WORK_MODE_REG         ((uint8_t)0x00)
/*
default：00
far away：C0
near：E0
 */
#define PROXIMITY_ID_REG      ((uint8_t)0x01)

/*
    BIT 7 ~ BIT 4:
    BIT 3 ~ BIT 0: touch points[3:0]
 */
#define TOUCH_NUM_REG         ((uint8_t)0x02)

/*
    BIT 7 ~ BIT 6: event_flg
    BIT 5 ~ BIT 4:
    BIT 3 ~ BIT 0: X_position[11:8]
 */
#define TOUCH1_XH_REG         ((uint8_t)0x03)
/*
    BIT 7 ~ BIT 0: X_position[7:0]
 */
#define TOUCH1_XL_REG         ((uint8_t)0x04)
/*
    BIT 7 ~ BIT 4: touch_ID[3:0]
    BIT 3 ~ BIT 0: Y_position[11:8]
 */
#define TOUCH1_YH_REG         ((uint8_t)0x05)
/*
    BIT 7 ~ BIT 0: Y_position[7:0]
 */
#define TOUCH1_YL_REG         ((uint8_t)0x06)
/*
    BIT 7 ~ BIT 6: event_flg
    BIT 5 ~ BIT 4:
    BIT 3 ~ BIT 0: X_position[11:8]
 */
#define TOUCH2_XH_REG         ((uint8_t)0x09)
/*
    BIT 7 ~ BIT 0: X_position[7:0]
 */
#define TOUCH2_XL_REG         ((uint8_t)0x10)
/*
    BIT 7 ~ BIT 4: touch_ID[3:0]
    BIT 3 ~ BIT 0: Y_position[11:8]
 */
#define TOUCH2_YH_REG         ((uint8_t)0x11)
/*
    BIT 7 ~ BIT 0: Y_position[7:0]
 */
#define TOUCH2_YL_REG         ((uint8_t)0x12)
/*
    BIT 7 ~ BIT 0: deepsleep[7:0] write 0X03 into deepsleep
 */
#define SLEEP_REG             ((uint8_t)0xA5)
/*
    Firmware version number
 */
#define FW_VERSION_0_7_REG    ((uint8_t)0xA6)
#define FW_VERSION_8_15_REG   ((uint8_t)0xA7)

#define MODULE_ID_REG         ((uint8_t)0xA8)
#define PROJECT_NAME_REG      ((uint8_t)0xA9)
#define CHIP_TYPE_0_7_REG     ((uint8_t)0xAA)
#define CHIP_TYPE_8_15_REG    ((uint8_t)0xAB)
#define CHECKSUM_0_7_REG      ((uint8_t)0xAC)
#define CHECKSUM_8_15_REG     ((uint8_t)0xAD)
/*
    write
    01H enter Proximity mode
    00H exit Proximity mode
 */
#define PROX_STATE_REG        ((uint8_t)0xB0)
/*
    write
    01H Enter gesture recognition mode
    00H Exit gesture mode
 */
#define GES_STATE_REG         ((uint8_t)0xD0)
/*
    Gesture mode is enabled to be effective
    double click: 0x24
    up: 0x22
    down: 0x23
    left: 0x20
    rignt: 0x21
    C: 0x34
    e:0x33
    m:0x32
    O: 0x30
    S: 0x46
    V: 0x54
    W: 0x31
    Z:0x65
 */
#define GESTURE_ID_REG        ((uint8_t)0xD3)

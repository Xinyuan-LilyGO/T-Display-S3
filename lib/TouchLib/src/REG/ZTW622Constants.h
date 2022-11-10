#pragma once

#define ZTW622_SLAVE_ADDRESS                  (0x20)

/*2. Register Map一些寄存器的定义，用户不需修改，直接使用就可以了*/
#define ZINITIX_SWRESET_CMD                   ((uint16_t)0x0000)
#define ZINITIX_WAKEUP_CMD                    ((uint16_t)0x0001)

#define ZINITIX_CLEAR_INT_CMD                 ((uint16_t)0x0003)
#define ZINITIX_IDLE_CMD                      ((uint16_t)0x0004)
#define ZINITIX_SLEEP_CMD                     ((uint16_t)0x0005)
#define ZINITIX_CLEAR_INT_STATUS_CMD          ((uint16_t)0x0003)
#define ZINITIX_CALIBRATE_CMD                 ((uint16_t)0x0006)
#define ZINITIX_SAVE_STATUS_CMD               ((uint16_t)0x0007)
#define ZINITIX_SAVE_CALIBRATION_CMD          ((uint16_t)0x0008)
#define ZINITIX_RECALL_FACTORY_CMD            ((uint16_t)0x000f)
#define ZINITIX_SENSITIVITY                   ((uint16_t)0x0020)
#define ZINITIX_I2C_CHECKSUM_WCNT             ((uint16_t)0x016a)
#define ZINITIX_I2C_CHECKSUM_RESULT           ((uint16_t)0x016c)
#define ZINITIX_DEBUG_REG                     ((uint16_t)0x0115) // 0~7
#define ZINITIX_TOUCH_MODE                    ((uint16_t)0x0010)
#define ZINITIX_CHIP_REVISION                 ((uint16_t)0x0011)
#define ZINITIX_FIRMWARE_VERSION              ((uint16_t)0x0012)
#define ZINITIX_MINOR_FW_VERSION              ((uint16_t)0x0121)
#define ZINITIX_DATA_VERSION_REG              ((uint16_t)0x0013)
#define ZINITIX_HW_ID                         ((uint16_t)0x0014)
#define ZINITIX_SUPPORTED_FINGER_NUM          ((uint16_t)0x0015)
#define ZINITIX_EEPROM_INFO                   ((uint16_t)0x0018)
#define ZINITIX_INITIAL_TOUCH_MODE            ((uint16_t)0x0019)
#define ZINITIX_TOTAL_NUMBER_OF_X             ((uint16_t)0x0060)
#define ZINITIX_TOTAL_NUMBER_OF_Y             ((uint16_t)0x0061)
#define ZINITIX_DELAY_RAW_FOR_HOST            ((uint16_t)0x007f)
#define ZINITIX_BUTTON_SUPPORTED_NUM          ((uint16_t)0x00B0)
#define ZINITIX_BUTTON_SENSITIVITY            ((uint16_t)0x00B2)
#define ZINITIX_X_RESOLUTION                  ((uint16_t)0x00C0)
#define ZINITIX_Y_RESOLUTION                  ((uint16_t)0x00C1)
#define ZINITIX_POINT_STATUS_REG              ((uint16_t)0x0080)
#define ZINITIX_ICON_STATUS_REG               ((uint16_t)0x00AA)
#define ZINITIX_AFE_FREQUENCY                 ((uint16_t)0x0100)
#define ZINITIX_DND_N_COUNT                   ((uint16_t)0x0122)
#define ZINITIX_RAWDATA_REG                   ((uint16_t)0x0200)
#define ZINITIX_EEPROM_INFO_REG               ((uint16_t)0x0018)
#define ZINITIX_INT_ENABLE_FLAG               ((uint16_t)0x00f0)
#define ZINITIX_PERIODICAL_INTERRUPT_INTERVAL ((uint16_t)0x00f1)
#define ZINITIX_CHECKSUM_RESULT               ((uint16_t)0x012c)
#define ZINITIX_INIT_FLASH                    ((uint16_t)0x01d0)
#define ZINITIX_WRITE_FLASH                   ((uint16_t)0x01d1)
#define ZINITIX_READ_FLASH                    ((uint16_t)0x01d2)
#define ZINITIX_POINTS_REG                                                                                                                           \
  { 0x0084, 0x008a, 0x0090, 0x0096, 0x009c }

#define BIT_DOWN                1
#define BIT_MOVE                2
#define BIT_UP                  3
#define BIT_PALM                4
#define BIT_PALM_REJECT         5
#define BIT_WAKEUP              6
#define RESERVED_1              7
#define BIT_WEIGHT_CHANGE       8
#define BIT_PT_NO_CHANGE        9
#define BIT_REJECT              10
#define BIT_PT_EXIST            11
#define RESERVED_2              12
#define BIT_MUST_ZERO           13
#define BIT_DEBUG               14
#define BIT_ICON_EVENT          15

/* Status Register LOW */
#define BIT_POINT_COUNT_CHANGE  _BV(0)
#define BIT_POINT_DOWN          _BV(1)
#define BIT_POINT_MOVE          _BV(2)
#define BIT_POINT_UP            _BV(3)
#define BIT_RESERVED04          _BV(4)
#define BIT_PALM_REPORT_GESTURE _BV(5)
#define BIT_WAKE_UP             _BV(6)
#define BIT_RESERVED07          _BV(7)
/* Status Register HIGH */
#define BIT_RESERVED08          _BV(0)
#define BIT_RESERVED09          _BV(1)
#define BIT_RESERVED10          _BV(2)
#define BIT_POINT_DETECTED      _BV(3)
#define BIT_RESERVED12          _BV(4)
#define BIT_MUST_ZERO           _BV(5)
#define BIT_DEBUG               _BV(6)
#define BIT_BUTTON_INFO         _BV(7)

#define SUB_BIT_EXIST           0
#define SUB_BIT_DOWN            1
#define SUB_BIT_MOVE            2
#define SUB_BIT_UP              3
#define SUB_BIT_UPDATE          4
#define SUB_BIT_WAIT            5

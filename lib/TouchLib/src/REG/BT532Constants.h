#pragma once



/* COMMAND LIST (READ BYTE:0)*/
// Software Reset
#define SOFT_RESET_CMD_REG                   ((uint16_t)0x0000)
// Wake Up
#define WAKE_UP_CMD_REG                      ((uint16_t)0x0001)
// Clearing the pended interrupt
#define CLEAR_INT_CMD_REG                    ((uint16_t)0x0003)
// Fall into the IDLE state : slow polling
#define GO_IDLE_CMD_REG                      ((uint16_t)0x0004)
// Fall into the SLEEP state : power saving
#define GO_SLEEP_CMD_REG                     ((uint16_t)0x0005)
#define CALIBRATE_CMD_REG                    ((uint16_t)0x0006)
// Save the current status to the flash
#define SAVE_STATUS_CMD_REG                  ((uint16_t)0x0007)
#define SAVE_CAL_DATA_CMD_REG                ((uint16_t)0x0008)
// Recall factory default status from flash
#define FACTORY_DEFAULT_CMD_REG              ((uint16_t)0x000F)

/* NORMAL CONFIGURE (READ BYTE:2)*/

// Set/Load the current touch mode
#define TOUCH_MODE_REG                       ((uint16_t)0x0010)
// Chip Revision
#define CHIP_REVISION_REG                    ((uint16_t)0x0011)
// Firmware Version
#define FIRMWARE_VERSION_REG                 ((uint16_t)0x0012)
// Register Data Version. (for driver)
#define REGISTER_DATA_VERSION_REG            ((uint16_t)0x0013)
// HW ID
#define HW_ID_REG                            ((uint16_t)0x0014)
// Supported Max Finger Num is 10.
#define SUPPORTED_FINGER_NUM_REG             ((uint16_t)0x0015)
// Max Y Line Number
#define MAX_Y_NUM_REG                        ((uint16_t)0x0016)
/*
Bit 0 : NOT USE 1ST BASELINE
Bit 1 : NOT USE 2ND BASELINE
Bit 2 : USE MINUS MAX FILTER
Bit 3 : USE ONLY SINGLE COORD
Bit 4 : USE SMALL PLAM CHECK
Bit 5 : USE LARGE PALM CHECK
Bit 6 : USE SINGLE COORD IN MULTI
Bit 7 : USE SMALL NOISE DENSITY CHECK
Bit 8 : USE BSR MP RATIO
Bit 9 : NOT USE MULTI AREA COORD
Bit 10 : USE MULTI AVERAGE COORD
Bit 11 : RESERVED
Bit 12 : USE ADPATIVE POINT FILTER
Bit 13 : USE LOW PALM CHECK
Bit 14 : NOT USE CUTOFF SENSITIVITY
Bit 15 : USE BSR REF BASELINE
 */
#define INTERNAL_FLAG_REG                    ((uint16_t)0x0017)

/*
EEPROM Info
Bit 0 : CALIBRATION BIT
Bit 1 : CONFIG INFO BIT (CHECK SUM)
Bit 2 : UPGRADE INFO BIT
 */
#define EEPROM_INFO_REG                      ((uint16_t)0x0018)

// Set/Load the inital touch mode
#define INITIAL_TOUCH_MODE_REG               ((uint16_t)0x0019)

// Chip H/W Revision
#define CHIP_HW_REVISION_REG                 ((uint16_t)0x001A)

// Calibration N Data total number
#define CAL_N_DATA_TOTAL_NUM_REG             ((uint16_t)0x001B)

// IC VENDOR ID : “ZI” (ASCII Code: 0x5A49)
#define IC_VENDOR_ID_REG                     ((uint16_t)0x001C)
// TSM Module ID
#define TSM_MODULE_ID_REG                    ((uint16_t)0x001E)
#define CURRENT_RAW_VARIATION_REG            ((uint16_t)0x001F)
#define DELAY_FOR_HOST_REG                   ((uint16_t)0x007C)
#define SCAN_COUNT_FOR_HOST_REG              ((uint16_t)0x007D)
#define DALAY_RAW_DATA_FOR_HOST_REG          ((uint16_t)0x007F)
/*
Bit 0 : USE DELY FOR HOST
Bit 1 : USE AUTO DELAY FOR HOST
Bit 2 : NOT USE CUTOFF WIDTH
Bit 3 : RESERVED
Bit 4 : NOT USE EDGE SCALING
Bit 5 : USE AUTO DETECT NOISE
Bit 6 : REVERSE ANGLE DIRECTION
Bit 7 : USE EDGE ACTIVE UP
Bit 8 : USE INIT VARIATION CHECK
Bit 9 : USE REJECT SENSITIVITY
Bit 10 : USE SCAN NOISE DATA
Bit 11 : COMPENSATION SPREAD DATA
Bit 12 : USE BSR TSP FLATENESS CHECK
Bit 13 : USE ESD CHECK
Bit 14 : DEF AUTO CPU OSC(ZFT)
Bit 15 : FREE SPACE RECOVERY
 */
#define INTERNAL_FLAG_01_REG                 ((uint16_t)0x011D)
#define INTERNAL_FLAG_02_REG                 ((uint16_t)0x011E)
#define INTERNAL_FLAG_03_REG                 ((uint16_t)0x011F)
#define CHIP_CODE_REG                        ((uint16_t)0x0120)

// Minor firmware version
#define MINOR_FIRMWARE_VERSION_REG           ((uint16_t)0x0121)

// 0x55AA
#define CHECKSUM_RESULT_REG                  ((uint16_t)0x012C)

// Check sum
#define SYSTEM_CHECKSUM_REG                  ((uint16_t)0x012E)

/* SENSITIVITY CONFIGURE (READ BYTE:2)*/
#define CURRENT_SENSITIVITY_TH_REG           ((uint16_t)0x001D)
/*
Sensitivity Threshold : 0 ~ 65535
‘0’ is least sensitive Threshold
‘65535’ is most insensitive Threshold
 */
#define SENSITIVITY_TH_REG                   ((uint16_t)0x0020)
// Side (Left/Right/Top/Bottom) Sensitivity Th
#define FIRST_Y0_SENSITIVITY_TH_REG          ((uint16_t)0x0021)
// Side (Left/Right/Top/Bottom) Sensitivity Th
#define LAST_Y_SENSITIVITY_TH_REG            ((uint16_t)0x0022)
// Side (Left/Right/Top/Bottom) Sensitivity Th
#define FIRST_X0_SENSITIVITY_TH_REG          ((uint16_t)0x0023)
// Side (Left/Right/Top/Bottom) Sensitivity Th
#define LAST_X_SENSITIVITY_TH_REG            ((uint16_t)0x0024)
// Sensitivity Coef.
#define ACTIVE_UP_SENSITIVITY_RATIO_REG      ((uint16_t)0x0025)
// Sensitivity Coef.
#define EDGE_ACTIVE_UP_ST_RATIO_REG          ((uint16_t)0x0027)
// Sensitivity Coef.
#define HYSTERISYS_ST_RATIO_REG              ((uint16_t)0x003D)

/* FILTER CONFIGURE (READ BYTE:2)*/
// Raw Data Filter Depth
#define FIR_COEFFICIENT_REG                  ((uint16_t)0x0030)
/*
Point Filter (Moving Average Filter) Depth
LSB : 1~32, MSB : 1~32
 */
#define POINT_FIR_COEFFICIENT_REG            ((uint16_t)0x0033)
// Width Filter (Moving Filter) Depth : 1~16
#define POINT_WIDTH_FIR_REG                  ((uint16_t)0x0034)
// Width Coef.
#define WIDTH_AREA_COEF_REG                  ((uint16_t)0x0035).
// Point Filter Coef
#define POINT_FIR_DISTANCE_REG               ((uint16_t)0x0037)
// Angle Filter Depth : 1~16
#define POINT_ANGLE_FIR_REG                  ((uint16_t)0x0038)
#define SW_ADAPTIVE_LIMIT_LENGTH_REG         ((uint16_t)0x00CE)
#define SW_ADAPTIVE_TIMEOUT_REG              ((uint16_t)0x00CF)

/* NORMAL NOISE (READ BYTE:2)*/
/*
Ignoring the initial point data : 1 ~ 16
1 : ignoring several initial point data
4 : ignoring many initial point data
 */
#define REACTION_COUNT_REG                   ((uint16_t)0x003B)
// Small finger reference value
#define SMALL_FINGER_REF_VAL_REG             ((uint16_t)0x0042)
// Noise reject Coef
#define CUTOFF_NOISE_IDATA_RATIO_REG         ((uint16_t)0x0043)
// Noise reject Coef
#define CUTOFF_NOISE_WIDTH_RATIO_REG         ((uint16_t)0x0044)
#define MINUSF_M_COMPS_RATIO_REG             ((uint16_t)0x0051)
// Max Node Length for Small Finger
#define SMALL_FINGER_MAX_NODE_LEN_REG        ((uint16_t)0x005A)
// Min Node Length for Large Finger
#define LARGE_FINGER_MIN_NODE_LEN_REG        ((uint16_t)0x005B)
// Max Node Length for Large Finger
#define LARGE_FINGER_MAX_NODE_LEN_REG        ((uint16_t)0x0075)
#define SPREAD_COMP_RATIO_REG                ((uint16_t)0x00F2)

/* BSR NOISE (READ BYTE:2)*/
// Duration for BSR Filter
#define BSR_DURATION_REG                     ((uint16_t)0x00D8)
// Variation Threshold for BSR Filter
#define BSR_VARIATION_TH_REG                 ((uint16_t)0x00D9)
// MP Reject Reference Value
#define MP_RATIO_REF_VAL_REG                 ((uint16_t)0x00E2)
// MP Reject Coef
#define MP_RATIO_SMALL_REG                   ((uint16_t)0x00E3)
// MP Reject Coef
#define MP_RATIO_LARGE_REG                   ((uint16_t)0x010A)
// Variation Threshold for Flatness
#define FLATNESS_VARIATION_TH_REG            ((uint16_t)0x00DD)
#define FLATNESS_LIMIT_THRESHOLD_REG         ((uint16_t)0x00DE)
// Reference Value for Flatness
#define FLATNESS_REF_VAL_REG                 ((uint16_t)0x00DF)
#define FLATNESS_CURRENT_VALUE_REG           ((uint16_t)0x00E0)
// Reference value for baseline update
#define REF_BASELINE_LIMIT_TH_REG            ((uint16_t)0x00E1)

/* SMALL NOISE (READ BYTE:2)*/
// Duration for Small Reject Baseline Update
#define SMALL_REJECT_BASELINE_PERIOD_REG     ((uint16_t)0x0049)
// Small noise density Coef
#define NOISE_REJECT_DENSITY_RATIO_REG       ((uint16_t)0x004A)
// Small noise Coef
#define NOISE_REJECT_SMALL_NOISE_REF_VAL_REG ((uint16_t)0x004B)
// Small noise density Coef
#define NOISE_REJECT_DENSITY_LIMIT_REG       ((uint16_t)0x004C)
// Small noise density Coef
#define NOISE_REJECT_DENSITY_MIN_LEN_REG     ((uint16_t)0x004D)

/* LOW PALM (READ BYTE:2)*/
// Low Palm sensitivity Th
#define LOW_PALM_SENSITIVITY_TH_REG          ((uint16_t)0x0026)
// Max X Line for Low Palm
#define LOW_PALM_MAX_X_REG                   ((uint16_t)0x0031)
// Max Y Line for Low Palm
#define LOW_PALM_MAX_Y_REG                   ((uint16_t)0x0032)
// Max Area size for Low Palm
#define LOW_PALM_MAX_AREA_REG                ((uint16_t)0x0036)
// Sensitivity Coef
#define LOW_PALM_ST_MAGNIFICATION_REG        ((uint16_t)0x0047)

/* SMALL PALM (READ BYTE:2)*/
// Area size for Small Palm
#define SMALL_PALM_AREA_THRESHOLD_REG        ((uint16_t)0x003C)
// Sensitivity Coef
#define SMALL_PALM_ST_MAGNIFICATION_REG      ((uint16_t)0x0045)
// Duration for Small Palm Baseline Update
#define SMALL_PALM_BS_UPDATE_PERIOD_REG      ((uint16_t)0x004F)

/* LARGE PALM (READ BYTE:2)*/
#define LARGE_PALM_REJECT_N_COUNT_REG        ((uint16_t)0x003E)
#define LARGE_PALM_REJECT_AREA_TH_REG        ((uint16_t)0x003F)
#define LARGE_PALM_REPORT_AREA_TH_REG        ((uint16_t)0x0040)
#define LARGE_PALM_UP_SKIP_CNT_REG           ((uint16_t)0x0041)
#define LARGE_PALM_ST_MAGNIFICATION_REG      ((uint16_t)0x0046)
#define LARGE_PALM_TIME_REG                  ((uint16_t)0x0048)
#define LARGE_PALM_BS_UPDATE_PERIOD_REG      ((uint16_t)0x004E)

/* AUTO NOISE LEVEL (READ BYTE:2)*/
// Raw Data variation
#define REAL_RAW_VARIATION_REG               ((uint16_t)0x001F)
// Raw Data Max
#define REAL_RAW_MAX_REG                     ((uint16_t)0x0073)
// Raw Data Min
#define REAL_RAW_MIN_REG                     ((uint16_t)0x0074)
// Current Level Depth
#define CUR_LEVEL_DEPTH_REG                  ((uint16_t)0x0101)
// Current Level Down Count
#define CUR_LEVEL_DOWN_CNT_REG               ((uint16_t)0x0102)
// Current ADC Variation
#define CUR_ADC_VARIATION_REG                ((uint16_t)0x0103)
// Current PP Noise Variation
#define CUR_PP_NOISE_VARIATION_REG           ((uint16_t)0x0104)
// Current PP Noise Max
#define CUR_PP_NOISE_MAX_REG                 ((uint16_t)0x0105)
// Current PP Noise Min
#define CUR_PP_NOISE_MIN_REG                 ((uint16_t)0x0106)
// Current Skip Step
#define CUR_SKIP_STEP_REG                    ((uint16_t)0x0107)
// Count for Level Down
#define DETECT_LEVEL_DOWN_CNT_REG            ((uint16_t)0x0108)
#define ADC_VARIATION_TIMEOUT_REG            ((uint16_t)0x0109)
// ADC variation threshold for LEVEL 0
#define LEVEL_0_ADC_VARIATION_TH_REG         ((uint16_t)0x010A)
// PP variation threshold for LEVEL 1
#define LEVEL_1_PP_VARIATION_TH_REG          ((uint16_t)0x010B)
// PP max threshold for LEVEL 1
#define LEVEL_1_PP_MAX_TH_REG                ((uint16_t)0x010C)
// PP min threshold for LEVEL 1
#define LEVEL_1_PP_MIN_TH_REG                ((uint16_t)0x010D)
// ADC variation threshold for LEVEL 1
#define LEVEL_1_ADC_VARIATION_TH_REG         ((uint16_t)0x010E)
#define LEVEL_1_SENSITIVITY_INC_REG          ((uint16_t)0x010F)
#define LEVEL_1_REACTION_CNT_INC_REG         ((uint16_t)0x0110)
// Raw data filter for LEVEL 1
#define LEVEL_1_FIR_REG                      ((uint16_t)0x0111)
// Point Filter Coef for LEVEL 1
#define LEVEL_1_SW_FIR_REG                   ((uint16_t)0x0112)
// Point Filter Coef for LEVEL 1
#define LEVEL_1_SW_FIR_DIST_REG              ((uint16_t)0x0113)
// Noise Reject Coef for LEVEL 1
#define LEVEL_1_CUTOFF_IDATA_REG             ((uint16_t)0x0115)

/* ESD NOISE (READ BYTE:2)*/
// ESD Noise Variation Threshold
#define ESD_NOISE_VARIATION_TH_REG           ((uint16_t)0x00E3)
// ESD Noise Count
#define ESD_NOISE_VARIATION_CNT_TH_REG       ((uint16_t)0x00E4)
#define ESD_NOISE_MP_RATIO_REG               ((uint16_t)0x00E5)
// ESD Noise Threshold
#define ESD_NOISE_THRESHOLD_REG              ((uint16_t)0x00E6)
// ESD Noise Min
#define ESD_NOISE_MIN_CRT_REG                ((uint16_t)0x00E7)
// ESD Noise Max
#define ESD_NOISE_MAX_CRT_REG                ((uint16_t)0x00E8)
// ESD Noise Variation Count
#define ESD_NOISE_CUR_VARITON_CNT_REG        ((uint16_t)0x00E9)

/* SCAN AREA CONFIGURE (READ BYTE:2)*/
#define CHECK_AREA_REF_VAL_RATIO_REG         ((uint16_t)0x0050)
// X Size for Multi Check
#define SKIP_MULTI_CHK_MAX_X_REG             ((uint16_t)0x0052)
// Y Size for Multi Check
#define SKIP_MULTI_CHK_MAX_Y_REG             ((uint16_t)0x0053)
// Max Area Count for Multi check
#define SKIP_MULTI_CHK_MAX_AREA_REG          ((uint16_t)0x0054)
// Min value for Multi Check
#define SKIP_MULTI_CHK_MIN_VAL_REG           ((uint16_t)0x0055)
// Adhesion Coef
#define ADHESION_RATIO_REG                   ((uint16_t)0x0056)
// Min Area Count for Multi Check
#define SKIP_MULTI_CHK_MIN_AREA_REG          ((uint16_t)0x0057)
// Noise reject Coef for Multi Check
#define MULTI_CUTOFF_RATIO_REG               ((uint16_t)0x0058)
// Adhesion Reference value Coef for Multi Check
#define ADHESION_REF_VAL_RATIO_REG           ((uint16_t)0x0059)
#define CAL_G_COORD_X_LEN_REG                ((uint16_t)0x005C)
#define CAL_G_COORD_Y_LEN_REG                ((uint16_t)0x005D)
#define ONE_RECT_COEF_REG                    ((uint16_t)0x005E)
// Area Count Reference Value Coef
#define AREA_COUNT_REF_VAL_RATIO_REG         ((uint16_t)0x005F)
// Adhesion Coef for Multi Check
#define MULTI_ADHESION_RATIO_REG             ((uint16_t)0x00C5)
// Free Space Coef for Multi Check
#define FREE_SPACE_RECOVERY_RATIO_REG        ((uint16_t)0x00D2)

/* BASELINE CONFIGURE (READ BYTE:2)*/
// Variation for 1st Baseline Update
#define _1ST_BASELINE_VARIATION_REG          ((uint16_t)0x0028)
// Variation for 2nd Baseline Update
#define _2ND_BASELINE_VARIATION_REG          ((uint16_t)0x0029)
// Duration for 1st Baseline Update
#define _1ST_BASELINE_PERIOD_REG             ((uint16_t)0x002A)
// Duration for 2nd Baseline Update
#define _2ND_BASELINE_PERIOD_REG             ((uint16_t)0x002B)
// Duration for Baseline Update in detecting finger.
#define BASELINE_FORCE_PERIOD_REG            ((uint16_t)0x002C)
// Step For 2nd Baseline Update
#define _2ND_BASELINE_STEP_REG               ((uint16_t)0x002D)
// Initial Count for Soft calibration
#define SOFT_CALIBRATION_INIT_COUNT_REG      ((uint16_t)0x0078)

/* CALIBRATION CONFIGURE (READ BYTE:2)*/
// Reference Value for H/W Calibration
#define CALIBRATION_REFERENCE_REG            ((uint16_t)0x0076)
// Scan Count for H/W Calibration
#define CALIBRATION_SCAN_COUNT_REG           ((uint16_t)0x0077)
// Afe N Count for H/W Calibration
#define CALIBRATION_DEFAULT_N_COUNT_REG      ((uint16_t)0x0078)
// Default Cap Value for H/W Calibration
#define CALIBRATION_DEFAULT_C_REG            ((uint16_t)0x0079)
// Calibration Mode
#define CALIBRATION_MODE_REG                 ((uint16_t)0x007A)

/* BUTTON CONFIGURE (READ BYTE:2)*/
// Supported Button Number
#define SUPPORTED_BUTTON_NUM_REG             ((uint16_t)0x00B0)
// Ignoring the initial button data : 1 ~ 16
#define BUTTON_REACTION_CNT_REG              ((uint16_t)0x00B1)
// Button Sensitivity Threshold
#define BUTTON_SENSITIVITY_TH_REG            ((uint16_t)0x00B2)
// 0 = X LINE, 1 = Y LINE
#define BUTTON_LINE_TYPE_REG                 ((uint16_t)0x00B3)
// First or last X or Y Line Num (ex) 0 or 15
#define BUTTON_LINE_NUM_REG                  ((uint16_t)0x00B4)
/*
Button Node Count
ex) If BUTTON LINE TYPE = 0, BUTTON LINE NUM =
15, BUTTON_0 START NODE = 3 and this reg’s
value 3, BUTTON 0 AREA is X = 15, Y= 3~5.
 */
#define BUTTON_RANGE_REG                     ((uint16_t)0x00B5)
// BUTTON_0 Start Node Num
#define BUTTON_0_START_NODE_REG              ((uint16_t)0x00B6)
#define BUTTON_1_START_NODE_REG              ((uint16_t)0x00B7)
#define BUTTON_2_START_NODE_REG              ((uint16_t)0x00B8)
#define BUTTON_3_START_NODE_REG              ((uint16_t)0x00B9)
#define BUTTON_4_START_NODE_REG              ((uint16_t)0x00BA)
#define BUTTON_5_START_NODE_REG              ((uint16_t)0x00BB)
#define BUTTON_6_START_NODE_REG              ((uint16_t)0x00BC)
#define BUTTON_7_START_NODE_REG              ((uint16_t)0x00BD)
#define BUTTON_WIDTH_MUL_RATIO_REG           ((uint16_t)0x00BE)
#define BUTTON_PRIORITY_RATIO_REG            ((uint16_t)0x00BF)

/* X/Y CHANNEL CONFIGURE (READ BYTE:2)*/
// X Channel number
#define TOTAL_NUM_OF_X_REG                   ((uint16_t)0x0060)
// Y Channel number
#define TOTAL_NUM_OF_Y_REG                   ((uint16_t)0x0061)
// X Pin mapping num with ITO
#define X00_01_DRIVE_NUM_REG                 ((uint16_t)0x0062)
#define X02_03_DRIVE_NUM_REG                 ((uint16_t)0x0063)
#define X04_05_DRIVE_NUM_REG                 ((uint16_t)0x0064)
#define X06_07_DRIVE_NUM_REG                 ((uint16_t)0x0065)
#define X08_09_DRIVE_NUM_REG                 ((uint16_t)0x0066)
#define X10_11_DRIVE_NUM_REG                 ((uint16_t)0x0067)
#define X12_13_DRIVE_NUM_REG                 ((uint16_t)0x0068)
#define X14_15_DRIVE_NUM_REG                 ((uint16_t)0x0069)
#define X16_17_DRIVE_NUM_REG                 ((uint16_t)0x006A)
#define X18_19_DRIVE_NUM_REG                 ((uint16_t)0x006B)
#define X20_21_DRIVE_NUM_REG                 ((uint16_t)0x006C)
#define X22_23_DRIVE_NUM_REG                 ((uint16_t)0x006D)
#define X24_25_DRIVE_NUM_REG                 ((uint16_t)0x006E)
#define X26_27_DRIVE_NUM_REG                 ((uint16_t)0x006F)
#define X28_29_DRIVE_NUM_REG                 ((uint16_t)0x0070)
#define X30_31_DRIVE_NUM_REG                 ((uint16_t)0x0071)
#define Y00_01_INPUT_NUM_REG                 ((uint16_t)0x00F3)
#define Y02_03_INPUT_NUM_REG                 ((uint16_t)0x00F4)
#define Y04_05_INPUT_NUM_REG                 ((uint16_t)0x00F5)
#define Y06_07_INPUT_NUM_REG                 ((uint16_t)0x00F6)
#define Y08_09_INPUT_NUM_REG                 ((uint16_t)0x00F7)
#define Y10_11_INPUT_NUM_REG                 ((uint16_t)0x00F8)
#define Y12_13_INPUT_NUM_REG                 ((uint16_t)0x00F9)
#define Y14_15_INPUT_NUM_REG                 ((uint16_t)0x00FA)
#define Y16_17_INPUT_NUM_REG                 ((uint16_t)0x00FB)

/* AFE FREQ CONFIGURE (READ BYTE:2)*/
// AFE Frequency
#define AFE_FREQUENCY_REG                    ((uint16_t)0x0100)

/* AFE OPERATION COEF (READ BYTE:2)*/
// Default N Count
#define AFE_DEFAULT_N_COUNT_REG              ((uint16_t)0x0122)
#define AFE_DEFAULT_C_REG                    ((uint16_t)0x0123)
/*
OSC Frequency
[ZMT200]
 0(default) : 14MHz
 1 : 18MHz
[ZFT400]
 0(default) : 16.6MHz
 21 : 33.3MHz
 11 : 40MHz
 */
#define CPU_OSD_FREQUENCY_REG                ((uint16_t)0x0125)
#define AFE_R_SHIFT_VALUE_REG                ((uint16_t)0x012B)

/* COORD. CONFIGURE (READ BYTE:2)*/
// X Resolution
#define RESOLUTION_OF_X_REG                  ((uint16_t)0x00C0)
// Y Resolution
#define RESOLUTION_OF_Y_REG                  ((uint16_t)0x00C1)
/*
BIT 0 : X FLIP (1)
BIT 1 : Y FLIP (1)
BIT 2 : XY SWAP (1)
 */
#define COORD_ORIENTATION_REG                ((uint16_t)0x00C2)
/*
Holding the point movement : 1 ~ 256
001 : point movement recognition is fast
128 : point movement recognition is slow
 */
#define HOLD_POINT_TRESHHOLD_REG             ((uint16_t)0x00C3)
// Holding the width change : 1 ~ 256
#define HOLD_WIDTH_TRESHHOLD_REG             ((uint16_t)0x00C4)
#define ASSUME_UP_THRESHHOLD_REG             ((uint16_t)0x00C6)
#define ASSUME_UP_SKIP_THRESHHOLD_REG        ((uint16_t)0x00C7)
// Shift X Coord for Accuracy Coef.
#define X_POINT_SHIFT_REG                    ((uint16_t)0x00C8)
// Shift Y Coord for Accuracy Coef.
#define Y_POINT_SHIFT_REG                    ((uint16_t)0x00C9)
// View Area First X Line Offset
#define VIEW_AREA_XF_OFFSET_REG              ((uint16_t)0x00CA)
// View Area Last X Line Offset
#define VIEW_AREA_XL_OFFSET_REG              ((uint16_t)0x00CB)
// View Area First Y Line Offset
#define VIEW_AREA_YF_OFFSET_REG              ((uint16_t)0x00CC)
// View Area Last Y Line Offset
#define VIEW_AREA_YL_OFFSET_REG              ((uint16_t)0x00CD)
// Coef. for calculation coord
#define COEF_X_GAIN_REG                      ((uint16_t)0x00D0)
// Coef. for calculation coord
#define COEF_Y_GAIN_REG                      ((uint16_t)0x00D1)
#define ACTIVE_UP_THRESHHOLD_REG             ((uint16_t)0x00D3)
// Process Data Coef.
#define PDATA_COEF1_REG                      ((uint16_t)0x00D5)
// Process Data Coef.
#define PDATA_COEF2_REG                      ((uint16_t)0x00D6)
// Process Data Coef.
#define PDATA_COEF3_REG                      ((uint16_t)0x00D7)
// Edge Area Scale
#define VIEW_EDGE_SCALE_VAL_REG              ((uint16_t)0x00EB)
// Edge Area First X Line Offset
#define VIEW_EDGE_XF_OFFSET_REG              ((uint16_t)0x00EC)
// Edge Area Last X Line Offset
#define VIEW_EDGE_XL_OFFSET_REG              ((uint16_t)0x00ED)
// Edge Area First Y Line Offset
#define VIEW_EDGE_YF_OFFSET_REG              ((uint16_t)0x00EE)
// Edge Area Last Y Line Offset
#define VIEW_EDGE_YL_OFFSET_REG              ((uint16_t)0x00EF)

/* INTERRUPT CONFIGURE (READ BYTE:2)*/
/*
BIT 00 : POINT COUNT CHANGE EVENT
BIT 01 : DOWN EVENT
BIT 02 : MOVE EVENT
BIT 03 : UP EVENT
BIT 04 : LARGE PALM REPORT
BIT 05 : LARGE PALM REJECT
BIT 06 : RESERVED
BIT 07 : RESERVED
BIT 08 : WIDTH CHANGED EVENT
BIT 09 : REJECT EVENT
BIT 10 : RESERVED
BIT 11 : POINT DETECTED EVENT
BIT 12 : RESERVED
BIT 13 : RESERVED
BIT 14 : DEBUG EVENT
BIT 15 : BUTTON EVENT
 */
#define INT_ENABLE_FLAG_REG                  ((uint16_t)0x00F0)
/*
0 : No Use
1~30000 : Occurred invalid Interrupt periodically
per setting Afe Cycle.
Invalid interrupt :
Status register = 0x00
Point Count = 100;
 */
#define ESD_INTERRUPT_INTERVAL_REG           ((uint16_t)0x00F1)

/* POINT INFO (READ BYTE:2)*/
/*
BIT 00 : POINT COUNT CHANGE EVENT
BIT 01 : DOWN EVENT
BIT 02 : MOVE EVENT
BIT 03 : UP EVENT
BIT 04 : LARGE PALM REPORT
BIT 05 : LARGE PALM REJECT
BIT 06 : RESERVED
BIT 07 : RESERVED
BIT 08 : WIDTH CHANGED EVENT
BIT 09 : REJECT EVENT
BIT 10 : RESERVED
BIT 11 : POINT DETECTED EVENT
BIT 12 : RESERVED
BIT 13 : RESERVED
BIT 14 : DEBUG EVENT
BIT 15 : BUTTON EVENT
 */
#define STATUS_REGISTER_REG                  ((uint16_t)0x0080)
/*
LSB : Point Count, MSB : Time Stamp
if TOUCH MODE = 1, using event flag.
 BIT 00 : Finger Idx00 Event
 BIT 01 : Finger Idx01 Event
 BIT 02 : Finger Idx01 Event
 …
 */
#define POINT_COUNT_TIMESTAMP_REG            ((uint16_t)0x0081)
/*
[TOUCH MODE 0] – Report All Point’s Info.
 1st Point X position(2BYTE) + 1st Point Y position(2BYTE) + 1st Major Width(1Byte) + 1st Sub Status(1Byte)
 2nd Point X position(2BYTE) + 2nd Point Y position(2BYTE) + 2nd Major Width(1Byte) + 2nd Sub Status(1Bytes) ...
< SUB STATUS INFO>
BIT 00 : Point Exist (0 : no, 1 : yes)
BIT 01 : Down
BIT 02 : Move
BIT 03 : Up
BIT 04 : Update
BIT 05 : Wait
BIT 06 : Reaction CNT Full
BIT 07 : Width change
[TOUCH MODE 2] – Report All Point’s Info.
 1st Point X position(2BYTE) + 1st Point Y position(2BYTE) + 1st Major Width(1Byte) + 1st Sub Status(1Byte)
+ 1st Minor Width(1Byte) + 1st Angle(1Byte) + 2nd Point X position(2BYTE) + 2nd Point Y position(2BYTE)
+ 2nd Major Width(1Byte) + 2nd Sub Status(1Bytes) + 2nd Minor Width(1Byte) + 2nd Angle(1Byte)….
 */
#define REPORT_ALL_POINTS_INFO_REG           ((uint16_t)0x0082)

/* BUTTON EVENT & OTHER EVENT INFO (READ BYTE:2)*/
/*
BIT 00 : BUTTON 0 DOWN
BIT 01 : BUTTON 1 DOWN
BIT 02 : BUTTON 2 DOWN
BIT 03 : BUTTON 3 DOWN
BIT 04 : BUTTON 4 DOWN
BIT 05 : BUTTON 5 DOWN
BIT 06 : BUTTON 6 DOWN
BIT 07 : BUTTON 7 DOWN
BIT 08 : BUTTON 0 UP
BIT 09 : BUTTON 1 UP
BIT 10 : BUTTON 2 UP
BIT 11 : BUTTON 3 UP
BIT 12 : BUTTON 4 UP
BIT 13 : BUTTON 5 UP
BIT 14 : BUTTON 6 UP
BIT 15 : BUTTON 7 UP
 */
#define BUTTON_EVENT_REG                     ((uint16_t)0x00AA)

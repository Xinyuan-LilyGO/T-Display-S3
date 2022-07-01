/**
 *Name        : hynitron_config.h
 *Author      : steven
 *Version     : V2.0
 *Create      : 2019-11-11
 *Copyright   : zxzz
 */


#ifndef HYNITRON_CONFIG_H__
#define HYNITRON_CONFIG_H__

/*
 * show debug log info
 * enable it for debug, disable it for release
 */
#define HYN_DEBUG_EN                            1

/*
 * Linux MultiTouch Protocol
 * 1: Protocol B(default), 0: Protocol A
 */
#define HYN_MT_PROTOCOL_B_EN                   0


/*
 * Report Pressure in multitouch
 * 1:enable(default),0:disable
*/
#define HYN_REPORT_PRESSURE_EN                  0

/*
 * Switch Gesture wakeup in multitouch
 * 1:enable,0:disable
*/
#define HYN_GESTURE_EN                         0


/*
 * Switch proximity detect in multitouch
 * 1:enable,0:disable
*/
#define HYN_PSENSOR_EN                          0


/*
 * Switch esd detect in multitouch
 * 1:enable,0:disable
*/
#define HYN_ESDCHECK_EN                          0

/*
 * Factory test support 
 * 1:enable,0:disable
 * only support cst3xx.
*/
#define HYN_AUTO_FACTORY_TEST_EN	            0


/*
 * Update firmware support 
 * choose the right chiptype
 * 1:enable,0:disable
 *
*/
 
#define HYN_EN_AUTO_UPDATE                      0
#define HYN_EN_AUTO_UPDATE_CST0xxSE             0
#define HYN_EN_AUTO_UPDATE_CST0xx               0
#define HYN_EN_AUTO_UPDATE_CST1xx               0
#define HYN_EN_AUTO_UPDATE_CST1xxSE             0
#define HYN_EN_AUTO_UPDATE_CST2xx               0
#define HYN_EN_AUTO_UPDATE_CST2xxSE             0
#define HYN_EN_AUTO_UPDATE_CST3xx               0
#define HYN_EN_AUTO_UPDATE_CST3xxSE             0
#define HYN_EN_AUTO_UPDATE_CST6xx				0
#define HYN_EN_AUTO_UPDATE_CST78xx              0
#define HYN_EN_AUTO_UPDATE_CST9xx               0
#define HYN_SYS_AUTO_SEARCH_FIRMWARE            0


/*
 * Node for debug
 * /proc/cst1xx_ts
 * /sys/hynitron_debug
 * 1:enable,0:disable
 *
*/
#define ANDROID_TOOL_SURPORT					1

#define HYN_SYSFS_NODE_EN                       1


#endif /*HYNITRON_CONFIG_H__*/


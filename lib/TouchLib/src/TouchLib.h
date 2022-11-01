/**
 * @file TouchLib.h
 * @author Micky (513673326@qq.com)
 * @brief
 * @version 0.1
 * @date 2022-10-24
 *
 * @copyright Copyright (c) 2022
 *
 */

#ifndef _TOUCHLIB_H_
#define _TOUCHLIB_H_

#if defined(TOUCH_MODULES_CST816S)

#elif defined(TOUCH_MODULES_CST_SELF)
#include "ModulesCSTSelf.tpp"
typedef TouchLibCSTSelf TouchLib;
#elif defined(TOUCH_MODULES_CST_MUTUAL)
#include "ModulesCSTMutual.tpp"
typedef TouchLibCSTMutual TouchLib;
#elif defined(TOUCH_MODULES_GT911)
#include "ModulesGT911.tpp"
typedef TouchLibGT911 TouchLib;
#elif defined(TOUCH_MODULES_ZTW622)
#include "ModulesZTW622.tpp"
typedef TouchLibZTW622 TouchLib;
#elif defined(TOUCH_MODULES_FT3267)

#else
#error "Please define a touch chip model."
#endif


#endif
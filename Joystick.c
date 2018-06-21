/*
 * Joystick.c
 *
 *  Created on: May 15, 2018
 *      Author: Vlad
 */


#include "JoyStick.h"
#include "LDrvAdc.h"
static buttonsState myCfg;
uint16_t x_Value,y_Value,button_Value;
uint16_t adcResultCh0A = 0;
uint16_t adcResultCh1A = 0;
uint16_t adcResultCh15A = 0;

void JoyStick_init()
{
	myCfg.StareX = (xAxisState) X_axis_CENTER;
	myCfg.StareY = (yAxisState) Y_axis_CENTER;
	myCfg.Button = (pushButtonState) NOT_PRESS;
}

void JoyStick_read_channels()
{
	Adc_StartSingleConversion(x_axis_adc);  //PTA0
	while(Adc_IsConversionComplete() == 0){};
	Adc_GetSingleResult(&adcResultCh0A);
    Adc_StartSingleConversion(y_axis_adc);       //PTA1
	while(Adc_IsConversionComplete() == 0){};
	Adc_GetSingleResult(&adcResultCh1A);
	Adc_StartSingleConversion(push_button_adc);  //PTF7
	while(Adc_IsConversionComplete() == 0){};
	Adc_GetSingleResult(&adcResultCh15A);
}

buttonsState get_state()
{
	JoyStick_read_channels();
	    if(adcResultCh0A > 600)
		  	     myCfg.StareX= (xAxisState) X_axis_UP;
	    else if (adcResultCh0A < 400)
		   	     myCfg.StareX= (xAxisState) X_axis_DOWN;
	    else     myCfg.StareX= (xAxisState) X_axis_CENTER;
	    if(adcResultCh1A > 600)
	    		myCfg.StareY = (yAxisState) Y_axis_LEFT;
	    else if (adcResultCh1A < 400)
	    		myCfg.StareY = (yAxisState) Y_axis_RIGHT;
	    else    myCfg.StareY = (yAxisState) Y_axis_CENTER;

	    if(adcResultCh15A == 0)
	    	     myCfg.Button = (pushButtonState) PRESS;
	    else     myCfg.Button = (pushButtonState) NOT_PRESS;

	    return myCfg;
}



 /* main.c              (c) 2015 Freescale Semiconductor, Inc.
 * Descriptions: ADC functions for conversion example
 * 28 Sept 2015 Pedro Aguayo: Initial version
 * 16 Dec 2015 S Mihalik, O Romero: Modified code
 */

#include "derivative.h"
//#include "ADC.h"
#include "LDrvAdc.h"
#include "clocks.h"
#include "lcd1602a.h"
#include "GPIO_module.h"
#include "types.h"
#include "UART.h"
#include "Joystick.h"
#include "Buttons.h"
#include "Delay_lib.h"
#include "Pwm.h"
#include "LED.h"
uint16_t adcResultCh0 = 0;
uint16_t adcResultCh1 = 0;
uint16_t adcResultCh15 = 0;

buttonsState myJoystickCfg;
static buttonsState lastValue;

static SwitchesState mySwitches;
static ConfigButtons Switches;
void init_main();
void init_clk();
void initInterfata();
void tx_position(buttonsState cfg);


int start=0;
int state=0;
int position=9;
int a=0;

char *array_led[]={"LED1","LED2","LED3","LED4","LEVEL"};
char *stare_led[]={"ON","OFF"};
char *indicator="*";
char *level[]={"1","2","3","4","5"};
uint8 count=0;
uint8 count_state=5;
SET_LED leduri;

typedef enum
{
	LED1=0,
	LED2,
	LED3,
	LED4,
	LEVEL1,
}MENIU;

typedef enum
{
	NONE=0,
	ON_OFF,
	LEVELS,
}SUBMENIU;

typedef struct
{
	MENIU meniu;
	SUBMENIU submeniu;
}INTERFATA;

static INTERFATA myINTERFACE;
INTERFATA *interf(buttonsState myJoystickCfg, INTERFATA *interfata);
void writeLCD();
int pickLCD(buttonsState cfg);
int main(void){



  int counter = 0;
  int j;
  init_main();
  init_clks_FEE_40MHz();
  init_IRQs();
  init_delay();
  Pwm_vInit();
  tAdcCfg * myCfg = Adc_GetCfgHandle();
  myCfg->AdcOpMode=ADC_OPMODE_SINGLESHOT ;
  myCfg->AdcResolution = ADC_RES_10_BIT;
  Adc_ConfigAcquisition(myCfg);
  UART_init();
  LCD_Init();
  LCD_Clear();
  JoyStick_init();
  Buttons_init();
  Buttons_set_portB(Switches);
  initInterfata();


  //X axis - ch 0; Y_axis - ch 1; Push_button - ch 15

  for(;;)
  {
	  myJoystickCfg=get_state(); // citire joystick
	  tx_position(myJoystickCfg); // transmitere pe seriala pozitie joystick
	  interf(myJoystickCfg, &myINTERFACE);

	  if(myJoystickCfg.StareX==X_axis_CENTER && myJoystickCfg.StareY==Y_axis_CENTER)
	  { } //nu scrie pe lcd
	  else writeLCD();


  }
}
int pickLCD(buttonsState cfg)
{

	if(start==0)
	{
		LCD_String_xy(1,9,indicator);
		start=1;
	}

	cfg=get_state();


	if(cfg.StareY==Y_axis_RIGHT && position==9)
	{
		LCD_String_xy(1,14,indicator);
		LCD_String_xy(1,9," ");
		position=14;
	}
	else if(cfg.StareY==Y_axis_LEFT && position==14){
		LCD_String_xy(1,9,indicator);
		LCD_String_xy(1,14," ");
		position=9;
	}

	else if(cfg.StareY==Y_axis_LEFT && position==9)
	{
		state=1;
	}
	if (cfg.Button==PRESS && position==9) {
		Pwm_u16SetDutyCycle(myINTERFACE.meniu,0);
	}
	else if(cfg.Button==PRESS && position==14)
	{
		Pwm_u16SetDutyCycle(myINTERFACE.meniu,400);
	}
	return state;


}
void writeLCD()
{
		LCD_Clear();

		if(myINTERFACE.submeniu==NONE)
		{
		  LCD_String_xy(0,0,array_led[myINTERFACE.meniu]);
		}
		else if (myINTERFACE.submeniu==ON_OFF)
		{
			LCD_String_xy(0,0,array_led[myINTERFACE.meniu]);
			LCD_String_xy(0,9,stare_led[1]);
			LCD_String_xy(0,14,stare_led[0]);

			buttonsState cfg;
			while(!a)
			{
				a=pickLCD(cfg);
			}
			a=0;
			position=9;
			state=0;
			start=0;
		}
		else if(myINTERFACE.submeniu==LEVELS)
		{
			LCD_String_xy(0,0,array_led[myINTERFACE.meniu]);
			LCD_String_xy(0,7,level[0]);
			LCD_String_xy(0,9,level[1]);
			LCD_String_xy(0,11,level[2]);
			LCD_String_xy(0,13,level[3]);
			LCD_String_xy(0,15,level[4]);
		}




}



INTERFATA *interf(buttonsState myJoystickCfg, INTERFATA *myINTERF)
{

	//******************************* NAVIGATE DOWN********************************//
	if	((myINTERF->meniu==LED1 && myINTERF->submeniu==NONE) && myJoystickCfg.StareX==X_axis_DOWN)
	{
		 myINTERF->meniu=LED2;

	}
	else if ((myINTERF->meniu==LED2 && myINTERF->submeniu==NONE ) && myJoystickCfg.StareX==X_axis_DOWN)
	{
			myINTERF->meniu=LED3;
	}
	else if ((myINTERF->meniu==LED3 && myINTERF->submeniu==NONE ) && myJoystickCfg.StareX==X_axis_DOWN)
	{
				myINTERF->meniu=LED4;
	}
	else if ((myINTERF->meniu==LED4 && myINTERF->submeniu==NONE ) && myJoystickCfg.StareX==X_axis_DOWN)
	{
				myINTERF->meniu=LEVEL1;
	}
	else if ((myINTERF->meniu==LEVEL1 && myINTERF->submeniu==NONE ) && myJoystickCfg.StareX==X_axis_DOWN)
	{
				myINTERF->meniu=LEVEL1;
	}

	//******************************* NAVIGATE DOWN********************************//


	//******************************* NAVIGATE UP********************************//
	if( (myINTERF->meniu==LED1 && myINTERF->submeniu==NONE ) && myJoystickCfg.StareX==X_axis_UP)
		{
			    myINTERF->meniu=LED1;

		}
		else if ((myINTERF->meniu==LED2 && myINTERF->submeniu==NONE ) && myJoystickCfg.StareX==X_axis_UP)
		{
			    myINTERF->meniu=LED1;
		}
		else if ((myINTERF->meniu==LED3 && myINTERF->submeniu==NONE ) && myJoystickCfg.StareX==X_axis_UP)
		{
			    myINTERF->meniu=LED2;
		}
		else if ((myINTERF->meniu==LED4 && myINTERF->submeniu==NONE ) && myJoystickCfg.StareX==X_axis_UP)
		{
			    myINTERF->meniu=LED3;
		}
		else if ((myINTERF->meniu==LEVEL1 && myINTERF->submeniu==NONE ) && myJoystickCfg.StareX==X_axis_UP)
		{
				myINTERF->meniu=LED4;
		}
	//******************************* NAVIGATE UP********************************//

	//******************************* NAVIGATE RIGHT********************************//
		if( ((myINTERF->meniu==LED1 || myINTERF->meniu==LED2 || myINTERF->meniu==LED3 || myINTERF->meniu==LED4 ) && myINTERF->submeniu==NONE ) && myJoystickCfg.StareY==Y_axis_RIGHT)
		{
				    myINTERF->submeniu=ON_OFF;

		}
		if ((myINTERF->meniu==LEVEL1 && myINTERF->submeniu==NONE ) && myJoystickCfg.StareY==Y_axis_RIGHT)
		{
					myINTERF->submeniu=LEVELS;
		}

		//******************************* NAVIGATE RIGHT********************************//

		//******************************* NAVIGATE BACK TO MENIU********************************//

		if( ((myINTERF->meniu==LED1 || myINTERF->meniu==LED2 || myINTERF->meniu==LED3 || myINTERF->meniu==LED4 || myINTERF->meniu==LEVEL1 ) && ( myINTERF->submeniu==ON_OFF || myINTERF->submeniu==LEVELS)) && myJoystickCfg.StareY==Y_axis_LEFT)
			{
					    myINTERF->submeniu=NONE;

			}


		//******************************* NAVIGATE BACK TO MENIU********************************//
		INTERFATA *myINTERFCopy=myINTERF;
		//***********************************center****************************************////
		if(myJoystickCfg.StareY==Y_axis_CENTER && myJoystickCfg.StareX==X_axis_CENTER )
		{
			myINTERF->meniu=myINTERFCopy->meniu;
			myINTERF->submeniu=myINTERFCopy->submeniu;
		}

		return myINTERF;
}

void initInterfata()
{
	myINTERFACE.meniu=LED1;
	myINTERFACE.submeniu=NONE;
	LCD_String_xy(0,0,array_led[0]);
}

void init_count()
{
	count=0;
	count_state=5;
}
void init_main()
{
	 lastValue.Button=(pushButtonState)NOT_PRESS;
	 lastValue.StareX=(xAxisState) X_axis_CENTER;
	 lastValue.StareY= (yAxisState) Y_axis_CENTER;
	 mySwitches.button1=OFF;
	 Switches.b1.pin=7;
	 Switches.b1.mode=1;

}
void tx_position(buttonsState cfg)
{
	/////////***************************************BUTON***********************///////////////////
	if(cfg.Button==(pushButtonState)PRESS &&  lastValue.Button != (pushButtonState)PRESS)
	{
		UART_string("buton_apasat");
        lastValue.Button=(pushButtonState) PRESS;
	}
	else if(cfg.Button==(pushButtonState) NOT_PRESS)
	{

		lastValue.Button=(pushButtonState) NOT_PRESS;
	}

	/////////***************************************BUTON***********************///////////////////



	/////////***************************************AXA X***********************///////////////////
	if(cfg.StareX==(xAxisState) X_axis_UP && lastValue.StareX!=(xAxisState) X_axis_UP)
	{

		UART_string("UP");
		lastValue.StareX=(xAxisState) X_axis_UP;
	}
	else if(cfg.StareX==(xAxisState) X_axis_DOWN & lastValue.StareX!=(xAxisState) X_axis_DOWN)
	{
			UART_string("DOWN");
			lastValue.StareX=(xAxisState) X_axis_DOWN;

	}
	else if (cfg.StareX==(xAxisState) X_axis_CENTER)
	{
		lastValue.StareX=(xAxisState) X_axis_CENTER;

	}
	/////////***************************************AXA X***********************///////////////////


	/////////***************************************AXA Y***********************///////////////////
	if(cfg.StareY==(yAxisState) Y_axis_LEFT && lastValue.StareY!=(yAxisState) Y_axis_LEFT)
	{
				UART_string("LEFT");
				lastValue.StareY= (yAxisState) Y_axis_LEFT;

	}
	else if(cfg.StareY==(xAxisState) Y_axis_RIGHT && lastValue.StareY!=(yAxisState) Y_axis_RIGHT)
	{
				UART_string("RIGHT");
				lastValue.StareY= (yAxisState) Y_axis_RIGHT;

	}
	else if ( cfg.StareY==(xAxisState) Y_axis_CENTER)
	{
		lastValue.StareY= (yAxisState) Y_axis_CENTER;

	}
	/////////***************************************AXA Y***********************///////////////////
}
void init_clk()
{
	ICS_C1|=ICS_C1_IRCLKEN_MASK; 		/* Enable the internal reference clock*/
	ICS_C3= 0x90;						        /* Reference clock frequency = 31.25 KHz*/
	while(!(ICS_S & ICS_S_LOCK_MASK)); /* Wait for PLL lock, now running at 40 MHz (1280 * 31.25Khz) */
    ICS_C2|=ICS_C2_BDIV(2)  ; 			/*div by 2*/
	ICS_S |= ICS_S_LOCK_MASK ; 			/* Clear Loss of lock sticky bit */
}



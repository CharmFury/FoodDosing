/**
  ******************************************************************************
  * File Name          :  stmicroelectronics_x-cube-nfc4_1_5_0.c
  * Description        : This file provides code for the configuration
  *                      of the STMicroelectronics.X-CUBE-NFC4.1.5.0 instances.
  ******************************************************************************
  *
  * COPYRIGHT 2019 STMicroelectronics
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  ******************************************************************************
  */

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "app_x-cube-nfc4.h"
#include "nfc04a1_nfctag.h"
#include "main.h"
#include "common.h"
#include "tagtype5_wrapper.h"
#include "lib_NDEF_URI.h"
#include "lib_NDEF_Text.h"
#include "lib_NDEF.h"
#include "stdio.h"

 /* Global variables ----------------------------------------------------------*/
  volatile uint8_t GPOActivated = 0;
  volatile uint8_t cnt = 0;
 ST25DV_FIELD_STATUS fieldpresence;
 sURI_Info URI;
 extern sCCFileInfo CCFileStruct;
 extern RTC_HandleTypeDef hrtc;
 /*My*/
 extern RTC_DateTypeDef Date;
 extern RTC_TimeTypeDef Time;
 extern char sDate;
 extern RTC_TimeTypeDef sTime = {0};
 extern RTC_DateTypeDef DateToUpdate = {0};
 extern RTC_AlarmTypeDef alarm = {0};

 RTC_TimeTypeDef timeNow = {0};
 RTC_DateTypeDef dateNow = {0};




char first_hour;
char first_minutes;
char second_hour;
char second_minutes;

char third_hour;
char third_minutes;
char fourth_hour;
char fourth_minutes;

char day;
char month;
char year;

char hourNow;
char minuteNow;
char secondNow;

char time_stamp[540];
uint8_t count_time_stamp=0;
uint8_t alarm_amount;
uint8_t alarm_special_number;

 char message[1000];


 extern uint8_t NDEF_Buffer [];
 //uint8_t  NDEF_Buffer;// [NDEF_MAX_SIZE];
 /*My*/
 char text;
 NDEF_Text_metadata_t metaData;
 sRecordInfo_t recordStruct;
 NDEF_Text_info_t infoText;
 sRecordInfo_t record;

 char uartmsg[80];
 uint8_t writedata = 0xAA;//170
 uint8_t readdata = 0x0;
 //uint8_t cnt = 0;
 uint32_t st25dvbmsize = 0;
 uint32_t memindex = 0;
 ST25DV_PASSWD passwd;
 ST25DV_I2CSSO_STATUS i2csso;
 ST25DV_MEM_SIZE st25dvmemsize;
 uint32_t ret;
 /* Private functions ---------------------------------------------------------*/
 void MX_NFC4_NDEF_Process_Write(char *text);
 void MX_NFC4_NDEF_Process_Read(void);
 void MX_NFC4_RTC_Set(uint8_t *NDEF_Buffer);
 void MX_NFC4_GPO_Init(void);
 void Food_Distribution(void);
 void MX_NFC4_GPO_Process(void);
 void BSP_GPO_Callback(void);

void MX_NFC_Init(void)
{
  /* USER CODE BEGIN SV */ 

  /* USER CODE END SV */

  /* USER CODE BEGIN NFC4_Library_Init_PreTreatment */
  
  /* USER CODE END NFC4_Library_Init_PreTreatment */

  /* Initialize the peripherals and the NFC4 components */
	MX_NFC4_GPO_Init();

  /* USER CODE BEGIN SV */ 

  /* USER CODE END SV */
  
  /* USER CODE BEGIN NFC4_Library_Init_PostTreatment */
  
  /* USER CODE END NFC4_Library_Init_PostTreatment */
}
/*
 * LM background task
 */
void MX_NFC_Process(void)
{
  /* USER CODE BEGIN NFC4_Library_Process */
	 MX_NFC4_GPO_Process();
  /* USER CODE END NFC4_Library_Process */
}

void MX_NFC4_GPO_Init(void)
{

  /* Configuration of X-NUCLEO-NFC04A1                                          */
  /******************************************************************************/
  /* Init of the Leds on X-NUCLEO-NFC04A1 board */
  NFC04A1_LED_Init(GREEN_LED );
  NFC04A1_LED_Init(BLUE_LED );
  NFC04A1_LED_Init(YELLOW_LED );
  NFC04A1_LED_On( GREEN_LED );
  HAL_Delay( 300 );
  NFC04A1_LED_On( BLUE_LED );
  HAL_Delay( 300 );
  NFC04A1_LED_On( YELLOW_LED );
  HAL_Delay( 300 );


  /* Init ST25DV driver */
  while( NFC04A1_NFCTAG_Init(NFC04A1_NFCTAG_INSTANCE) != NFCTAG_OK );

  /* Set EXTI settings for GPO Interrupt */
  NFC04A1_GPO_Init();

  /* Set GPO Configuration */
  //int32_t status = NFC04A1_NFCTAG_ConfigIT(NFC04A1_NFCTAG_INSTANCE,ST25DV_GPO_ENABLE_MASK | ST25DV_GPO_FIELDCHANGE_MASK );

  /* Init done */
  NFC04A1_LED_Off( GREEN_LED );
  HAL_Delay( 300 );
  NFC04A1_LED_Off( BLUE_LED );
  HAL_Delay( 300 );
  NFC04A1_LED_Off( YELLOW_LED );
  HAL_Delay( 300 );




}


/**
 * @brief Process NDEF message from EEPROM
 *
 *
 *
 */
void MX_NFC4_NDEF_Process_Read()
{


	/* Reset Mailbox enable to allow write to EEPROM */
		  NFC04A1_NFCTAG_ResetMBEN_Dyn(NFC04A1_NFCTAG_INSTANCE);

		  /*Choose TAG_TYPE*/
		  NfcTag_SelectProtocol(NFCTAG_TYPE5);

		  /* Check if no NDEF detected, init mem in Tag Type 5 */
		 	    if( NfcType5_NDEFDetection( ) != NDEF_OK )
		 	    {
		 	      CCFileStruct.MagicNumber = NFCT5_MAGICNUMBER_E1_CCFILE;
		 	      CCFileStruct.Version = NFCT5_VERSION_V1_0;
		 	      CCFileStruct.MemorySize = ( ST25DV_MAX_SIZE / 8 ) & 0xFF;//maybe need to lower the size
		 	      CCFileStruct.TT5Tag = 0x05;
		 	      /* Init of the Type Tag 5 component (M24LR) */
		 	      while( NfcType5_TT5Init( ) != NFCTAG_OK );
		 	    }

		 	    //restarting amount available food
		 	    foodAmount=0;

	/*Read NDEF message from EEPROM*/
		    NfcTag_ReadNDEF(&NDEF_Buffer);

		    /*Setting RTC & alarm*/
		    MX_NFC4_RTC_Set(&NDEF_Buffer);

}

void MX_NFC4_NDEF_Process_Write(char *text)
{


	/* Reset Mailbox enable to allow write to EEPROM */
		  NFC04A1_NFCTAG_ResetMBEN_Dyn(NFC04A1_NFCTAG_INSTANCE);

		  /*Choose TAG_TYPE*/
		  NfcTag_SelectProtocol(NFCTAG_TYPE5);

		  /* Check if no NDEF detected, init mem in Tag Type 5 */
		 	    if( NfcType5_NDEFDetection( ) != NDEF_OK )
		 	    {
		 	      CCFileStruct.MagicNumber = NFCT5_MAGICNUMBER_E1_CCFILE;
		 	      CCFileStruct.Version = NFCT5_VERSION_V1_0;
		 	      CCFileStruct.MemorySize = ( ST25DV_MAX_SIZE / 8 ) & 0xFF;//maybe need to lower the size
		 	      CCFileStruct.TT5Tag = 0x05;
		 	      /* Init of the Type Tag 5 component (M24LR) */
		 	      while( NfcType5_TT5Init( ) != NFCTAG_OK );
		 	    }

	/*Write message into EEPROM*/
//while( NDEF_WriteText( "application/vnd.xamarin.nfcxample 15:26,18:39" ) != NDEF_OK );

		 	   while( NDEF_WriteText(&text) != NDEF_OK );
}


void MX_NFC4_RTC_Set(uint8_t *NDEF_Buffer)
{
	alarm_amount=NDEF_Buffer[56];




	//Switch based on amount of alarms choosed by user in application
	switch(NDEF_Buffer[56])
	{
	case 1:
	{

	first_hour=NDEF_Buffer[58];
		first_hour +=NDEF_Buffer[59];
		first_minutes=NDEF_Buffer[60];
		first_minutes+=NDEF_Buffer[61];

		break;
	}
	case 2:
	{
		first_hour=NDEF_Buffer[58];
					first_hour +=NDEF_Buffer[59];
					first_minutes=NDEF_Buffer[60];
					first_minutes+=NDEF_Buffer[61];

				second_hour=NDEF_Buffer[63];
					second_hour +=NDEF_Buffer[64];
					second_minutes=NDEF_Buffer[65];
					second_minutes+=NDEF_Buffer[66];

			break;
	}
	case 3:
	{
		first_hour=NDEF_Buffer[58];
					first_hour +=NDEF_Buffer[59];
					first_minutes=NDEF_Buffer[60];
					first_minutes+=NDEF_Buffer[61];

				second_hour=NDEF_Buffer[63];
					second_hour +=NDEF_Buffer[64];
					second_minutes=NDEF_Buffer[65];
					second_minutes+=NDEF_Buffer[66];

				third_hour=NDEF_Buffer[68];
					third_hour +=NDEF_Buffer[69];
					third_minutes=NDEF_Buffer[70];
					third_minutes+=NDEF_Buffer[71];

			break;
	}
	case 4:
	{
		first_hour=NDEF_Buffer[58];
			first_hour +=NDEF_Buffer[59];
			first_minutes=NDEF_Buffer[60];
			first_minutes+=NDEF_Buffer[61];

		second_hour=NDEF_Buffer[63];
			second_hour +=NDEF_Buffer[64];
			second_minutes=NDEF_Buffer[65];
			second_minutes+=NDEF_Buffer[66];

		third_hour=NDEF_Buffer[68];
			third_hour +=NDEF_Buffer[69];
			third_minutes=NDEF_Buffer[70];
			third_minutes+=NDEF_Buffer[71];

		fourth_hour=NDEF_Buffer[73];
			fourth_hour +=NDEF_Buffer[74];
			fourth_minutes=NDEF_Buffer[76];
			fourth_minutes+=NDEF_Buffer[77];

			break;
	}

	}
		//TODOOO
	//Verification which alarm should come first


		/*Re-writing basic date & time parameters to variables from EEPROM */

	 day=NDEF_Buffer[36];
	 day+=NDEF_Buffer[37];
	 month=NDEF_Buffer[39];
	 month+=NDEF_Buffer[40];
	 year=NDEF_Buffer[44];
	 year+=NDEF_Buffer[45];

	 hourNow=NDEF_Buffer[47];;
	 hourNow+=NDEF_Buffer[48];
	 minuteNow=NDEF_Buffer[50];
	 minuteNow+=NDEF_Buffer[51];
	 secondNow=NDEF_Buffer[53];
	 secondNow+=NDEF_Buffer[54];




		/*Setting current date and time, based on information from user's phone*/
		  DateToUpdate.Month = month;
		  DateToUpdate.Date = day;
		  DateToUpdate.Year = year;

		  if (HAL_RTC_SetDate(&hrtc, &DateToUpdate, RTC_FORMAT_BIN) != HAL_OK)
		    {
		      Error_Handler();
		    }


		  sTime.Hours = hourNow;
		  sTime.Minutes = minuteNow;
		  sTime.Seconds = secondNow;

		  	if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN) != HAL_OK)
		  	  {
		  	    Error_Handler();
		  	  }

			/*Setting first alarm*/

		  	//restarting alarm counting
		  	alarmNumber=0;

		  	RTC_TimeTypeDef alarmTime = {0};
		  			alarmTime.Hours=first_hour;
		  			alarmTime.Minutes=first_minutes;
		  			alarmTime.Seconds=0;

		  			alarm.Alarm=RTC_ALARM_A;
		  			alarm.AlarmTime=alarmTime;

		  	if(HAL_RTC_SetAlarm_IT(&hrtc, &alarm,RTC_FORMAT_BIN) != HAL_OK)
		  	  {
		  		Error_Handler();
		  	  }



}



void MX_NFC4_SET_Alarm(uint8_t number)
{
	switch(number)
	{

	case 0:
	{

		RTC_TimeTypeDef alarmTime = {0};
				  			alarmTime.Hours=first_hour;
				  			alarmTime.Minutes=first_minutes;
				  			alarmTime.Seconds=0;

				  			alarm.Alarm=RTC_ALARM_A;
				  			alarm.AlarmTime=alarmTime;

				  	if(HAL_RTC_SetAlarm_IT(&hrtc, &alarm,RTC_FORMAT_BIN) != HAL_OK)
				  	  {
				  		Error_Handler();
				  	  }
				  	break;
	}

	case 1:
	{

	RTC_TimeTypeDef alarmTime = {0};
			  			alarmTime.Hours=second_hour;
			  			alarmTime.Minutes=second_minutes;
			  			alarmTime.Seconds=0;

			  			alarm.Alarm=RTC_ALARM_A;
			  			alarm.AlarmTime=alarmTime;

			  	if(HAL_RTC_SetAlarm_IT(&hrtc, &alarm,RTC_FORMAT_BIN) != HAL_OK)
			  	  {
			  		Error_Handler();
			  	  }
			  	break;
	}

	case 2:
	{

		RTC_TimeTypeDef alarmTime = {0};
				  			alarmTime.Hours=third_hour;
				  			alarmTime.Minutes=third_minutes;
				  			alarmTime.Seconds=0;

				  			alarm.Alarm=RTC_ALARM_A;
				  			alarm.AlarmTime=alarmTime;

				  	if(HAL_RTC_SetAlarm_IT(&hrtc, &alarm,RTC_FORMAT_BIN) != HAL_OK)
				  	  {
				  		Error_Handler();
				  	  }
				  	break;
		}
	case 3:

		{

			RTC_TimeTypeDef alarmTime = {0};
					  			alarmTime.Hours=fourth_hour;
					  			alarmTime.Minutes=fourth_minutes;
					  			alarmTime.Seconds=0;

					  			alarm.Alarm=RTC_ALARM_A;
					  			alarm.AlarmTime=alarmTime;

					  	if(HAL_RTC_SetAlarm_IT(&hrtc, &alarm,RTC_FORMAT_BIN) != HAL_OK)
					  	  {
					  		Error_Handler();
					  	  }
					  	break;
		}


	}
}

//called by DemoCycle() when passive tag is present
void Food_Distribution(void)
{
	if(foodAmount>0)
	{
		//moving step motor
			for(int i=0;i<6400;i++)
			  {
			  HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_7);
			  HAL_Delay(1);

			  }
			//decrease foodAmount;
		foodAmount--;




	HAL_RTC_GetDate(&hrtc, &dateNow, RTC_FORMAT_BIN);

	HAL_RTC_GetTime(&hrtc, &timeNow, RTC_FORMAT_BIN);

	char time_from_RTC[17];
			char date_from_RTC[11];

		time_from_RTC[15]=timeNow.Hours;
		date_from_RTC[15]=dateNow.Date;

		//concatenate of time
		char colon[]=":";
	strcat(time_from_RTC,colon );
	strcat(time_from_RTC,timeNow.Minutes );

	//concatenate of date
	char dot[]=".";
	strcat(date_from_RTC,dot );
	strcat(date_from_RTC,dateNow.Month);
	strcat(date_from_RTC,dot);
	strcat(date_from_RTC,dateNow.Year);

	//concatenating of time and date

	strcat(time_from_RTC,date_from_RTC);


	char n[]="\n";
	//using global variable creating
	if(count_time_stamp==0)
	{
	strcat(time_stamp,time_from_RTC);
	//incrementation to be able to reset after 7 days
	count_time_stamp++;
	}
	else
		if(count_time_stamp<(alarm_amount*7))
	{
			strcat(time_stamp,n);
		strcat(time_stamp,time_from_RTC);
			//incrementation to be able to reset after 7 days
			count_time_stamp++;
	}
		else
		{
			memset(&time_stamp[0], 0, sizeof(time_stamp));
		}

//after 7 days start re-writing variable from beginning

	if(alarm_special_number==(7*alarm_amount));
	{
		alarm_special_number=0;
	}


	uint8_t j=19*alarm_special_number;
for(int i=0;i<19;i++)
{
	time_stamp[(j+i)]=time_from_RTC[i];


}
alarm_special_number++;
	MX_NFC4_NDEF_Process_Write(&time_stamp);
	}

}
/**
  * @brief  Process of the NFC4 GPO application
  * @retval None
  */
void MX_NFC4_GPO_Process(void)
{


}

/**
  * @brief  NFC4 GPO callback
  * @retval None
  */
void BSP_GPO_Callback(void)
{
  /* Prevent unused argument(s) compilation warning */
//	++cnt;
//  GPOActivated = 1;

	NFC04A1_LED_On( YELLOW_LED );
	MX_NFC4_NDEF_Process_Read();
  /* This function should be implemented by the user application.
     It is called into this driver when an event on Button is triggered. */
}

#ifdef __cplusplus
}
#endif

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

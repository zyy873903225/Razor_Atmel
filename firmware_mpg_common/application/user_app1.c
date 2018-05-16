/**********************************************************************************************************************
File: user_app.c                                                                

----------------------------------------------------------------------------------------------------------------------
To start a new task using this user_app as a template:
 1. Copy both user_app.c and user_app.h to the Application directory
 2. Rename the files yournewtaskname.c and yournewtaskname.h
 3. Add yournewtaskname.c and yournewtaskname.h to the Application Include and Source groups in the IAR project
 4. Use ctrl-h (make sure "Match Case" is checked) to find and replace all instances of "user_app" with "yournewtaskname"
 5. Use ctrl-h to find and replace all instances of "UserApp1" with "YourNewTaskName"
 6. Use ctrl-h to find and replace all instances of "USER_APP" with "YOUR_NEW_TASK_NAME"
 7. Add a call to YourNewTaskNameInitialize() in the init section of main
 8. Add a call to YourNewTaskNameRunActiveState() in the Super Loop section of main
 9. Update yournewtaskname.h per the instructions at the top of yournewtaskname.h
10. Delete this text (between the dashed lines) and update the Description below to describe your task
----------------------------------------------------------------------------------------------------------------------

Description:
This is a user_app.c file template 

------------------------------------------------------------------------------------------------------------------------
API:

Public functions:


Protected System functions:
void UserApp1Initialize(void)
Runs required initialzation for the task.  Should only be called once in main init section.

void UserApp1RunActiveState(void)
Runs current task state.  Should only be called once in main loop.


**********************************************************************************************************************/

#include "configuration.h"

/***********************************************************************************************************************
Global variable definitions with scope across entire project.
All Global variable names shall start with "G_"
***********************************************************************************************************************/
/* New variables */
volatile u32 G_u32UserApp1Flags;                       /* Global state flags */


/*--------------------------------------------------------------------------------------------------------------------*/
/* Existing variables (defined in other files -- should all contain the "extern" keyword) */
extern u32 G_u32AntApiCurrentDataTimeStamp;                       /* From ant_api.c */
extern AntApplicationMessageType G_eAntApiCurrentMessageClass;    /* From ant_api.c */
extern u8 G_au8AntApiCurrentMessageBytes[ANT_APPLICATION_MESSAGE_BYTES];  /* From ant_api.c */
extern AntExtendedDataType G_sAntApiCurrentMessageExtData;        /* From ant_api.c */

extern volatile u32 G_u32SystemFlags;                  /* From main.c */
extern volatile u32 G_u32ApplicationFlags;             /* From main.c */

extern volatile u32 G_u32SystemTime1ms;                /* From board-specific source file */
extern volatile u32 G_u32SystemTime1s;                 /* From board-specific source file */



/***********************************************************************************************************************
Global variable definitions with scope limited to this local application.
Variable names shall start with "UserApp1_" and be declared as static.
***********************************************************************************************************************/
static u32 UserApp1_u32DataMsgCount = 0;             /* Counts the number of ANT_DATA packets received */
static u32 UserApp1_u32TickMsgCount = 0;             /* Counts the number of ANT_TICK packets received */

static fnCode_type UserApp1_StateMachine;            /* The state machine function pointer */
static u32 UserApp1_u32Timeout;                      /* Timeout counter used across states */


/**********************************************************************************************************************
Function Definitions
**********************************************************************************************************************/

/*--------------------------------------------------------------------------------------------------------------------*/
/* Public functions                                                                                                   */
/*--------------------------------------------------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------------------------------------------------*/
/* Protected functions                                                                                                */
/*--------------------------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------------------------
Function: UserApp1Initialize

Description:
Initializes the State Machine and its variables.

Requires:
  -

Promises:
  - 
*/
void UserApp1Initialize(void)
{
  u8 au8WelcomeMessage[] = "     Press B0 to";
  u8 au8Instructions[] = "      power on";
  AntAssignChannelInfoType sAntSetupData;
  
  /* Clear screen and place start messages */
#ifdef EIE1
  LCDCommand(LCD_CLEAR_CMD);
  LCDMessage(LINE1_START_ADDR, au8WelcomeMessage); 
  LCDMessage(LINE2_START_ADDR, au8Instructions); 

  /* Start with LED0 in RED state = channel is not configured */
  LedOn(RED);
#endif /* EIE1 */
  
#ifdef MPG2
  PixelAddressType sStringLocation = {LCD_SMALL_FONT_LINE0, LCD_LEFT_MOST_COLUMN}; 
  LcdClearScreen();
  LcdLoadString(au8WelcomeMessage, LCD_FONT_SMALL, &sStringLocation); 
  sStringLocation.u16PixelRowAddress = LCD_SMALL_FONT_LINE1;
  LcdLoadString(au8Instructions, LCD_FONT_SMALL, &sStringLocation); 
  
  /* Start with LED0 in RED state = channel is not configured */
  LedOn(RED0);
#endif /* MPG2 */
  
 /* Configure ANT for this application */
  sAntSetupData.AntChannel          = ANT_CHANNEL_USERAPP;
  sAntSetupData.AntChannelType      = ANT_CHANNEL_TYPE_USERAPP;
  sAntSetupData.AntChannelPeriodLo  = ANT_CHANNEL_PERIOD_LO_USERAPP;
  sAntSetupData.AntChannelPeriodHi  = ANT_CHANNEL_PERIOD_HI_USERAPP;
  
  sAntSetupData.AntDeviceIdLo       = ANT_DEVICEID_LO_USERAPP;
  sAntSetupData.AntDeviceIdHi       = ANT_DEVICEID_HI_USERAPP;
  sAntSetupData.AntDeviceType       = ANT_DEVICE_TYPE_USERAPP;
  sAntSetupData.AntTransmissionType = ANT_TRANSMISSION_TYPE_USERAPP;
  sAntSetupData.AntFrequency        = ANT_FREQUENCY_USERAPP;
  sAntSetupData.AntTxPower          = ANT_TX_POWER_USERAPP;

  sAntSetupData.AntNetwork          = ANT_NETWORK_DEFAULT;
  
  sAntSetupData.AntNetworkKey[0] = N_0;
  sAntSetupData.AntNetworkKey[1] = N_1;
  sAntSetupData.AntNetworkKey[2] = N_2;
  sAntSetupData.AntNetworkKey[3] = N_3;
  sAntSetupData.AntNetworkKey[4] = N_4;
  sAntSetupData.AntNetworkKey[5] = N_5;
  sAntSetupData.AntNetworkKey[6] = N_6;
  sAntSetupData.AntNetworkKey[7] = N_7;
  
  
  
  /* If good initialization, set state to Idle */
  if( AntAssignChannel(&sAntSetupData) )
  {
    /* Channel is configured, so change LED to yellow */
#ifdef EIE1
    LedOff(RED);
    LedOn(YELLOW);
#endif /* EIE1 */
    
#ifdef MPG2
    LedOn(GREEN0);
#endif /* MPG2 */
    
    UserApp1_StateMachine = UserApp1SM_WaitChannelAssign;
  }
  else
  {
    /* The task isn't properly initialized, so shut it down and don't run */
#ifdef EIE1
    LedBlink(RED, LED_4HZ);
#endif /* EIE1 */
    
#ifdef MPG2
    LedBlink(RED0, LED_4HZ);
#endif /* MPG2 */

    UserApp1_StateMachine = UserApp1SM_Error;
  }

} /* end UserApp1Initialize() */


/*----------------------------------------------------------------------------------------------------------------------
Function UserApp1RunActiveState()

Description:
Selects and runs one iteration of the current state in the state machine.
All state machines have a TOTAL of 1ms to execute, so on average n state machines
may take 1ms / n to execute.

Requires:
  - State machine function pointer points at current state

Promises:
  - Calls the function to pointed by the state machine function pointer
*/
void UserApp1RunActiveState(void)
{
  UserApp1_StateMachine();

} /* end UserApp1RunActiveState */


/*--------------------------------------------------------------------------------------------------------------------*/
/* Private functions                                                                                                  */
/*--------------------------------------------------------------------------------------------------------------------*/


/**********************************************************************************************************************
State Machine Function Definitions
**********************************************************************************************************************/

/*-------------------------------------------------------------------------------------------------------------------*/
/* Wait for the ANT channel assignment to finish */
static void UserApp1SM_WaitChannelAssign(void)
{
  /* Check if the channel assignment is complete */
  if(AntRadioStatusChannel(ANT_CHANNEL_USERAPP) == ANT_CONFIGURED)
  {
    UserApp1_StateMachine = UserApp1SM_Idle;
  }
  
  /* Monitor for timeout */
  if( IsTimeUp(&UserApp1_u32Timeout, 3000) )
  {
    DebugPrintf("\n\r***Channel assignment timeout***\n\n\r");
    UserApp1_StateMachine = UserApp1SM_Error;
  }
      
} /* end UserApp1SM_WaitChannelAssign() */


  /*-------------------------------------------------------------------------------------------------------------------*/
/* Wait for a message to be queued */
static void UserApp1SM_Idle(void)
{
  static u8 u8MusicNoteIndex = 0;
  static u8 u8MusicTime = 0;
  static bool bMusic = FALSE;
  static u16 u16MusicNote[7] = {C6,D6,E6,F6,G6,A6,B6}; 
  
  /* Look for BUTTON 0 to open channel */
  if(WasButtonPressed(BUTTON0))
  {
    /* Got the button, so complete one-time actions before next state */
    ButtonAcknowledge(BUTTON0);
    
    /* Queue open channel and change LED0 from yellow to blinking green to indicate channel is opening */
    AntOpenChannelNumber(ANT_CHANNEL_USERAPP);
    
    bMusic = TRUE;

#ifdef MPG1
    LedOff(YELLOW);
    LedBlink(GREEN, LED_2HZ);
#endif /* MPG1 */    
    
#ifdef MPG2
    LedOff(RED0);
    LedBlink(GREEN0, LED_2HZ);
#endif /* MPG2 */
    
    /* Set timer and advance states 
    UserApp1_u32Timeout = G_u32SystemTime1ms;
    UserApp1_StateMachine = UserApp1SM_WaitChannelOpen;*/
  }
  
  if (bMusic)
  {
    u8MusicTime++;
    
    /*Delay of 10ms*/
    if (u8MusicTime == 100)
    {
      u8MusicTime = 0;
      PWMAudioSetFrequency(BUZZER1,u16MusicNote[u8MusicNoteIndex]);
      u8MusicNoteIndex++;
      PWMAudioOn(BUZZER1);
      if (u8MusicNoteIndex > 6)
      {
        u8MusicNoteIndex = 0;
        bMusic=FALSE;
        PWMAudioOff(BUZZER1);
        
        /* Set timer and advance states */
        UserApp1_u32Timeout = G_u32SystemTime1ms;
        UserApp1_StateMachine = UserApp1SM_WaitChannelOpen;
      }
    }
  }
} /* end UserApp1SM_Idle() */


/*-------------------------------------------------------------------------------------------------------------------*/
/* Wait for channel to open */
static void UserApp1SM_WaitChannelOpen(void)
{
  /* Monitor the channel status to check if channel is opened */
  if(AntRadioStatusChannel(ANT_CHANNEL_USERAPP) == ANT_OPEN)
  {
#ifdef MPG1
    LedOn(GREEN);
#endif /* MPG1 */    
    
#ifdef MPG2
    LedOn(GREEN0);
#endif /* MPG2 */
    
    UserApp1_StateMachine = UserApp1SM_ChannelOpen;
  }
  
  /* Check for timeout */
  if( IsTimeUp(&UserApp1_u32Timeout, TIMEOUT_VALUE) )
  {
    AntCloseChannelNumber(ANT_CHANNEL_USERAPP);

#ifdef MPG1
    LedOff(GREEN);
    LedOn(YELLOW);
#endif /* MPG1 */    
    
#ifdef MPG2
    LedOn(RED0);
    LedOn(GREEN0);
#endif /* MPG2 */
    
    UserApp1_StateMachine = UserApp1SM_Idle;
  }
    
} /* end UserApp1SM_WaitChannelOpen() */


/*-------------------------------------------------------------------------------------------------------------------*/
/* Channel is open, so monitor data */
static void UserApp1SM_ChannelOpen(void)
{
  static u8 u8MAX_Heart_rate                      =0;
  static u8 u8MIN_Heart_rate                      =255;
  static u8 u8MusicTime                           =0;
  static u8 u8MusicNoteIndex                      = 0;
  
  static u8 au8Current_Heart_rate[3];
  static u8 au8MAX_Heart_rate[3];
  static u8 au8MIN_Heart_rate[3];
  static u8 au8Battery_level[3];
  static u8 au8Cumulative_operating_time[8];
  static u8 au8Battery_Status[8]                  = "Invalid";
  static u8 au8Heart_rate_trendgram[101];
  
  static bool bdisplay_alarm1                     = FALSE;
  static bool bdisplay_alarm2                     = FALSE;
  static bool bToggle                             = TRUE;
  static bool bdisplay_Heart_rate                 = TRUE;
  static bool bdisplay_Cumulative_operating_time  = FALSE;
  static bool bdisplay_Battery_level              = FALSE;
  static bool bMusic                              = FALSE;
  static bool bTurn_off_screen                    = FALSE;
  
  static u8 au8askfor_Battery_level[]             = {0x46, 0xFF, 0xFF, 0xFF, 0xFF, 0x80, 0x07, 0x01};
  static u8 au8askfor_Cumulative_operating_time[] = {0x46, 0xFF, 0xFF, 0xFF, 0xFF, 0x80, 0x01, 0x01};
  static u16 u16MusicNote[7] = {B6,A6,G6,F6,E6,D6,C6};
  
  if( AntReadAppMessageBuffer() )
  {
    /* New message from ANT task: check what it is */
    if( G_eAntApiCurrentMessageClass == ANT_DATA )
    {        
      /* display heart rate trendgram */  
      for(u8 i = 0;i<100;i++)
      {
        au8Heart_rate_trendgram[i] = ' ';
      }     
      
      for(u8 i = 0;i<( G_au8AntApiCurrentMessageBytes[7] / 2 );i++)
      {
        au8Heart_rate_trendgram[i] = '*';
      }     
      
      DebugPrintf(au8Heart_rate_trendgram);
      DebugLineFeed();
      
      
      /* Record the maximum and minimum values of heart rate */     
      if( G_au8AntApiCurrentMessageBytes[7] >= u8MAX_Heart_rate )
      {
        u8MAX_Heart_rate = G_au8AntApiCurrentMessageBytes[7];
      }
      if( G_au8AntApiCurrentMessageBytes[7] <= u8MIN_Heart_rate )
      {
        u8MIN_Heart_rate = G_au8AntApiCurrentMessageBytes[7];
      }
      
      /* Convert the value of Heart Rate into an ASCII string. */
      NumberToAscii(G_au8AntApiCurrentMessageBytes[7],au8Current_Heart_rate);
      NumberToAscii(u8MAX_Heart_rate,au8MAX_Heart_rate);
      NumberToAscii(u8MIN_Heart_rate,au8MIN_Heart_rate);
      
      
      if( bdisplay_Heart_rate )
      {
        LCDCommand(LCD_CLEAR_CMD);
        LCDMessage(LINE1_START_ADDR, "Heart Rate:");
        LCDMessage(LINE1_START_ADDR + 11, au8Current_Heart_rate);
        LCDMessage(LINE1_START_ADDR + 14, "bpm");
        
        if( bdisplay_alarm1 )
        {
          LCDMessage(LINE2_START_ADDR, "Heart rate is high!");
        }
        else if( bdisplay_alarm2 )
        {
          LCDMessage(LINE2_START_ADDR, "Heart rate is low!");
        }
        else
        {
          LCDMessage(LINE2_START_ADDR, "MAX:         MIN:");
          LCDMessage(LINE2_START_ADDR + 4 , au8MAX_Heart_rate);
          //LCDMessage(LINE2_START_ADDR + 13, "MIN:");
          LCDMessage(LINE2_START_ADDR + 17, au8MIN_Heart_rate);
        }
      }
      
      
       
      
      /* it will alarm when the heart rate is less than 40 or more than 160*/
      if( G_au8AntApiCurrentMessageBytes[7] > 160 )
      {
        bdisplay_alarm1 = TRUE;
      }
      else if( G_au8AntApiCurrentMessageBytes[7] < 40 )
      {
        bdisplay_alarm2 = TRUE;
      }
      else
      {
        if( !bTurn_off_screen )
        {
          LedPWM(LCD_RED, LED_PWM_100);
          LedPWM(LCD_GREEN, LED_PWM_100);
          LedPWM(LCD_BLUE, LED_PWM_100);
        }
    
        PWMAudioOff(BUZZER2);
        
        bdisplay_alarm1 = FALSE;
        bdisplay_alarm2 = FALSE;
        
        
      }
      
      if( bdisplay_alarm1 || bdisplay_alarm2 )
      {
        if( bToggle )
        {
          LedPWM(LCD_RED, LED_PWM_100);
          LedPWM(LCD_GREEN, LED_PWM_0);
          LedPWM(LCD_BLUE, LED_PWM_0);
          
          PWMAudioSetFrequency(BUZZER2, NOTE_A6_SHARP);
          PWMAudioOn(BUZZER2);
          bToggle = FALSE;
          
          if( bdisplay_alarm1 )
          {
            LCDMessage(LINE2_START_ADDR, "Heart rate is high!");
          }
          else if( bdisplay_alarm2 )
          {
            LCDMessage(LINE2_START_ADDR, "Heart rate is low!");
          }
        }
        else
        {    
          if( !bTurn_off_screen )
          {
            LedPWM(LCD_RED, LED_PWM_100);
            LedPWM(LCD_GREEN, LED_PWM_100);
            LedPWM(LCD_BLUE, LED_PWM_100);
          }
          else
          {
            LedPWM(LCD_RED, LED_PWM_0);
            LedPWM(LCD_GREEN, LED_PWM_0);
            LedPWM(LCD_BLUE, LED_PWM_0);
          }
          
          PWMAudioSetFrequency(BUZZER2, NOTE_D6_SHARP);
          bToggle = TRUE;
        }
      }
      else if( bTurn_off_screen ) 
      {       
        LCDCommand(LCD_CLEAR_CMD);
      }
      
      
      
      
      /* We got some data about cumulative operating time */  
      if( G_au8AntApiCurrentMessageBytes[0] == 0x01 || G_au8AntApiCurrentMessageBytes[0] == 0x81 )
      {
        /*将这个十六进制化为十进制再乘2 就是累积运行时间   单位为秒  
        最大范围33554432s  约9320小时*/       
        au8Cumulative_operating_time[1] = G_au8AntApiCurrentMessageBytes[1] * 2;    
        
        /* Convert the value of cumulative operating time into an ASCII string. */
        NumberToAscii(G_au8AntApiCurrentMessageBytes[1], au8Cumulative_operating_time);
        
      }
      
      if( bdisplay_Cumulative_operating_time )
      {
        LCDCommand(LCD_CLEAR_CMD);
        LCDMessage(LINE1_START_ADDR, "You have use it:");
        LCDMessage(LINE2_START_ADDR, au8Cumulative_operating_time);
        LCDMessage(LINE2_START_ADDR + 8, "Seconds");
      }
      
      /* We got some data about battery status */
      if( G_au8AntApiCurrentMessageBytes[0] == 0x07 || G_au8AntApiCurrentMessageBytes[0] == 0x87 )
      {
        /* Convert the value of battery level into an ASCII string. */
        NumberToAscii(G_au8AntApiCurrentMessageBytes[1], au8Battery_level);
        
        if( ( G_au8AntApiCurrentMessageBytes[3] >= 0x10 && G_au8AntApiCurrentMessageBytes[3] <= 0x1F ) || ( G_au8AntApiCurrentMessageBytes[3] >= 0x90 && G_au8AntApiCurrentMessageBytes[3] <= 0x9F ) )
        {
          strcpy(au8Battery_Status,"New");
        }
        
        if( ( G_au8AntApiCurrentMessageBytes[3] >= 0x20 && G_au8AntApiCurrentMessageBytes[3] <= 0x2F ) || ( G_au8AntApiCurrentMessageBytes[3] >= 0xA0 && G_au8AntApiCurrentMessageBytes[3] <= 0xAF ) )
        {
          strcpy(au8Battery_Status,"Good");
        }
        
        if( ( G_au8AntApiCurrentMessageBytes[3] >= 0x30 && G_au8AntApiCurrentMessageBytes[3] <= 0x3F ) || ( G_au8AntApiCurrentMessageBytes[3] >= 0xB0 && G_au8AntApiCurrentMessageBytes[3] <= 0xBF ) )
        {
          strcpy(au8Battery_Status,"Ok");
        }
        
        if( ( G_au8AntApiCurrentMessageBytes[3] >= 0x40 && G_au8AntApiCurrentMessageBytes[3] <= 0x4F ) || ( G_au8AntApiCurrentMessageBytes[3] >= 0xC0 && G_au8AntApiCurrentMessageBytes[3] <= 0xCF ) )
        {
          strcpy(au8Battery_Status,"Low");
        }
        
        if( ( G_au8AntApiCurrentMessageBytes[3] >= 0x50 && G_au8AntApiCurrentMessageBytes[3] <= 0x5F ) || ( G_au8AntApiCurrentMessageBytes[3] >= 0xD0 && G_au8AntApiCurrentMessageBytes[3] <= 0xDF ) )
        {
          strcpy(au8Battery_Status,"Critical");
        }       
      }
      
      if( bdisplay_Battery_level )
      {
        LCDCommand(LCD_CLEAR_CMD);
        LCDMessage(LINE1_START_ADDR, "Battery Level:");
        LCDMessage(LINE2_START_ADDR, au8Battery_Status);  
        LCDMessage(LINE2_START_ADDR+13, au8Battery_level);  
        LCDMessage(LINE2_START_ADDR+16, "%");  
      }
      
    }
  } /* end AntReadData() */
      
  
  
  
  /* Press button 0 to display the value of Heart Rate */
  if(WasButtonPressed(BUTTON0))
  {
    /* Got the button, so complete one-time actions before next state */
    ButtonAcknowledge(BUTTON0);
    
    LedPWM(LCD_RED, LED_PWM_100);
    LedPWM(LCD_GREEN, LED_PWM_100);
    LedPWM(LCD_BLUE, LED_PWM_100);
    
    bdisplay_Heart_rate = TRUE;
    bdisplay_Cumulative_operating_time = FALSE;
    bdisplay_Battery_level = FALSE;
    bTurn_off_screen = FALSE;
  }
  
  /* Press button 1 to display the battery level */ 
  if(WasButtonPressed(BUTTON1))
  {
    /* Got the button, so complete one-time actions before next state */
    ButtonAcknowledge(BUTTON1);
    
    AntQueueBroadcastMessage(ANT_CHANNEL_USERAPP, au8askfor_Cumulative_operating_time);
    
    LedPWM(LCD_RED, LED_PWM_100);
    LedPWM(LCD_GREEN, LED_PWM_100);
    LedPWM(LCD_BLUE, LED_PWM_100);
    
    bdisplay_Heart_rate = FALSE;
    bdisplay_Cumulative_operating_time = TRUE;
    bdisplay_Battery_level = FALSE;
    bTurn_off_screen = FALSE;
  }
  
  
  if(WasButtonPressed(BUTTON2))
  {
    /* Got the button, so complete one-time actions before next state */
    ButtonAcknowledge(BUTTON2);
    
    AntQueueBroadcastMessage(ANT_CHANNEL_USERAPP, au8askfor_Battery_level);
    
    LedPWM(LCD_RED, LED_PWM_100);
    LedPWM(LCD_GREEN, LED_PWM_100);
    LedPWM(LCD_BLUE, LED_PWM_100);
    
    bdisplay_Heart_rate = FALSE; 
    bdisplay_Cumulative_operating_time = FALSE;
    bdisplay_Battery_level = TRUE;
    bTurn_off_screen = FALSE;
  }
  
  if(WasButtonPressed(BUTTON3))
  {
    /* Got the button, so complete one-time actions before next state */
    ButtonAcknowledge(BUTTON3);
    
    LCDCommand(LCD_CLEAR_CMD);
    LedPWM(LCD_RED, LED_PWM_0);
    LedPWM(LCD_GREEN, LED_PWM_0);
    LedPWM(LCD_BLUE, LED_PWM_0);
    
    bdisplay_Heart_rate = FALSE; 
    bdisplay_Cumulative_operating_time = FALSE;
    bdisplay_Battery_level = FALSE;
    bMusic = TRUE;
    bTurn_off_screen  = TRUE;
  }
  
  
  if (bMusic)
  {
    u8MusicTime++;
    
    /*Delay of 10ms*/
    if (u8MusicTime == 100)
    {
      u8MusicTime = 0;
      PWMAudioSetFrequency(BUZZER1,u16MusicNote[u8MusicNoteIndex]);
      u8MusicNoteIndex++;
      PWMAudioOn(BUZZER1);
      if (u8MusicNoteIndex > 6)
      {
        u8MusicNoteIndex = 0;
        bMusic=FALSE;
        PWMAudioOff(BUZZER1);
      }
    }
  }

} /* end UserApp1SM_ChannelOpen() */


/*-------------------------------------------------------------------------------------------------------------------*/
/* Wait for channel to close */
static void UserApp1SM_WaitChannelClose(void)
{
  /* Monitor the channel status to check if channel is closed */
  if(AntRadioStatusChannel(ANT_CHANNEL_USERAPP) == ANT_CLOSED)
  {
#ifdef MPG1
    LedOff(GREEN);
    LedOn(YELLOW);
#endif /* MPG1 */
    
#ifdef MPG2
    LedOn(GREEN0);
    LedOn(RED0);
#endif /* MPG2 */
    UserApp1_StateMachine = UserApp1SM_Idle;
  }
  
  /* Check for timeout */
  if( IsTimeUp(&UserApp1_u32Timeout, TIMEOUT_VALUE) )
  {
#ifdef MPG1
    LedOff(GREEN);
    LedOff(YELLOW);
    LedBlink(RED, LED_4HZ);
#endif /* MPG1 */
    
#ifdef MPG2
    LedBlink(RED0, LED_4HZ);
    LedOff(GREEN0);
#endif /* MPG2 */
    
    UserApp1_StateMachine = UserApp1SM_Error;
  }
  
} /* end UserApp1SM_WaitChannelClose() */


/*-------------------------------------------------------------------------------------------------------------------*/
/* Handle an error */
static void UserApp1SM_Error(void)          
{
  
} /* end UserApp1SM_Error() */



/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/

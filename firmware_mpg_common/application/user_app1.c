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

static AntAssignChannelInfoType UserApp1_sMasterChannel;
static AntAssignChannelInfoType UserApp1_sSlaveChannel;
  
static u8 ANT_CHANNEL_USERAPP = ANT_CHANNEL_0;
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
  u8 au8WelcomeMessage[] = "Hide and Go Seek!";
  
  /* set BUZZER 1 */
  PWMAudioSetFrequency(BUZZER1,700);
  
  /* Clear screen and place welcome messages */
#ifdef EIE1
  LCDCommand(LCD_CLEAR_CMD);
  LCDMessage(LINE1_START_ADDR, au8WelcomeMessage); 

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
   
  /* Configure ANT for this Master and Slave */
  /* Master (Channel 0) */
  UserApp1_sMasterChannel.AntChannel          = ANT_CHANNEL_0;
  UserApp1_sMasterChannel.AntChannelType      = CHANNEL_TYPE_MASTER;
  UserApp1_sMasterChannel.AntChannelPeriodHi  = ANT_CHANNEL_PERIOD_HI_USERAPP;
  UserApp1_sMasterChannel.AntChannelPeriodLo  = ANT_CHANNEL_PERIOD_LO_USERAPP;
  
  UserApp1_sMasterChannel.AntDeviceIdHi       = ANT_DEVICEID_HI_USERAPP;
  UserApp1_sMasterChannel.AntDeviceIdLo       = ANT_DEVICEID_LO_USERAPP;;
  UserApp1_sMasterChannel.AntDeviceType       = ANT_DEVICE_TYPE_USERAPP;
  UserApp1_sMasterChannel.AntTransmissionType = ANT_TRANSMISSION_TYPE_USERAPP;
  
  UserApp1_sMasterChannel.AntFrequency        = ANT_FREQUENCY_USERAPP;
  UserApp1_sMasterChannel.AntTxPower          = ANT_TX_POWER_USERAPP;
  UserApp1_sMasterChannel.AntNetwork          = ANT_NETWORK_DEFAULT;
  
  /* Slave (Channel 1) */
  UserApp1_sSlaveChannel.AntChannel           = ANT_CHANNEL_1;
  UserApp1_sSlaveChannel.AntChannelType       = CHANNEL_TYPE_SLAVE;
  UserApp1_sSlaveChannel.AntChannelPeriodHi   = ANT_CHANNEL_PERIOD_HI_USERAPP;
  UserApp1_sSlaveChannel.AntChannelPeriodLo   = ANT_CHANNEL_PERIOD_LO_USERAPP;
  
  UserApp1_sSlaveChannel.AntDeviceIdHi        = ANT_DEVICEID_HI_USERAPP;
  UserApp1_sSlaveChannel.AntDeviceIdLo        = ANT_DEVICEID_LO_USERAPP;
  UserApp1_sSlaveChannel.AntDeviceType        = ANT_DEVICE_TYPE_USERAPP;
  UserApp1_sSlaveChannel.AntTransmissionType  = ANT_TRANSMISSION_TYPE_USERAPP;
  
  UserApp1_sSlaveChannel.AntFrequency         = ANT_FREQUENCY_USERAPP;
  UserApp1_sSlaveChannel.AntTxPower           = ANT_TX_POWER_USERAPP;
  UserApp1_sSlaveChannel.AntNetwork           = ANT_NETWORK_DEFAULT;
  
  for(u8 i = 0; i < ANT_NETWORK_NUMBER_BYTES; i++)
  {
    UserApp1_sMasterChannel.AntNetworkKey[i] = ANT_DEFAULT_NETWORK_KEY;
    UserApp1_sSlaveChannel.AntNetworkKey[i] = ANT_DEFAULT_NETWORK_KEY;
  }
 
  /* If good initialization, set state to Idle */
 
 if( AntAssignChannel(&UserApp1_sMasterChannel) )
  {
    /* Channel is configured, so change LED to yellow */
#ifdef EIE1
    LedOff(RED);
    LedOn(YELLOW);
#endif /* EIE1 */
    
#ifdef MPG2
    LedOn(GREEN0);
#endif /* MPG2 */
    
    UserApp1_StateMachine = UserApp1SM_WaitMasterChannelAssign;
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
static void UserApp1SM_WaitMasterChannelAssign(void)
{
  /* Check if  the channel 0 assignment is complete */
  if( ( AntRadioStatusChannel(ANT_CHANNEL_0) == ANT_CONFIGURED ) )
  {
    if( AntAssignChannel(&UserApp1_sSlaveChannel) )
    {
      /* Channel is configured, so change LED to yellow */
#ifdef EIE1
      LedOff(RED);
      LedOn(YELLOW);
#endif /* EIE1 */
      
#ifdef MPG2
      LedOn(GREEN0);
#endif /* MPG2 */
      
      UserApp1_StateMachine = UserApp1SM_WaitSlaveChannelAssign;
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
  }
  
  /* Monitor for timeout */
  if( IsTimeUp(&UserApp1_u32Timeout, 3000) )
  {
    DebugPrintf("\n\r***Channel assignment timeout***\n\n\r");
    UserApp1_StateMachine = UserApp1SM_Error;
  }
  
} /* end UserApp1SM_WaitMasterChannelAssign() */


/*-------------------------------------------------------------------------------------------------------------------*/
/* Wait for the ANT channel assignment to finish */
static void UserApp1SM_WaitSlaveChannelAssign(void)
{
  /* Check if  the channel 1 assignment is complete */
  if( ( AntRadioStatusChannel(ANT_CHANNEL_1) == ANT_CONFIGURED ) )
  {
    UserApp1_StateMachine = UserApp1SM_ChooseChannel;
  }
  
  /* Monitor for timeout */
  if( IsTimeUp(&UserApp1_u32Timeout, 3000) )
  {
    DebugPrintf("\n\r***Channel assignment timeout***\n\n\r");
    UserApp1_StateMachine = UserApp1SM_Error;
  }
      
} /* end UserApp1SM_WaitSlaveChannelAssign() */


/*-------------------------------------------------------------------------------------------------------------------*/
/* Wait for choose Master or Slave  */
static void UserApp1SM_ChooseChannel(void)
{
  u8 au8Instructions[]   = " Press B0 to Start!";
  u8 auChooseMessage[]   = "Choose Your Role!";
  u8 auRoleMessage[]     = "B1:Hider   B2:Seeker";
  static u16 u16delay    = 0;
  
  u16delay++;
  if( u16delay == 500 )
  {
    u16delay = 0;
    LCDCommand( LCD_CLEAR_CMD );
    LCDMessage( LINE1_START_ADDR , auChooseMessage );
    LCDMessage( LINE2_START_ADDR , auRoleMessage );
  }
  
  /* Look for BUTTON 1 to open channel 0 */
  if(WasButtonPressed(BUTTON1))
  {
    /* Got the button, so complete one-time actions before next state */
    ButtonAcknowledge(BUTTON1);
    
    ANT_CHANNEL_USERAPP = ANT_CHANNEL_0;
    
    LCDCommand( LCD_CLEAR_CMD );
    LCDMessage( LINE1_START_ADDR , "      Hider!!!" );
    LCDMessage( LINE2_START_ADDR , au8Instructions );
    
    UserApp1_StateMachine = UserApp1SM_Idle;
  }
  
  /* Look for BUTTON 2 to open channel 1 */
  if(WasButtonPressed(BUTTON2))
  {
    /* Got the button, so complete one-time actions before next state */
    ButtonAcknowledge(BUTTON2);
    
    ANT_CHANNEL_USERAPP = ANT_CHANNEL_1;
    
    LCDCommand( LCD_CLEAR_CMD );
    LCDMessage( LINE1_START_ADDR , "     Seeker!!!" );
    LCDMessage( LINE2_START_ADDR , au8Instructions );
    
    UserApp1_StateMachine = UserApp1SM_Idle;
  }
}


/*-------------------------------------------------------------------------------------------------------------------*/
/* Wait for a message to be queued */
static void UserApp1SM_Idle(void)
{
  u8 auSeekerMessage[] = "Your role is Seeker!";
  u8 auHideMessage[]   = "Your role is Hider!";
  
  
  /* Look for BUTTON 0 to open channel */
  if(WasButtonPressed(BUTTON0))
  {
    /* Got the button, so complete one-time actions before next state */
    ButtonAcknowledge(BUTTON0);
    
    /* Queue open channel and change LED0 from yellow to blinking green to indicate channel is opening */
    AntOpenChannelNumber(ANT_CHANNEL_USERAPP);
    
    /*Display "Seeker!" or "Hide!"*/
    LCDCommand( LCD_CLEAR_CMD );
    if( ANT_CHANNEL_USERAPP == ANT_CHANNEL_0 ) 
    {
      LCDMessage(LINE1_START_ADDR, auHideMessage);
    }
    else if( ANT_CHANNEL_USERAPP == ANT_CHANNEL_1 ) 
    {
      LCDMessage(LINE1_START_ADDR, auSeekerMessage);
    }
    
    /* Set timer and advance states */
    UserApp1_u32Timeout = G_u32SystemTime1ms;
    UserApp1_StateMachine = UserApp1SM_WaitChannelOpen;
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
    if( ANT_CHANNEL_USERAPP == ANT_CHANNEL_0 ) 
    {
      UserApp1_StateMachine = UserApp1SM_MasterChannelOpen;
    }
    else if( ANT_CHANNEL_USERAPP == ANT_CHANNEL_1 ) 
    {
      UserApp1_StateMachine = UserApp1SM_SlaveChannelOpen;
    }
    
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
/* Channel of Master is open, so monitor data */
static void UserApp1SM_MasterChannelOpen(void)
{
  u8 auHideFound[]             = "You found me!";
  u8 au8TestMessage[]          = {0xAB, 0xCD, 0xEF, 0xAB, 0xCD, 0xEF, 0xAB, 0xCD};
  static u16 u16_10s_countdown = 0;
  static bool bgameover        = FALSE;
  static bool b10s_countdown   = TRUE;
  
  
  
  /* 10 second countdown */
  if( b10s_countdown )
  {
    u16_10s_countdown++;
    if( u16_10s_countdown == 10000 )
    {
      /* set the countdown to 0 */
      u16_10s_countdown = 0;
      b10s_countdown = FALSE;
      
      /* Update LCD to starting screen. */
      LCDCommand( LCD_CLEAR_CMD );
      LCDMessage( LINE1_START_ADDR, "10s has gone!" );
      LCDMessage( LINE2_START_ADDR, "Seeker is finding!" );     
    }
  }
  
  if( (!b10s_countdown) && (!bgameover) )
  {
    //AntQueueBroadcastMessage(ANT_CHANNEL_0, au8TestMessage);
    
    if( AntReadAppMessageBuffer() )
    
      if(G_eAntApiCurrentMessageClass == ANT_DATA)
      {
        bgameover = TRUE;
        
        LCDCommand( LCD_CLEAR_CMD );
        LCDMessage( LINE1_START_ADDR , auHideFound );
        LCDMessage( LINE2_START_ADDR , "GAME OVER B0:RESTART" );
        
      }
      LedOn(WHITE);
    
  }
  
  if( bgameover )
  {
    if(WasButtonPressed(BUTTON0))
    {
      /* Got the button, so complete one-time actions before next state */
      ButtonAcknowledge(BUTTON0);
       
      LedOff(WHITE);
      LedOff(PURPLE);
      LedOff(BLUE);
      LedOff(CYAN);
      LedOff(GREEN);
      LedOff(YELLOW);
      LedOff(ORANGE);
      LedOff(RED);
      
      bgameover = FALSE;
      b10s_countdown = TRUE;
      
      AntCloseChannelNumber( ANT_CHANNEL_0 );
      
      UserApp1_u32Timeout = G_u32SystemTime1ms;
      UserApp1_StateMachine = UserApp1SM_MasterWaitChannelClose;
    }
  }
  
   /* press B3:restart */
  if(WasButtonPressed(BUTTON3))
    {
      /* Got the button, so complete one-time actions before next state */
      
      ButtonAcknowledge(BUTTON3);
      
      LedOff(WHITE);
      LedOff(PURPLE);
      LedOff(BLUE);
      LedOff(CYAN);
      LedOff(GREEN);
      LedOff(YELLOW);
      LedOff(ORANGE);
      LedOff(RED);
      
      bgameover = FALSE;
      b10s_countdown = TRUE;
      
      AntCloseChannelNumber( ANT_CHANNEL_0 );
      
      UserApp1_u32Timeout = G_u32SystemTime1ms;
      UserApp1_StateMachine = UserApp1SM_MasterWaitChannelClose;
    }
  
}


/*-------------------------------------------------------------------------------------------------------------------*/
/* Channel is of Slave open, so monitor data */
static void UserApp1SM_SlaveChannelOpen(void)
{
  u8 auSeekerMessage1[]                       = "Ready or not!";
  u8 auSeekerMessage2[]                       = "Here I come!";
  u8 auSeekerSucceed[]                        = "Found you!";
  u16 song[] = {
                  277,277,415,415,466,466,415,
                  370,370,330,330,311,311,277,
                  415,415,370,370,330,330,311,
                  415,415,370,370,330,330,311,
                  277,277,415,415,466,466,415,
                  370,370,330,330,311,311,277
                };
  
  static u16 u8MusicTime                      = 0;
  static u8 songindex                         = 0;
  static s8 s8Last_RSSI                       = 0;
  static u16 u16_10s_countdown                = 0;
  static u16 u16Set_Sound_Frequency_of_BUZZER = 1400;
  static u16 u16Sound_Frequency_of_BUZZER     = 0;
  static u8 au8RSSI_value[]                   = "0";
  static u8 au8TestMessage[]                  = {0xAB, 0xCD, 0xEF, 0xAB, 0xCD, 0xEF, 0xAB, 0xCD};
  static bool bgameover                       = FALSE;
  static bool b10s_countdown                  = TRUE;
  static bool bBUZZER                         = FALSE;
  
  /* 10 second countdown */
  if( b10s_countdown )
  {
    u16_10s_countdown++;
    if( u16_10s_countdown == 10000 )
    {
      /* set the countdown to 0 */
      u16_10s_countdown = 0;
      b10s_countdown = FALSE;
      
      /* Update LCD to starting screen. */
      LCDCommand( LCD_CLEAR_CMD );
      LCDMessage( LINE1_START_ADDR, auSeekerMessage1 );
      LCDMessage( LINE2_START_ADDR, auSeekerMessage2 );    
    }
  }
  
  if( !b10s_countdown && !bgameover )
  {
    /* Change Sound Frequency of BUZZER according to distance. */
    u16Sound_Frequency_of_BUZZER++;
    if( u16Sound_Frequency_of_BUZZER == 100 )
    {
      PWMAudioOff(BUZZER1);
    }
    
    if( u16Sound_Frequency_of_BUZZER == u16Set_Sound_Frequency_of_BUZZER || u16Sound_Frequency_of_BUZZER == 1400 )
    {
      u16Sound_Frequency_of_BUZZER = 0;
      PWMAudioOn(BUZZER1);
    }
    
    
    if( AntReadAppMessageBuffer() )
    {
      if(G_eAntApiCurrentMessageClass == ANT_DATA)
      {
         /* display RSSI */
        if( s8Last_RSSI != G_sAntApiCurrentMessageExtData.s8RSSI )
        {
          s8Last_RSSI = G_sAntApiCurrentMessageExtData.s8RSSI;
          
          AntGetdBmAscii( s8Last_RSSI , au8RSSI_value );
          
          LCDCommand( LCD_CLEAR_CMD );
          LCDMessage( LINE1_START_ADDR , "Seeking..." );
          LCDMessage( LINE1_START_ADDR + 9 , "RSSI:" );
          LCDMessage( LINE2_START_ADDR, au8RSSI_value );
          LCDMessage( LINE2_START_ADDR + 3 , "dBm" );
        }             
        
        if( G_sAntApiCurrentMessageExtData.s8RSSI > -45 && G_sAntApiCurrentMessageExtData.s8RSSI <= -40 )
        {
          /* change LED */
          LedOn(WHITE);
          LedOn(PURPLE);
          LedOn(BLUE);
          LedOn(CYAN);
          LedOn(GREEN);
          LedOn(YELLOW);
          LedOn(ORANGE);
          LedOn(RED);
          
          bgameover = TRUE;
          bBUZZER   = TRUE;
          
          /* Tell hider that seeker has found him */
          AntQueueBroadcastMessage(ANT_CHANNEL_1, au8TestMessage);
          
          /* Update LCD to starting screen. */
          LCDCommand( LCD_CLEAR_CMD );
          LCDMessage( LINE1_START_ADDR, auSeekerSucceed );
          LCDMessage( LINE2_START_ADDR , "GAME OVER B0:RESTART" );
        }
        
        if( G_sAntApiCurrentMessageExtData.s8RSSI > -56 && G_sAntApiCurrentMessageExtData.s8RSSI <= -45 )
        {    
          /* change LED */
          LedOn(WHITE);
          LedOn(PURPLE);
          LedOn(BLUE);
          LedOn(CYAN);
          LedOn(GREEN);
          LedOn(YELLOW);
          LedOn(ORANGE);
          LedOff(RED);
          
          u16Set_Sound_Frequency_of_BUZZER = 200;
          
        }
        
        if( G_sAntApiCurrentMessageExtData.s8RSSI > -64 && G_sAntApiCurrentMessageExtData.s8RSSI <= -56 )
        {
          /* change LED */
          LedOn(WHITE);
          LedOn(PURPLE);
          LedOn(BLUE);
          LedOn(CYAN);
          LedOn(GREEN);
          LedOn(YELLOW);
          LedOff(ORANGE);
          LedOff(RED);
          
          u16Set_Sound_Frequency_of_BUZZER = 400;
          
        }
        
        if( G_sAntApiCurrentMessageExtData.s8RSSI > -72 && G_sAntApiCurrentMessageExtData.s8RSSI <= -64 )
        {
          /* change LED */
          LedOn(WHITE);
          LedOn(PURPLE);
          LedOn(BLUE);
          LedOn(CYAN);
          LedOn(GREEN);
          LedOff(YELLOW);
          LedOff(ORANGE);
          LedOff(RED);
          
          u16Set_Sound_Frequency_of_BUZZER = 600;
          
        }
        
        if( G_sAntApiCurrentMessageExtData.s8RSSI > -80 && G_sAntApiCurrentMessageExtData.s8RSSI <= -72 )
        {
          /* change LED */
          LedOn(WHITE);
          LedOn(PURPLE);
          LedOn(BLUE);
          LedOn(CYAN);
          LedOff(GREEN);
          LedOff(YELLOW);
          LedOff(ORANGE);
          LedOff(RED);
          
          u16Set_Sound_Frequency_of_BUZZER = 800;
          
        }
        
        if( G_sAntApiCurrentMessageExtData.s8RSSI > -88 && G_sAntApiCurrentMessageExtData.s8RSSI <= -80 )
        {
          /* change LED */
          LedOn(WHITE);
          LedOn(PURPLE);
          LedOn(BLUE);
          LedOff(CYAN);
          LedOff(GREEN);
          LedOff(YELLOW);
          LedOff(ORANGE);
          LedOff(RED);
          
          u16Set_Sound_Frequency_of_BUZZER = 1000;
          
        }
        
        if( G_sAntApiCurrentMessageExtData.s8RSSI > -96 && G_sAntApiCurrentMessageExtData.s8RSSI <= -88 )
        {
          /* change LED */
          LedOff(WHITE);
          LedOn(PURPLE);
          LedOff(BLUE);
          LedOff(CYAN);
          LedOff(GREEN);
          LedOff(YELLOW);
          LedOff(ORANGE);
          LedOff(RED);
          
          u16Set_Sound_Frequency_of_BUZZER = 1200;
          
        }
        
        if(G_sAntApiCurrentMessageExtData.s8RSSI <= -96 )
        {
          /* change LED */
          LedOn(WHITE);
          LedOff(PURPLE);
          LedOff(BLUE);
          LedOff(CYAN);
          LedOff(GREEN);
          LedOff(YELLOW);
          LedOff(ORANGE);
          LedOff(RED);
          
          u16Set_Sound_Frequency_of_BUZZER = 1400;
          
        }
      }
    }   
  }
  
  if( bBUZZER )
  {
    u8MusicTime++;
    if ( u8MusicTime == 400 )
    {
      if( songindex == 41)
      {
        songindex = 0;
      }
      u8MusicTime = 0;
      PWMAudioSetFrequency( BUZZER2 , song[songindex] );
      PWMAudioOn( BUZZER2 );
      PWMAudioOff( BUZZER1 );
      songindex++;  
    }
    
    if ( u8MusicTime == 1000 )
    {
      u8MusicTime = 0;
      PWMAudioOff( BUZZER2 );
    }
  }
  
  if( bgameover )
  {
    if(WasButtonPressed(BUTTON0))
    {
      /* Got the button, so complete one-time actions before next state */
      
      ButtonAcknowledge(BUTTON0);
      
      LedOff(WHITE);
      LedOff(PURPLE);
      LedOff(BLUE);
      LedOff(CYAN);
      LedOff(GREEN);
      LedOff(YELLOW);
      LedOff(ORANGE);
      LedOff(RED);
      
      bgameover = FALSE;
      bBUZZER = FALSE;
      b10s_countdown = TRUE;
      PWMAudioOff( BUZZER2 );
      
      AntCloseChannelNumber( ANT_CHANNEL_1 );
      
      UserApp1_u32Timeout = G_u32SystemTime1ms;
      UserApp1_StateMachine = UserApp1SM_SlaveWaitChannelClose;
    }
  }
  
  /* press B3:restart */
  if(WasButtonPressed(BUTTON3))
    {
      /* Got the button, so complete one-time actions before next state */
      
      ButtonAcknowledge(BUTTON3);
      
      LedOff(WHITE);
      LedOff(PURPLE);
      LedOff(BLUE);
      LedOff(CYAN);
      LedOff(GREEN);
      LedOff(YELLOW);
      LedOff(ORANGE);
      LedOff(RED);
      
      bgameover = FALSE;
      bBUZZER = FALSE;
      b10s_countdown = TRUE;
      PWMAudioOff( BUZZER2 );
      
      AntCloseChannelNumber( ANT_CHANNEL_1 );
      
      UserApp1_u32Timeout = G_u32SystemTime1ms;
      UserApp1_StateMachine = UserApp1SM_SlaveWaitChannelClose;
    }
} /* end UserApp1SM_ChannelOpen() */


/*-------------------------------------------------------------------------------------------------------------------*/
/* Wait for channel to close */
static void UserApp1SM_MasterWaitChannelClose(void)
{
  /* Monitor the channel status to check if channel is closed */
  if(AntRadioStatusChannel(ANT_CHANNEL_0) == ANT_CLOSED)
  {
#ifdef MPG1
    LedOff(GREEN);
    LedOn(YELLOW);
#endif /* MPG1 */

#ifdef MPG2
    LedOn(GREEN0);
    LedOn(RED0);
#endif /* MPG2 */
    UserApp1_StateMachine = UserApp1SM_ChooseChannel;
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
/* Wait for channel to close */
static void UserApp1SM_SlaveWaitChannelClose(void)
{
  /* Monitor the channel status to check if channel is closed */
  if(AntRadioStatusChannel(ANT_CHANNEL_1) == ANT_CLOSED)
  {
#ifdef MPG1
    LedOff(GREEN);
    LedOn(YELLOW);
#endif /* MPG1 */

#ifdef MPG2
    LedOn(GREEN0);
    LedOn(RED0);
#endif /* MPG2 */
    UserApp1_StateMachine = UserApp1SM_ChooseChannel;
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

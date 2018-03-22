/**********************************************************************************************************************
File: user_app1.c                                                                

----------------------------------------------------------------------------------------------------------------------
To start a new task using this user_app1 as a template:
 1. Copy both user_app1.c and user_app1.h to the Application directory
 2. Rename the files yournewtaskname.c and yournewtaskname.h
 3. Add yournewtaskname.c and yournewtaskname.h to the Application Include and Source groups in the IAR project
 4. Use ctrl-h (make sure "Match Case" is checked) to find and replace all instances of "user_app1" with "yournewtaskname"
 5. Use ctrl-h to find and replace all instances of "UserApp1" with "YourNewTaskName"
 6. Use ctrl-h to find and replace all instances of "USER_APP1" with "YOUR_NEW_TASK_NAME"
 7. Add a call to YourNewTaskNameInitialize() in the init section of main
 8. Add a call to YourNewTaskNameRunActiveState() in the Super Loop section of main
 9. Update yournewtaskname.h per the instructions at the top of yournewtaskname.h
10. Delete this text (between the dashed lines) and update the Description below to describe your task
----------------------------------------------------------------------------------------------------------------------

Description:
This is a user_app1.c file template 

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
All Global variable names shall start with "G_UserApp1"
***********************************************************************************************************************/
/* New variables */
volatile u32 G_u32UserApp1Flags;                       /* Global state flags */


/*--------------------------------------------------------------------------------------------------------------------*/
/* Existing variables (defined in other files -- should all contain the "extern" keyword) */
extern volatile u32 G_u32SystemFlags;                  /* From main.c */
extern volatile u32 G_u32ApplicationFlags;             /* From main.c */

extern volatile u32 G_u32SystemTime1ms;                /* From board-specific source file */
extern volatile u32 G_u32SystemTime1s;                 /* From board-specific source file */

extern u8 G_au8DebugScanfBuffer[];  /* From debug.c */
extern u8 G_u8DebugScanfCharCount;  /* From debug.c */

/***********************************************************************************************************************
Global variable definitions with scope limited to this local application.
Variable names shall start with "UserApp1_" and be declared as static.
***********************************************************************************************************************/
static fnCode_type UserApp1_StateMachine;            /* The state machine function pointer */
//static u32 UserApp1_u32Timeout;                      /* Timeout counter used across states */


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
 
  /* If good initialization, set state to Idle */
  if( 1 )
  {
    UserApp1_StateMachine = UserApp1SM_Idle;
  }
  else
  {
    /* The task isn't properly initialized, so shut it down and don't run */
    UserApp1_StateMachine = UserApp1SM_Error;
  }

  LedOff(RED);
  LedOff(WHITE);
  LedOff(PURPLE);
  LedOff(ORANGE);
  LedOff(GREEN);
  LedOff(CYAN);
  LedOff(BLUE);
  LedOff(YELLOW);
  
  PWMAudioSetFrequency(BUZZER1, 500);
  PWMAudioSetFrequency(BUZZER2, 2000);
  
  LCDCommand(LCD_CLEAR_CMD);
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
/* Wait for ??? */
static void UserApp1SM_Idle(void)
{
  u8 au8Message_state1[]="Entering state1\n\n\r";
  
  u8 au8Message_state2[]="Entering state2\n\n\r";
  
  u8 au8Message_error[]="Error! Press Button1 or Button2\n\n\r";
  u8 au8Message_error1[]="Error!   Press \n\n\r";
  u8 au8Message_error2[]="Button1 or Button2\n\n\r";
  
  static u32 u32time_BUZZER=0; //timer
  static bool bstart_BUZZER=FALSE; //100ms 200Hz tone every second for BUZZER1
  
  //STATE 1
  if( WasButtonPressed(BUTTON1) || G_au8DebugScanfBuffer[0]=='1' )
  {
    //about Debug
    DebugScanf(G_au8DebugScanfBuffer);
    DebugLineFeed();
    DebugPrintf(au8Message_state1);
    
    //about Button
    ButtonAcknowledge(BUTTON1);
    
    //about Buzzer
    PWMAudioOff(BUZZER1);
    PWMAudioOff(BUZZER2);
    bstart_BUZZER=FALSE;
    
    //about LCD
    LCDCommand(LCD_CLEAR_CMD);
    LCDMessage(LINE1_START_ADDR, au8Message_state1);
    
    //about Leds
    LedOff(GREEN);
    LedOff(YELLOW);
    LedOff(ORANGE);
    LedOff(RED);
    
    LedOn(WHITE);
    LedOn(PURPLE);
    LedOn(BLUE);
    LedOn(CYAN);
    
  }
  
  //STATE 2
  if( WasButtonPressed(BUTTON2) || G_au8DebugScanfBuffer[0]=='2')
  {
    //about Debug
    DebugScanf(G_au8DebugScanfBuffer);
    DebugLineFeed();
    DebugPrintf(au8Message_state2);
    
    //about Button
    ButtonAcknowledge(BUTTON2);
    
    //about Buzzer
    PWMAudioOn(BUZZER1);
    PWMAudioOff(BUZZER2);
    u32time_BUZZER=0;
    bstart_BUZZER=TRUE;
    
    //about LCD
    LCDCommand(LCD_CLEAR_CMD);
    LCDMessage(LINE1_START_ADDR, au8Message_state2);
    
    //about Leds
    LedOff(WHITE);
    LedOff(PURPLE);
    LedOff(BLUE);
    LedOff(CYAN);
    
    LedOn(GREEN);
    LedBlink(GREEN, LED_1HZ);
    LedOn(YELLOW);
    LedBlink(YELLOW, LED_2HZ);
    LedOn(ORANGE);
    LedBlink(ORANGE, LED_4HZ);
    LedOn(RED);
    LedBlink(RED, LED_8HZ);
  }
  
  //ERROR Alarm
  if( WasButtonPressed(BUTTON0) || WasButtonPressed(BUTTON3) || ( DebugScanf(G_au8DebugScanfBuffer)>0 && ( G_au8DebugScanfBuffer[0]!='1' || G_au8DebugScanfBuffer[0]!='2' ) ) )
  {
    //about Debug
    DebugScanf(G_au8DebugScanfBuffer);
    DebugLineFeed();
    DebugPrintf(au8Message_error);
    
    //about Button
    ButtonAcknowledge(BUTTON0);
    ButtonAcknowledge(BUTTON3);
    
    //about Buzzer
    PWMAudioOff(BUZZER1);
    PWMAudioOn(BUZZER2);//alarm
    bstart_BUZZER=FALSE;
    
    //about LCD
    LCDCommand(LCD_CLEAR_CMD);
    LCDMessage(LINE1_START_ADDR, au8Message_error1);
    LCDMessage(LINE2_START_ADDR, au8Message_error2);
    
    //about Leds
    LedOff(RED);
    LedOff(WHITE);
    LedOff(PURPLE);
    LedOff(ORANGE);
    LedOff(GREEN);
    LedOff(CYAN);
    LedOff(BLUE);
    LedOff(YELLOW);
  }
  
  if( bstart_BUZZER )
  {
    u32time_BUZZER++;
    
    if( u32time_BUZZER == 100 )
    {
      PWMAudioOff(BUZZER1);
    }
    
    if( u32time_BUZZER == 1000 )
    {
      u32time_BUZZER=0;
      PWMAudioOn(BUZZER1);
    }
    
  }
} /* end UserApp1SM_Idle() */
    

/*-------------------------------------------------------------------------------------------------------------------*/
/* Handle an error */
static void UserApp1SM_Error(void)          
{
  
} /* end UserApp1SM_Error() */



/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/

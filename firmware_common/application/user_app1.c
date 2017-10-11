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
  LedOff(WHITE);
  LedOff(PURPLE);
  LedOff(BLUE);
  LedOff(CYAN);
  LedOff(GREEN);
  LedOff(YELLOW);
  LedOff(ORANGE);
  LedOff(RED);
  
  LedOff(LCD_RED);
  LedOff(LCD_GREEN);
  LedOff(LCD_BLUE);
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
  static u8 u8Counter = 10;
  static u8 u8LCD_Color = 0;  
  static u8 u8CurrentLedIndex = 0; 
  static u16 u16Interval = 500;
  static LedNumberType aeCurrentLed[] = {WHITE,PURPLE,BLUE,CYAN,GREEN,YELLOW,ORANGE,RED};
  static LedRateType aeLedCurrentLevel[] = {LED_PWM_100,LED_PWM_70,LED_PWM_50,LED_PWM_30,LED_PWM_20,LED_PWM_15,LED_PWM_10,LED_PWM_5};
  static u16 u16LedInterval[] = {500,400,300,250,200,150,100,50};
  
  /*Change the brightness of the Leds*/
  u8Counter--;
  if(u8Counter == 0)
  {
    u8Counter=10;
    LedOn((LedNumberType) aeCurrentLed[u8CurrentLedIndex]);
    LedPWM((LedNumberType) aeCurrentLed[u8CurrentLedIndex],(LedRateType) aeLedCurrentLevel[u8CurrentLedIndex]);
  }
    

  u16Interval--;
  if(u16Interval == 0)
  { 
    LedOff((LedNumberType) aeCurrentLed[u8CurrentLedIndex]);
    u8CurrentLedIndex++;
    
    
    if(u8CurrentLedIndex == 8)
    {
      u8CurrentLedIndex=0;
      
      /*Change the color of LCD per cycle*/
      switch(u8LCD_Color)
      {
        case 0: LedPWM(LCD_RED, LED_PWM_100);
                LedPWM(LCD_GREEN, LED_PWM_0);
                LedPWM(LCD_BLUE, LED_PWM_0);
                break;
           
        case 1: LedPWM(LCD_RED, LED_PWM_0);
                LedPWM(LCD_GREEN, LED_PWM_100);
                LedPWM(LCD_BLUE, LED_PWM_0);
                break;
           
        case 2: LedPWM(LCD_RED, LED_PWM_0);
                LedPWM(LCD_GREEN, LED_PWM_0);
                LedPWM(LCD_BLUE, LED_PWM_100);
                break;
           
        case 3: LedPWM(LCD_RED, LED_PWM_100);
                LedPWM(LCD_GREEN, LED_PWM_100);
                LedPWM(LCD_BLUE, LED_PWM_0);
                break;
           
        case 4: LedPWM(LCD_RED, LED_PWM_100);
                LedPWM(LCD_GREEN, LED_PWM_0);
                LedPWM(LCD_BLUE, LED_PWM_100);
                break;
           
        case 5: LedPWM(LCD_RED, LED_PWM_0);
                LedPWM(LCD_GREEN, LED_PWM_100);
                LedPWM(LCD_BLUE, LED_PWM_100);
                break;
           
        case 6: LedPWM(LCD_RED, LED_PWM_100);
                LedPWM(LCD_GREEN, LED_PWM_100);
                LedPWM(LCD_BLUE, LED_PWM_100);
                break;
           
        case 7: LedOff(LCD_RED);
                LedOff(LCD_GREEN);
                LedOff(LCD_BLUE);
                break;
      }  
  
      u8LCD_Color++;
      if(u8LCD_Color == 8)
      {
        u8LCD_Color=0;
      }
    }
    
    /*Change the Interval*/
    u16Interval=u16LedInterval[u8CurrentLedIndex];

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

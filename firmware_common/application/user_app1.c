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
  static u8 u8index=0;
  static u8 u8PinLength=0;
  static u8 u8result=0;/*Determine whether the password is entered correctly*/
  static u8 aePinNum[10];/*Set the password*/
  static u8 aePinPutin[10];/*Input the password*/
  static bool bflag1=FALSE;/*The flag of the mode to change the password*/
  static bool bflag2=FALSE;/*The flag of the mode of inputting the password*/
  static bool bflag3=FALSE;/*The flag of comparing the input password to the set password*/
  static bool bflag4=FALSE;/*The flag of the mode to inputting the password again*/

/* Enter the mode to change the password*/
  if( IsButtonHeld(BUTTON3,2000) )
  {
    ButtonAcknowledge(BUTTON3);
    LedOn(GREEN);
    LedOn(RED);
    LedBlink(GREEN, LED_1HZ);
    LedBlink(RED, LED_1HZ);
    bflag1=TRUE;
  }

  if(bflag1)
  { 
    if( WasButtonPressed(BUTTON0) )
    {
      ButtonAcknowledge(BUTTON0);
      aePinNum[u8PinLength]=0;
      u8PinLength++;
      LedOn(WHITE);
      LedOff(PURPLE);
      LedOff(BLUE);
    }
        
    if( WasButtonPressed(BUTTON1) )
    {
      ButtonAcknowledge(BUTTON1);
      aePinNum[u8PinLength]=1;
      u8PinLength++;
      LedOff(WHITE);
      LedOn(PURPLE);
      LedOff(BLUE);
    }
    
    if( WasButtonPressed(BUTTON2) )
    {
      ButtonAcknowledge(BUTTON2);
      aePinNum[u8PinLength]=2;
      u8PinLength++;
      LedOff(WHITE);
      LedOff(PURPLE);
      LedOn(BLUE);
    }
    
    if( WasButtonPressed(BUTTON3) )
    {
      ButtonAcknowledge(BUTTON3);
      LedOff(GREEN);
      LedOff(RED);
      bflag2=TRUE;
      bflag1=FALSE;
      LedOff(WHITE);
      LedOff(PURPLE);
      LedOff(BLUE);
    }
  }
  
/*Enter the mode of inputting the password*/
  if(bflag2)
  {
    bflag4=FALSE;
    
    if( WasButtonPressed(BUTTON0) )
    {
      ButtonAcknowledge(BUTTON0);
      aePinPutin[u8index]=0;
      u8index++;
      LedOn(WHITE);
      LedOff(PURPLE);
      LedOff(BLUE);
      LedOn(YELLOW);
    }
    
    if( WasButtonPressed(BUTTON1) )
    {
      ButtonAcknowledge(BUTTON1);
      aePinPutin[u8index]=1;
      u8index++;
      LedOff(WHITE);
      LedOn(PURPLE);
      LedOff(BLUE);
      LedOn(YELLOW);
    }
    
    if( WasButtonPressed(BUTTON2) )
    {
      ButtonAcknowledge(BUTTON2);
      aePinPutin[u8index]=2;
      u8index++;
      LedOff(WHITE);
      LedOff(PURPLE);
      LedOn(BLUE);
      LedOn(YELLOW);
    }
  }
  
  if(u8index==u8PinLength&&u8PinLength>0)
  {
    bflag2=FALSE;
    bflag3=TRUE;
    LedOff(WHITE);
    LedOff(PURPLE);
    LedOff(BLUE);
    LedOff(RED);
    LedOff(GREEN);
  }
  
/*Compare the input password to the set password*/
  if(bflag3)
  {
    LedOff(YELLOW);

    if( WasButtonPressed(BUTTON3) )
    {
      ButtonAcknowledge(BUTTON3);
      
      for(u8index=0;u8index<u8PinLength;u8index++)
      {
        if(aePinNum[u8index]==aePinPutin[u8index])
        {
          u8result++;
        }
      }
      
      if(u8result==u8PinLength)
      {
        LedBlink(GREEN, LED_4HZ);
        bflag4=TRUE;
      }
      else
      {
        LedBlink(RED, LED_4HZ);
        bflag4=TRUE;

      }
    }
  }
  
 /*Enter the mode of inputting the password again*/
  if(bflag4)
  {
    bflag2=TRUE;
    bflag3=FALSE;
    u8result=0;
    u8index=0;
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

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
   LCDCommand(LCD_CLEAR_CMD);
   PWMAudioSetFrequency(BUZZER2, 1500);
  
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
  u8 au8Message1[]="0:Name  1:Input char";
  u8 au8Message2[]="3:Back to menu";
  u8 au8Message3[]="0:move right";
  u8 au8Message4[]="1:move left";
  
  static u8 au8Dispaly_Name[13]="Zheng Yangyi";
  static u8 au8Input_the_char[]="Input the char";
  
  static u8 i=0;
  static u8 temp=0;
  static u8 move=0;
  static u8 u8Index=0;
  static u32 u32Counter=0;
  static u8 au8Input[2]= {0};/*Temporarily store the contents of the cache*/
  static u8 au8Data[128]={0};/*store the contents of the cache*/
  static u8 au8Data1[128]={0};/*put the contents from au8Data[]*/
  static u8 au8Data2[128]={0};/*put the contents from au8Data[]*/
  static bool bButton0=FALSE;
  static bool bDispaly_Name_mode=FALSE;/*flag of mode 1*/
  static bool bInput_mode=FALSE;/*flag of mode 2*/
  static bool bBack_to_menu_mode=TRUE;/*flag of mode 3*/
  static bool bstatic=TRUE;
  static bool bdymaic=FALSE;
  
  
  
  /*Menu*/
  if(bBack_to_menu_mode)
  {
    LCDMessage(LINE1_START_ADDR,au8Message1);
    LCDMessage(LINE2_START_ADDR,au8Message2);
    bBack_to_menu_mode=FALSE;
  }
  
  /*mode1*/
  if(!bButton0)
  {
    if(WasButtonPressed(BUTTON0))
    {
      ButtonAcknowledge(BUTTON0);
      
      LCDCommand(LCD_CLEAR_CMD);
      LCDMessage(LINE1_START_ADDR,au8Message3);
      LCDMessage(LINE2_START_ADDR,au8Message4);
      
      bButton0=TRUE;
      bDispaly_Name_mode=TRUE;
      bInput_mode=FALSE;
      bBack_to_menu_mode=FALSE;
      bdymaic=FALSE;
    }
  }
  
  if(bDispaly_Name_mode)
  {    
    
    /*move right*/
    if(IsButtonPressed(BUTTON1))
    {
      PWMAudioSetFrequency(BUZZER1, 500);
      PWMAudioOn(BUZZER1);
    }
    else
    {
      PWMAudioOff(BUZZER1);
    } 
    
    if(WasButtonPressed(BUTTON1))
    {
      ButtonAcknowledge(BUTTON1);
      LCDCommand(LCD_CLEAR_CMD);
      move++;
      LCDMessage(LINE1_START_ADDR+move, au8Dispaly_Name); 
    }
    
    
    /*move left*/
    if(bButton0)
    {
      if(IsButtonPressed(BUTTON0))
      {
        PWMAudioSetFrequency(BUZZER1, 1500);
        PWMAudioOn(BUZZER1);
      }
      else
      {
        PWMAudioOff(BUZZER1);
      } 
      
      if(WasButtonPressed(BUTTON0))
      {
        ButtonAcknowledge(BUTTON0);
        LCDCommand(LCD_CLEAR_CMD);
        move--;
        LCDMessage(LINE1_START_ADDR+move, au8Dispaly_Name);
      }
    }
    
    
    /*limit to not over move and alarm*/   
    if(move+sizeof(au8Dispaly_Name)>21)
    {
      PWMAudioSetFrequency(BUZZER2, 5500);
      PWMAudioOn(BUZZER2);
    }
    else
    {
      PWMAudioOff(BUZZER2);
    }
  }
  
  /*mode 2*/
  if(WasButtonPressed(BUTTON1))
  {
    ButtonAcknowledge(BUTTON1);
    LCDCommand(LCD_CLEAR_CMD);
    LCDMessage(LINE1_START_ADDR, au8Input_the_char); 
    
    bDispaly_Name_mode=FALSE;
    bInput_mode=TRUE;
    bBack_to_menu_mode=FALSE;
  }
  
  
  if(bInput_mode)
  {    
    /*Put the contents of the cache into an array*/
    if(DebugScanf(au8Input)>0)
    {  
      au8Data[u8Index]=au8Input[0];
      if(u8Index<20)
      {
        au8Data1[u8Index]=au8Input[0];
        u8Index++;   
      }
      else
      {
        au8Data2[u8Index-20]=au8Input[0];
        u8Index++;   
      }
      
      if(u8Index==40)
      {
        bstatic=FALSE;
        bdymaic=TRUE;
        bInput_mode=FALSE;
      }
      
      /*display what you input*/
      if(bstatic)
      {
        LCDCommand(LCD_CLEAR_CMD);           
        LCDMessage(LINE1_START_ADDR,au8Data1);                  
        LCDMessage(LINE2_START_ADDR,au8Data2);              
      }
    }
  }
  
  /*make contents dymaic*/
  if(bdymaic)
  {
    u32Counter++;
    
    if(u32Counter==500)
    {
      u32Counter=0;
      
      au8Data[39]=au8Data[0];
      
      for(i=0;i<39;i++)
      {
        temp=au8Data[i+1];
        au8Data[i]=temp;
      }
      
      for(i=0;i<20;i++)
      {
        au8Data1[i]=au8Data[i];
      }
      
      for(i=20;i<40;i++)
      {
        au8Data2[i-20]=au8Data[i];
      }
      
      LCDCommand(LCD_CLEAR_CMD);           
      LCDMessage(LINE1_START_ADDR,au8Data1);                  
      LCDMessage(LINE2_START_ADDR,au8Data2);              
    }
  }
  
  /*mode 3*/
  if(WasButtonPressed(BUTTON3))
  {
    ButtonAcknowledge(BUTTON3);
    LCDCommand(LCD_CLEAR_CMD);
    
    bDispaly_Name_mode=FALSE;
    bInput_mode=FALSE;
    bBack_to_menu_mode=TRUE;
    bdymaic=FALSE;
    bButton0=FALSE;
    PWMAudioOff(BUZZER2);
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

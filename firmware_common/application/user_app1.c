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
  LedOn(GREEN);
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
  static u8 u8model = 0;
  static u8 u8volume =0;
  static u16 u16LCD_delay = 0;
  static u16 u16LED_delay = 0; 
  static u16 u16ADC_current_voltage = 0;
  static bool bled_red = FALSE;
     
  u16LCD_delay++;
  
  /*Turn up the volume*/
  if( WasButtonPressed(BUTTON0) )
  {
    /* Be sure to acknowledge the button press */
    ButtonAcknowledge(BUTTON0);
    bled_red = TRUE;
    LedOff(WHITE);
    
    AT91C_BASE_PIOA -> PIO_CODR |= PA_11_BLADE_UPIMO;  /*CS  低电平有效 */
    AT91C_BASE_PIOA -> PIO_SODR |= PA_12_BLADE_UPOMI;  /*U/D 高电平*/
    
    for(u8 i=0;i<15;i++)
    {
      AT91C_BASE_PIOA -> PIO_SODR |= PA_14_BLADE_MOSI;    /*INC 置为高电平*/
      AT91C_BASE_PIOA -> PIO_CODR |= PA_14_BLADE_MOSI;   /*INC 下降沿有效*/
    }    
  }
  
  /*Turn down the volume*/
  if( WasButtonPressed(BUTTON1) )
  {
    /* Be sure to acknowledge the button press */
    ButtonAcknowledge(BUTTON1);
    bled_red = TRUE;
    LedOff(WHITE);
    
    AT91C_BASE_PIOA -> PIO_CODR |= PA_11_BLADE_UPIMO;  /*CS  低电平有效 */
    AT91C_BASE_PIOA -> PIO_CODR |= PA_12_BLADE_UPOMI;  /*U/D 低电平*/
    
    for(u8 i=0;i<15;i++)
    {
      AT91C_BASE_PIOA -> PIO_SODR |= PA_14_BLADE_MOSI;    /*INC 置为高电平*/
      AT91C_BASE_PIOA -> PIO_CODR |= PA_14_BLADE_MOSI;   /*INC 下降沿有效*/
    }    
  }
  
  if( WasButtonPressed(BUTTON2) )
  {
    /* Be sure to acknowledge the button press */
    ButtonAcknowledge(BUTTON2);    
    LedOn(WHITE);
    
    AT91C_BASE_PIOA -> PIO_SODR |= PA_13_BLADE_MISO; /*4053C*/
    AT91C_BASE_PIOA -> PIO_CODR |= PA_15_BLADE_SCK;  /*4053B*/
    AT91C_BASE_PIOA -> PIO_CODR |= PA_16_BLADE_CS;   /*4053A*/
    
    Adc12StartConversion(ADC12_CH2);
    u16ADC_current_voltage = AT91C_BASE_ADC12B->ADC12B_CDR[ADC12_CH2];
    
    u8volume = (u16ADC_current_voltage*100)/0xFFF;

  }
  
  /*Change the model: Moblie / MIC / Mute*/
  if( WasButtonPressed(BUTTON3) )
  {
    /* Be sure to acknowledge the button press */
    ButtonAcknowledge(BUTTON3);
    bled_red = TRUE;
    LedOff(WHITE);
    
    u8model++;
    if( u8model == 3 )
    {
      u8model = 0;
    }
    
    /*Moblie 0,0,0*/
    if( u8model == 0 )
    {
      AT91C_BASE_PIOA -> PIO_CODR |= PA_13_BLADE_MISO; /*4053C*/
      AT91C_BASE_PIOA -> PIO_CODR |= PA_15_BLADE_SCK;  /*4053B*/
      AT91C_BASE_PIOA -> PIO_CODR |= PA_16_BLADE_CS;   /*4053A*/
      
      LedOn(GREEN);
      LedOff(BLUE);
      LedOff(PURPLE);
    }
    
    /*MIC 0,0,1*/
    if( u8model == 1 )
    {
      AT91C_BASE_PIOA -> PIO_CODR |= PA_13_BLADE_MISO; /*4053C*/
      AT91C_BASE_PIOA -> PIO_CODR |= PA_15_BLADE_SCK;  /*4053B*/
      AT91C_BASE_PIOA -> PIO_SODR |= PA_16_BLADE_CS;   /*4053A*/
      
      LedOff(GREEN);
      LedOn(BLUE);
      LedOff(PURPLE);
    }
    
    /*Mute 0,1,0*/
    if( u8model == 2 )
    {
      AT91C_BASE_PIOA -> PIO_CODR |= PA_13_BLADE_MISO; /*4053C*/
      AT91C_BASE_PIOA -> PIO_SODR |= PA_15_BLADE_SCK;  /*4053B*/
      AT91C_BASE_PIOA -> PIO_CODR |= PA_16_BLADE_CS;   /*4053A*/
      
      LedOff(GREEN);
      LedOff(BLUE);
      LedOn(PURPLE);
    }
  }
  
  /*Display volume level and channel number*/
  if( u16LCD_delay == 300 )
  {
    u16LCD_delay = 0;
    
    LCDCommand(LCD_CLEAR_CMD);
    LCDMessage(LINE1_START_ADDR, "Volume level:");
    LCDMessage(LINE2_START_ADDR, "Channel:");
    
    LCDMessage(LINE1_START_ADDR+13, u8volume);
    
    if( u8model == 0 )
    {
      LCDMessage(LINE2_START_ADDR+9, "Moblie");
    }
    
    if( u8model == 1 )
    {
      LCDMessage(LINE2_START_ADDR+9, "MIC");
    }
    
    if( u8model == 2 )
    {
      LCDMessage(LINE2_START_ADDR+9, "Mute");
    }
  }
  
  /*Led_Red blink*/
  if( bled_red )
  {
    u16LED_delay++;
    LedOn(RED);
    
    if( u16LED_delay ==200)
    {
      bled_red = FALSE;
      u16LED_delay = 0;
      LedOff(RED);
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

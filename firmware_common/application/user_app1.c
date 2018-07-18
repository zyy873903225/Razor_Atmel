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
static u8 zheng[16][2] = {0x44,0x00,0x48,0x7C,0x28,0x44,0xFE,0x24,
                          0x10,0x24,0x10,0x14,0x10,0x24,0xFF,0x25,
                          0x10,0x44,0x10,0x44,0x28,0x44,0x48,0x2C,
                          0x84,0x14,0x82,0x04,0x01,0x04,0x00,0x04};/*"Ö£",0*/

static u8 yang[16][2] = {0x08,0x00,0x88,0x1F,0x08,0x08,0x08,0x04,
                         0x3F,0x02,0x08,0x01,0x88,0x7F,0x28,0x49,
                         0x18,0x49,0x0C,0x49,0x8B,0x48,0x88,0x44,
                         0x48,0x44,0x28,0x42,0x0A,0x29,0x84,0x10};/*"Ñï",0*/

static u8 yi[16][2] = {0x08,0x00,0x10,0x1E,0xFE,0x12,0x44,0x12,
                       0x28,0x12,0xFF,0x61,0x08,0x00,0x94,0x3F,
                       0x73,0x22,0x28,0x22,0x64,0x14,0xB3,0x14,
                       0xA8,0x08,0x24,0x14,0x2B,0x22,0x90,0x41};/*"Òã",1*/

static u8 bi[16][2] = {0x00,0x01,0x04,0x01,0x04,0x01,0x04,0x21,
                       0x04,0x11,0x04,0x09,0x7C,0x05,0x04,0x03,
                       0x04,0x01,0x04,0x01,0x04,0x01,0x04,0x41,
                       0x64,0x41,0x1C,0x41,0x04,0x7E,0x00,0x00};/*"±È",2*/

static u8 xin[16][2] = {0x00,0x00,0x40,0x00,0x80,0x00,0x00,0x01,
                        0x00,0x01,0x20,0x00,0x20,0x10,0x24,0x20,
                        0x24,0x20,0x24,0x40,0x22,0x40,0x22,0x48,
                        0x21,0x08,0x20,0x08,0xC0,0x0F,0x00,0x00};/*"ÐÄ",3*/

static u8 aixin[16][2] = {0x00,0x00,0x00,0x00,0x3C,0x3C,0x64,0x26,
                          0xC2,0x43,0x82,0x41,0x02,0x40,0x06,0x60,
                          0x04,0x20,0x0C,0x30,0x18,0x18,0x30,0x0C,
                          0x60,0x06,0xC0,0x03,0x00,0x00,0x00,0x00};/*°®ÐÄ",0*/
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
  AT91C_BASE_PIOA -> PIO_CODR |= PA_00_TP54;          /*GND*/   
  
  AT91C_BASE_PIOA -> PIO_SODR |= PA_05_HSMCI_MCDA0;   /* STB == 1 */
  AT91C_BASE_PIOA -> PIO_CODR |= PA_08_SD_CS_MCDA3;   /* INH == 0 */
  
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


static void Get_Chinese(u8 u8display[16][10],u8 u8Chinese1[16][2],u8 u8Chinese2[16][2],u8 u8Chinese3[16][2],u8 u8Chinese4[16][2],u8 u8Chinese5[16][2])
{
  for(u8 i=0;i<16;i++)
  {
    for(u8 j=0;j<10;j++)
    {
      switch(j)
      {
      case 0 : u8display[i][j] = u8Chinese1[i][0];
      break;
      
      case 1 : u8display[i][j] = u8Chinese1[i][1];
      break;
      
      case 2 : u8display[i][j] = u8Chinese2[i][0];
      break;
      
      case 3 : u8display[i][j] = u8Chinese2[i][1];
      break;
      
      case 4 : u8display[i][j] = u8Chinese3[i][0];
      break;
      
      case 5 : u8display[i][j] = u8Chinese3[i][1];
      break;
      
      case 6 : u8display[i][j] = u8Chinese4[i][0];
      break;
      
      case 7 : u8display[i][j] = u8Chinese4[i][1];
      break;
      
      case 8 : u8display[i][j] = u8Chinese5[i][0];
      break;
      
      case 9 : u8display[i][j] = u8Chinese5[i][1];
      break;
      }
    }
  }
}/* end Get_Chinese() */

static void Combine_two_array(u8 u8combine[16][20],u8 u8array[16][10])
{ 
  for(u8 i=0;i<16;i++)
  {
    for(u8 j=0;j<20;j++)
    {
      if( j < 10 )
      {
        u8combine[i][j] = 0;
      }
      else
      {
        u8combine[i][j] = u8array[i][j-10];
      }
    }
  }

}/* end Combine_two_array() */


static void Choose_Line(u8 Line_number)
{
  if( ( 0x01 & Line_number ) == 0x01 )
  {
    AT91C_BASE_PIOA -> PIO_SODR |= PA_03_HSMCI_MCCK;  /* A == 1 */
  }
  else
  {
    AT91C_BASE_PIOA -> PIO_CODR |= PA_03_HSMCI_MCCK;  /* A == 0 */
  }
  
  if( ( 0x02 & Line_number ) == 0x02 )
  {
    AT91C_BASE_PIOA -> PIO_SODR |= PA_07_HSMCI_MCDA2;  /* B == 1 */
  }
  else
  {
    AT91C_BASE_PIOA -> PIO_CODR |= PA_07_HSMCI_MCDA2;  /* B == 0 */
  }
  
  if( ( 0x04 & Line_number ) == 0x04 )
  {
    AT91C_BASE_PIOA -> PIO_SODR |= PA_06_HSMCI_MCDA1;  /* C == 1 */
  }
  else
  {
    AT91C_BASE_PIOA -> PIO_CODR |= PA_06_HSMCI_MCDA1;  /* C == 0 */
  }
  
  if( ( 0x08 & Line_number ) == 0x08 )
  {
    AT91C_BASE_PIOA -> PIO_SODR |= PA_04_HSMCI_MCCDA;  /* D == 1 */
  }
  else
  {
    AT91C_BASE_PIOA -> PIO_CODR |= PA_04_HSMCI_MCCDA;  /* D == 0 */
  }
}/* end Choose_Line() */


static void Display(u8 u8display[16][10])
{
  static u8 Line_number = 0;
  static u8 u8data = 0;
  
  /*choose the line*/
  Choose_Line(Line_number);
  
  AT91C_BASE_PIOA -> PIO_SODR |= PA_11_BLADE_UPIMO;  /* LE == 1  transfer the data*/
  AT91C_BASE_PIOA -> PIO_SODR |= PA_12_BLADE_UPOMI;  /* OE == 1 */
  
  for(u8 i=0;i<10;i++)
  {
    u8data = u8display[Line_number][9-i];
    
    for(u8 j=0;j<8;j++)
    {
      
      if( ( 0x80 & u8data ) == 0x80 )
      {
        AT91C_BASE_PIOA -> PIO_SODR |= PA_15_BLADE_SCK;  /* SDI == 1 */
      }
      else
      {
        AT91C_BASE_PIOA -> PIO_CODR |= PA_15_BLADE_SCK;  /* SDI == 0 */
      }
      
      AT91C_BASE_PIOA -> PIO_CODR |= PA_14_BLADE_MOSI;  /* CLK == 0 */
      AT91C_BASE_PIOA -> PIO_SODR |= PA_14_BLADE_MOSI;  /* CLK == 1 input data*/
      
      u8data = u8data << 1;
    }
    
  }
  
  //AT91C_BASE_PIOA -> PIO_CODR |= PA_11_BLADE_UPIMO;  /* LE == 0 store the data*/
  AT91C_BASE_PIOA -> PIO_CODR |= PA_12_BLADE_UPOMI;  /* OE == 0  output the data */
  
  Line_number++;
  
  if( Line_number > MAX_LINE_NUMBER )
  {
    Line_number = 0;
  }
}/* end Display() */


static void Cycle_Display(u8 u8display[16][10])
{
  static u8 u8delay = 100;
  static u8 u8index = 9;
  static u8 u8data[16][10]; 
  static bool btransfer = TRUE;
  
  if( btransfer )
  {     
    for(u8 i=0;i<16;i++)
    { 
      for(u8 j=0;j<10;j++)
      {
        u8data[i][j] = u8display[i][j];
      }
    }
    
    btransfer = FALSE;
  }
  
  u8delay--; 
  
  /*delay 100ms*/
  if( u8delay == 0)
  {
    Display(u8data);
    
    for(u8 i=0;i<16;i++)
    {
      for(u8 j=0;j<u8index;j++)
      {
        u8data[i][j] =  u8data[i][j+1];
      }   
      u8data[i][u8index]=0x00;
    } 
    
    u8delay = 100;
    u8index--;
    if( u8index == 0 )
    {
      u8index = 9;
      btransfer = TRUE;
    }    
  }
  
}/* end Cycle_Display() */


/**********************************************************************************************************************
State Machine Function Definitions
**********************************************************************************************************************/

/*-------------------------------------------------------------------------------------------------------------------*/
/* Wait for ??? */
static void UserApp1SM_Idle(void)
{ 
  u8 au8data[16][10];
  u8 au8combinedata[16][20];
  
  static u8 a = 0;
  static u8 u8delay = 40;
  static u8 u8index = 16;
  static u8 u8data[16][10]; 
  static bool btransfer = TRUE;
  
  
  
   /*give the chinese displayed*/
   Get_Chinese(au8data,yang,yi,aixin,bi,xin);
   
   Combine_two_array(au8combinedata,au8data);
   
   if( btransfer )
     {     
       for(u8 i=0;i<16;i++)
       { 
         for(u8 j=a;j<a+10;j++)
         {
           u8data[i][j] = au8combinedata[i][j];
         }
       }
       a = a+1;
       if( a == 11 )
       {
        a=0;
       }
       btransfer = FALSE;
     }
   
    /*display the chinese*/
   Display(u8data);
   
   u8delay--; 
   
    if(u8delay == 0)
    { 
     for(u8 i=0;i<16;i++)
     {
       for(u8 j=0;j<10;j++)
       {
         u8data[i][j] = u8data[i][j] >> 1;
         
         if( ( u8data[i][j+1] & 0x01 == 0x01 ) && j!=9 )
         {
           u8data[i][j] = u8data[i][j] | 0x80;
         }
         
       }   
     }
     u8delay = 40;
     u8index--;
     if( u8index == 0 )
     {
       u8index = 16;
       btransfer = TRUE;
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

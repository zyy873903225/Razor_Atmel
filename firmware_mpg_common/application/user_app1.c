/**********************************************************************************************************************
File: user_app1.c                                                                

Description:
Provides a Tera-Term driven system to display, read and write an LED command list.

Test1.
Test2 from Engenuics.
Test3.

------------------------------------------------------------------------------------------------------------------------
API:

Public functions:
None.

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
extern volatile u32 G_u32SystemFlags;                  /* From main.c */
extern volatile u32 G_u32ApplicationFlags;             /* From main.c */

extern volatile u32 G_u32SystemTime1ms;                /* From board-specific source file */
extern volatile u32 G_u32SystemTime1s;                 /* From board-specific source file */

extern u8 G_au8DebugScanfBuffer[DEBUG_SCANF_BUFFER_SIZE]; /* From debug.c */
extern u8 G_u8DebugScanfCharCount;                        /* From debug.c */


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
  u8 au8UserApp1Start1[] = "LED program task started\n\r";
  
  /* Turn off the Debug task command processor and announce the task is ready */
  DebugSetPassthrough();
  DebugPrintf(au8UserApp1Start1);
  
    /* If good initialization, set state to Idle */
  if( 1 )
  {
    UserApp1_StateMachine = UserApp1SM_Idle;
  }
  else
  {
    /* The task isn't properly initialized, so shut it down and don't run */
    UserApp1_StateMachine = UserApp1SM_FailedInit;
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
/* Wait for input */
static void UserApp1SM_Idle(void)
{
  /*message about menu*/
  u8 au8menu_Message1[]="\n\rLED Programming Interface\n\r";
  u8 au8menu_Message2[]="Press 1 to program LED command sequence\n\r";
  u8 au8menu_Message3[]="Press 2 show current USER Program\n\r";
  u8 au8menu_Message4[]="Press 3 to clear the USERLIST\n\r";
  u8 au8menu_Message5[]="\n\n\rPlease press 1 or 2 or 3\n\n\r";
  u8 au8output_rim1[]="***********************************************************************\n\r";
  
  /*message about mode1*/
  u8 au8mode1_Message1[]="\n\n\rEnter commands as LED-ONTIME-OFFTIME and press Enter\n\r";
  u8 au8mode1_Message2[]="Time is in milliseconds,max 100commands\n\r";
  u8 au8mode1_Message3[]="LED colors: R, O, Y, G, C, B, P, W\n\r";
  u8 au8mode1_Message4[]="Example:R-100-200 (Red on at 100ms and off at 200ms)\n\r";
  u8 au8mode1_Message5[]="Press Enter on blank line to end\n\r";
  u8 au8mode1_Message6[]=": ";
  u8 au8mode1_Error_Message[]="\n\rInvalid command: incorrect format. Please use L-ONTIME-OFFTIME\n\r";
  u8 au8mode1_Error_Message1[]="\n\rThe abbreviation of LEDs' color must be from  R, O, Y, G, C, B, P, W!\n\r";
  u8 au8mode1_Error_Message2[]="\n\rONTIME and OFFTIME can not be the character except number! \n\r";
  u8 au8mode1_Error_Message3[]="\n\rOFFTIME is better to be more than ONTIME 200ms! \n\r";
  
  /*message about mode2*/
  u8 au8mode2_Message1[]="\n\n\rCurrent USER program:\n\r";
  u8 au8mode2_Message2[]="LED  ON TIME   OFF TIME\n\r";
  u8 au8mode2_Message3[]="Command entry complete.\n\r";
  u8 au8mode2_Message4[]="Commands entered:";
  u8 au8mode2_Message5[]="New USER program:\n\n\r";
  u8 au8output_rim2[]="-----------------------\n\r";
  
  /*message about mode3*/
  u8 au8mode3_Message1[]="\n\rclear the USERLIST successfully!\n\r";
  
  static u8 u8tempdata[2]={0};                          /*store the what user input*/
  static u8 u8divide1[10];                              /*divide the command into three parts: part 1*/
  static u8 u8divide2[10];                              /*divide the command into three parts: part 2*/
  static u8 u8divide3[10];                              /*divide the command into three parts: part 3*/
  static u16 u16MusicNote[7] = {C6,D6,E6,F6,G6,A6,B6};  /*when the command is right,music will be on in this*/
  static u16 u16AlarmNote[7] = {B6,A6,G6,F6,E6,D6,C6};  /*when the command is wrong,music will be on in this*/

  
  static u8 i=0;                                /*the Index for 'for cycle' */
  static u8 u8divide1_Index=0;                               /*the Index of u8divide1[]*/
  static u8 u8divide2_Index=0;                               /*the Index of u8divide2[]*/
  static u8 u8divide3_Index=0;                               /*the Index of u8divide3[]*/
  static u8 u8aeUserList_Index=0;               /*the Index of aeUserList*/
  static u8 u8PrintListLine_Index=0;            /*the Index of LedDisplayPrintListLine()*/
  static u8 u8PrintNewListLine_Index=0;         /*the Index of LedDisplayPrintListLine() to print new command*/
  static u8 u8result=0;                         /*judge the number of '-'*/
  static u32 u32sequence=1;                     /*the sequence number of the command*/
  static u8 u8Counter=0;                        /*the counter of mode2 to make the debug can output more than 10 commands*/
  static u8 u8Last_Number=0;                    /* the last sequence number of the command*/
  static u8 u8MusicTime=0;                      /*the counter of BUZZER*/
  static u8 u8MusicNoteIndex=0;                 /*the Index of u16MusicNote&u16AlarmNote*/

  
  static bool bjudge=TRUE;                      /*the flag of print the statement of error*/
  static bool bInput=TRUE;                      /*the flag of put contents into u8divide2[] and u8divide3[]*/
  static bool bMusic=FALSE;
  static bool bAlarm=FALSE;
  static bool bDisplay_New_program=FALSE;
  static bool bmode1_Error_Message1=FALSE;
  static bool bmode1_Error_Message2=FALSE;
  static bool bmode1_Error_Message3=FALSE;
  
  
  static bool bDisplay_menu=TRUE;
  static bool bmode1_Input=FALSE;
  static bool bmode2_Display=FALSE;
  static bool bmode3_Clear=FALSE;
  
  
  static LedCommandType aeUserList[100];

  /*menu*/
  if(bDisplay_menu)
  {
    DebugLineFeed();
    DebugPrintf(au8output_rim1);
    DebugPrintf(au8menu_Message1);
    DebugPrintf(au8menu_Message2);
    DebugPrintf(au8menu_Message3);
    DebugPrintf(au8menu_Message4);
    DebugPrintf(au8output_rim1);
    DebugLineFeed();
    bDisplay_menu=FALSE;
  }
    
  
  if(!bmode1_Input && !bmode2_Display)
  {
    if(DebugScanf(u8tempdata)>0)
    {  
      switch(u8tempdata[0])
      {
      case '1': bmode1_Input=TRUE;
                bmode2_Display=FALSE;
                DebugPrintf(au8mode1_Message1);
                DebugPrintf(au8mode1_Message2);
                DebugPrintf(au8mode1_Message3);
                DebugPrintf(au8mode1_Message4);
                DebugPrintf(au8mode1_Message5);   
                DebugPrintNumber(u32sequence);
                DebugPrintf(au8mode1_Message6);  
                break;
      
       case '2': bmode1_Input=FALSE;
                 bmode2_Display=TRUE;
                 break;
                 
       case '3': bmode1_Input=FALSE;
                 bmode2_Display=FALSE;
                 bmode3_Clear=TRUE;
                 break;
                 
       default:  DebugPrintf(au8menu_Message5);
           
      }
    }
  }
  
  /*mode1*/
  if(bmode1_Input)
  {
    if(DebugScanf(u8tempdata)>0)
    {                   
      if(u8tempdata[0]=='-')
      {
        u8result++;
        bInput=FALSE;
      }
      else
      {
        bInput=TRUE;
      }
      
      /*divide 3 arrary and input content in three*/
      if(u8result==0)
      {
        /*put contents in u8divide1*/
        u8divide1[u8divide1_Index]=u8tempdata[0];
        u8divide1_Index++;
        
        /*when the first char is 'Enter',then come back the menu*/
        if(u8divide1[0]==0x0d)
        {
          bmode1_Input=FALSE;
          bmode2_Display=FALSE;
          bDisplay_menu=TRUE;
          
          u8result=0;
          u8divide1_Index=0;
          u8divide2_Index=0;
          u8divide3_Index=0;
          
        }
        
        
        switch(u8divide1[0])
        {
        case 'W':break;
        
        case 'P':break;
        
        case 'B':break;
        
        case 'C':break;
        
        case 'G':break;
        
        case 'Y':break;
        
        case 'O':break;
        
        case 'R':break;
        
        case 0x0d:break;
        
        default:bjudge=FALSE;  
                bmode1_Error_Message1=TRUE;
        }
            
        if(u8divide1_Index>1)
        {
           bjudge=FALSE;
           bmode1_Error_Message1=TRUE;
        }
      }
      else if(u8result==1 && bInput)
      {
        /*put contents in u8divide2*/
        u8divide2[u8divide2_Index]=u8tempdata[0];
        u8divide2_Index++;
      }
      else if(u8result==2 && bInput)
      {
        /*put contents in u8divide3*/
        u8divide3[u8divide3_Index]=u8tempdata[0];
       
        if(u8divide3[u8divide3_Index]==0x0d && u8divide3_Index>0)
        {
          u8divide3[u8divide3_Index]='\0';
        } 
        else
        {
          u8divide3_Index++;
        }
      }
      else if(u8result>2)
      {
         bjudge=FALSE;
         bmode1_Error_Message2=TRUE;
      }
      
      /*judge the end of a command by key "ENTER"*/
      if(bmode1_Input)
      {
        /*judge it is wheather right*/
        
          for(i=0;i<u8divide2_Index;i++)
          {
            if(u8divide2[i]<0x30 || u8divide2[i]>0x39)
            {
              bjudge=FALSE;
              bmode1_Error_Message2=TRUE;
            }
          }
          
          for(i=0;i<u8divide3_Index;i++)
          {
            if(u8divide3[i]<0x30 || u8divide3[i]>0x39)
            {
              bjudge=FALSE;
              bmode1_Error_Message2=TRUE;
            }
          }
        
        /*input 'ENTER' to go to next line*/
        if(u8tempdata[0]==0x0d)
        {
          u8result=0;
          u8divide1_Index=0;
          u8divide2_Index=0;
          u8divide3_Index=0;   
          
          /*compare ON-TIME with OFF-TIME*/
          if(atoi((char*)u8divide2)>atoi((char*)u8divide3) || u8divide2[0]==0x0d)
          {
            bjudge=FALSE;
            bmode1_Error_Message3=TRUE;
          }
          else
          {
            u32sequence++;
            DebugLineFeed();
            DebugPrintNumber(u32sequence);
            DebugPrintf(au8mode1_Message6); 
          }
        }
                  

        /*put what users input add in the userlist*/
        
        if(u8tempdata[0]==0x0d && bjudge)
        {
          switch(u8divide1[0])
          {
          case 'W':aeUserList[u8aeUserList_Index].eLED=WHITE;break;
          
          case 'P':aeUserList[u8aeUserList_Index].eLED=PURPLE;break;
          
          case 'B':aeUserList[u8aeUserList_Index].eLED=BLUE;break;
          
          case 'C':aeUserList[u8aeUserList_Index].eLED=CYAN;break;
          
          case 'G':aeUserList[u8aeUserList_Index].eLED=GREEN;break;
          
          case 'Y':aeUserList[u8aeUserList_Index].eLED=YELLOW;break;
          
          case 'O':aeUserList[u8aeUserList_Index].eLED=ORANGE;break;
          
          case 'R':aeUserList[u8aeUserList_Index].eLED=RED;break;
          
          default:bjudge=FALSE;           
          }
                   
        }
        
        if(u8tempdata[0]==0x0d && bjudge)
        {
          bMusic=TRUE;
          
          aeUserList[u8aeUserList_Index].u32Time=atoi((char*)u8divide2);/*change u8divide2 from char to number*/
          memset(u8divide2,'\0',sizeof(u8divide2));/*clear the u8divide2*/
          aeUserList[u8aeUserList_Index].bOn=TRUE;
          aeUserList[u8aeUserList_Index].eCurrentRate=LED_PWM_0;
          LedDisplayAddCommand(USER_LIST, &aeUserList[u8aeUserList_Index]);
          u8aeUserList_Index++;
          
          switch(u8divide1[0])
          {
            case'W':aeUserList[u8aeUserList_Index].eLED=WHITE;break;
            
            case'P':aeUserList[u8aeUserList_Index].eLED=PURPLE;break;
            
            case'B':aeUserList[u8aeUserList_Index].eLED=BLUE;break;
            
            case'C':aeUserList[u8aeUserList_Index].eLED=CYAN;break;
            
            case'G':aeUserList[u8aeUserList_Index].eLED=GREEN;break;
            
            case'Y':aeUserList[u8aeUserList_Index].eLED=YELLOW;break;
            
            case'O':aeUserList[u8aeUserList_Index].eLED=ORANGE;break;
            
            case'R':aeUserList[u8aeUserList_Index].eLED=RED;break;     
          
            default:bjudge=FALSE;    
          }
          
        
          aeUserList[u8aeUserList_Index].u32Time=atoi((char*)u8divide3);/*change u8divide3 from char to number*/
          memset(u8divide3,'\0',sizeof(u8divide3));/*clear the u8divide3*/
          aeUserList[u8aeUserList_Index].bOn=FALSE;
          aeUserList[u8aeUserList_Index].eCurrentRate=LED_PWM_100;
          LedDisplayAddCommand(USER_LIST, &aeUserList[u8aeUserList_Index]);
          u8aeUserList_Index++;
       
        }
        
        /*When the error exist,it will display content followed*/
        if(!bjudge)
        {
          u8result=0;
          u8divide1_Index=0;
          u8divide2_Index=0;
          u8divide3_Index=0;
          bjudge=TRUE;
          bAlarm=TRUE;
          
          
          if(bmode1_Error_Message1)
          {
            bmode1_Error_Message1=FALSE;
            DebugPrintf(au8mode1_Error_Message1);
          }
          
           if(bmode1_Error_Message2)
          {
            bmode1_Error_Message2=FALSE;
            DebugPrintf(au8mode1_Error_Message2);
          }
          
           if(bmode1_Error_Message3)
          {
            bmode1_Error_Message3=FALSE;
            DebugPrintf(au8mode1_Error_Message3);
          }
          
          DebugPrintf(au8mode1_Error_Message);
          DebugPrintNumber(u32sequence);
          DebugPrintf(au8mode1_Message6); 
          
          memset(u8divide1,'\0',sizeof(u8divide1));
          memset(u8divide2,'\0',sizeof(u8divide2));
          memset(u8divide3,'\0',sizeof(u8divide3));
        }
      }
    }
    
    /*turn on the music*/
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
    
    if (bAlarm)
    {
      u8MusicTime++;
      
     if (u8MusicTime == 100)
      {
        u8MusicTime = 0;
        PWMAudioSetFrequency(BUZZER2,u16AlarmNote[u8MusicNoteIndex]);
        u8MusicNoteIndex++;
        PWMAudioOn(BUZZER2);
        if (u8MusicNoteIndex > 6)
        {
          u8MusicNoteIndex = 0;
          bAlarm=FALSE;
          PWMAudioOff(BUZZER2);
        }
      }
    }
  }
  
  
  if(bmode2_Display)
  {
    
    u8result=0;

    u8divide1_Index=0;
    u8divide2_Index=0;
    u8divide3_Index=0;
    
    u8Counter++;
     
    /*Delay of 10ms*/
    if(u8Counter==10)
    { 
      u8Counter=0;
      
      
      if(!bDisplay_New_program)
      {
        /*display all the command*/
        if(u8PrintListLine_Index==0)
        {
          DebugPrintf(au8mode2_Message1);
          DebugPrintf(au8mode2_Message2);
          DebugPrintf(au8output_rim2);
        }
        
        LedDisplayPrintListLine(u8PrintListLine_Index);
        u8PrintListLine_Index++;
        
        if(u8PrintListLine_Index>u8aeUserList_Index/2)
        {
          
          DebugPrintf(au8output_rim2);  
          DebugLineFeed();
          DebugLineFeed(); 
          
          
          DebugPrintf(au8mode2_Message3);
          DebugPrintf(au8mode2_Message4);
          DebugPrintNumber((u8aeUserList_Index)/2-u8Last_Number); 
          
          u8Last_Number = (u8aeUserList_Index)/2;
          bDisplay_New_program=TRUE;
          
          DebugLineFeed();
          DebugLineFeed(); 
          DebugPrintf(au8mode2_Message5);
          DebugPrintf(au8mode2_Message2);
          DebugPrintf(au8output_rim2);
        }
      }
      else
      {
        /*display the new command*/
        LedDisplayPrintListLine(u8PrintNewListLine_Index);
        u8PrintNewListLine_Index++;
        
        
        if(u8PrintNewListLine_Index>u8aeUserList_Index/2)
        {
          DebugPrintf(au8output_rim2);     
          
          u8PrintNewListLine_Index=u8PrintListLine_Index-1;
          u8PrintListLine_Index=0;
          bDisplay_New_program=FALSE;
          bDisplay_menu=TRUE; 
          bmode2_Display=FALSE;           
          
        }
      }   
    }     
  }
  
  if(bmode3_Clear)
  { 
    /*clear all the Index*/
    u8PrintListLine_Index=0;
    u8PrintNewListLine_Index=0;
    u8Last_Number=0;
    u8aeUserList_Index=0;
    u32sequence=1;
    
    bmode3_Clear=FALSE;
    bDisplay_menu=TRUE; 
    LedDisplayStartList();
    DebugPrintf(au8mode3_Message1);
    DebugLineFeed();    
  }
  
} /* end UserApp1SM_Idle() */
                      
            
#if 0
/*-------------------------------------------------------------------------------------------------------------------*/
/* Handle an error */
static void UserApp1SM_Error(void)          
{
  
} /* end UserApp1SM_Error() */
#endif


/*-------------------------------------------------------------------------------------------------------------------*/
/* State to sit in if init failed */
static void UserApp1SM_FailedInit(void)          
{
    
} /* end UserApp1SM_FailedInit() */


/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/

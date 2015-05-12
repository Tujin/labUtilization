#include <genieArduino.h>

// definitions for key strokes
#define BACKSPACE 110
#define CANCEL 107
#define SUBMIT 13

//definitions for buffer sizes (including space for null terminator)
#define WWID_SIZE 9
#define DUR_SIZE 3

//definitions for screen objects
#define STRING_WWID 0
#define STRING_DUR 8
#define STRING_TOOL 9
#define STRING_WWID_BUTTON_MASK 1
#define STRING_DUR_BUTTON_MASK 2
#define BUTTON_WWID 0
#define BUTTON_DUR 1
#define BUTTON_TOOL 2

//Definitions for selected int
#define SELECTED_NONE 0
#define SELECTED_WWID 1
#define SELECTED_DUR 2

//Definitions for tool strings object
#define TOOL_STRINGS_ELEMENTS 8

Genie genie;

char wwidBuf[WWID_SIZE]; // declare a char array to hold our WWID
char durBuf[DUR_SIZE]; // declare a char array to hold keystrokes for duration

int wwidIndex;
int durIndex;
int selectedButton;
int selectedTool;
boolean selectedChange;

void setup()
{  
  Serial1.begin(115200);  // Serial1 (pins 0 and 1 on Galileo) set to 115200 baud
  genie.Begin(Serial1);   // Use Serial1 for talking to the Genie Library, and to the 4D Systems display
  Serial.begin(9600);     // Use serial0 for communicating with host PC over USB
  wwidIndex = 0;

  genie.AttachEventHandler(myGenieEventHandler); // Attach the user function Event Handler for processing events

  //Turn the Display on (Contrast)
  genie.WriteContrast(15); // 1 = Display ON, 0 = Display OFF.
  //For uLCD43, uLCD-70DT, and uLCD-35DT, use 0-15 for Brightness Control, where 0 = Display OFF, though to 15 = Max Brightness ON.
  
  
  genie.WriteObject (GENIE_OBJ_FORM, 0, 0); // Switch to form 0
  selectedButton = SELECTED_WWID; // start with WWID selected
  selectedTool = 0; // start with first string selected in tool list
  selectedChange = true;
  wwidBuf[0] = 0; // set first character as null terminator
  durBuf[0] = 0; // set first character as null terminator
  wwidIndex = 0; // initialize to 0
  durIndex = 0; // initialize to 0
  drawScreen(); // draw screen for the first time
  genie.WriteObject(GENIE_OBJ_STRINGS,0,0); // display the box for strings object, without this line the box doesn't appear until you begin typing
  Serial.println("Setup finished.");
}

void loop()
{
  genie.DoEvents(); // This calls the library each loop to process the queued responses from the display
}

void drawScreen()
{
  genie.WriteObject(GENIE_OBJ_STRINGS,STRING_TOOL,selectedTool); // draw tool button with appropriate string selected
  if(selectedChange)
  {
    genie.WriteObject(GENIE_OBJ_USERBUTTON,BUTTON_WWID,0); // bring black wwid user button to the front, obscuring the text element and highlighting box
    genie.WriteObject(GENIE_OBJ_USERBUTTON,BUTTON_DUR,0); // bring black duration user button to the front, obscuring the text element and highlighting box
    selectedChange = false;
  }
    if(selectedButton == SELECTED_WWID) 
      genie.WriteObject(GENIE_OBJ_STRINGS,STRING_WWID_BUTTON_MASK,0); // bring highlighting box to the front for WWID
    else if(selectedButton == SELECTED_DUR)
      genie.WriteObject(GENIE_OBJ_STRINGS,STRING_DUR_BUTTON_MASK,0);  // bring highlighting box to the front for duration
  // else if (selectedButton == SELECTED_NONE) //do nothing
  genie.WriteStr(STRING_WWID,wwidBuf); // write wwid buffer to strings object
  genie.WriteStr(STRING_DUR,durBuf); // write duration bufer to strings object 
}
void keyPress(int key)
{
  if(key == SUBMIT)
    {
      //doSubmit();
    }
  char * buf;
  int bufSize;
  int * bufIndex;
  if(selectedButton == SELECTED_WWID)
  {
    buf = wwidBuf; // buffer now points to wwidBuf in memory
    bufSize = WWID_SIZE; // set bufSize to prevent overflows
    bufIndex = &wwidIndex; // bufIndex now points to wwidIndex
  }
  else if(selectedButton == SELECTED_DUR)
  {
    buf = durBuf; // buffer now points to durBuf in memory
    bufSize = DUR_SIZE; // set bufSize to prevent overflows
    bufIndex = &durIndex; // bufIndex now points to durIndex
  }
  else if(selectedButton == SELECTED_NONE) // no field selected, we only care about a submit press
  {
    return;
  }
  if(key == BACKSPACE && *bufIndex > 0) // keystroke received is a backspace, and we have digits to delete
  {
    buf[*bufIndex] = 0;
    *bufIndex--;
  }
  else if (key == CANCEL) // pressed cancel, set index to 0 to erase their entire entry
    *bufIndex = 0;
  int digit = key - '0'; // get actual numerical value of keypress
  if(digit >= 0 && digit <= 9 && *bufIndex < bufSize - 1) // keypress is within the acceptable range and we have room in our char array for it (subtract 1 because the last character can only be a null terminator
  {
    buf[*bufIndex] = key; // set curret location to keypress
    (*bufIndex)++; // move forward
  }
  buf[*bufIndex] = 0; // set the value at end of array to be our null terminator
}
void myGenieEventHandler (void)
{
  int keyboardValue; // int to store keypress
  
  genieFrame Event; // declare an event to store the current event
  genie.DequeueEvent(&Event); // Remove this event from the queue

  if (Event.reportObject.cmd != GENIE_REPORT_EVENT) // If this event is NOT a Reported Message
  {
    return ; // Leave the handler
  }

  if (Event.reportObject.object == GENIE_OBJ_KEYBOARD) // If this event is from a Keyboard
  {
    if (Event.reportObject.index == 0)	// If from Keyboard0
    {
      keyboardValue = genie.GetEventData(&Event); // Get data from Keyboard0
      keyPress(keyboardValue); // pass data to the keyPress function
    }
  }
  else if(Event.reportObject.object == GENIE_OBJ_USERBUTTON)
  {
   if(Event.reportObject.index == BUTTON_WWID)
    {
      selectedButton = SELECTED_WWID;
      selectedChange = true;
    }
   else if(Event.reportObject.index == BUTTON_DUR)
   {
     selectedButton = SELECTED_DUR;
     selectedChange = true;
   }
   else if(Event.reportObject.index == BUTTON_TOOL)
   {
     selectedTool++;
     if(selectedTool == TOOL_STRINGS_ELEMENTS) //went past last object
       selectedTool = 0;
   }
  }
  drawScreen();
}


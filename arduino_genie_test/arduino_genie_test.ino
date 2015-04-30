#include <genieArduino.h>

Genie genie;

char wwid[9];
int wwidIndex;

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
  genie.WriteObject(GENIE_OBJ_STRINGS,0,0); // display the box for strings object, without this line the box doesn't appear until you begin typing
  Serial.println("Setup finished.");
}

void loop()
{
  genie.DoEvents(); // This calls the library each loop to process the queued responses from the display
}

void keyPress(int key)
{
  Serial.print("Keypress received: ");
  Serial.println(key);
  if(key == 110 && wwidIndex > 0) // keystroke received is a backspace, and we have digits to delete
  {
    wwid[wwidIndex] = 0; //Set current location in char array to 0 (null terminator)
    wwidIndex--; // back up index
  }
  else if (key == 107) // pressed cancel, set index to 0 to erase their entire entry
    wwidIndex = 0;
  int digit = key - '0'; // get actual numerical value of keypress
  if(digit >= 0 && digit <= 9 && wwidIndex < 8) // keypress is within the acceptable range and we have room in our char array for it
  {
    wwid[wwidIndex] = key; // set curret location to keypress
    wwidIndex++; // move forward
  }
  wwid[wwidIndex] = 0; // set the value at wwidIndex to be our null terminator
  genie.WriteStr(0,wwid); // write our char array to strings object 0
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
}


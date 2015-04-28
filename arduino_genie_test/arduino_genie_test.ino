#include <genieArduino.h>

// This Demo communicates with a 4D Systems Display, configured with ViSi-Genie, utilising the Genie Arduino Library - https://github.com/4dsystems/ViSi-Genie-Arduino-Library.
// The display has a slider, a cool gauge, an LED Digits, a string box and a User LED.
// The program receives messages from the Slider0 object using the Reported Events. This is triggered each time the Slider changes on the display, and an event
// is genereated and sent automatically. Reported Events originate from the On-Changed event from the slider itself, set in the Workshop4 software.
// Coolgauge is written to using Write Object, and the String is updated using the Write String command, showing the version of the library.
// The User LED is updated by the Arduino, by first doing a manual read of the User LED and then toggling it based on the state received back.

// As the slider changes, it sends its value to the Arduino (Arduino also polls its value using genie.ReadObject, as above), and the Arduino then
// tells the LED Digit to update its value using genie.WriteObject. So the Slider message goes via the Arduino to the LED Digit.
// Coolgauge is updated via simple timer in the Arduino code, and updates the display with its value.
// The User LED is read using genie.ReadObject, and then updated using genie.WriteObject. It is manually read, it does not use an Event.

// This demo illustrates how to use genie.ReadObject, genie.WriteObject, Reported Messages (Events), genie.WriteStr, genie.WriteContrast, plus supporting functions.

// Application Notes on the 4D Systems Website that are useful to understand this library are found: http://www.4dsystems.com.au/appnotes 
// Good App Notes to read are: 
// ViSi-Genie Connecting a 4D Display to an Arduino Host - http://www.4dsystems.com.au/appnote/4D-AN-00017/
// ViSi-Genie Writing to Genie Objects Using an Arduino Host - http://www.4dsystems.com.au/appnote/4D-AN-00018/
// ViSi-Genie A Simple Digital Voltmeter Application using an Arduino Host - http://www.4dsystems.com.au/appnote/4D-AN-00019/
// ViSi-Genie Connection to an Arduino Host with RGB LED Control - http://www.4dsystems.com.au/appnote/4D-AN-00010/
// ViSi-Genie Displaying Temperature values from an Arduino Host - http://www.4dsystems.com.au/appnote/4D-AN-00015/
// ViSi-Genie Arduino Danger Shield - http://www.4dsystems.com.au/appnote/4D-AN-00025/

Genie genie;
#define RESETLINE 4  // Change this if you are not using an Arduino Adaptor Shield Version 2 (see code below)

char wwid[9];
int wwidIndex;
void setup()
{
  // NOTE, the genieBegin function (e.g. genieBegin(GENIE_SERIAL_0, 115200)) no longer exists.  Use a Serial Begin and serial port of your choice in
  // your code and use the genie.Begin function to send it to the Genie library (see this example below)
  // 200K Baud is good for most Arduinos. Galileo should use 115200.  
  Serial1.begin(115200);  // Serial0 @ 200000 (200K) Baud
  genie.Begin(Serial1);   // Use Serial0 for talking to the Genie Library, and to the 4D Systems display
  Serial.begin(9600);
  wwidIndex = 0;

  genie.AttachEventHandler(myGenieEventHandler); // Attach the user function Event Handler for processing events

  // Reset the Display (change D4 to D2 if you have original 4D Arduino Adaptor)
  // THIS IS IMPORTANT AND CAN PREVENT OUT OF SYNC ISSUES, SLOW SPEED RESPONSE ETC
  // If NOT using a 4D Arduino Adaptor, digitalWrites must be reversed as Display Reset is Active Low, and
  // the 4D Arduino Adaptors invert this signal so must be Active High.  
  pinMode(RESETLINE, OUTPUT);  // Set D4 on Arduino to Output (4D Arduino Adaptor V2 - Display Reset)
  digitalWrite(RESETLINE, 1);  // Reset the Display via D4
  delay(100);
  digitalWrite(RESETLINE, 0);  // unReset the Display via D4
  pinMode(8,OUTPUT);
  digitalWrite(8,HIGH);
  delay (3500); //let the display start up after the reset (This is important)
  digitalWrite(8,LOW);
  //Turn the Display on (Contrast) - (Not needed but illustrates how)
  genie.WriteContrast(15); // 1 = Display ON, 0 = Display OFF.
  //For uLCD43, uLCD-70DT, and uLCD-35DT, use 0-15 for Brightness Control, where 0 = Display OFF, though to 15 = Max Brightness ON.

  //Write a string to the Display to show the version of the library used
  genie.WriteObject (GENIE_OBJ_FORM, 0, 0);
  Serial.println("Setup finished.");
}

void loop()
{
  static long waitPeriod = millis();
  static int gaugeAddVal = 1;
  static int gaugeVal = 50;

  genie.DoEvents(); // This calls the library each loop to process the queued responses from the display

  if (millis() >= waitPeriod)
  {
    // Write to CoolGauge0 with the value in the gaugeVal variabl

    waitPeriod = millis() + 50; // rerun this code to update Cool Gauge and Slider in another 50ms time.
  }
}

void keyPress(int key)
{
  Serial.print("Keypress received: ");
  Serial.println(key);
  if(key == 110 && wwidIndex > 0)
  {
    wwid[wwidIndex] = 0;
    wwidIndex--;
  }
  else if (key == 107)
    wwidIndex = 0;
  int digit = key - '0';
  if(digit >= 0 && digit <= 9 && wwidIndex < 8)
  {
    wwid[wwidIndex] = key;
    wwidIndex++;
  }
  char part[9];
  memcpy(part,wwid,wwidIndex);
  part[wwidIndex] = 0;
  genie.WriteStr(0,part);
}
void myGenieEventHandler (void)
{
  int keyboardValue;
  
  genieFrame Event;
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
      keyPress(keyboardValue); // pass data to the keyPress
    }
  }
}


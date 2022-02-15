Genesis to CDi
-----------

GenesisToCDi is an adaptation of SNEStoCDi to use Genesis controllers instead.  Why?  Because DB-9 sockets can be sourced from any electronics supplier.  SNES sockets are significantly harder to find.

I also fixed a problem in the original code where control inputs would occasionally get lost.  While the SNEStoCDi would stop sending data when assertRTS was false, it would still save all the padbyteX values as if they had been sent.  The CDi has the odd habbit of interrupting a send, and asking again later.

Genesispad is an Arduino library for reading 3-button and 6-button Genesis controllers based off of the SNESpad library by Rob Duarte that was used in the original.  As of this writing, it is working, but has to be called twice in a row to read 6-button controllers.  That will get fixed before the library is posted as a stand alone project.

Credits:
Laurent Berta: SNEStoCDi original code.
Rob Duarte: SNESpad library as basis for Genesispad
Paul Hackmann: CDi documentation

Like the original, GenesisToCDi is released under Creative Common CC-BY 4.0 licence : http://creativecommons.org/licenses/by/4.0/legalcode

Brian Harrison


The original readme is below:


SNES to CDi
-----------

SNEStoCDi is an Arduino-based project that allows to use a Nintendo SNES (Super NES) / SFC (Super Famicom) gamepad on a Philips CDi player / game console. SNEStoCDi requires no more electronic than the Arduino itself. It's a pretty easy project for anyone with basic knowledge about Arduino and soldering.

Backported lost-send fix from GenesisToCDi.  While the original would stop sending data when assertRTS was false, it would still save all the padbyteX values as if they had been sent.  The CD-i has the odd habbit of interrupting a send, and asking again later.  --Brian Harrison

SNEStoCDi is released under Creative Common CC-BY 4.0 licence : http://creativecommons.org/licenses/by/4.0/legalcode


How to make your own SNEStoCDi adapter
--------------------------------------

First of all, you need the Arduino IDE. If you need to, you can download it for free here : https://www.arduino.cc/en/Main/Software

You also need an Arduino, of course. I myself use Arduinos UNO and Nano but SNEStoCDi should work with most 5v Arduino versions.

To do a good and durable adapter, you will need a SNES/SFC gamepad connector (a simple way is to recycle an extension cable) and a mini-DIN 8 pin male connector.

Do the wiring :
- connect the SNES/SFC connector to your Arduino : please report to wiring.jpg for more informations.
- connect the min-DIN CDi connector to your Arduino : please report to wiring.jpg for more informations.

IMPORTANT : 
- the RTS line from the CDi *must* be connected to an analog input. if you're curious about that, report to the Tech Stuff part.
- the Arduino will be powered by the VCC provided by the CDi. do not connect any other power source to the Arduino !

Upload the SNEStoCDi software to your Arduino :
- connect the Arduino to your computer.
- add the SNESpad library (by Rob Duarte) to the Arduino IDE. the library provided with SNEStoCDi is a modified version of SNESpad, optimized for performances and better compatibility with the current Arduino IDE. for information on installing libraries, see : http://arduino.cc/en/Guide/Libraries
- open the SNEStoCDi sketch.
- check that the Arduino pins on the sketch match your wiring, if not adjust the sketch with the right pins.
  the sketch matches the pinout showed on the wiring.jpg file.
- upload the compiled sketch to your Arduino.

That's it, you already finished :) the SNEStoCDi adapter is plug-and-play : just plug it to your CDi, plug your SNES/SFC pad, start your CDi and go suffer on some terrible games ... :)
I'd be happy to know about your build, so don't hesistate to send me an email to let me know you made your own SNEStoCDi adapter.

SNEStoCDi has been tested successfully with Arduinos UNO and Nano, on a Philips CDi 470.


SNEStoCDi mapping & fancy stuff
-------------------------------

The buttons mapping is probably exactly what you expect :
- the Dpad acts as a Dpad.
- the "Y" button acts as "Button 1".
- the "B" button acts as "Button 2".
- the "A" and "X" buttons both act as "Button 3", which is in fact "Button 1 + Button 2".

The SNES/SFC provides way more buttons than a CDi gamepad, so I had the opportunity to add some fancy stuff :
- the cursor speed is adjustable in 5 steps. the "L" trigger shifts one step down, the "R" trigger shifts one step up.
- the "start" button resets the speed to the default one (step 3).
- the speed setting is saved into the Arduino's EEPROM, so SNEStoCDi will apply your last speed setting the next time you will use it.
- the "select" button changes the buttons mapping and inverts "B" and "Y". another "select" press changes the mapping again.


Tech stuff
----------

You might be curious about the use of an analog input where there is no analog signal. The reason is that the CDi and the Arduino have different logical levels : the CDi asserts the RTS line at about 2.4 volts when the Arduino needs about 3 volts on digital inputs. By reading the RTS line on an analog input, I've been able to set manually a lower "virtual logical level". This allows to use the Arduino alone, without any other electronic component to adapt the levels.

For some reason, all my attemps to use the hardware UART to send data to the CDi failed. This is why I used NewSoftSerial (called SoftwareSerial in the Arduino IDE) to emulate another UART. As it is only used to send data, the RX interrupt is not really used and this is why the RX pin is not connected. The CDi doesn't send anything on its TX, it only communicates on the RTS line, this is the reason why the communication between the Arduino and the CDi is kind of asymetrical.


Thanks
------

Thank you very much for reading my bad english (I'm french), I really hope SNEStoCDi will help you to use your CDi without spending insane money on an original CDi gamepad. If you build your SNEStoCDi, please let me know on my email, I would be very happy to know my work has been useful :)

Special thanks to Rob Duarte who created the great SNESpad library, and to Paul Hackmann who provided some very useful documentation about the CDi devices and communication protocol.

bye !
Laurent Berta / terbac_@hotmail.com

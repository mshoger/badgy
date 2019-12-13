## Over WiFi
1. Charge your Badgy through USB or a rechargeable battery, or both. Make sure the power switch is in the off position (up)
2. Hold down the center button and slide the power switch on (down). Release the center button.
3. The screen should now update letting you know to connect to the "Badgy AP" network.
4. Using your mobile phone, connect to the "Badgy AP" WiFi network
5. Follow the captive portal instructions to select your desired WiFi connection. You do NOT need to set an IP address if you want badgy to grab a DHCP IP.
6. Once connected, Badgy is ready to receive new firmware! Go to http://*YOUR_IP_ADDRESS*:8888/update to upload your new firmware (see below)
7. WiFi credentials are automatically saved onboard, your Badgy will automatically connect to your selected network the next time you perform an update

## ShadowBadge
Within the Arduino IDE (install that first), you need to add the module for the badgy board. First, go into the preference for the IDE and set the Additional Boards Manager URL to https://arduino.esp8266.com/stable/package_esp8266com_index.json

Next, go to Tools --> Boards --> Boards Manager and install the ESP8266 board profile.

Now install the following libraries (Mandatory!). Go to Sketch --> Include Libraries --> Manage Libraries then search for:

1. GxEPD library: you can find the repo [here](https://github.com/ZinggJM/GxEPD).
2. AdaFruit GFX library: installation instructions available [here](https://learn.adafruit.com/adafruit-gfx-graphics-library/overview).
3. WifiManager library: you can find the repo [here](https://github.com/tzapu/WiFiManager).  You can also find this in the Library Manager under 'WiFiManager by tzapu'.
4. Time library: you can find the repo [here](https://github.com/PaulStoffregen/Time).  
5. ArduinoJson library: you can find the repo [here](https://github.com/bblanchon/ArduinoJson). You can also find this in the Library Manager under 'ArduinoJson by Benoit Blanchon'.   Make sure that you install a 5.x version since some of the examples require that.
6. AWS-SDK-ESP8266 (sha256 functionality)
7. QR Code qrcode_getModule

In addition, if you're on a Mac, you need to have Python 3 installed or none of this will compile correctly. Once all of these are installed and configured, grab the code in my hello folder. The .ino file is the file that defines what the badgy will display. You can edit your info in lines 41-45 unless you want to be me... You also need to update line 325. The URL inside the quotes can be literally anything. I suggest pointing it at your LinkedIn profile, GitHub, etc.

Now - even though you've got the right board profile installed, you need to set the IDE to compile to a different mode. Use Tools --> Board and set it to the NodeMCU 1.0 (ESP-12E Module). Don't ask me why...

Save the ino file and then compile your own bin file by clicking (in Arduino IDE) Sketch --> Export Compiled Library. Now return to your badgy's web page from before. Depending on your current firmware, you'll have a prompt to either upload a new sketch or a new firmware. Either way, point it at your new bin file and click upload. Once it says it's rebooting, the badgy should flash several times and display your info! If it "hangs," just turn it off for 10 seconds and turn it back on. Should fire right up.

Note that you might have to play with the number of characters on a given line so they don't wrap or overwrite your QR code.

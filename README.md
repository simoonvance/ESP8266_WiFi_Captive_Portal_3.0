# (Modded) ESP8266 WiFi Captive Portal 3.0

## Disclaimer
This project is for testing and educational purposes. Use it only against your own networks and devices. I don't take any responsibility for what you do with this program.

## About this project
WiFi captive portal for the Wemos D1 Mini Pro (ESP8266 Module) with DNS spoofing. It asks the user for an email and a password for a fake sign in, in order to "get connected to the internet".

I modified this code for my Senior capstone project at the vocational school I attend.

This project is built and optimized to run off a battery and a single board, lasting indifinitely while idle and ~10 days of broadcasting depending on your battery. The biggest features I added are network-range geofencing, overwrite friendly persistent memory, and a realistic portal page. The geofencing is triggered by being in range of any of 3 SSIDs, for development and failsafes.

The built-in LED will blink 5 times when some credentials are posted, and once per minute while broadcasting.

<b>Update!</b> Your saved credentials will be saved in overwrite-friendly flash storage using ESP_EEPROM. 

<b>Note:</b> If you want to see the stored credetials go to "**172.0.0.1**<a>/creds1</a>"

# Networking

This isn't my donation link but by all means support adamff-dev

<a href="https://www.buymeacoffee.com/rSiZtB3" target="_blank"><img src="https://www.buymeacoffee.com/assets/img/custom_images/orange_img.png" alt="Buy Me A Coffee" style="height: 41px !important;width: 174px !important;box-shadow: 0px 3px 2px 0px rgba(190, 190, 190, 0.5) !important;-webkit-box-shadow: 0px 3px 2px 0px rgba(190, 190, 190, 0.5) !important;" ></a>


# Installation (Arduino IDE)

1. Open your <a href="https://www.arduino.cc/en/main/software">Arduino IDE</a> and go to "File -> Preferences -> Boards Manager URLs" and paste the following link:
``http://arduino.esp8266.com/stable/package_esp8266com_index.json``
2. Go to "Tools -> Board -> Boards Manager", search "esp8266" and install esp8266
3. Go to "Tools -> Board" and select you board"
4. Download and open the sketch "<a href="https://github.com/simoonvance/ESP8266_WiFi_Captive_Portal_3.0/blob/master/ESP8266_WiFi_Captive_Portal_3.0.ino"><b>ESP8266_WiFi_Captive_Portal_3.0.ino</b></a>"
5. You can optionally change some parameters like the SSID name and texts of the page like title, subtitle, text body...
6. Upload the code into your board.
7. You are done!

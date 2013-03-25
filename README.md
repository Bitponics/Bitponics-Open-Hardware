bitponics_firmware
==================

### Setup
- Use Arduino 1.0.1
- In Terminal: 
    git submodule init
    git submodule update
- Copy all directories from `./lib` into your Arduino's global `libraries` folder.
- Arduino App: 
    - Menu Settings:
        - "Tools/Board" = "Arduino Mega 2560"
        - "Tools/Serial Port" = "dev/tty.usbmodem000492331"
    - Download "programmers.txt" from here: https://www.dropbox.com/s/ncnh1q3lldi8j44/programmers.txt
    - Right-click on Arduino app in Finder, select "Show Package Contents"
    - Paste programmers.txt into "Contents/Resources/Java/hardware/arduino"
    - Restart Arduino app.
    - Set "Tools/Programmer" to "AVR ISP v2"

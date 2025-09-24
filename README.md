Digital clock with esp32 + RGB LED matrix display.

TODO: Add a picture and schematic.

Currently the code is still a bit scuffed and WIP, but works :D.
Time is synced over WIFI every 24h.
There are multiple color styles including a slow hue shift. One button is used to switch styles.
Another button is used to change brightness. There is also a auto brightness setting that uses a photoresistor.

Used seeed xiao esp32-c6 and [this](https://botland.com.pl/wyswietlacze-segmentowe-i-matryce-led/6185-elastyczna-matryca-8x32-256-led-rgb-ws2812b-indywidualnie-adresowane-5904422374952.html) random 32x8 rgb led matrix.
Other esp variants can be used but you may need to change the pins used.

All pins and other constants (wifi passowrd etc...) are at the top of the .ino file.
You should also change the time zone, tz codes can be found here: https://github.com/nayarsystems/posix_tz_db/blob/fb5fa340cfa7599467358a347e5d6e6724d92bb2/zones.csv.

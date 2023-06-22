set COMPORT=COM3
python -m esptool --port %COMPORT% erase_flash
python -m esptool --port %COMPORT% --chip esp8266 write_flash --flash_size=detect -fm dio 0 "ESP_Code.ino.generic.bin"
pause

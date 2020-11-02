# airella-sensor-device

## Configuration via configuration file on SD card
Add configuration file in location `/airella.conf` on your SD card, insert card, and run station  
File structure:
```
{
    "wifi-ssid": "<string>",
    "wifi-password": "<string>",
    "register-token": "<string>",
    "refresh-token": "<string>",
    "api-url": "http://airella.cyfrogen.com/api",
    "api-station-id": "<string>",
    "station-name": "<string>",
    "address-country": "<string>",
    "address-city": "<string>",
    "address-street": "<string>",
    "address-number": "<string>",
    "loc-latitude": "<string>",
    "loc-longitude": "<string>",
    "loc-manual": false,
    "reg-state": 0,
    "inet-conn": 0,
    "access-token": "<string>",
    "persistedTime": 1604218415,
    "gsm-ext-url": "https://gsm-extender.airella.cyfrogen.com/",
    "gsm-config": "<string>"
}
```
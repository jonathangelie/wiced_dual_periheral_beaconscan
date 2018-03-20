Cypress Wiced Peripheral and Beacon scanner

## Objective

Creating a Bluetooth LE peripheral using Cypress Wiced SDK aims to:
advertising for accepting incoming connection.

## Advertising

Device is advertising during 30 seconds the complete Local name "beacon_scanner"

## Service discovery

Beacon scanner device is exposing following services and characteristics :

|     Service    |    Device Information    |     0x180A     |
|:--------------:|:------------------------:|:--------------:|
| Characteristic <td rowspan=3>| Manufacturer name String | Jonathan Gelie |
|                | Model Number String      | beacon scanner |
|                | Software Revision String |      1.0.0     |


## On disconnection event

If a disconnection occurs, the device is advertising again, to allow re-connection procedure.
None white list is in used.

## Advertising Timeout

If none connection has been established while advertising, an advertising timeout id triggered.
The device will start beacon scan procedure.

## Beacon scan

Smart device is scanning for beacon during 30 seconds.

For each single advertising report is checking if advertising packet is related to one of following beacons:

| Beacon Type |   |
|:-----------:|---|
|   iBeacon   | 0 |
|  Eddystone  | 1 |
|  Altbeacon  | 2 |


Then it will possible to localize your tracker by matching RSSI and calibrated TxPower within Beacons database

## Beacon scan timeout

As soon as beacon scan expired, we starting advertising again.

## Author
Jonathan Gelie <[contact@jonathangelie.com](mailto:contact@jonathangelie.com)>

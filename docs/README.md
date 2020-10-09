# Abstract

This project proposes a Low Level CAN Binding for redpesk@, with the following features:

* CAN messages and diagnostic messages (OBD-II) are currently supported
* Description of CAN messages is  done using JSON definition following OpenXC 
specifications (project OpenXC/vi-firmware)
* Encoding/Decoding of CAN messages reuses OpenXC libraries
* Proposes an API to other redpesk@ applications and services to read/write on 
CAN bus(es).
* Uses a publish/subscribe model with filtering for sending messages to clients
for efficiency

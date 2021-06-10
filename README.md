#  Nami_test_2

## Architecture
There are three main functions: 
- udp_task - Task which takes CSI data from queue and send it by UDP
- tcp_task - Task which creates a TCP socket and waiting for commands from controller 
- _wifi_csi_cb - callback function which called each time when new CSI data appears and send it to queue

## Protocol
UDP: just a row data (wifi_pkt_rx_ctrl_t + mac adress)
From esp_wifi_types.h
```c
    signed rssi:8;                /**< Received Signal Strength Indicator(RSSI) of packet. unit: dBm */
    unsigned rate:5;              /**< PHY rate encoding of the packet. Only valid for non HT(11bg) packet */
    unsigned :1;                  /**< reserved */
    unsigned sig_mode:2;          /**< 0: non HT(11bg) packet; 1: HT(11n) packet; 3: VHT(11ac) packet */
    unsigned :16;                 /**< reserved */
    unsigned mcs:7;               /**< Modulation Coding Scheme. If is HT(11n) packet, shows the modulation, range from 0 to 76(MSC0 ~ MCS76) */
    unsigned cwb:1;               /**< Channel Bandwidth of the packet. 0: 20MHz; 1: 40MHz */
    unsigned :16;                 /**< reserved */
    unsigned smoothing:1;         /**< reserved */
    unsigned not_sounding:1;      /**< reserved */
    unsigned :1;                  /**< reserved */
    unsigned aggregation:1;       /**< Aggregation. 0: MPDU packet; 1: AMPDU packet */
    unsigned stbc:2;              /**< Space Time Block Code(STBC). 0: non STBC packet; 1: STBC packet */
    unsigned fec_coding:1;        /**< Flag is set for 11n packets which are LDPC */
    unsigned sgi:1;               /**< Short Guide Interval(SGI). 0: Long GI; 1: Short GI */
    signed noise_floor:8;         /**< noise floor of Radio Frequency Module(RF). unit: 0.25dBm*/
    unsigned ampdu_cnt:8;         /**< ampdu cnt */
    unsigned channel:4;           /**< primary channel on which this packet is received */
    unsigned secondary_channel:4; /**< secondary channel on which this packet is received. 0: none; 1: above; 2: below */
    unsigned :8;                  /**< reserved */
    unsigned timestamp:32;        /**< timestamp. The local time when this packet is received. It is precise only if modem sleep or light sleep is not enabled. unit: microsecond */
    unsigned :32;                 /**< reserved */
    unsigned :31;                 /**< reserved */
    unsigned ant:1;               /**< antenna number from which this packet is received. 0: WiFi antenna 0; 1: WiFi antenna 1 */
    unsigned sig_len:12;          /**< length of packet including Frame Check Sequence(FCS) */
    unsigned :12;                 /**< reserved */
    unsigned rx_state:8;          /**< state of the packet. 0: no error; others: error numbers which are not public */
    char MAC [6];               /**< MAC adress */
```
TCP:
**[head] [length] [data] [CRC16]**
- head: [0x02, 0x54, 0x51]
- length: 2 bytes, length of data field
- data: 
	- [command] 	- 1 byte
	- [length] 		- 2 bytes
	- [data]		- length bytes

Command ‘C’ - config processing node
- [0x43] [0x00] [0x06] [processing node IP] [port] [CRC16] 	
- Example: **02 54 51 00 09 43 00 06 C0 A8 58 E8 18 CE 6F 07** for 192.168.88.232:6350

Command ‘A’ - add new transmitter
- [0x41] [0x00] [0x06] [transmitter IP] [CRC16] 
- Example: **02 54 51 00 09 41 00 06 30 E3 7A 29 04 00 CB CA** for add 30:E3:7A:29:04:00 at list
Command ‘D’ - delete transmitter
- [0x44] [0x00] [0x06] [transmitter IP] [CRC16] 
- Example: **02 54 51 00 09 44 00 06 30 E3 7A 29 04 00 CB CA** for delete 30:E3:7A:29:04:00 from list

Responses:
- OKEY: "\x02\x54\x41\x00\x00\x00\x01\x4C"
- ERR:  "\x02\x54\x41\xFF\x00\x00\xCE\x2F"
- CRC error: "\x02\x54\x41\xFE\x00\x00\xF9\x1F"
- TIMEOUT: "\x02\x54\x41\xFD\x00\x00\xA0\x4F"
- NOACCESS: "\x02\x54\x41\xFC\x00\x00\x97\x7F"
- UNKNOW: "\x02\x54\x41\xFF\x00\x00\xCE\x2F"

## How to build
Just downolad project and build with
```c
idf.py build
```
## Note
It's only up to 16 transmitters posible in this version

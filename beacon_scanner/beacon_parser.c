/*
 *  Copyright (C) 2018  Jonathan Gelie <contact@jonathangelie.com>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "bleprofile.h"
#include "blecen.h"
#include "bleapp.h"
#include "string.h"
#include "stdio.h"

#define APPLE_COMPANY_IDENTIFIER 	0x004C
#define APPLE_IBEACON_TYPE			0x02

#define EDDYSTONE_SERVICE_UUID		0xFEAA

#define ALTBEACON_ADV_DATA_LEN 		0x1B
#define ALTBEACON_DATA_LEN 			0x1C
#define ALTBEACON_CODE				0xBEAC

static inline UINT16 uint16_decode(const UINT8 *data)
{
	return ( (((UINT16)((UINT8 *)data)[0])) |
			 (((UINT16)((UINT8 *)data)[1]) << 8 ));
}

static BOOL beacon_parser_is_ibeacon(HCIULP_ADV_PACKET_REPORT_WDATA *evt)
{
	UINT8 *pdata;
	UINT8 state = 0;
	enum {
		FLAGS_CHECKED 		= 0x01,
		VENDOR_ID_CHECKED 	= 0x02,
	};

	for (pdata = (UINT8 *)&evt->data[0];
		 pdata < &evt->data[evt->dataLen];
		 pdata += pdata[0] + 1) { /* length */

		BLE_ADV_FIELD field = {0};

		field.len = pdata[0];
		field.val = pdata[1];
		memcpy(&field.data[0], &pdata[2], field.len - 1);

		switch (field.val) {
		case ADV_FLAGS:
			if (field.data[0] & BR_EDR_NOT_SUPPORTED) {
				state |= FLAGS_CHECKED;
			}
			break;
		case ADV_MANUFACTURER_DATA: {
			UINT16 uuid;
			uuid = uint16_decode((const UINT8 *)&field.data[0]);
			if (APPLE_COMPANY_IDENTIFIER == uuid &&
				APPLE_IBEACON_TYPE == field.data[2]) { /* iBeacon type */
				state |= VENDOR_ID_CHECKED;
			}
		}
			break;
		default:
			break;
		}

		if ((state & FLAGS_CHECKED) &&
			(state & VENDOR_ID_CHECKED)) {
			ble_trace1("AltBeacon detected\n");
			return TRUE;
		}
	}
	return FALSE;
}

static BOOL beacon_parser_is_eddystone(HCIULP_ADV_PACKET_REPORT_WDATA *evt)
{
	UINT8 *pdata;
	UINT8 state = 0;
	enum {
		UUID_LIST_CHECKED 		= 0x01,
		SERVICE_DATA_CHECKED 	= 0x02,
	};
	enum eddystone_type {
		EDDYSTONE_UID	= 0x00,
		EDDYSTONE_URL	= 0x10,
		EDDYSTONE_TLM	= 0x20,
		EDDYSTONE_EIP	= 0x30,
	};

	for (pdata = (UINT8 *)&evt->data[0];
		 pdata < &evt->data[evt->dataLen];
		 pdata += pdata[0] + 1) { /* length */

		BLE_ADV_FIELD field = {0};

		field.len = pdata[0];
		field.val = pdata[1];
		memcpy(&field.data[0], &pdata[2], field.len - 1);

		switch (field.val) {
		case ADV_SERVICE_UUID16_COMP: {
			UINT16 uuid;
			uuid = uint16_decode((const UINT8 *)&field.data[0]);
			if (EDDYSTONE_SERVICE_UUID == uuid) {
				state |= UUID_LIST_CHECKED;
			}
		}	break;
		case ADV_SERVICE_DATA: {
			UINT16 uuid;
			uuid = uint16_decode((const UINT8 *)&field.data[0]);
			if (EDDYSTONE_SERVICE_UUID == uuid) {
				state |= SERVICE_DATA_CHECKED;
			}
		}	break;

		default:
			break;
		}

		if ((state & UUID_LIST_CHECKED) &&
			(state & SERVICE_DATA_CHECKED)) {
			if (EDDYSTONE_UID == evt->data[11])
				ble_trace1("Eddystone UID detected\n");
			else if (EDDYSTONE_URL == evt->data[11])
				ble_trace1("Eddystone URL detected\n");
			else if (EDDYSTONE_TLM == evt->data[11])
				ble_trace1("Eddystone TLM detected\n");
			else if (EDDYSTONE_EIP == evt->data[11])
				ble_trace1("Eddystone EIP detected\n");
			return TRUE;
		}
	}
	return FALSE;
}

static BOOL beacon_parser_is_altbeacon(HCIULP_ADV_PACKET_REPORT_WDATA *evt)
{
	if (ALTBEACON_DATA_LEN == evt->dataLen) {
		BLE_ADV_FIELD field = {0};

		field.len = evt->data[0];
		field.val = evt->data[1];

		if (ALTBEACON_ADV_DATA_LEN == field.len &&
			ADV_MANUFACTURER_DATA == field.val) {

			UINT16 uuid;
			memcpy(&field.data[0], &evt->data[2], field.len - 1);

			uuid = uint16_decode((const UINT8 *)&field.data[2]);
			if (ALTBEACON_CODE == uuid) {
				ble_trace1("AltBeacon detected\n");
				return TRUE;
			}
		}
	}
	return FALSE;
}

void beacon_parser(HCIULP_ADV_PACKET_REPORT_WDATA *evt, enum beacon_type *btype)
{

	if (!btype)
		return;

	*btype = TYPE_UNKNOWN;

	if (beacon_parser_is_ibeacon(evt)) {
		*btype = TYPE_IBEACON;
	} else if (beacon_parser_is_eddystone(evt)) {
		*btype = TYPE_EDDYSTONE;
	} else if (beacon_parser_is_altbeacon(evt)) {
		*btype = TYPE_ALTBEACON;
	}
}

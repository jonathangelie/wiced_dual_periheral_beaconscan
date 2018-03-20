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

#ifndef BEACON_PARSER_H
#define BEACON_PARSER_H

/*
 * @brief beacon type supported
 */
enum beacon_type {
	TYPE_IBEACON 	= 0x00,
	TYPE_EDDYSTONE 	= 0x01,
	TYPE_ALTBEACON 	= 0x02,
	TYPE_UNKNOWN 	= 0xFF,
};

/*
 * @brief parsing a beacon
 * @param evt advertising packet
 * @param btype type of beacon found @ref beacon_type
 */
void beacon_parser(HCIULP_ADV_PACKET_REPORT_WDATA *evt,
				   enum beacon_type *btype);
#endif /* BEACON_PARSER_H */

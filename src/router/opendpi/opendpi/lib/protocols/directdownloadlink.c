/*
 * directdownloadlink.c
 * Copyright (C) 2009-2010 by ipoque GmbH
 * 
 * This file is part of OpenDPI, an open source deep packet inspection
 * library based on the PACE technology by ipoque GmbH
 * 
 * OpenDPI is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * OpenDPI is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with OpenDPI.  If not, see <http://www.gnu.org/licenses/>.
 * 
 */


#include "ipq_protocols.h"
#ifdef IPOQUE_PROTOCOL_DIRECT_DOWNLOAD_LINK


#ifdef IPOQUE_DEBUG_DIRECT_DOWNLOAD_LINK
//#define IPOQUE_DEBUG_DIRECT_DOWNLOAD_LINK_NOTHING_FOUND
//#define IPOQUE_DEBUG_DIRECT_DOWNLOAD_LINK_PACKET_TOO_SMALL
#define IPOQUE_DEBUG_DIRECT_DOWNLOAD_LINK_IP
#endif

static void ipoque_int_direct_download_link_add_connection(struct
														   ipoque_detection_module_struct
														   *ipoque_struct)
{
	struct ipoque_packet_struct *packet = &ipoque_struct->packet;
	struct ipoque_flow_struct *flow = ipoque_struct->flow;

	ipq_connection_detected(ipoque_struct, IPOQUE_PROTOCOL_DIRECT_DOWNLOAD_LINK);
	flow->ddlink_server_direction = packet->packet_direction;
}



/*
  return 0 if nothing has been detected
  return 1 if it is a megaupload packet
*/
static u8 search_ddl_domains(struct ipoque_detection_module_struct *ipoque_struct);
static u8 search_ddl_domains(struct ipoque_detection_module_struct *ipoque_struct)
{
	struct ipoque_packet_struct *packet = &ipoque_struct->packet;
	struct ipoque_parse_data pd;
	const u8 *p, *end, *line;
	int len;

	u16 filename_start = 0;
	u8 i = 1;
	u16 host_line_len_without_port;

	memset(&pd, 0, sizeof(pd));

	if (packet->payload_packet_len < 100) {
		IPQ_LOG(IPOQUE_PROTOCOL_DIRECT_DOWNLOAD_LINK, ipoque_struct, IPQ_LOG_DEBUG, "DDL: Packet too small.\n");
		goto end_ddl_nothing_found;
	}



	if (memcmp(packet->payload, "POST ", 5) == 0) {
		filename_start = 5;		// POST
		IPQ_LOG(IPOQUE_PROTOCOL_DIRECT_DOWNLOAD_LINK, ipoque_struct, IPQ_LOG_DEBUG, "DDL: POST FOUND\n");
	} else if (memcmp(packet->payload, "GET ", 4) == 0) {
		filename_start = 4;		// GET
		IPQ_LOG(IPOQUE_PROTOCOL_DIRECT_DOWNLOAD_LINK, ipoque_struct, IPQ_LOG_DEBUG, "DDL: GET FOUND\n");
	} else {
		goto end_ddl_nothing_found;
	}

	p = packet->payload;
	end = p + packet->payload_packet_len;

	if (!get_next_line(&p, end, &line, &len))
		goto end_ddl_nothing_found;

	if (len < 9 + filename_start || memcmp(&line[len - 9], " HTTP/1.", 8) != 0) {
		IPQ_LOG(IPOQUE_PROTOCOL_DIRECT_DOWNLOAD_LINK, ipoque_struct,
				IPQ_LOG_DEBUG, "DDL: PACKET NOT HTTP CONFORM.\nXXX%.*sXXX\n",
				8, &line[len - 9]);
		goto end_ddl_nothing_found;
	}
	// parse packet
	ipq_parse_packet_line_info(ipoque_struct, &pd);

	if (pd.host_line.ptr == NULL) {
		IPQ_LOG(IPOQUE_PROTOCOL_DIRECT_DOWNLOAD_LINK, ipoque_struct, IPQ_LOG_DEBUG, "DDL: NO HOST FOUND\n");
		goto end_ddl_nothing_found;
	}

	IPQ_LOG(IPOQUE_PROTOCOL_DIRECT_DOWNLOAD_LINK, ipoque_struct, IPQ_LOG_DEBUG, "DDL: Host: found\n");

// BEGIN OF AUTOMATED CODE GENERATION
	// first see if we have ':port' at the end of the line
	host_line_len_without_port = pd.host_line.len;
	if (host_line_len_without_port >= i && pd.host_line.ptr[host_line_len_without_port - i] >= '0'
		&& pd.host_line.ptr[pd.host_line.len - i] <= '9') {
		i = 2;
		while (host_line_len_without_port >= i && pd.host_line.ptr[host_line_len_without_port - i] >= '0'
			   && pd.host_line.ptr[host_line_len_without_port - i] <= '9') {
			IPQ_LOG(IPOQUE_PROTOCOL_DIRECT_DOWNLOAD_LINK, ipoque_struct, IPQ_LOG_DEBUG, "DDL: number found\n");
			i++;
		}
		if (host_line_len_without_port >= i && pd.host_line.ptr[host_line_len_without_port - i] == ':') {
			IPQ_LOG(IPOQUE_PROTOCOL_DIRECT_DOWNLOAD_LINK, ipoque_struct, IPQ_LOG_DEBUG, "DDL: ':' found\n");
			host_line_len_without_port = host_line_len_without_port - i;
		}
	}
	// then start automated code generation



	if (host_line_len_without_port >= 0 + 4
		&& memcmp((void *) &pd.host_line.ptr[host_line_len_without_port - 0 - 4], ".com", 4) == 0) {
		if (host_line_len_without_port >= 4 + 1 && pd.host_line.ptr[host_line_len_without_port - 4 - 1] == 'd') {
			if (host_line_len_without_port >= 5 + 6 + 1
				&& memcmp((void *) &pd.host_line.ptr[host_line_len_without_port - 5 - 6], "4share", 6) == 0
				&& (pd.host_line.ptr[host_line_len_without_port - 5 - 6 - 1] == ' '
					|| pd.host_line.ptr[host_line_len_without_port - 5 - 6 - 1] == '.')) {
				goto end_ddl_found;
			}
			if (host_line_len_without_port >= 5 + 8 + 1
				&& memcmp((void *) &pd.host_line.ptr[host_line_len_without_port - 5 - 8], "fileclou", 8) == 0
				&& (pd.host_line.ptr[host_line_len_without_port - 5 - 8 - 1] == ' '
					|| pd.host_line.ptr[host_line_len_without_port - 5 - 8 - 1] == '.')) {
				goto end_ddl_found;
			}
			if (host_line_len_without_port >= 5 + 5
				&& memcmp((void *) &pd.host_line.ptr[host_line_len_without_port - 5 - 5], "uploa", 5) == 0) {
				if (host_line_len_without_port >= 10 + 6 + 1
					&& memcmp((void *) &pd.host_line.ptr[host_line_len_without_port - 10 - 6], "files-", 6) == 0
					&& (pd.host_line.ptr[host_line_len_without_port - 10 - 6 - 1] == ' '
						|| pd.host_line.ptr[host_line_len_without_port - 10 - 6 - 1] == '.')) {
					goto end_ddl_found;
				}
				if (host_line_len_without_port >= 10 + 4 + 1
					&& memcmp((void *) &pd.host_line.ptr[host_line_len_without_port - 10 - 4], "mega", 4) == 0
					&& (pd.host_line.ptr[host_line_len_without_port - 10 - 4 - 1] == ' '
						|| pd.host_line.ptr[host_line_len_without_port - 10 - 4 - 1] == '.')) {
					goto end_ddl_found;
				}
				if (host_line_len_without_port >= 10 + 5 + 1
					&& memcmp((void *) &pd.host_line.ptr[host_line_len_without_port - 10 - 5], "rapid", 5) == 0
					&& (pd.host_line.ptr[host_line_len_without_port - 10 - 5 - 1] == ' '
						|| pd.host_line.ptr[host_line_len_without_port - 10 - 5 - 1] == '.')) {
					goto end_ddl_found;
				}
				if (host_line_len_without_port >= 10 + 5 + 1
					&& memcmp((void *) &pd.host_line.ptr[host_line_len_without_port - 10 - 5], "turbo", 5) == 0
					&& (pd.host_line.ptr[host_line_len_without_port - 10 - 5 - 1] == ' '
						|| pd.host_line.ptr[host_line_len_without_port - 10 - 5 - 1] == '.')) {
					goto end_ddl_found;
				}
				goto end_ddl_nothing_found;
			}
			goto end_ddl_nothing_found;
		}
		if (host_line_len_without_port >= 4 + 1 && pd.host_line.ptr[host_line_len_without_port - 4 - 1] == 'o') {
			if (host_line_len_without_port >= 5 + 6 + 1
				&& memcmp((void *) &pd.host_line.ptr[host_line_len_without_port - 5 - 6], "badong", 6) == 0
				&& (pd.host_line.ptr[host_line_len_without_port - 5 - 6 - 1] == ' '
					|| pd.host_line.ptr[host_line_len_without_port - 5 - 6 - 1] == '.')) {
				goto end_ddl_found;
			}
			if (host_line_len_without_port >= 5 + 5 + 1
				&& memcmp((void *) &pd.host_line.ptr[host_line_len_without_port - 5 - 5], "fileh", 5) == 0
				&& (pd.host_line.ptr[host_line_len_without_port - 5 - 5 - 1] == ' '
					|| pd.host_line.ptr[host_line_len_without_port - 5 - 5 - 1] == '.')) {
				goto end_ddl_found;
			}
			goto end_ddl_nothing_found;
		}
		if (host_line_len_without_port >= 4 + 1 && pd.host_line.ptr[host_line_len_without_port - 4 - 1] == 'g') {
			if (host_line_len_without_port >= 5 + 2
				&& memcmp((void *) &pd.host_line.ptr[host_line_len_without_port - 5 - 2], "in", 2) == 0) {
				if (host_line_len_without_port >= 7 + 4
					&& memcmp((void *) &pd.host_line.ptr[host_line_len_without_port - 7 - 4], "shar", 4) == 0) {
					if (host_line_len_without_port >= 11 + 4 + 1
						&& memcmp((void *) &pd.host_line.ptr[host_line_len_without_port - 11 - 4], "best", 4) == 0
						&& (pd.host_line.ptr[host_line_len_without_port - 11 - 4 - 1] == ' '
							|| pd.host_line.ptr[host_line_len_without_port - 11 - 4 - 1] == '.')) {
						goto end_ddl_found;
					}
					if (host_line_len_without_port >= 11 + 5 + 1
						&& memcmp((void *) &pd.host_line.ptr[host_line_len_without_port - 11 - 5], "quick", 5) == 0
						&& (pd.host_line.ptr[host_line_len_without_port - 11 - 5 - 1] == ' '
							|| pd.host_line.ptr[host_line_len_without_port - 11 - 5 - 1] == '.')) {
						goto end_ddl_found;
					}
					goto end_ddl_nothing_found;
				}
				if (host_line_len_without_port >= 7 + 6 + 1
					&& memcmp((void *) &pd.host_line.ptr[host_line_len_without_port - 7 - 6], "upload", 6) == 0
					&& (pd.host_line.ptr[host_line_len_without_port - 7 - 6 - 1] == ' '
						|| pd.host_line.ptr[host_line_len_without_port - 7 - 6 - 1] == '.')) {
					goto end_ddl_found;
				}
				goto end_ddl_nothing_found;
			}
			if (host_line_len_without_port >= 5 + 7 + 1
				&& memcmp((void *) &pd.host_line.ptr[host_line_len_without_port - 5 - 7], "sharebi", 7) == 0
				&& (pd.host_line.ptr[host_line_len_without_port - 5 - 7 - 1] == ' '
					|| pd.host_line.ptr[host_line_len_without_port - 5 - 7 - 1] == '.')) {
				goto end_ddl_found;
			}
			goto end_ddl_nothing_found;
		}
		if (host_line_len_without_port >= 4 + 8 + 1
			&& memcmp((void *) &pd.host_line.ptr[host_line_len_without_port - 4 - 8], "bigfilez", 8) == 0
			&& (pd.host_line.ptr[host_line_len_without_port - 4 - 8 - 1] == ' '
				|| pd.host_line.ptr[host_line_len_without_port - 4 - 8 - 1] == '.')) {
			goto end_ddl_found;
		}
		if (host_line_len_without_port >= 4 + 1 && pd.host_line.ptr[host_line_len_without_port - 4 - 1] == 'e') {
			if (host_line_len_without_port >= 5 + 3
				&& memcmp((void *) &pd.host_line.ptr[host_line_len_without_port - 5 - 3], "fil", 3) == 0) {
				if (host_line_len_without_port >= 8 + 2
					&& memcmp((void *) &pd.host_line.ptr[host_line_len_without_port - 8 - 2], "mo", 2) == 0) {
					if (host_line_len_without_port >= 10 + 5 + 1
						&& memcmp((void *) &pd.host_line.ptr[host_line_len_without_port - 10 - 5], "china", 5) == 0
						&& (pd.host_line.ptr[host_line_len_without_port - 10 - 5 - 1] == ' '
							|| pd.host_line.ptr[host_line_len_without_port - 10 - 5 - 1] == '.')) {
						goto end_ddl_found;
					}
					if (host_line_len_without_port >= 8 + 2 + 1
						&& (pd.host_line.ptr[host_line_len_without_port - 8 - 2 - 1] == ' '
							|| pd.host_line.ptr[host_line_len_without_port - 8 - 2 - 1] == '.')) {
						goto end_ddl_found;
					}
				}
				if (host_line_len_without_port >= 8 + 3 + 1
					&& memcmp((void *) &pd.host_line.ptr[host_line_len_without_port - 8 - 3], "hot", 3) == 0
					&& (pd.host_line.ptr[host_line_len_without_port - 8 - 3 - 1] == ' '
						|| pd.host_line.ptr[host_line_len_without_port - 8 - 3 - 1] == '.')) {
					goto end_ddl_found;
				}
				if (host_line_len_without_port >= 8 + 6 + 1
					&& memcmp((void *) &pd.host_line.ptr[host_line_len_without_port - 8 - 6], "keepmy", 6) == 0
					&& (pd.host_line.ptr[host_line_len_without_port - 8 - 6 - 1] == ' '
						|| pd.host_line.ptr[host_line_len_without_port - 8 - 6 - 1] == '.')) {
					goto end_ddl_found;
				}
				if (host_line_len_without_port >= 8 + 1
					&& pd.host_line.ptr[host_line_len_without_port - 8 - 1] == 'e') {
					if (host_line_len_without_port >= 9 + 3 + 1
						&& memcmp((void *) &pd.host_line.ptr[host_line_len_without_port - 9 - 3], "sav", 3) == 0
						&& (pd.host_line.ptr[host_line_len_without_port - 9 - 3 - 1] == ' '
							|| pd.host_line.ptr[host_line_len_without_port - 9 - 3 - 1] == '.')) {
						goto end_ddl_found;
					}
					if (host_line_len_without_port >= 9 + 5 + 1
						&& memcmp((void *) &pd.host_line.ptr[host_line_len_without_port - 9 - 5], "sendm", 5) == 0
						&& (pd.host_line.ptr[host_line_len_without_port - 9 - 5 - 1] == ' '
							|| pd.host_line.ptr[host_line_len_without_port - 9 - 5 - 1] == '.')) {
						goto end_ddl_found;
					}
					goto end_ddl_nothing_found;
				}
				if (host_line_len_without_port >= 8 + 8 + 1
					&& memcmp((void *) &pd.host_line.ptr[host_line_len_without_port - 8 - 8], "sharebig", 8) == 0
					&& (pd.host_line.ptr[host_line_len_without_port - 8 - 8 - 1] == ' '
						|| pd.host_line.ptr[host_line_len_without_port - 8 - 8 - 1] == '.')) {
					goto end_ddl_found;
				}
				if (host_line_len_without_port >= 8 + 3 + 1
					&& memcmp((void *) &pd.host_line.ptr[host_line_len_without_port - 8 - 3], "up-", 3) == 0
					&& (pd.host_line.ptr[host_line_len_without_port - 8 - 3 - 1] == ' '
						|| pd.host_line.ptr[host_line_len_without_port - 8 - 3 - 1] == '.')) {
					goto end_ddl_found;
				}
				goto end_ddl_nothing_found;
			}
			if (host_line_len_without_port >= 5 + 1 && pd.host_line.ptr[host_line_len_without_port - 5 - 1] == 'r') {
				if (host_line_len_without_port >= 6 + 3
					&& memcmp((void *) &pd.host_line.ptr[host_line_len_without_port - 6 - 3], "sha", 3) == 0) {
					if (host_line_len_without_port >= 9 + 1
						&& pd.host_line.ptr[host_line_len_without_port - 9 - 1] == '-') {
						if (host_line_len_without_port >= 10 + 4 + 1
							&& memcmp((void *) &pd.host_line.ptr[host_line_len_without_port - 10 - 4], "easy",
									  4) == 0 && (pd.host_line.ptr[host_line_len_without_port - 10 - 4 - 1] == ' '
												  || pd.host_line.ptr[host_line_len_without_port - 10 - 4 - 1] ==
												  '.')) {
							goto end_ddl_found;
						}
						if (host_line_len_without_port >= 10 + 4 + 1
							&& memcmp((void *) &pd.host_line.ptr[host_line_len_without_port - 10 - 4], "fast",
									  4) == 0 && (pd.host_line.ptr[host_line_len_without_port - 10 - 4 - 1] == ' '
												  || pd.host_line.ptr[host_line_len_without_port - 10 - 4 - 1] ==
												  '.')) {
							goto end_ddl_found;
						}
						if (host_line_len_without_port >= 10 + 4 + 1
							&& memcmp((void *) &pd.host_line.ptr[host_line_len_without_port - 10 - 4], "live",
									  4) == 0 && (pd.host_line.ptr[host_line_len_without_port - 10 - 4 - 1] == ' '
												  || pd.host_line.ptr[host_line_len_without_port - 10 - 4 - 1] ==
												  '.')) {
							goto end_ddl_found;
						}
						goto end_ddl_nothing_found;
					}
					if (host_line_len_without_port >= 9 + 4 + 1
						&& memcmp((void *) &pd.host_line.ptr[host_line_len_without_port - 9 - 4], "ftp2", 4) == 0
						&& (pd.host_line.ptr[host_line_len_without_port - 9 - 4 - 1] == ' '
							|| pd.host_line.ptr[host_line_len_without_port - 9 - 4 - 1] == '.')) {
						goto end_ddl_found;
					}
					if (host_line_len_without_port >= 9 + 4 + 1
						&& memcmp((void *) &pd.host_line.ptr[host_line_len_without_port - 9 - 4], "gige", 4) == 0
						&& (pd.host_line.ptr[host_line_len_without_port - 9 - 4 - 1] == ' '
							|| pd.host_line.ptr[host_line_len_without_port - 9 - 4 - 1] == '.')) {
						goto end_ddl_found;
					}
					if (host_line_len_without_port >= 9 + 4 + 1
						&& memcmp((void *) &pd.host_line.ptr[host_line_len_without_port - 9 - 4], "mega", 4) == 0
						&& (pd.host_line.ptr[host_line_len_without_port - 9 - 4 - 1] == ' '
							|| pd.host_line.ptr[host_line_len_without_port - 9 - 4 - 1] == '.')) {
						goto end_ddl_found;
					}
					if (host_line_len_without_port >= 9 + 5 + 1
						&& memcmp((void *) &pd.host_line.ptr[host_line_len_without_port - 9 - 5], "rapid", 5) == 0
						&& (pd.host_line.ptr[host_line_len_without_port - 9 - 5 - 1] == ' '
							|| pd.host_line.ptr[host_line_len_without_port - 9 - 5 - 1] == '.')) {
						goto end_ddl_found;
					}
					goto end_ddl_nothing_found;
				}
				if (host_line_len_without_port >= 6 + 7 + 1
					&& memcmp((void *) &pd.host_line.ptr[host_line_len_without_port - 6 - 7], "mediafi", 7) == 0
					&& (pd.host_line.ptr[host_line_len_without_port - 6 - 7 - 1] == ' '
						|| pd.host_line.ptr[host_line_len_without_port - 6 - 7 - 1] == '.')) {
					goto end_ddl_found;
				}
				goto end_ddl_nothing_found;
			}
			if (host_line_len_without_port >= 5 + 7 + 1
				&& memcmp((void *) &pd.host_line.ptr[host_line_len_without_port - 5 - 7], "gigasiz", 7) == 0
				&& (pd.host_line.ptr[host_line_len_without_port - 5 - 7 - 1] == ' '
					|| pd.host_line.ptr[host_line_len_without_port - 5 - 7 - 1] == '.')) {
				goto end_ddl_found;
			}
			if (host_line_len_without_port >= 5 + 8 + 1
				&& memcmp((void *) &pd.host_line.ptr[host_line_len_without_port - 5 - 8], "sendspac", 8) == 0
				&& (pd.host_line.ptr[host_line_len_without_port - 5 - 8 - 1] == ' '
					|| pd.host_line.ptr[host_line_len_without_port - 5 - 8 - 1] == '.')) {
				goto end_ddl_found;
			}
			if (host_line_len_without_port >= 5 + 7 + 1
				&& memcmp((void *) &pd.host_line.ptr[host_line_len_without_port - 5 - 7], "sharebe", 7) == 0
				&& (pd.host_line.ptr[host_line_len_without_port - 5 - 7 - 1] == ' '
					|| pd.host_line.ptr[host_line_len_without_port - 5 - 7 - 1] == '.')) {
				goto end_ddl_found;
			}
			if (host_line_len_without_port >= 5 + 11 + 1
				&& memcmp((void *) &pd.host_line.ptr[host_line_len_without_port - 5 - 11], "sharebigfli", 11) == 0
				&& (pd.host_line.ptr[host_line_len_without_port - 5 - 11 - 1] == ' '
					|| pd.host_line.ptr[host_line_len_without_port - 5 - 11 - 1] == '.')) {
				goto end_ddl_found;
			}
			goto end_ddl_nothing_found;
		}
		if (host_line_len_without_port >= 4 + 1 && pd.host_line.ptr[host_line_len_without_port - 4 - 1] == 's') {
			if (host_line_len_without_port >= 5 + 1 && pd.host_line.ptr[host_line_len_without_port - 5 - 1] == 'e') {
				if (host_line_len_without_port >= 6 + 10 + 1
					&& memcmp((void *) &pd.host_line.ptr[host_line_len_without_port - 6 - 10], "depositfil",
							  10) == 0 && (pd.host_line.ptr[host_line_len_without_port - 6 - 10 - 1] == ' '
										   || pd.host_line.ptr[host_line_len_without_port - 6 - 10 - 1] == '.')) {
					goto end_ddl_found;
				}
				if (host_line_len_without_port >= 6 + 8 + 1
					&& memcmp((void *) &pd.host_line.ptr[host_line_len_without_port - 6 - 8], "megashar", 8) == 0
					&& (pd.host_line.ptr[host_line_len_without_port - 6 - 8 - 1] == ' '
						|| pd.host_line.ptr[host_line_len_without_port - 6 - 8 - 1] == '.')) {
					goto end_ddl_found;
				}
				goto end_ddl_nothing_found;
			}
			if (host_line_len_without_port >= 5 + 10 + 1
				&& memcmp((void *) &pd.host_line.ptr[host_line_len_without_port - 5 - 10], "fileupyour", 10) == 0
				&& (pd.host_line.ptr[host_line_len_without_port - 5 - 10 - 1] == ' '
					|| pd.host_line.ptr[host_line_len_without_port - 5 - 10 - 1] == '.')) {
				goto end_ddl_found;
			}
			goto end_ddl_nothing_found;
		}
		if (host_line_len_without_port >= 4 + 11 + 1
			&& memcmp((void *) &pd.host_line.ptr[host_line_len_without_port - 4 - 11], "filefactory", 11) == 0
			&& (pd.host_line.ptr[host_line_len_without_port - 4 - 11 - 1] == ' '
				|| pd.host_line.ptr[host_line_len_without_port - 4 - 11 - 1] == '.')) {
			goto end_ddl_found;
		}
		if (host_line_len_without_port >= 4 + 1 && pd.host_line.ptr[host_line_len_without_port - 4 - 1] == 't') {
			if (host_line_len_without_port >= 5 + 8 + 1
				&& memcmp((void *) &pd.host_line.ptr[host_line_len_without_port - 5 - 8], "filefron", 8) == 0
				&& (pd.host_line.ptr[host_line_len_without_port - 5 - 8 - 1] == ' '
					|| pd.host_line.ptr[host_line_len_without_port - 5 - 8 - 1] == '.')) {
				goto end_ddl_found;
			}
			if (host_line_len_without_port >= 5 + 10 + 1
				&& memcmp((void *) &pd.host_line.ptr[host_line_len_without_port - 5 - 10], "uploadingi", 10) == 0
				&& (pd.host_line.ptr[host_line_len_without_port - 5 - 10 - 1] == ' '
					|| pd.host_line.ptr[host_line_len_without_port - 5 - 10 - 1] == '.')) {
				goto end_ddl_found;
			}
			if (host_line_len_without_port >= 5 + 11 + 1
				&& memcmp((void *) &pd.host_line.ptr[host_line_len_without_port - 5 - 11], "yourfilehos", 11) == 0
				&& (pd.host_line.ptr[host_line_len_without_port - 5 - 11 - 1] == ' '
					|| pd.host_line.ptr[host_line_len_without_port - 5 - 11 - 1] == '.')) {
				goto end_ddl_found;
			}
			goto end_ddl_nothing_found;
		}
		if (host_line_len_without_port >= 4 + 1 && pd.host_line.ptr[host_line_len_without_port - 4 - 1] == 'r') {
			if (host_line_len_without_port >= 5 + 8 + 1
				&& memcmp((void *) &pd.host_line.ptr[host_line_len_without_port - 5 - 8], "mytempdi", 8) == 0
				&& (pd.host_line.ptr[host_line_len_without_port - 5 - 8 - 1] == ' '
					|| pd.host_line.ptr[host_line_len_without_port - 5 - 8 - 1] == '.')) {
				goto end_ddl_found;
			}
			if (host_line_len_without_port >= 5 + 10 + 1
				&& memcmp((void *) &pd.host_line.ptr[host_line_len_without_port - 5 - 10], "uploadpowe", 10) == 0
				&& (pd.host_line.ptr[host_line_len_without_port - 5 - 10 - 1] == ' '
					|| pd.host_line.ptr[host_line_len_without_port - 5 - 10 - 1] == '.')) {
				goto end_ddl_found;
			}
			goto end_ddl_nothing_found;
		}
		goto end_ddl_nothing_found;
	}
	if (host_line_len_without_port >= 0 + 4
		&& memcmp((void *) &pd.host_line.ptr[host_line_len_without_port - 0 - 4], ".net", 4) == 0) {
		if (host_line_len_without_port >= 4 + 7 + 1
			&& memcmp((void *) &pd.host_line.ptr[host_line_len_without_port - 4 - 7], "badongo", 7) == 0
			&& (pd.host_line.ptr[host_line_len_without_port - 4 - 7 - 1] == ' '
				|| pd.host_line.ptr[host_line_len_without_port - 4 - 7 - 1] == '.')) {
			goto end_ddl_found;
		}
		if (host_line_len_without_port >= 4 + 1 && pd.host_line.ptr[host_line_len_without_port - 4 - 1] == 'd') {
			if (host_line_len_without_port >= 5 + 3
				&& memcmp((void *) &pd.host_line.ptr[host_line_len_without_port - 5 - 3], "loa", 3) == 0) {
				if (host_line_len_without_port >= 8 + 5 + 1
					&& memcmp((void *) &pd.host_line.ptr[host_line_len_without_port - 8 - 5], "fast-", 5) == 0
					&& (pd.host_line.ptr[host_line_len_without_port - 8 - 5 - 1] == ' '
						|| pd.host_line.ptr[host_line_len_without_port - 8 - 5 - 1] == '.')) {
					goto end_ddl_found;
				}
				if (host_line_len_without_port >= 8 + 2
					&& memcmp((void *) &pd.host_line.ptr[host_line_len_without_port - 8 - 2], "up", 2) == 0) {
					if (host_line_len_without_port >= 10 + 5 + 1
						&& memcmp((void *) &pd.host_line.ptr[host_line_len_without_port - 10 - 5], "file-", 5) == 0
						&& (pd.host_line.ptr[host_line_len_without_port - 10 - 5 - 1] == ' '
							|| pd.host_line.ptr[host_line_len_without_port - 10 - 5 - 1] == '.')) {
						goto end_ddl_found;
					}
					if (host_line_len_without_port >= 10 + 6 + 1
						&& memcmp((void *) &pd.host_line.ptr[host_line_len_without_port - 10 - 6], "simple",
								  6) == 0 && (pd.host_line.ptr[host_line_len_without_port - 10 - 6 - 1] == ' '
											  || pd.host_line.ptr[host_line_len_without_port - 10 - 6 - 1] ==
											  '.')) {
						goto end_ddl_found;
					}
					if (host_line_len_without_port >= 10 + 3 + 1
						&& memcmp((void *) &pd.host_line.ptr[host_line_len_without_port - 10 - 3], "wii", 3) == 0
						&& (pd.host_line.ptr[host_line_len_without_port - 10 - 3 - 1] == ' '
							|| pd.host_line.ptr[host_line_len_without_port - 10 - 3 - 1] == '.')) {
						goto end_ddl_found;
					}
					goto end_ddl_nothing_found;
				}
				goto end_ddl_nothing_found;
			}
			if (host_line_len_without_port >= 5 + 7 + 1
				&& memcmp((void *) &pd.host_line.ptr[host_line_len_without_port - 5 - 7], "filesen", 7) == 0
				&& (pd.host_line.ptr[host_line_len_without_port - 5 - 7 - 1] == ' '
					|| pd.host_line.ptr[host_line_len_without_port - 5 - 7 - 1] == '.')) {
				goto end_ddl_found;
			}
			goto end_ddl_nothing_found;
		}
		if (host_line_len_without_port >= 4 + 5 + 1
			&& memcmp((void *) &pd.host_line.ptr[host_line_len_without_port - 4 - 5], "filer", 5) == 0
			&& (pd.host_line.ptr[host_line_len_without_port - 4 - 5 - 1] == ' '
				|| pd.host_line.ptr[host_line_len_without_port - 4 - 5 - 1] == '.')) {
			goto end_ddl_found;
		}
		if (host_line_len_without_port >= 4 + 9 + 1
			&& memcmp((void *) &pd.host_line.ptr[host_line_len_without_port - 4 - 9], "livedepot", 9) == 0
			&& (pd.host_line.ptr[host_line_len_without_port - 4 - 9 - 1] == ' '
				|| pd.host_line.ptr[host_line_len_without_port - 4 - 9 - 1] == '.')) {
			goto end_ddl_found;
		}
		if (host_line_len_without_port >= 4 + 1 && pd.host_line.ptr[host_line_len_without_port - 4 - 1] == 'e') {
			if (host_line_len_without_port >= 5 + 5 + 1
				&& memcmp((void *) &pd.host_line.ptr[host_line_len_without_port - 5 - 5], "mofil", 5) == 0
				&& (pd.host_line.ptr[host_line_len_without_port - 5 - 5 - 1] == ' '
					|| pd.host_line.ptr[host_line_len_without_port - 5 - 5 - 1] == '.')) {
				goto end_ddl_found;
			}
			if (host_line_len_without_port >= 5 + 17 + 1
				&& memcmp((void *) &pd.host_line.ptr[host_line_len_without_port - 5 - 17], "odsiebie.najlepsz",
						  17) == 0 && (pd.host_line.ptr[host_line_len_without_port - 5 - 17 - 1] == ' '
									   || pd.host_line.ptr[host_line_len_without_port - 5 - 17 - 1] == '.')) {
				goto end_ddl_found;
			}
			if (host_line_len_without_port >= 5 + 5 + 1
				&& memcmp((void *) &pd.host_line.ptr[host_line_len_without_port - 5 - 5], "zshar", 5) == 0
				&& (pd.host_line.ptr[host_line_len_without_port - 5 - 5 - 1] == ' '
					|| pd.host_line.ptr[host_line_len_without_port - 5 - 5 - 1] == '.')) {
				goto end_ddl_found;
			}
			goto end_ddl_nothing_found;
		}
		goto end_ddl_nothing_found;
	}
	if (host_line_len_without_port >= 0 + 1 && pd.host_line.ptr[host_line_len_without_port - 0 - 1] == 'u') {
		if (host_line_len_without_port >= 1 + 6 + 1
			&& memcmp((void *) &pd.host_line.ptr[host_line_len_without_port - 1 - 6], "data.h", 6) == 0
			&& (pd.host_line.ptr[host_line_len_without_port - 1 - 6 - 1] == ' '
				|| pd.host_line.ptr[host_line_len_without_port - 1 - 6 - 1] == '.')) {
			goto end_ddl_found;
		}
		if (host_line_len_without_port >= 1 + 2
			&& memcmp((void *) &pd.host_line.ptr[host_line_len_without_port - 1 - 2], ".r", 2) == 0) {
			if (host_line_len_without_port >= 3 + 10 + 1
				&& memcmp((void *) &pd.host_line.ptr[host_line_len_without_port - 3 - 10], "filearchiv", 10) == 0
				&& (pd.host_line.ptr[host_line_len_without_port - 3 - 10 - 1] == ' '
					|| pd.host_line.ptr[host_line_len_without_port - 3 - 10 - 1] == '.')) {
				goto end_ddl_found;
			}
			if (host_line_len_without_port >= 3 + 8 + 1
				&& memcmp((void *) &pd.host_line.ptr[host_line_len_without_port - 3 - 8], "filepost", 8) == 0
				&& (pd.host_line.ptr[host_line_len_without_port - 3 - 8 - 1] == ' '
					|| pd.host_line.ptr[host_line_len_without_port - 3 - 8 - 1] == '.')) {
				goto end_ddl_found;
			}
			if (host_line_len_without_port >= 3 + 7 + 1
				&& memcmp((void *) &pd.host_line.ptr[host_line_len_without_port - 3 - 7], "ifolder", 7) == 0
				&& (pd.host_line.ptr[host_line_len_without_port - 3 - 7 - 1] == ' '
					|| pd.host_line.ptr[host_line_len_without_port - 3 - 7 - 1] == '.')) {
				goto end_ddl_found;
			}
			goto end_ddl_nothing_found;
		}
		goto end_ddl_nothing_found;
	}
	if (host_line_len_without_port >= 0 + 11 + 1
		&& memcmp((void *) &pd.host_line.ptr[host_line_len_without_port - 0 - 11], "filehost.tv", 11) == 0
		&& (pd.host_line.ptr[host_line_len_without_port - 0 - 11 - 1] == ' '
			|| pd.host_line.ptr[host_line_len_without_port - 0 - 11 - 1] == '.')) {
		goto end_ddl_found;
	}
	if (host_line_len_without_port >= 0 + 3
		&& memcmp((void *) &pd.host_line.ptr[host_line_len_without_port - 0 - 3], ".to", 3) == 0) {
		if (host_line_len_without_port >= 3 + 1 && pd.host_line.ptr[host_line_len_without_port - 3 - 1] == 'e') {
			if (host_line_len_without_port >= 4 + 7 + 1
				&& memcmp((void *) &pd.host_line.ptr[host_line_len_without_port - 4 - 7], "filesaf", 7) == 0
				&& (pd.host_line.ptr[host_line_len_without_port - 4 - 7 - 1] == ' '
					|| pd.host_line.ptr[host_line_len_without_port - 4 - 7 - 1] == '.')) {
				goto end_ddl_found;
			}
			if (host_line_len_without_port >= 4 + 8 + 1
				&& memcmp((void *) &pd.host_line.ptr[host_line_len_without_port - 4 - 8], "sharebas", 8) == 0
				&& (pd.host_line.ptr[host_line_len_without_port - 4 - 8 - 1] == ' '
					|| pd.host_line.ptr[host_line_len_without_port - 4 - 8 - 1] == '.')) {
				goto end_ddl_found;
			}
			goto end_ddl_nothing_found;
		}
		if (host_line_len_without_port >= 3 + 5 + 1
			&& memcmp((void *) &pd.host_line.ptr[host_line_len_without_port - 3 - 5], "files", 5) == 0
			&& (pd.host_line.ptr[host_line_len_without_port - 3 - 5 - 1] == ' '
				|| pd.host_line.ptr[host_line_len_without_port - 3 - 5 - 1] == '.')) {
			goto end_ddl_found;
		}
		if (host_line_len_without_port >= 3 + 1 && pd.host_line.ptr[host_line_len_without_port - 3 - 1] == 'd') {
			if (host_line_len_without_port >= 4 + 3
				&& memcmp((void *) &pd.host_line.ptr[host_line_len_without_port - 4 - 3], "loa", 3) == 0) {
				if (host_line_len_without_port >= 7 + 7 + 1
					&& memcmp((void *) &pd.host_line.ptr[host_line_len_without_port - 7 - 7], "file-up", 7) == 0
					&& (pd.host_line.ptr[host_line_len_without_port - 7 - 7 - 1] == ' '
						|| pd.host_line.ptr[host_line_len_without_port - 7 - 7 - 1] == '.')) {
					goto end_ddl_found;
				}
				if (host_line_len_without_port >= 4 + 3 + 1
					&& (pd.host_line.ptr[host_line_len_without_port - 4 - 3 - 1] == ' '
						|| pd.host_line.ptr[host_line_len_without_port - 4 - 3 - 1] == '.')) {
					goto end_ddl_found;
				}
			}
			if (host_line_len_without_port >= 4 + 7 + 1
				&& memcmp((void *) &pd.host_line.ptr[host_line_len_without_port - 4 - 7], "uploade", 7) == 0
				&& (pd.host_line.ptr[host_line_len_without_port - 4 - 7 - 1] == ' '
					|| pd.host_line.ptr[host_line_len_without_port - 4 - 7 - 1] == '.')) {
				goto end_ddl_found;
			}
			goto end_ddl_nothing_found;
		}
		goto end_ddl_nothing_found;
	}
	if (host_line_len_without_port >= 0 + 1 && pd.host_line.ptr[host_line_len_without_port - 0 - 1] == 'z') {
		if (host_line_len_without_port >= 1 + 14 + 1
			&& memcmp((void *) &pd.host_line.ptr[host_line_len_without_port - 1 - 14], "leteckaposta.c", 14) == 0
			&& (pd.host_line.ptr[host_line_len_without_port - 1 - 14 - 1] == ' '
				|| pd.host_line.ptr[host_line_len_without_port - 1 - 14 - 1] == '.')) {
			goto end_ddl_found;
		}
		if (host_line_len_without_port >= 1 + 12 + 1
			&& memcmp((void *) &pd.host_line.ptr[host_line_len_without_port - 1 - 12], "yourfiles.bi", 12) == 0
			&& (pd.host_line.ptr[host_line_len_without_port - 1 - 12 - 1] == ' '
				|| pd.host_line.ptr[host_line_len_without_port - 1 - 12 - 1] == '.')) {
			goto end_ddl_found;
		}
		goto end_ddl_nothing_found;
	}
	if (host_line_len_without_port >= 0 + 10 + 1
		&& memcmp((void *) &pd.host_line.ptr[host_line_len_without_port - 0 - 10], "netload.in", 10) == 0
		&& (pd.host_line.ptr[host_line_len_without_port - 0 - 10 - 1] == ' '
			|| pd.host_line.ptr[host_line_len_without_port - 0 - 10 - 1] == '.')) {
		goto end_ddl_found;
	}
	if (host_line_len_without_port >= 0 + 3
		&& memcmp((void *) &pd.host_line.ptr[host_line_len_without_port - 0 - 3], ".de", 3) == 0) {
		if (host_line_len_without_port >= 3 + 5
			&& memcmp((void *) &pd.host_line.ptr[host_line_len_without_port - 3 - 5], "share", 5) == 0) {
			if (host_line_len_without_port >= 8 + 5 + 1
				&& memcmp((void *) &pd.host_line.ptr[host_line_len_without_port - 8 - 5], "rapid", 5) == 0
				&& (pd.host_line.ptr[host_line_len_without_port - 8 - 5 - 1] == ' '
					|| pd.host_line.ptr[host_line_len_without_port - 8 - 5 - 1] == '.')) {
				goto end_ddl_found;
			}
			if (host_line_len_without_port >= 8 + 5 + 1
				&& memcmp((void *) &pd.host_line.ptr[host_line_len_without_port - 8 - 5], "ultra", 5) == 0
				&& (pd.host_line.ptr[host_line_len_without_port - 8 - 5 - 1] == ' '
					|| pd.host_line.ptr[host_line_len_without_port - 8 - 5 - 1] == '.')) {
				goto end_ddl_found;
			}
			goto end_ddl_nothing_found;
		}
		if (host_line_len_without_port >= 3 + 15 + 1
			&& memcmp((void *) &pd.host_line.ptr[host_line_len_without_port - 3 - 15], "uploadyourfiles", 15) == 0
			&& (pd.host_line.ptr[host_line_len_without_port - 3 - 15 - 1] == ' '
				|| pd.host_line.ptr[host_line_len_without_port - 3 - 15 - 1] == '.')) {
			goto end_ddl_found;
		}
		goto end_ddl_nothing_found;
	}
	if (host_line_len_without_port >= 0 + 14 + 1
		&& memcmp((void *) &pd.host_line.ptr[host_line_len_without_port - 0 - 14], "speedshare.org", 14) == 0
		&& (pd.host_line.ptr[host_line_len_without_port - 0 - 14 - 1] == ' '
			|| pd.host_line.ptr[host_line_len_without_port - 0 - 14 - 1] == '.')) {
		goto end_ddl_found;
	}
// END OF AUTOMATED CODE GENERATION

	/* This is the hard way. We do this in order to find the download of services when other
	   domains are involved. This is not significant if ddl is blocked. --> then the link can not be started because
	   the ads are not viewed. But when ddl is only limited then the download is the important part.
	 */

  end_ddl_nothing_found:
	IPQ_LOG(IPOQUE_PROTOCOL_DIRECT_DOWNLOAD_LINK, ipoque_struct, IPQ_LOG_DEBUG,
			"Nothing Found\n%.*s\n", packet->payload_packet_len, packet->payload);
	return 0;

  end_ddl_found:
	IPQ_LOG(IPOQUE_PROTOCOL_DIRECT_DOWNLOAD_LINK, ipoque_struct, IPQ_LOG_DEBUG, "DDL: DIRECT DOWNLOAD LINK FOUND\n");
	ipoque_int_direct_download_link_add_connection(ipoque_struct);
	return 1;
}


static void ipoque_search_direct_download_link_tcp(struct
											ipoque_detection_module_struct
											*ipoque_struct)
{
	struct ipoque_packet_struct *packet = &ipoque_struct->packet;
	struct ipoque_flow_struct *flow = ipoque_struct->flow;

	// do not detect again if it is already ddl
	if (packet->detected_protocol != IPOQUE_PROTOCOL_DIRECT_DOWNLOAD_LINK) {
		if (search_ddl_domains(ipoque_struct) != 0) {
			return;
		}
		IPOQUE_ADD_PROTOCOL_TO_BITMASK(flow->excluded_protocol_bitmask, IPOQUE_PROTOCOL_DIRECT_DOWNLOAD_LINK);
	}

}
#endif

/*
 * smb.c
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
#ifdef IPOQUE_PROTOCOL_SMB

static void ipoque_search_smb_tcp(struct ipoque_detection_module_struct *ipoque_struct)
{
	struct ipoque_packet_struct *packet = &ipoque_struct->packet;
	struct ipoque_flow_struct *flow = ipoque_struct->flow;

	IPQ_LOG(IPOQUE_PROTOCOL_SMB, ipoque_struct, IPQ_LOG_DEBUG, "search SMB.\n");


	if (packet->tcp->dest == htons(445)
		&& packet->payload_packet_len > (32 + 4 + 4)
		&& (packet->payload_packet_len - 4) == ntohl(get_u32(packet->payload, 0))
		&& get_u32(packet->payload, 4) == htonl(0xff534d42)) {
		IPQ_LOG(IPOQUE_PROTOCOL_SMB, ipoque_struct, IPQ_LOG_DEBUG, "found SMB.\n");
		ipq_connection_detected(ipoque_struct, IPOQUE_PROTOCOL_SMB);
		return;

	}


	IPQ_LOG(IPOQUE_PROTOCOL_SMB, ipoque_struct, IPQ_LOG_DEBUG, "exclude SMB.\n");
	IPOQUE_ADD_PROTOCOL_TO_BITMASK(flow->excluded_protocol_bitmask, IPOQUE_PROTOCOL_SMB);
}

#endif

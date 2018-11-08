
#ifndef _NETFLOW_V5_INC
#define _NETFLOW_V5_INC


#include "debug.h"


#define UDP_V5_MAGIC 0x31343634

typedef struct _V5_head
{
	U32 magic;
	U16 version;
	U16 count;
	U32 SysUptime;
	U32 unix_secs;
	U32 unix_nsecs;
	U32 flow_sequence;
	BYTE engine_type;
	BYTE engine_id;
	U16 reserved;
} V5_head, *PV5_head;

#define NETFLOW_V5_HEAD_LEN		(sizeof(V5_head))
#define NETFLOW_V5_CELL_LEN		48

/*
typedef struct _V5_field
{
	U32 srcIp;
	U32 dstIp;
	U32 nexthop;
	U16 inputSNMP;
	U16 outputSNMP;
	U32 inPkts;
	U32 inBytes;

	U32 firstSwitchID;
	U32 lastWwitchID;
	U16 srcPort;
	U16 dstPort;

	U16 pad1;
	BYTE proto;
	BYTE tos;
	BYTE flags;
	U16 pad2;
	BYTE pad3;
	U32 reserved;
}V5_field, *PV5_field;
*/

/*
nprobe: covert

IPV4_SRC_ADDR|IPV4_DST_ADDR|INPUT_SNMP|OUTPUT_SNMP|IN_PKTS|IN_BYTES|
FIRST_SWITCHED|LAST_SWITCHED|L4_SRC_PORT|L4_DST_PORT|
TCP_FLAGS|PROTOCOL|SRC_TOS|SRC_AS|DST_AS
*/

#define V5_FIELD_NUM 17

typedef struct _V5_body
{
	U32 srcIp;
	U32 dstIp;
	U32 nexthop;
	U16 inputSNMP;
	U16 outputSNMP;
	U32 inPkts;
	U32 inBytes;

	U32 firstSwitchID;
	U32 lastWwitchID;
	U16 srcPort;
	U16 dstPort;

	BYTE pad1;
	BYTE flags;
	BYTE proto;
	BYTE tos;

	U32 srcAS;
	U32 dstAS;
}V5_body, *PV5_body;



int field_width[V5_FIELD_NUM] = 
{
	4, 4, 4, 2, 2, 4, 4, 4, 4, 2, 2, 1, 1, 1, 1, 4, 4
};


#endif




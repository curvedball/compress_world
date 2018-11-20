

#ifndef _NETFLOW_V9_INC
#define _NETFLOW_V9_INC


#include "debug.h"


#define UDP_V9_MAGIC 0x30383834


#pragma pack(1)


typedef struct _V9_head
{
	U32 magic;
	U16 version;
	U16 count;
	U32 SysUptime;
	U32 unix_secs;
	U32 flow_sequence;
	U16 template_type;
	U16 template_length;
	U16 template_id;
	U16 template_field_num;
} V9_head, *PV9_head;

#define NETFLOW_V9_HEAD_LEN		(sizeof(V9_head))



typedef struct _V9_body
{
	U32 InBytes;
	U32 InPkts;
	
	BYTE proto;
	BYTE tos;
	BYTE flags;
	
	U16 srcPort;
	U32 srcIp;	
	U16 dstPort;
	U32 dstIP;

	U64 flow_start_milli_seconds;
	U64 flow_end_milli_seconds;
}V9_body, *PV9_body;


#define NETFLOW_V9_CELL_LEN		sizeof(V9_body)


#define UDP_V9_MAGIC2 0x30313630
#define UDP_V9_MAGIC2_BLOCK_LEN		232

#define UDP_V9_MAGIC3 0x30393132
#define UDP_V9_MAGIC3_BLOCK_LEN		916



#define V9_FIELD_NUM 11



extern int v9_field_width[V9_FIELD_NUM];



#endif




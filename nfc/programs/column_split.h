
/*
	column_split.h
*/


#include "debug.h"
#include "file_op.h"
#include "netflow_v5.h"
#include "netflow_v9.h"
#include "parse_config.h"

int extract_netflow_v5_field_column(char* input_filename, int width, char* base_filename, int column_id);
int extract_netflow_v9_field_column(char* input_filename, int width, char* base_filename, int column_id);







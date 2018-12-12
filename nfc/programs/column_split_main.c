
/*
	column_split_main.c
*/

#include "netflow_v5.h"
#include "netflow_v9.h"
#include "column_split.h"


#if 0
int main(int argc , char* argv[])
{
	DbgPrint("%s main begin.\n", argv[0]);
	if (argc != 3)
	{
		printf("Usage: ./program input_filename netflow_version\n");
		return -1;
	}

	int netflow_version = atoi(argv[2]);
	if (netflow_version < 0 || (netflow_version != 5 && netflow_version != 9))
	{
		printf("Invalid netflow_proto_num! Must be 5 or 9! netflow_version: %d\n", netflow_version);
		return -1;
	}

	if (netflow_version == 5)
	{
		int field_column_count = 0;
		int i;
		for (i = 0; i < V5_FIELD_NUM; i++)
		{
			field_column_count += v5_field_width[i];
		}

		//
		int control_column_count = 0;
		char input_filename[MAX_PATHNAME_LEN];
		for (i = 0; i < V5_FIELD_NUM; i++)
		{
			sprintf(input_filename, "%s%.2d_control", argv[1], i);
			if (UTIL_isRegularFile(input_filename))
			{
				control_column_count++;
			}
		}

		//
		int column_num = field_column_count + control_column_count;
		int column_id = 0;
		for (i = 0; i < V5_FIELD_NUM; i++)
		{
			sprintf(input_filename, "%s%.2d_r_delta", argv[1], i); //zb: higher previlege
			if (UTIL_isRegularFile(input_filename))
			{
				if (extract_netflow_v5_field_column(input_filename, v5_field_width[i], argv[1], column_id))
				{
					printf("extract_netflow_v5_field_column error!\n");
					return -1;
				}
				column_id += v5_field_width[i];
				continue;
			}

			sprintf(input_filename, "%s%.2d_r", argv[1], i); //zb: higher previlege
			if (UTIL_isRegularFile(input_filename))
			{
				if (extract_netflow_v5_field_column(input_filename, v5_field_width[i], argv[1], column_id))
				{
					printf("extract_netflow_v5_field_column error!\n");
					return -1;
				}
				column_id += v5_field_width[i];
				continue;
			}

			sprintf(input_filename, "%s%.2d_delta", argv[1], i); //zb: higher previlege
			if (UTIL_isRegularFile(input_filename))
			{
				if (extract_netflow_v5_field_column(input_filename, v5_field_width[i], argv[1], column_id))
				{
					printf("extract_netflow_v5_field_column error!\n");
					return -1;
				}
				column_id += v5_field_width[i];
				continue;
			}

			sprintf(input_filename, "%s%.2d", argv[1], i);
			if (UTIL_isRegularFile(input_filename))
			{
				if (extract_netflow_v5_field_column(input_filename, v5_field_width[i], argv[1], column_id))
				{
					printf("extract_netflow_v5_field_column error!\n");
					return -1;
				}
				column_id += v5_field_width[i];
				continue;
			}

			printf("Error! Can't find the field[%d]!\n", i);
			return -1;
		}


		//========================================================
		for (i = 0; i < V5_FIELD_NUM; i++)
		{
			sprintf(input_filename, "%s%.2d_control", argv[1], i);
			if (UTIL_isRegularFile(input_filename))
			{
				if (extract_netflow_v5_field_column(input_filename, 1, argv[1], column_id))
				{
					printf("extract_netflow_v5_field_column error!\n");
					return -1;
				}
				column_id += 1;
			}
		}
	}
	else
	{
		int field_column_count = 0;
		int i;
		for (i = 0; i < V9_FIELD_NUM; i++)
		{
			field_column_count += v9_field_width[i];
		}

		//
		int control_column_count = 0;
		char input_filename[MAX_PATHNAME_LEN];
		for (i = 0; i < V9_FIELD_NUM; i++)
		{
			sprintf(input_filename, "%s%.2d_control", argv[1], i);
			if (UTIL_isRegularFile(input_filename))
			{
				control_column_count++;
			}
		}

		//
		int column_num = field_column_count + control_column_count;
		int column_id = 0;
		for (i = 0; i < V9_FIELD_NUM; i++)
		{
			sprintf(input_filename, "%s%.2d_r_delta", argv[1], i); //zb: higher previlege
			if (UTIL_isRegularFile(input_filename))
			{
				if (extract_netflow_v9_field_column(input_filename, v9_field_width[i], argv[1], column_id))
				{
					printf("extract_netflow_v9_field_column error!\n");
					return -1;
				}
				column_id += v9_field_width[i];
				continue;
			}

			sprintf(input_filename, "%s%.2d_r", argv[1], i); //zb: higher previlege
			if (UTIL_isRegularFile(input_filename))
			{
				if (extract_netflow_v9_field_column(input_filename, v9_field_width[i], argv[1], column_id))
				{
					printf("extract_netflow_v9_field_column error!\n");
					return -1;
				}
				column_id += v9_field_width[i];
				continue;
			}

			sprintf(input_filename, "%s%.2d_delta", argv[1], i); //zb: higher previlege
			if (UTIL_isRegularFile(input_filename))
			{
				if (extract_netflow_v9_field_column(input_filename, v9_field_width[i], argv[1], column_id))
				{
					printf("extract_netflow_v9_field_column error!\n");
					return -1;
				}
				column_id += v9_field_width[i];
				continue;
			}			

			sprintf(input_filename, "%s%.2d", argv[1], i);
			if (UTIL_isRegularFile(input_filename))
			{
				if (extract_netflow_v9_field_column(input_filename, v9_field_width[i], argv[1], column_id))
				{
					printf("extract_netflow_v9_field_column error!\n");
					return -1;
				}
				column_id += v9_field_width[i];
				continue;
			}

			printf("Error! Can't find the field[%d]!\n", i);
			return -1;
		}


		//========================================================
		for (i = 0; i < V9_FIELD_NUM; i++)
		{
			sprintf(input_filename, "%s%.2d_control", argv[1], i);
			if (UTIL_isRegularFile(input_filename))
			{
				if (extract_netflow_v9_field_column(input_filename, 1, argv[1], column_id))
				{
					printf("extract_netflow_v9_field_column error!\n");
					return -1;
				}
				column_id += 1;
			}
		}			
	}

	DbgPrint("%s main end.\n", argv[0]);
	return 0;
}
#endif


int main(int argc , char* argv[])
{
	DbgPrint("%s main begin.\n", argv[0]);
	if (argc != 3)
	{
		printf("Usage: ./program input_filename width(bytes)\n");
		return -1;
	}


	int width = atoi(argv[2]);
	if (width <= 0 || (width != 1 && width != 2 && width != 4 && width != 8))
	{
		printf("Invalid width! Width must be 1, 2, 4 or 8! width: %d.\n", width);
		return -1;
	}


	char input_filename[MAX_PATHNAME_LEN];
	sprintf(input_filename, "%s", argv[1]);
	if (UTIL_isRegularFile(input_filename))
	{
		if (column_split_by_width(input_filename, width))
		{
			printf("column_split_u16_func error!\n");
			return -1;
		}
		/*
		if (width == 1)
		{
		}
		else if (width == 2)
		{
			
		}
		else if (width == 4)
		{
			
		}
		else if (width == 8)
		{
			
		}
		else
		{
			
		}
		*/
	}


	DbgPrint("%s main end.\n", argv[0]);
	return 0;
}





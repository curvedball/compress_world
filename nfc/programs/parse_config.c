
/*
  parse_config.c

    Search configuration file in current directory.
*/

#include "debug.h"
#include "file_op.h"
#include "parse_config.h"


int isInvlidStr(char* str)
{
	int len = strlen(str);
	for (int i = 0; i < len; i++)
	{
		if (str[i] != ' ' || str[i] != '\t' || str[i] != '\r' || str[i] != '\n')
		{
			return 1;
		}
	}
	return 0;
}


int ParseConfigFile(FIELD_DESC* pfield_desc, int* pfield_num)
{
	char dir_path[MAX_PATHNAME_LEN];
	if (UTIL_getCurrentWorkingDirectory(dir_path, MAX_PATHNAME_LEN))
	{
		return -1;
	}

	char config_file_path[MAX_PATHNAME_LEN];
	sprintf(config_file_path, "%s/nfc.cfg", dir_path);
	if (!UTIL_isRegularFile(config_file_path))
	{
		return -1;
	}
	printf("Configuration file [%s] exists!\n", config_file_path);

	//
	memset(pfield_desc, 0, sizeof(FIELD_DESC) * (*pfield_num));

	
	FILE* fp = fopen(config_file_path, "r");
	if (fp == NULL)
	{
		printf("Open configruration file error! [%s]\n", config_file_path);
		return -1;
	}

	FIELD_DESC* pcurrent_field_desc = pfield_desc;
	char buffer[256];
	char* delim_attribute = " \t";
	char* pattribute;
	int field_num = 0;
	char* temp;
	int i;
	while (!feof(fp))
	{
		temp = fgets(buffer, 256, fp);
		if (!temp)
		{
			//printf("fgets return value is NULL!\n");
			break;
		}
		//printf("%s", buffer);

		//
		pattribute = strtok(buffer, delim_attribute);
		if (pattribute == NULL)
		{
			printf("pattribute is NULL!\n");
			return -1;
		}

		//
		//printf("pattribute: %s len: %ld\n", pattribute, strlen(pattribute));
		if (strlen(pattribute) < 2) //zb: Invalid field name [at least 3 characters, e.g, tos]
		{
			continue;
		}

		//
		i = 1;
		strcpy(pcurrent_field_desc->name, pattribute);
		while((pattribute = strtok(NULL, delim_attribute)))
		{
			i++;
			if (i == 2)
			{
				//
				pcurrent_field_desc->width = atoi(pattribute);
				if (pcurrent_field_desc->width <= 0)
				{
					printf("Error width in configuration file! field_width: %d\n", pcurrent_field_desc->width);
					return -1;
				}
			}
			else
			{
				if (memcmp(pattribute, "r", 1) == 0 && strlen(pattribute) > 1)
				{
					int v = atoi(pattribute + 1);
					if (v <= 0)
					{
						printf("Unknow attribute! Invalid reverse_number in %s\n", pattribute);
						return -1;
					}
					pcurrent_field_desc->reverse_coding = TRUE;
					pcurrent_field_desc->reverse_number = v;
				}
				else if (memcmp(pattribute, "d", 1) == 0)
				{
					pcurrent_field_desc->delta_coding = TRUE;
				}
				else if (memcmp(pattribute, "dod", 3) == 0)
				{
					pcurrent_field_desc->deltaofdelta_coding = TRUE;
				}
				else
				{
					printf("Unknow attribute! %s\n", pattribute);
					return -1;
				}	
			}

		}	
		
		if (i > 1) //Ignore invalid field description(without width_parameter)
		{
			pcurrent_field_desc->id = field_num;
			pcurrent_field_desc++;	
			field_num++;
		}
	}

	fclose(fp);
	*pfield_num = field_num;

	//
	for (i = 0; i < field_num; i++)
	{
		DbgPrint("FieldDesc%.2d [name: %30s \twidth: %d \treverse_coding: %d \treverse_number: %d \tdelta_coding: %d]\n", i, pfield_desc[i].name, pfield_desc[i].width, pfield_desc[i].reverse_coding, pfield_desc[i].reverse_number, pfield_desc[i].delta_coding);
	}
	DbgPrint("\n");
	
	return 0;
}





int GetCellLength(FIELD_DESC* pfield_desc, int field_num)
{
	int cell_length = 0;
	for (int i = 0; i < field_num; i++)
	{
		cell_length += pfield_desc[i].width;
	}
	return cell_length;	
}






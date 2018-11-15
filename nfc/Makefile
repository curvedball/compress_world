


#make
#For nprobe-generated netflow v5 format
#./extract_flow a.flows
#./extract_field a.flows_Record
#./reverse_coding /a/a.flows_Record_Field00 /a/a.flows_Record_Field01 4
#./reverse_coding /a/a.flows_Record_Field09 /a/a.flows_Record_Field10 2
#./column_split /a/a.flows_Record_Field 50
#./delta_coding a.flows_Record_Field_column									### Not Used!
#./manual_delta_coding a.flows_Record_Field_column32
#./rle_coding a.flows_Record_Field_column 50
#./general_coding a.flows_Record_Field_column_delta 50 /zb/a/zstd
#./calculate_size a.flows_Record_Field_column_deltaofdelta 50  				###Not used!
#./merge_file a.flows_Record_Field_column_deltaofdelta 50 zst




all: extract_flow extract_field reverse_coding column_split delta_coding manual_delta_coding deltaofdelta_coding rle_coding general_coding calculate_size merge_file nfc





extract_flow: extract_flow.o debug.o file_op.o
	gcc $^ -o $@




extract_field: extract_field.o debug.c file_op.o
	gcc $^ -o $@ 



reverse_coding: reverse_coding.o debug.o file_op.o
	gcc $^ -o $@



column_split: column_split.o debug.o file_op.o
	gcc $^ -o $@



delta_coding: delta_coding.o debug.o file_op.o
	gcc $^ -o $@

manual_delta_coding: manual_delta_coding.o debug.o file_op.o
	gcc $^ -o $@

	

deltaofdelta_coding: deltaofdelta_coding.o debug.o file_op.o
	gcc $^ -o $@


rle_coding: rle_coding.o debug.o file_op.o
	gcc $^ -o $@




general_coding: general_coding.o debug.o file_op.o
	gcc $^ -o $@




calculate_size: calculate_size.o debug.o file_op.o
	gcc $^ -o $@



merge_file: merge_file.o debug.o file_op.o
	gcc $^ -o $@



nfc: nfc.o debug.o file_op.o parse_config.o
	gcc $^ -o $@


clean:
	rm -f *.o
	rm -f extract_flow extract_field reverse_coding column_split delta_coding manual_delta_coding deltaofdelta_coding rle_coding general_coding calculate_size merge_file nfc
















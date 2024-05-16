CC = gcc
all:
	$(CC) -o pds_tester IMT2022043_contact.c IMT2022043_pds.c bst.c pds_tester.c

clean:
	rm newdemo.dat pds_tester newdemo.ndx

check_testcases:
	./pds_tester testcase.in

check_testcases_part_2:
	./pds_tester testcase1.in
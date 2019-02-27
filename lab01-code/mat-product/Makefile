CFLAGS = -Wall -g
CC = gcc $(CFLAGS)

op_main : op_main.o outer_product.o free_matrix.o
	$(CC) -o $@ $^

op_main.o : op_main.c mat.h
	$(CC) -c $<

outer_product.o : outer_product.c mat.h
	$(CC) -c $<

free_matrix.o : free_matrix.c mat.h
	$(CC) -c $<

test : test_outer_product
	 ./test_outer_product

test_outer_product : test_outer_product.o outer_product.o mat.h
	$(CC) -o  $@ $^

test_outer_product.o : test_outer_product.c mat.h
	$(CC) -c $<

clean :
	@echo Cleaning object files
	rm -f *.o op_main test_outer_product

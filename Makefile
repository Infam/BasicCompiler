run: vm.c
	gcc vm.c
	./a.out > log
clean: 
	rm ./a.out
	rm log

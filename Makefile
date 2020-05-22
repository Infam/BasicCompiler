vm: vm.c
	gcc -g vm.c
	./a.out
parse: parser.c
	gcc -g parser.c
	./a.out
clean: 
	rm ./a.out
	rm log

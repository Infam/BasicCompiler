vm: vm.c
	gcc -g vm.c
	./a.out
parse: parser.c
	gcc -g parser.c
	./a.out
clean: 
	[ -f a.out ] && rm a.out || true
	[ -f log ] && rm log || true

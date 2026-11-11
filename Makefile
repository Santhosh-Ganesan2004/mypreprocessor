compiler =cc
flag = -c

exe: mpp.o comments.o headers.o macros.o
	$(compiler) $^ -o $@

mpp.o: mpp.c
	$(compiler) $(flag) $^ -o $@
headers.o: headers.c
	$(compiler) $(flag) $^ -o $@
comments.o: comments.c
	$(compiler) $(flag) $^ -o $@
macros.o: macros.c
	$(compiler) $(flag) $^ -o $@

clear: 
	@echo "Cleaning Up..";
	@rm -r *.o
	@rm -r *exe

.PHONY: clear

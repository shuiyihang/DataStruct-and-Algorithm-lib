FILE = base.o menuBase.o userPage.o
build:$(FILE)
	gcc -o base.out $(FILE)
	@rm -rf *.o


menuBase.o:menuBase.h

userPage.o:userPage.h


clean:
	@rm -rf *.o *.out
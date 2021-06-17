
VPATH = ./menuGuiLib ./userPage

obj := base.o ./menuGuiLib/menuBase.o ./menuGuiLib/pageManager.o

obj += ./userPage/showListPage.o

build:$(obj)
	echo $<
	gcc -o base.out $(obj)

base.o : base.c

menuBase.o : menuBase.c
	$(MAKE) -C ./menuGuiLib

pageManager.o : pageManager.c
	$(MAKE) -C ./menuGuiLib

showListPage.o : showListPage.c
	$(MAKE) -C ./userPage

.PHONY : clean

clean:
	@rm -rf *.o *.out
	@rm -rf ./menuGuiLib/*.o
	@rm -rf ./userPage/*.o


# TODO
# 多文件下makefile
# $@ 目标文件
# $< 第一个依赖目标
# $^ 所有依赖目标的集合
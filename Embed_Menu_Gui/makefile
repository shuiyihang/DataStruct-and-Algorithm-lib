
VPATH = ./menuGuiLib ./userPage

obj := ./menuGuiLib/menuBase.o ./menuGuiLib/pageManager.o

obj += ./userPage/showListPage.o

obj += ./userPage/showAboutPage.o
# 添加新文件之后在下面接着加obj就可以了

build : base.o $(obj)
	gcc -o base.out $^
	@rm -rf *.o
	@rm -rf ./menuGuiLib/*.o
	@rm -rf ./userPage/*.o

base.o : base.c

$(obj) : %.o:%.c
	$(if $(findstring menuGuiLib,$<),$(MAKE) -C ./menuGuiLib,$(MAKE) -C ./userPage)


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
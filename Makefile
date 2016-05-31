CC = gcc -g
BISON = bison
FLEX = flex
OBJS = driver.o y.tab.o tree.o lex.yy.o check.o code.o instr.o generate.o

ada: $(OBJS)
	$(CC) -o ada $(OBJS)

y.tab.o: y.tab.c y.tab.h tree.h
y.tab.c: p2.y
	$(BISON) -y -d p2.y
y.tab.h: p2.y
	$(BISON) -y -d p2.y

tree.o: tree.c tree.h

check.o: st.h tree.h y.tab.h

instr.o: tree.h y.tab.h instr.h code.h

code.o: tree.h y.tab.h instr.h code.h

generate.o: tree.h y.tab.h instr.h code.h

driver.o: driver.c tree.h y.tab.h check.c generate.h

lex.yy.o: lex.yy.c
lex.yy.c: p1.l
	$(FLEX) p1.l
clean:
	rm tree.o y.tab.h y.tab.c y.tab.o ada driver.o lex.yy.o lex.yy.c check.o code.o generate.o

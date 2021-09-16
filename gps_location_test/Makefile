obj := main.o parse.o confirm.o
LDD_FLAGS += -lm

all: $(obj)
	gcc $^ -o tk_test $(LDD_FLAGS)

%.o: %.c
	gcc -c $< -o $@

clean:
	rm -rf tk_test *.o

.PHONY:
	all clean

build:
	g++ main.cc -o main

run:
	./main

.PHONY: clean

clean:
	rm -f main

CFLAGS=-g -Wall -Wextra -Isrc/include -Itests/include -O2 -DNDEBUG -fPIC $(OPTFLAGS)
LIBS=-ldl $(OPTLIBS)
LDFLAGS=

SOURCES=$(wildcard src/**/*.c src/*.c)
OBJECTS=$(patsubst src/%.c, build/%.o, $(SOURCES))
TESTS_SRC=$(wildcard tests/*.c)
TESTS=$(patsubst tests/%.c, build/tests/%, $(TESTS_SRC))

SO_TARGET=libSMART_PTR.so

all: $(SO_TARGET) tests

dev: CFLAGS = -g -Wall -Wextra -Isrc/include -Itests/include -fPIC $(OPTFLAGS)
dev: CFLAGS += -fsanitize=address -O1 -fno-omit-frame-pointer
dev: LDFLAGS+= -fsanitize=address 
dev: all

$(SO_TARGET): $(OBJECTS)
	$(CC) -shared $(LDFLAGS) $(OBJECTS) -o build/$@ 

build/%.o: src/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(LDFLAGS) -c $< -o $@

.PHONY: tests
tests: $(TESTS)
	@sh ./tests/run_tests.sh
	
build/tests/%: tests/%.c build/$(SO_TARGET)
	@mkdir -p build/tests
	$(CC) $(CFLAGS) $(LDFLAGS) $< -o $@ -Lbuild -lSMART_PTR $(LIBS)

clean:
	rm -rf build
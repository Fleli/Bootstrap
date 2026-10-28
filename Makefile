CXX = c++
CXXFLAGS = -std=c++17 -Wall -Wextra -pedantic
CPPFLAGS = -Isluggish/compiler

SOURCES = sluggish/compiler/main.cpp sluggish/compiler/lex/Lexer.cpp sluggish/compiler/stage-emit/Emit.cpp
HEADERS = sluggish/compiler/lex/Lexer.hpp sluggish/compiler/lex/Token.hpp sluggish/compiler/stage-emit/Emit.hpp

.PHONY: run

run: sluggish/build/sluggish
	./sluggish/build/sluggish < sluggish/design/language/dev-script.slug

sluggish/build/sluggish: $(SOURCES) $(HEADERS) Makefile
	mkdir -p sluggish/build
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(SOURCES) -o $@

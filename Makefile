CXX = c++
CXXFLAGS = -std=c++17 -Wall -Wextra -pedantic
CPPFLAGS = -Isluggish/compiler

COMPILER_SOURCES = sluggish/compiler/Compiler.cpp \
	sluggish/compiler/type-system/Type.cpp \
	sluggish/compiler/lex/Lexer.cpp \
	sluggish/compiler/parse/Parser.cpp \
	sluggish/compiler/resolver/Resolver.cpp \
	sluggish/compiler/lower/BasicBlock.cpp \
	sluggish/compiler/stage-emit/Emit.cpp

COMPILER_HEADERS = sluggish/compiler/Compiler.hpp \
	sluggish/compiler/IR.hpp \
	sluggish/compiler/type-system/Type.hpp \
	sluggish/compiler/lex/Lexer.hpp \
	sluggish/compiler/lex/Token.hpp \
	sluggish/compiler/parse/Parser.hpp \
	sluggish/compiler/resolver/Resolver.hpp \
	sluggish/compiler/resolver/ResolvedStatement.hpp \
	sluggish/compiler/resolver/Scope.hpp \
	sluggish/compiler/lower/BasicBlock.hpp \
	sluggish/compiler/stage-emit/Emit.hpp

BACKEND_SOURCES = simplecpu/sluggish-backend/build-assembly/main.cpp \
	simplecpu/sluggish-backend/build-assembly/BuildAssembly.cpp

BACKEND_HEADERS = simplecpu/sluggish-backend/build-assembly/BuildAssembly.hpp

EMULATOR_SOURCES = simplecpu/emulator/main.cpp simplecpu/emulator/Emulator.cpp

.PHONY: assembly run

assembly: simplecpu/build/build-assembly
	mkdir -p simplecpu/intermediate
	./simplecpu/build/build-assembly < sluggish/design/language/dev-script.slug > simplecpu/intermediate/assembly.txt

run: simplecpu/build/emulator
	./simplecpu/build/emulator

simplecpu/build/emulator: $(EMULATOR_SOURCES) simplecpu/emulator/Emulator.hpp Makefile
	mkdir -p simplecpu/build
	$(CXX) $(CXXFLAGS) $(EMULATOR_SOURCES) -o $@

simplecpu/build/build-assembly: $(COMPILER_SOURCES) $(COMPILER_HEADERS) $(BACKEND_SOURCES) $(BACKEND_HEADERS) Makefile
	mkdir -p simplecpu/build
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(COMPILER_SOURCES) $(BACKEND_SOURCES) -o $@

sluggish/build/sluggish: sluggish/compiler/main.cpp $(COMPILER_SOURCES) $(COMPILER_HEADERS) Makefile
	mkdir -p sluggish/build
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(COMPILER_SOURCES) sluggish/compiler/main.cpp -o $@

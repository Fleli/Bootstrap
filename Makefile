CXX = c++
CXXFLAGS = -std=c++17 -Wall -Wextra -pedantic
CPPFLAGS = -Isluggish/compiler -Isimplecpu

COMPILER_SOURCES = sluggish/compiler/Compiler.cpp \
	sluggish/compiler/type-system/Type.cpp \
	sluggish/compiler/lex/Lexer.cpp \
	sluggish/compiler/parse/Parser.cpp \
	sluggish/compiler/parse/ParseExpression.cpp \
	sluggish/compiler/parse/ParseStatement.cpp \
	sluggish/compiler/parse/Format.cpp \
	sluggish/compiler/resolver/Resolver.cpp \
	sluggish/compiler/resolver/ResolveExpression.cpp \
	sluggish/compiler/resolver/ResolveStatement.cpp \
	sluggish/compiler/lower/BasicBlock.cpp \
	sluggish/compiler/lower/AllocateSlot.cpp \
	sluggish/compiler/lower/LowerExpression.cpp \
	sluggish/compiler/lower/LowerFunction.cpp \
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

CPU_SOURCES = simplecpu/main.cpp \
	simplecpu/isa/Instruction.cpp \
	simplecpu/sluggish-backend/build-assembly/BuildAssembly.cpp \
	simplecpu/assembler/Assembler.cpp \
	simplecpu/emulator/Emulator.cpp

CPU_HEADERS = simplecpu/isa/Instruction.hpp \
	simplecpu/sluggish-backend/build-assembly/BuildAssembly.hpp \
	simplecpu/assembler/Assembler.hpp \
	simplecpu/emulator/Emulator.hpp

.PHONY: assemble assembly run regression regressions

assemble: simplecpu/build/simplecpu
	./simplecpu/build/simplecpu < sluggish/design/language/dev-script.slug

assembly: assemble

run: simplecpu/build/simplecpu
	./simplecpu/build/simplecpu --run < sluggish/design/language/dev-script.slug

regression: regressions

regressions: sluggish/build/regressions
	./sluggish/build/regressions

simplecpu/build/simplecpu: $(COMPILER_SOURCES) $(COMPILER_HEADERS) $(CPU_SOURCES) $(CPU_HEADERS) Makefile
	mkdir -p simplecpu/build
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(COMPILER_SOURCES) $(CPU_SOURCES) -o $@

sluggish/build/sluggish: sluggish/compiler/main.cpp $(COMPILER_SOURCES) $(COMPILER_HEADERS) Makefile
	mkdir -p sluggish/build
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(COMPILER_SOURCES) sluggish/compiler/main.cpp -o $@

sluggish/build/regressions: sluggish/regressions/main.cpp $(COMPILER_SOURCES) $(filter-out simplecpu/main.cpp,$(CPU_SOURCES)) $(COMPILER_HEADERS) $(CPU_HEADERS) Makefile
	mkdir -p sluggish/build
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(COMPILER_SOURCES) $(filter-out simplecpu/main.cpp,$(CPU_SOURCES)) sluggish/regressions/main.cpp -o $@

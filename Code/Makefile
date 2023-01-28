## Si es desitja utilitzar el driver_esinmath.cpp ( per a utilitzar l'opció make test ), s'ha de canviar consola per driver_esinmath

OPCIONS = -g -O0 -Wall -Wno-deprecated -std=c++14
program.exe: consola.o racional.o token.o expressio.o math_io.o math_sessio.o variables.o
	g++ -o program.exe consola.o racional.o token.o expressio.o math_io.o math_sessio.o variables.o -lesin -lreadline

consola.o: consola.cpp racional.hpp racional.rep token.hpp token.rep expressio.hpp expressio.rep math_io.hpp  math_sessio.hpp math_sessio.rep variables.hpp variables.rep
	g++ -c consola.cpp $(OPCIONS)

racional.o: racional.cpp racional.hpp racional.rep
	g++ -c racional.cpp $(OPCIONS)
token.o: token.cpp token.hpp token.rep
	g++ -c token.cpp $(OPCIONS)
math_io.o: math_io.cpp math_io.hpp
	g++ -c math_io.cpp $(OPCIONS)
math_sessio.o: math_sessio.cpp math_sessio.hpp math_sessio.rep
	g++ -c math_sessio.cpp $(OPCIONS)
variables.o: variables.cpp variables.hpp variables.rep
	g++ -c variables.cpp $(OPCIONS)
expressio.o: expressio.cpp expressio.hpp expressio.rep
	g++ -c expressio.cpp $(OPCIONS)

clean:
	rm *.o
	rm *.exe

test:
	@echo ""
	@echo "JOCS DE PROVA"
	@echo "======================================================"
	./program.exe < tests/jp_public_racional.in | diff -b - tests/jp_public_racional.res;
	./program.exe < tests/jp_public_token.in | diff -b - tests/jp_public_token.res;
	./program.exe < tests/jp_public_expressio.in | diff -b - tests/jp_public_expressio.res;
	./program.exe < tests/jp_public_variables.in | diff -b - tests/jp_public_variables.res;
	./program.exe < tests/jp_public_math_sessio.in | diff -b - tests/jp_public_math_sessio.res;
	./program.exe < tests/jp_public_math_io.in | diff -b - tests/jp_public_math_io.res;
	./program.exe < tests/jp_public.in | diff -b - tests/jp_public.res;
	@echo "======================================================"
	@echo ""

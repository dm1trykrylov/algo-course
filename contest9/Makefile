in := input.txt
out := output.txt
task := test
test := taskF_test
buildsan: $(task).cpp
	g++ -Wall -Wextra -fdiagnostics-color=always $(task).cpp -o ./exec/$(task) -fsanitize=address -g

build: $(task).cpp
	g++ -Wall -Wextra -fdiagnostics-color=always $(task).cpp -o ./exec/$(task) -g

br: $(task).cpp
	g++ -Wall -Wextra -fdiagnostics-color=always $(task).cpp -o ./exec/$(task) -g
	./exec/$(task) < $(in) 

run: ./$(task) $(in)
	

runvg: ./$(task) $(in)
	valgrind ./exec/$(task) < $(in) 

vg: $(task).cpp $(in)
	g++ -Wall -Wextra -fdiagnostics-color=always $(task).cpp -o ./exec/$(task) -g
	valgrind --leak-check=full --show-leak-kinds=all ./exec/$(task) < $(in) 

san: $(task).cpp $(in)
	g++ -Wall -Wextra -fdiagnostics-color=always -DDebug $(task).cpp -o ./exec/$(task) -fsanitize=address -g
	./exec/$(task) < $(in) 

schlang: $(task).cpp $(in)
	clang++ -pedantic -Werror -Wextra -fsanitize=address -fsanitize=undefined -std=c++20 -DDebug $(task).cpp -o ./exec/$(task) -g
	./exec/$(task) < $(in) 

test: $(test).cpp $(in)
	g++ -fdiagnostics-color=always -DDebug $(test).cpp -o ./exec/$(test) -fsanitize=address -g
	./exec/$(test) < $(in) 

clang: $(task).cpp
	clang++ -Wall -Wextra -fdiagnostics-color=always $(task).cpp -o ./exec/$(task) -g


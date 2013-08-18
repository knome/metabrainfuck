
fail:
	g++ -Wall -Werror -Wfatal-errors -std=c++0x -ftemplate-depth=16384 bf.cpp

# fail differently

clang:
	# clang seems to be more space compact while compiling
	clang -Wall -Werror -Wfatal-errors -std=c++0x -ftemplate-depth-65536 bf.cpp

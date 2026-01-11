NAME = animal

CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -pedantic

SRCS = main.cpp Animal.cpp Dog.cpp Cat.cpp Brain.cpp
OBJS = $(SRCS:.cpp=.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(NAME) $(OBJS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

abstract:
	@echo "Testing if Animal can be instantiated..."
	@echo 'int main() { Animal a; return 0; }' | $(CXX) $(CXXFLAGS) -I. -include Animal.hpp -x c++ - Animal.cpp -o /tmp/test_animal 2>&1 && echo "Animal CAN be instantiated (not abstract)" || echo "Animal CANNOT be instantiated (abstract class)"
	@rm -f /tmp/test_animal

.PHONY: all clean fclean re abstract

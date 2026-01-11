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
	@echo "Testing that Animal cannot be instantiated..."
	@echo 'int main() { Animal a; return 0; }' | $(CXX) $(CXXFLAGS) -I. -include Animal.hpp -x c++ - 2>&1 || true
	@echo ""
	@echo "If you see an error above, the abstract class is working correctly!"

.PHONY: all clean fclean re abstract

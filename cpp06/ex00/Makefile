NAME = convert
CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -pedantic
SRCS = main.cpp ScalarConverter.cpp
OBJS = $(SRCS:.cpp=.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(NAME) $(OBJS)

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re:
	@$(MAKE) fclean
	@$(MAKE) all

.PHONY: all clean fclean re

NAME = webserv
CPP = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98

SRCS = sources/main.cpp \
		sources/system/System.cpp \
		sources/system/SystemError.cpp \
		sources/server/Server.cpp
OBJS = $(SRCS:.cpp=.o)

%.o : %.cpp %.hpp
	$(CPP) $(CXXFLAGS) -c $< -o $@

all: $(NAME)

$(NAME) : $(OBJS)
	$(CPP) $(CXXFLAGS) -o $@ $(OBJS)

clean:
	rm -rf $(OBJS)

fclean:
	$(MAKE) clean
	rm -rf $(NAME)

re:
	$(MAKE) fclean 
	$(MAKE) all

.PHONY: all clean fclean re
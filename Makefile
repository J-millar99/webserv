NAME = webserv
CPP = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98

SRCS = sources/main.cpp sources/utils.cpp \
		sources/system/System.cpp \
		sources/system/SystemParser.cpp \
		sources/system/SystemUtils.cpp \
		sources/server/Server.cpp \
		sources/server/ServerParser.cpp \
		sources/server/ServerUtils.cpp \
		sources/server/Receive.cpp \
		sources/server/Response.cpp \
		sources/http/HttpHeader.cpp \
		sources/http/HttpRequest.cpp \
		sources/http/HttpResponse.cpp \
		
		
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
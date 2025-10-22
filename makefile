NAME    = philo
CC      = cc
CFLAGS  = -Wall -Wextra -Werror -Iinclude -pthread

SRCS = \
	source/main.c \
	source/parsing.c \
	source/init.c \
	source/routine.c \
	source/time.c \
	source/print.c \
	src/gg_init.c \
	src/gg_malloc.c \
	src/gg_free.c

OBJS = $(SRCS:.c=.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(OBJS) -pthread -o $(NAME)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re

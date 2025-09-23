# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: rbiskin <rbiskin@student.42.fr>            +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/09/23 08:20:07 by rbiskin           #+#    #+#              #
#    Updated: 2025/09/23 09:19:44 by rbiskin          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME    = philo
CC      = cc
CFLAGS  = -Wall -Wextra -Werror -Iinclude
LIBS    = -pthread              
RM      = rm -f

SRCDIR  = source
SRC     = $(SRCDIR)/main.c \
          $(SRCDIR)/parsing.c \
          $(SRCDIR)/init.c \
          $(SRCDIR)/routine.c \
          $(SRCDIR)/time.c \
          $(SRCDIR)/print.c \
          $(SRCDIR)/gg_collect.c

OBJ     = $(SRC:.c=.o)

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(NAME) $(LIBS)  

clean:
	$(RM) $(OBJ)

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re

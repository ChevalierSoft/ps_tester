NAME		= pt
CC			= gcc
FLAGS		= -g
PT_OBJS 	= $(PT_SRCS:.c=.o)

PT_SRCS		= ./ps_tester/ps_tester.c

all : pt

pt : $(PT_OBJS)
	@gcc -O3 -o $(NAME) $(PT_OBJS)
	@echo "paf c'est pret"

%.o : %.c
	@$(CC) $(FLAGS) -c $< -o $@

clean :
	rm -f $(PT_OBJS)

fclean : clean
	rm -f pt

re : fclean all

.PHONY : all re fclean

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekeller-@student.42sp.org.br <ekeller-@    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/16 17:09:03 by ekeller-@st       #+#    #+#             */
/*   Updated: 2025/04/24 18:28:01 by ekeller-@st      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser.h"

//returns current token and advances state to the next
t_token	*advance_token(t_parser_state *p_state)
{
	t_token	*token; 
	
	token = p_state->current;
	if (p_state->current != NULL)
		p_state->current = p_state->current->next; 
	return (token);	
}

t_command	*init_command_struct(void)
{
	t_command	*cmd;

	cmd = malloc(sizeof(t_command));
	if (!cmd)
	{
		perror("Malloc parser cmd failed");
		exit(EXIT_FAILURE);
	}
	cmd->command_name = NULL;
	cmd->args = NULL;
	cmd->args_count = 0;
	cmd->redirs = NULL;
	cmd->next = NULL;
	return (cmd);
}

void	ft_error(char *msg)
{
	printf("%s", msg);
	exit(EXIT_FAILURE);
}

int	count_args(t_parser_state *p_state)
{
	t_parser_state	temp;
	int				arg_count;
	
	temp = *p_state;
	arg_count = 0;
	while (temp.current && temp.current->type == WORD)
	{
		arg_count++;
		temp.current = temp.current->next;
	}
	return (arg_count);
}
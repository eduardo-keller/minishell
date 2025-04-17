/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekeller-@student.42sp.org.br <ekeller-@    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/16 17:08:20 by ekeller-@st       #+#    #+#             */
/*   Updated: 2025/04/17 19:07:28 by ekeller-@st      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser.h"

int	main(void)
{
	t_token	*token_list;
	t_parser_state *current_node;
	t_command	*cmd_pipeline;
	
	current_node->current = token_list;
	cmd_pipeline = parse_pipeline(&current_node);
}

t_command	*parse_pipeline(t_parser_state *p_state)
{
	t_command	*first_cmd;
	t_command	*next_cmd;
	t_command	*last_cmd;

	first_cmd = parse_command(p_state);
	last_cmd = first_cmd;
	while (p_state->current && p_state->current->type == PIPE)
	{
		advance_token(p_state);
		next_cmd = parse_command(p_state);
		last_cmd->next = next_cmd;
		last_cmd = next_cmd;
	}
	return (first_cmd);
}

t_command	*parse_command(t_parser_state *p_state)
{
	t_command	*cmd;
	t_token		*token;

	cmd = init_command_struct();
	check_redirections(p_state, cmd);
	
}

t_command	*check_redirections(t_parser_state *p_state, t_command	*cmd) //return or not?
{
	t_redirections *redir;
	t_token			*curr_token;
	
	curr_token = p_state->current;
	while (curr_token && (curr_token->type != WORD
			|| curr_token->type != PIPE || curr_token->type != DOLLAR))
	{
		redir = parse_redirection(p_state); //maybe init redir with null here
		if (!cmd->redirs)
			cmd->redirs = redir;
		else
		{
			while (cmd->redirs->next)
				cmd->redirs = cmd->redirs->next;
			cmd->redirs->next = redir;
		}
	}
	
}

t_redirections	*parse_redirection(t_parser_state *p_state)
{
	t_token			*token; //not used yet
	t_redirections	*redir;
	
	if (!p_state->current)
		error("Unexpected end of tokens while parsing redirection");
	redir = malloc(sizeof(t_redirections));
	if (!redir) 
	{
        perror("Malloc parser redirection failed");
        exit(EXIT_FAILURE);
    }
	redir = assign_redir_type(p_state, redir);
	
}


t_redirections	*assign_redir_type(t_parser_state *p_state, t_redirections *redir)
{
	t_token			*token;
	
	token = p_state->current;
	if (token->type == REDIR_IN)
		redir->type = REDIR_IN;
	else if (token->type == REDIR_OUT)
		redir->type = REDIR_OUT;
	else if (token->type == REDIR_DELIMITER)
		redir->type = REDIR_DELIMITER;
	else if (token->type == REDIR_APPEND)
		redir->type = REDIR_APPEND;
	else
		error("Invalid redirection operator");
	return (redir);
}
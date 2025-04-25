/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekeller-@student.42sp.org.br <ekeller-@    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/16 17:08:20 by ekeller-@st       #+#    #+#             */
/*   Updated: 2025/04/25 17:29:31 by ekeller-@st      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser.h"

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

	cmd = init_command_struct();
	cmd = check_redirections(p_state, cmd);
	cmd = check_command_args(p_state, cmd);
	cmd = check_redirections(p_state, cmd);
	return (cmd);
}

//cmd->args[0] is the name of the command. 
t_command	*check_command_args(t_parser_state *p_state, t_command *cmd)
{
	t_token	*token;
	int		args_count;
	int		i;

	i = 0;
	token = p_state->current;
	args_count = count_args(p_state);
	cmd->args_count = args_count;
	if (token->type == WORD && token)
		cmd->command_name = ft_strdup(token->value);
	cmd->args = malloc(sizeof(char *) * (args_count + 1));
	if (!cmd->args)
		ft_error("Malloc cmd->args failed");
	while (p_state->current && p_state->current->type == WORD)
	{
		cmd->args[i++] = ft_strdup(p_state->current->value);
		advance_token(p_state);
	}
	cmd->args[i] = NULL;
	return (cmd);
}

t_command	*check_redirections(t_parser_state *p_state, t_command	*cmd)
{
	t_redirections	*redir;
	t_redirections	*last_redir;
	t_token			*curr_token;

	curr_token = p_state->current;
	while (curr_token && (curr_token->type != WORD
			&& curr_token->type != PIPE && curr_token->type != DOLLAR))
	{
		redir = parse_redirection(p_state);
		if (!cmd->redirs)
			cmd->redirs = redir;
		else
		{
			last_redir = cmd->redirs;
			while (last_redir->next)
				last_redir = last_redir->next;
			last_redir->next = redir;
		}
		curr_token = p_state->current;
	}
	return (cmd);
}

//assigns the redirection type to t_redirections which is a 
//component of t_command advances to the next token which must
//be a word and assign the word as redirection file
t_redirections	*parse_redirection(t_parser_state *p_state)
{
	t_redirections	*redir;

	if (!p_state->current)
		ft_error("Unexpected end of tokens while parsing redirection");
	redir = malloc(sizeof(t_redirections));
	if (!redir)
		ft_error("Malloc parser redirection failed");
	redir = assign_redir_type(p_state, redir);
	if (p_state->current->type != WORD || !p_state->current)
		ft_error("Expected filename after redirection operator");
	redir->filename = ft_strdup(p_state->current->value);
	redir->next = NULL;
	advance_token(p_state);
	return (redir);
}

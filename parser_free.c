/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_free.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekeller-@student.42sp.org.br <ekeller-@    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/25 18:14:13 by ekeller-@st       #+#    #+#             */
/*   Updated: 2025/04/25 19:00:21 by ekeller-@st      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser.h"

void	free_token_list(t_token *head)
{
	t_token	*temp;

	while (head)
	{
		temp = head->next;
		free(head->value);
		free(head);
		head = temp;
	}
}

void	free_redirections(t_redirections *redir)
{
	t_redirections	*temp;

	while (redir)
	{
		temp = redir->next;
		free(redir->filename);
		free(redir);
		redir = temp;
	}
}

void	free_command_list(t_command *head)
{
	t_command	*temp;
	int			i;

	i = 0;
	while (head)
	{
		temp = head->next;
		if (head->command_name)
			free(head->command_name);
		if (head->args)
		{
			while (head->args[i])
				free(head->args[i++]);
			free(head->args);
		}
		if (head->redirs)
			free_redirections(head->redirs);
		free(head);
		head = temp;
	}
}

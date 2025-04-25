/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekeller-@student.42sp.org.br <ekeller-@    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/25 17:39:52 by ekeller-@st       #+#    #+#             */
/*   Updated: 2025/04/25 19:03:37 by ekeller-@st      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser.h"

/* --- Functions to Create a Linked List of Tokens for Demonstration --- */

// Utility to create a new token.
t_token	*create_tok(t_token_type type, const char *value)
{
	t_token	*new_tok = malloc(sizeof(t_token));
	
	if (!new_tok)
	{
		perror("malloc");
		exit(EXIT_FAILURE);
	}
	new_tok->type = type;
	new_tok->value = ft_strdup(value);
	new_tok->next = NULL;
	return (new_tok);
}

/*
 * For demonstration, create a linked list of tokens representing:
 * ls -l | grep minishell > output.txt
 */
t_token	*create_demo_token_list(void)
{
	// Manually create each t_token.
	t_token *t1 = create_tok(WORD, "ls");
	t_token *t2 = create_tok(WORD, "-l");
	t_token *t3 = create_tok(PIPE, "|");
	t_token *t4 = create_tok(WORD, "grep");
	t_token *t5 = create_tok(WORD, "minishell");
	t_token *t6 = create_tok(REDIR_OUT, ">");
	t_token *t7 = create_tok(WORD, "output.txt");   
	// Link the tokens together.
	t1->next = t2;
	t2->next = t3;
	t3->next = t4;
	t4->next = t5;
	t5->next = t6;
	t6->next = t7;
	t7->next = NULL;
	// t7->next remains NULL.   
	return t1;
}

int	main(void)
{
	t_token	*token_list;
	t_parser_state p_state;
	t_command	*cmd_pipeline;
	t_command   *cmd;
	int cmd_num;
	t_redirections  *redir;
	
	token_list = create_demo_token_list();
	p_state.current = token_list;
	cmd_pipeline = parse_pipeline(&p_state);
	cmd = cmd_pipeline;
	
	cmd_num = 1;
	while (cmd != NULL)
	{
		printf("Command %d:\n", cmd_num);
		printf("  Command name: %s\n", cmd->command_name);
		printf("  Arguments (%d): ", cmd->args_count);
		for (int i = 0; i < cmd->args_count; i++)
			printf("%s ", cmd->args[i]);
		printf("\n");	
		redir = cmd->redirs;
		while (redir)
		{
			if (redir->type == 0)
				printf("Redir type: %c, file name: %s", '<', redir->filename);
			if (redir->type == 1)
				printf("Redir type: %c, file name: %s", '>', redir->filename);
			if (redir->type == 2)
				printf("Redir type: %s, file name: %s", "<<", redir->filename);
			if (redir->type == 3)
				printf("Redir type: %s, file name: %s", ">>", redir->filename);            
			redir = redir->next; 
		}
		cmd = cmd->next;
		cmd_num++;
    }
	free_token_list(token_list);
	free_command_list(cmd_pipeline);
}
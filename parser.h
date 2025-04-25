/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekeller-@student.42sp.org.br <ekeller-@    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/16 14:56:50 by ekeller-@st       #+#    #+#             */
/*   Updated: 2025/04/25 18:59:51 by ekeller-@st      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSER_H
# define PARSER_H

# include <stdio.h>
# include <stdlib.h>
# include <string.h>

// vai ser recebido. REMOVER
typedef enum e_token_type // remove
{
	WORD,
	PIPE,
	REDIR_IN,
	REDIR_OUT,
	REDIR_DELIMITER,
	REDIR_APPEND,
	DOLLAR
}	t_token_type;

typedef struct s_token // remove
{
	t_token_type	type;
	char			*value;
	struct s_token	*next;
}	t_token;

// my structs below
// instead of using  "t_token **current", use "parser_state *current". 
typedef struct parser_state
{
	struct s_token	*current;
}	t_parser_state;

typedef enum e_redirtype
{
	R_IN,
	R_OUT,
	R_DELIMITER,
	R_APPEND
}	t_redir_type;

typedef struct redirection
{
	t_redir_type		type;
	char				*filename;
	struct redirection	*next;
}	t_redirections;

typedef struct s_command
{
	char				*command_name;
	char				**args;
	int					args_count;
	t_redirections		*redirs;
	struct s_command	*next;
}	t_command;

//parser_utils.c
t_token			*advance_token(t_parser_state *p_state);
t_command		*init_command_struct(void);
void			ft_error(char *msg);
int				count_args(t_parser_state *p_state);
t_redirections	*assign_redir_type(t_parser_state *p_state,
					t_redirections *redir);

//parser.c
t_command		*parse_pipeline(t_parser_state *p_state);
t_command		*parse_command(t_parser_state *p_state);
t_command		*check_command_args(t_parser_state *p_state, t_command *cmd);
t_command		*check_redirections(t_parser_state *p_state, t_command	*cmd);
t_redirections	*parse_redirection(t_parser_state *p_state);

//parser_free.c
void			free_token_list(t_token *head);
void			free_redirections(t_redirections *redir);
void			free_command_list(t_command *head);

//libft.c REMOVE!!!!
char			*ft_strdup(const char *s);
void			*ft_calloc(size_t nmemb, size_t size);
void			*ft_memset(void *s, int c, size_t n);

//parser_test.c REMOVE
t_token			*create_demo_token_list(void);
t_token			*create_tok(t_token_type type, const char *value);

#endif
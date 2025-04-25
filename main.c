#include "parser.h"

/* --- Functions to Create a Linked List of Tokens for Demonstration --- */

// Utility to create a new token.
t_token *create_tok(t_token_type type, const char *value) {
    t_token *new_tok = malloc(sizeof(t_token));
    if (!new_tok) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    new_tok->type = type;
    new_tok->value = ft_strdup(value);
    new_tok->next = NULL;
    return new_tok;
}

/*
 * For demonstration, create a linked list of tokens representing:
 * ls -l | grep minishell > output.txt
 */
t_token *create_demo_token_list(void) {
    // Manually create each t_token.
    t_token *t1 = create_tok(WORD, "ls");
    t_token *t2 = create_tok(WORD, "-l");
    t_token *t3 = create_tok(PIPE, "|");
    t_token *t4 = create_tok(WORD, "grep");
    t_token *t5 = create_tok(PIPE, "minishell");
    t_token *t6 = create_tok(REDIR_OUT, ">");
    t_token *t7 = create_tok(WORD, "output.txt");

    // Link the tokens together.
    t1->next = t2;
    t2->next = t3;
    t3->next = t4;
    t4->next = t5;
    t5->next = NULL;
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

        cmd = cmd->next;
        cmd_num++;
    }
}
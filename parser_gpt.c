/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_gpt.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekeller-@student.42sp.org.br <ekeller-@    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/16 14:59:11 by ekeller-@st       #+#    #+#             */
/*   Updated: 2025/04/24 13:02:22 by ekeller-@st      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* --- Token Definitions --- */

typedef enum TokenType {
    TOKEN_WORD,
    TOKEN_PIPE,
    TOKEN_REDIRECT_IN,   // "<"
    TOKEN_REDIRECT_OUT,  // ">"
    TOKEN_HEREDOC,       // "<<"
    TOKEN_APPEND         // ">>"
} TokenType;

typedef struct Token {
    TokenType type;
    char *value;
    struct Token *next;  // Linked list pointer
} Token;

/* --- Parser State --- */

typedef struct ParserState {
    Token *current;  // Pointer to the current token in the linked list
} ParserState;

/* --- Redirection Structure --- */

typedef enum RedirType {
    REDIR_IN,
    REDIR_OUT,
    REDIR_HEREDOC,
    REDIR_APPEND
} RedirType;

typedef struct Redirection {
    RedirType type;
    char *filename;
    struct Redirection *next;
} Redirection;

/* --- Command Structure --- */

typedef struct Command {
    char *command_name;  // First word token of the command
    char **args;         // Arguments array (starting with command name)
    int args_count;      // Number of arguments
    Redirection *redirs; // Linked list of redirections
    struct Command *next; // Pointer to the next command in a pipeline
} Command;

/* --- Utility Functions for ParserState --- */

// Returns the current token (or NULL if at the end)
Token *current_token(ParserState *ps) {
    return ps->current;
}

// Consumes the current token and advances the list pointer.
Token *consume_token(ParserState *ps) {
    Token *tok = ps->current;
    if (ps->current != NULL)
        ps->current = ps->current->next;
    return tok;
}

// Simple error reporting function.
void error(const char *msg) {
    fprintf(stderr, "Parse error: %s\n", msg);
    exit(EXIT_FAILURE);
}

/* --- Parser Functions --- */

/*
 * parse_redirection:
 *   Expects the current token to be a redirection operator (<, >, <<, >>).
 *   Consumes the operator and then expects a WORD token for the filename.
 *   Returns a pointer to a Redirection structure.
 */
Redirection *parse_redirection(ParserState *ps) {
    Token *tok = current_token(ps);
    if (!tok)
        error("Unexpected end of tokens while parsing redirection");

    Redirection *redir = malloc(sizeof(Redirection));
    if (!redir) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    // Determine the redirection type based on the token.
    if (tok->type == TOKEN_REDIRECT_IN)
        redir->type = REDIR_IN;
    else if (tok->type == TOKEN_REDIRECT_OUT)
        redir->type = REDIR_OUT;
    else if (tok->type == TOKEN_HEREDOC)
        redir->type = REDIR_HEREDOC;
    else if (tok->type == TOKEN_APPEND)
        redir->type = REDIR_APPEND;
    else
        error("Invalid redirection operator");

    consume_token(ps);  // Consume the redirection operator token.

    // The next token should be a WORD representing the filename.
    Token *filename_tok = current_token(ps);
    if (!filename_tok || filename_tok->type != TOKEN_WORD)
        error("Expected filename after redirection operator");

    redir->filename = strdup(filename_tok->value);
    redir->next = NULL;
    consume_token(ps);  // Consume the filename token.

    return redir;
}

/*
 * parse_command:
 *   Parses a single command. The first WORD token is the command name.
 *   Subsequent WORD tokens are added as arguments.
 *   If a redirection operator is encountered, parse_redirection() is called.
 *   Parsing stops when a PIPE token or the end-of-input is reached.
 */
Command *parse_command(ParserState *ps) {
    Command *cmd = malloc(sizeof(Command));
    if (!cmd) {
        perror("malloc");ft_error("Malloc parser redirection failed");
        exit(EXIT_FAILURE);
    }
    cmd->command_name = NULL;
    cmd->args = NULL;
    cmd->args_count = 0;
    cmd->redirs = NULL;
    cmd->next = NULL;

    // The command must start with a W#include "parser.h"ORD token (command name).
    Token *tok = current_token(ps);
    if (!tok || tok->type != TOKEN_WORD)
        error("Expected command name");
    
    cmd->command_name = strdup(tok->value);

    // Initialize the arguments list with the command name.
    cmd->args_count = 1;
    cmd->args = malloc(sizeof(char *) * cmd->args_count);
    if (!cmd->args) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    cmd->args[0] = strdup(tok->value);
    consume_token(ps); // Consume the command name token.

    // Process the remaining tokens until we reach a PIPE or end-of-input.
    while ((tok = current_token(ps)) != NULL && tok->type != TOKEN_PIPE) {
        if (tok->type == TOKEN_REDIRECT_IN ||
            tok->type == TOKEN_REDIRECT_OUT ||
            tok->type == TOKEN_HEREDOC ||
            tok->type == TOKEN_APPEND) {
            // Parse and append the redirection.
            Redirection *redir = parse_redirection(ps);
            if (cmd->redirs == NULL) {
                cmd->redirs = redir;
            } else {
                Redirection *r = cmd->redirs;
                while (r->next != NULL)
                    r = r->next;
                r->next = redir;
            }
        }
        else if (tok->type == TOKEN_WORD) {
            // Allocate space for the new argument.
            cmd->args_count++;
            cmd->args = realloc(cmd->args, sizeof(char *) * cmd->args_count);
            if (!cmd->args) {
                perror("realloc");
                exit(EXIT_FAILURE);
            }
            cmd->args[cmd->args_count - 1] = strdup(tok->value);
            consume_token(ps);
        }
        else {
            error("Unexpected token in command");
        }
    }
    return cmd;
}

/*
 * parse_pipeline:
 *   Parses a pipeline of commands. A pipeline is one or more commands
 *   separated by the PIPE token.
 *   Returns the first Command structure in the linked list (each command's
 *   'next' pointer links to the next command in the pipeline).
 */
Command *parse_pipeline(ParserState *ps) {
    Command *first_cmd = parse_command(ps);
    Command *last_cmd = first_cmd;

    // While there is a PIPE token, continue parsing subsequent commands.
    while (current_token(ps) && current_token(ps)->type == TOKEN_PIPE) {
        consume_token(ps); // Consume the PIPE token.
        Command *next_cmd = parse_command(ps);
        last_cmd->next = next_cmd;
        last_cmd = next_cmd;
    }
    return first_cmd;
}

/* --- Functions to Create a Linked List of Tokens for Demonstration --- */

// Utility to create a new token.
Token *create_token(TokenType type, const char *value) {
    Token *new_tok = malloc(sizeof(Token));
    if (!new_tok) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    new_tok->type = type;
    new_tok->value = strdup(value);
    new_tok->next = NULL;
    return new_tok;
}

/*
 * For demonstration, create a linked list of tokens representing:
 * ls -l > output.txt | grep minishell
 */
Token *create_demo_token_list(void) {
    // Manually create each token.
    Token *t1 = create_token(TOKEN_WORD, "ls");
    Token *t2 = create_token(TOKEN_WORD, "-l");
    Token *t3 = create_token(TOKEN_REDIRECT_OUT, ">");
    Token *t4 = create_token(TOKEN_WORD, "output.txt");
    Token *t5 = create_token(TOKEN_PIPE, "|");
    Token *t6 = create_token(TOKEN_WORD, "grep");
    Token *t7 = create_token(TOKEN_WORD, "minishell");

    // Link the tokens together.
    t1->next = t2;
    t2->next = t3;
    t3->next = t4;
    t4->next = t5;
    t5->next = t6;
    t6->next = t7;
    // t7->next remains NULL.

    return t1;
}

/* --- Example Main Function --- 
 * This main() function is provided for demonstration purposes.
 * In your project, you would integrate the parser into your shell's command loop.
 */
int main(void) {
    // Create a linked list of tokens.
    Token *token_list = create_demo_token_list();

    // Initialize the parser state with the linked list.
    ParserState ps;
    ps.current = token_list;

    // Parse the pipeline of commands.
    Command *cmd_pipeline = parse_pipeline(&ps);

    // For demonstration: print out the parsed structure.
    Command *cmd = cmd_pipeline;
    int cmd_num = 1;
    while (cmd != NULL) {
        printf("Command %d:\n", cmd_num);
        printf("  Command name: %s\n", cmd->command_name);
        printf("  Arguments (%d): ", cmd->args_count);
        for (int i = 0; i < cmd->args_count; i++) {
            printf("%s ", cmd->args[i]);
        }
        printf("\n");

        Redirection *redir = cmd->redirs;
        while (redir != NULL) {
            char *redir_type;
            switch (redir->type) {
                case REDIR_IN: redir_type = "<"; break;
                case REDIR_OUT: redir_type = ">"; break;
                case REDIR_HEREDOC: redir_type = "<<"; break;
                case REDIR_APPEND: redir_type = ">>"; break;
                default: redir_type = "?";
            }
            printf("  Redirection: %s %s\n", redir_type, redir->filename);
            redir = redir->next;
        }
        cmd = cmd->next;
        cmd_num++;
    }

    // (Memory cleanup code would go here.)

    return 0;
}

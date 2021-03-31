#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_WORD_LEN 100

struct tree_node
{
  char *word;
  int count;
  struct tree_node *left;
  struct tree_node *right;
};

struct tree_node *add_to_tree(struct tree_node *node_p, char *word);
void print_tree(struct tree_node *node_p);

// There is a strdup available with POSIX, but it's not part of ISO C.
char *str_dup(char *src);

void skip_blanks();

int get_word(char *word, int max_word_len);

int main(void)
{
  struct tree_node *tree_root = NULL;
  char word[MAX_WORD_LEN];

  while (get_word(word, MAX_WORD_LEN) != EOF)
  {
    if (isalpha(word[0]))
    {
      tree_root = add_to_tree(tree_root, word);
    }
  }

  print_tree(tree_root);

  return EXIT_SUCCESS;
}

char *str_dup(char *src)
{
  char *dest = (char *)malloc(strlen(src) + 1);
  if (dest != NULL)
  {
    strcpy(dest, src);
  }
  return dest;
}

void skip_blanks()
{
  int c;
  while (isblank(c = getc(stdin)))
    ;
  ungetc(c, stdin);
}

int get_word(char *word, int max_word_len)
{
  skip_blanks();

  int c = getc(stdin);
  size_t i = 0;

  if (c != EOF)
  {
    word[i++] = c;
  }

  if (!isalpha(c) && c != '_')
  {
    word[i] = '\0';
    return c;
  }

  while ((isalnum(c = getc(stdin)) || c == '_') && i < max_word_len)
  {
    word[i++] = c;
  }
  ungetc(c, stdin);
  word[i] = '\0';

  return word[0];
}

struct tree_node *add_to_tree(struct tree_node *node_p, char *word)
{
  int cond;

  if (node_p == NULL)
  {
    node_p = (struct tree_node *)malloc(sizeof(struct tree_node));
    node_p->word = str_dup(word);
    node_p->count = 1;
    node_p->left = node_p->right = NULL;
  }
  else if ((cond = strcmp(word, node_p->word)) == 0)
  {
    node_p->count++;
  }
  else if (cond < 0)
  {
    node_p->left = add_to_tree(node_p->left, word);
  }
  else if (cond > 0)
  {
    node_p->right = add_to_tree(node_p->right, word);
  }

  return node_p;
}

void print_tree(struct tree_node *node_p)
{
  if (node_p != NULL)
  {
    print_tree(node_p->left);
    printf("%4d %s\n", node_p->count, node_p->word);
    print_tree(node_p->right);
  }
}

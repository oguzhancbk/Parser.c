#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#define CHUNK        1024

#define NONE         2
#define NUMBER       4
#define IDENTIFIER   6
#define LPAR         8
#define RPAR         10
#define LBRACE       12
#define RBRACE       14
#define PLUS         16
#define MINUS        18
#define STAR         20
#define SLASH        22
#define BANG         24
#define EQUAL        26
#define LESS         28
#define GREATER      30
#define EQEQUAL      32
#define LESSEQUAL    34
#define GREATEREQUAL 36
#define NOTEQUAL     38
#define SPACE        40
#define NEWLINE      42
#define IFEXPR       44
#define ELSEIFEXPR   46
#define ELSEEXPR     48
	

typedef struct token {
	int type;
	int lineno;
	int begin_colno_offset;
	int end_colno_offset;
	char *text;
	struct token *next;
} TOKEN;

int GetType(char chr)
{
   switch (chr) 
   {
   case '(':
      return LPAR;
   case ')':
      return RPAR;
   case '{':
      return LBRACE;
   case '}':
      return RBRACE;
   case '<':
      return LESS;
   case '>':
      return GREATER;
   case '=':
      return EQUAL;
   case '+':
      return PLUS;
   case '-':
      return MINUS;
   case '*':
      return STAR;
   case '/':
      return SLASH;
   case '!':
      return BANG;
   case ' ':
      return SPACE;
   case '\n':
   case '\r':
      return NEWLINE;
   }

   if (isalpha(chr) != 0)
      return IDENTIFIER;
   else if (isdigit(chr) != 0)
      return NUMBER;

  return NONE;
}

int GetTokenType(char *chr)
{
   if (strcmp(chr, ">=") == 0)
      return LESSEQUAL;
   if (strcmp(chr, "<=") == 0)
      return GREATEREQUAL; 
   if (strcmp(chr, "!=") == 0)
      return NOTEQUAL;
   if (strcmp(chr, "==") == 0)
      return EQEQUAL;
   if (strcmp(chr, "if") == 0)
      return IFEXPR;
   if (strcmp(chr, "elseif") == 0)
      return ELSEIFEXPR;
   if (strcmp(chr, "else") == 0)
      return ELSEEXPR;

   if (strlen(chr) == 1) {
      return GetType(chr[0]);
   }

   int i;
   int type_counter = 0;
   int size = strlen(chr);

   for (i = 0; i < size; i++)
   {
      if (isdigit(chr[i]))
         type_counter++;
   }

   if (type_counter == size)
      return NUMBER;

   if (size != 0)
      return IDENTIFIER;

   return NONE;
}

char* GetTokenTypeName(int token_type)
{
   switch (token_type)
   {
   case NONE:
      return "NONE";
   case NUMBER:
      return "NUMBER";
   case IDENTIFIER:
      return "IDENTIFIER";
   case LPAR:
      return "LPAR";
   case RPAR:
      return "RPAR";
   case LBRACE:
      return "LBRACE";
   case RBRACE:
      return "RBRACE";
   case PLUS:
      return "PLUS";
   case MINUS:
      return "MINUS";
   case STAR:
      return "STAR";
   case SLASH:
      return "SLASH";
   case BANG:
      return "BANG";
   case EQUAL:
      return "EQUAL";
   case LESS:
      return "LESS";
   case GREATER:
      return "GREATER";
   case EQEQUAL:
      return "EQEQUAL";
   case LESSEQUAL:
      return "LESSEQUAL";
   case GREATEREQUAL:
      return "GREATEREQUAL";
   case NOTEQUAL:
      return "NOTEQUAL";
   case SPACE:
      return "SPACE";
   case NEWLINE:
      return "NEWLINE";
   case IFEXPR:
      return "IFEXPR";
   case ELSEIFEXPR:
      return "ELSEIFEXPR";
   case ELSEEXPR:
      return "ELSEEXPR";
   }

   return "NONE";
}


void NextToken(char *text, char *buffer, int *iter, char *current_char, int *current_type, int *begin_colno_offset, int *end_colno_offset)
{
   strncat(text, current_char, 1);

   *current_char = buffer[*iter];
   *current_type = GetType(*current_char);
   (*end_colno_offset)++;
   (*iter)++;
}

TOKEN* CreateToken(char *text, int lineno, int begin_colno_offset, int end_colno_offset, TOKEN *next_token)
{
   next_token->text = (char*) malloc(sizeof(char));
   strcpy(next_token->text, text);
   next_token->lineno = lineno;
   next_token->begin_colno_offset = begin_colno_offset;
   next_token->end_colno_offset = end_colno_offset;
   next_token->type = GetTokenType(text);

   return next_token->next;
}

TOKEN* StartTokenize(char *buffer, int size)
{

   int current_type;
   int lineno = 1;
   int begin_colno_offset = 1, end_colno_offset = 0;
   int iter = 0;

   char current_char;

   char *text = (char*) malloc(sizeof(char)*256);
   memset(text, 0, sizeof(char)*256);
   
   TOKEN *token_root = (TOKEN*) malloc(sizeof(TOKEN));
   TOKEN *token_iter = token_root;

   NextToken(text, buffer, &iter, &current_char, &current_type, &begin_colno_offset, &end_colno_offset);

   while (iter <= size)
   {
      switch (current_type)
      {
      case IDENTIFIER:
         do {
            NextToken(text, buffer, &iter, &current_char, &current_type, &begin_colno_offset, &end_colno_offset);
         } while (current_type == IDENTIFIER || current_type == NUMBER);
         break;
      case NUMBER:
         do {
            NextToken(text, buffer, &iter, &current_char, &current_type, &begin_colno_offset, &end_colno_offset);
         } while (current_type == NUMBER);
         break;
      case EQUAL:
      case GREATER:
      case LESS:
         NextToken(text, buffer, &iter, &current_char, &current_type, &begin_colno_offset, &end_colno_offset);

         switch (current_type)
         {
         case EQUAL:
            NextToken(text, buffer, &iter, &current_char, &current_type, &begin_colno_offset, &end_colno_offset);
            break;
         }
         break;
      case NEWLINE:
         lineno++;
         begin_colno_offset = 1; end_colno_offset = 0;
         NextToken(text, buffer, &iter, &current_char, &current_type, &begin_colno_offset, &end_colno_offset);
         memset(text, 0, sizeof(text)+1);
         continue;
      case NONE:
         return NULL;
      case SPACE:
         NextToken(text, buffer, &iter, &current_char, &current_type, &begin_colno_offset, &end_colno_offset);
         memset(text, 0, sizeof(text)+1);
         continue;
      default:
         NextToken(text, buffer, &iter, &current_char, &current_type, &begin_colno_offset, &end_colno_offset);
         break;
      }

      token_iter->next = (TOKEN*) malloc(sizeof(TOKEN));
      token_iter = CreateToken(text, lineno, begin_colno_offset, end_colno_offset, token_iter);
      memset(text, 0, sizeof(text)+1);
      begin_colno_offset = end_colno_offset;
   }

   token_iter = NULL;
   free(token_iter);
   return token_root;
}

void SyntaxError(TOKEN** t, char *msg) 
{
   printf("%s <%d:%d>\n", msg, (*t)->lineno, (*t)->end_colno_offset-(*t)->begin_colno_offset);
}

void VerboseError(TOKEN* t, char *buffer)
{
   int i = 0;
   int lineno = 1;
   while (lineno < t->lineno) {
      if (buffer[i] == '\n')
         lineno++;
      printf("%c", buffer[i]);
      i++;
   }

   int j = i;
   while (buffer[j] != '\n') {
      printf("%c", buffer[j]);
      j++;
   } 

   printf("\n");

   int col = 0;
   while (col <= t->end_colno_offset) {
      printf("^");
      col++;
   }

   printf("\n");
}

int DIGIT_EXPR(TOKEN *t) {
   if (t->type != NUMBER)
      return 0;
	
	int i;
   for (i = 0; i < strlen(t->text); i++) {
      if (t->text[i] < 48 && t->text[i] > 57)
         return 0;
   }

   return 1;
}

int NUMBER_EXPR(TOKEN *t) {
   return DIGIT_EXPR(t);
}

int ID_EXPR(TOKEN *t) {
   if (t->type == IDENTIFIER)
      return 1;
   
   if (!NUMBER_EXPR(t)) return 0;

   return 1;
}

int WORD_EXPR(TOKEN **t) {
   if (ID_EXPR(*t)) {
      *t = (*t)->next;
   } else {
      SyntaxError(t, "Taninmayan tanimlayici");
      return 0;
   }

   if ((*t)->type == EQUAL) {
      *t = (*t)->next;
   } else {
      SyntaxError(t, "'=' eksik");
      return 0;
   }

   if (ID_EXPR(*t)) {
      *t = (*t)->next;

      switch ((*t)->type) {
      case PLUS:
      case MINUS:
      case STAR:
      case SLASH:
         *t = (*t)->next;
         if (ID_EXPR(*t)) {
            *t = (*t)->next;
         } else {
            SyntaxError(t, "Taninmayan tanimlayici");
            return 0;
         }
      }
   } else {
      SyntaxError(t, "Taninmayan tanimlayici");
      return 0;
   }

   return 1;
}

int OP_EXPR(TOKEN *t) {
   if (t->type != GREATER
   && t->type != LESS
   && t->type != GREATEREQUAL
   && t->type != LESSEQUAL
   && t->type != EQEQUAL) return 0;

   return 1;
}

int COND_EXPR(TOKEN **t) {
   if (ID_EXPR(*t)) {
      *t = (*t)->next;
   } else {
      SyntaxError(t, "Taninmayan tanimlayici");
      return 0;
   }

   if (OP_EXPR(*t)) {
      *t = (*t)->next;
   } else {
      SyntaxError(t, "Taninmayan tanimlayici");
      return 0;
   }

   if (ID_EXPR(*t)) {
      *t = (*t)->next;
   } else {
      SyntaxError(t, "Taninmayan tanimlayici");
      return 0;
   }

   return 1;
}

int BLOCK(TOKEN **t) {
   if ((*t)->type == LBRACE) {
      *t = (*t)->next;
   } else {
      SyntaxError(t, "'{' eksik");
      return 0;
   }

   if (!WORD_EXPR(t)) return 0;

   if ((*t)->type == RBRACE) {
      *t = (*t)->next;
   } else {
      SyntaxError(t, "'}' eksik");
      return 0;
   }

   return 1;
}

int ELSEIF_EXPR(TOKEN **t) {
   if ((*t)->type == ELSEIFEXPR) {
      *t = (*t)->next;

      if ((*t)->type == LPAR) {
         *t = (*t)->next;
      } else {
         SyntaxError(t, "'(' eksik");
         return 0;
      }

      if (!COND_EXPR(t)) return 0;

      if ((*t)->type == RPAR) {
         *t = (*t)->next;
      } else {
         SyntaxError(t, "')' eksik");
         return 0;
      }

      if (!BLOCK(t)) return 0;
   } else {
      SyntaxError(t, "Beklenen 'elseif' ");
      return 0;
   }
   return 1;
}

int ELSE_EXPR(TOKEN **t) {
   if ((*t)->type == ELSEEXPR) {
      *t = (*t)->next;
      if (!BLOCK(t)) return 0;
   } else {
      SyntaxError(t, "Beklenen 'else' ");
      return 0;
   }

   return 1;
}

int IF_EXPR(TOKEN **t) {
	
   if ((*t)->type == IFEXPR) {
      *t = (*t)->next;

      if ((*t)->type == LPAR) {
         *t = (*t)->next;
      } else {
         SyntaxError(t, "'(' eksik");
         return 0;
      }

      if (!COND_EXPR(t)) return 0;

      if ((*t)->type == RPAR) {
         *t = (*t)->next;
      } else {
         SyntaxError(t, "')' eksik");
         return 0;
      }

      if (!BLOCK(t)) return 0;

      while ((*t)->text != NULL && (*t)->type != ELSEEXPR) {
         if (!ELSEIF_EXPR(t)) return 0;
      }

      if ((*t)->next != NULL && (*t)) {
         if (!ELSE_EXPR(t)) return 0;
      }

      if ((*t)->next != NULL) {
         SyntaxError(t, "Taninmayan tanimlayici");
         return 0;
      }
   } else {
      SyntaxError(t, "Beklenen 'if' ");
      return 0;
   }

   return 1;
}

int StartParser(TOKEN *token, char* buffer)
{
   TOKEN *token_iter = token;

   if (!IF_EXPR(&token_iter)) {
      VerboseError(token_iter, buffer);
      return 0;
   }

   return 1;
}

int main(int argc, char **argv)
{
   char *file_name = (char*) malloc(sizeof(char)*256);

	if (argc > 1)
	{
      file_name = argv[1];
	} else {
      printf("Dosya adi giriniz: ");
      scanf("%s", file_name);

      if (strlen(file_name) <= 0) {
         printf("Dosya Bulunmuyor!\n");
         exit(EXIT_FAILURE);
      }
   }

   FILE *file;
   char buffer[CHUNK];

   if (buffer == NULL) {
      printf("MALLOC HATASI");
      exit(EXIT_FAILURE);
   }

   file = fopen(file_name, "r");
   int lc = 0;
   char line;

   while ((line = getc(file)) != EOF) 
      buffer[lc++] = line;

   if (strlen(buffer) == 0)
   {
      return 0;
   }

   TOKEN* token_root = StartTokenize(buffer, lc); 
   TOKEN* token = token_root;

   if (token == NULL) 
   {
      printf("BASARISIZ: Tokenizer\n");
      exit(EXIT_FAILURE);
   }
	
   int parserResult = StartParser(token, buffer);

   if (!parserResult) {
      printf("BAÞARISIZ: Parser\n");
   } else {
       printf("%s", buffer);
      printf("\nBASARILI: Parser\n");
   }
   fclose(file);
   return EXIT_SUCCESS;
}

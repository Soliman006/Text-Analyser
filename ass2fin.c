/* Text Analysis program
 * Code written by Soliman Ait Maamar, 695180, May 2016
 *
 * "Programming, Problem Solving, and Abstraction with C",revised edition,
 * by Alistair Moffat, will be refered to as "the textbook" with respect to 
 * attribution
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>
#include <assert.h>

/*Constant definitions*/
#define LINE_LEN 97
#define DIC_SIZE 100
#define VAR_DIC_SIZE 500

#define NUM_POS   5
#define NUM_VAR   4
#define NUM_EQ    25

#define LEN_STEM  20
#define LEN_POS   4
#define LEN_VAR   23
#define SENT_WORD 23

#define HASH      '#'
#define DOLLAR    '$'
#define SPACE     ' '
#define NULL_BYTE '\0'
#define ENTER     '\n'


typedef char pos_t[LEN_POS+1];
typedef char var_t[LEN_VAR+2];
typedef char stem_t[LEN_STEM+1];
typedef char sent_t[SENT_WORD+1];
typedef char nindex_var_t[LEN_VAR+1];

typedef struct {
	stem_t stem;
	int len_stem;
	pos_t pos_tags[NUM_POS];
	int num_pos;
	var_t variations[NUM_VAR];
	int num_vars;
} word_t;

/*Lines 49-54 were taken from the textbook on page 171
 */
typedef struct node node_t;

struct node{
	sent_t word;
	node_t *next;
};

/* the struct type queue_t is an edited version of struct type list_t from 
 * the textbook in figure 10.4 on page 172
 */
typedef struct{
	node_t *start;
	node_t *end;
} queue_t;

typedef struct{
	nindex_var_t nind_var;
	word_t *pstem;
} noindex_var_t;

/*Function prototypes*/
void init_dic(word_t dic[]);
void read_dic(char *line, char c);
void print_eq();
void print_stage(int stage_num);
void add_word(word_t *w,char *line);
void add_POS(word_t *w,char *line);
void print_POS(word_t *w);
void add_vars(word_t *w,char *line);
void print_vars(word_t *w);
int find_total_chars(word_t dic[],int num_words);
queue_t *make_empty_queue(void);
queue_t *insert_at_end(queue_t *queue,sent_t word);
void print_space(int word_length);
int compare_stage4(const void *p1, const void *p2);
void init_var_dic(noindex_var_t var_dic[]);
void assign_var_dic(word_t dic[],noindex_var_t var_dic[],int num_words,
	int*num_vars);
int compare_qsort(const void *p1, const void *p2);
int compare_stage5(const void *p1, const void *p2);

int
main(int argc, char *argv[]) {
	int num_words=0,sum_chars,sent_chars=0,sent_size=SENT_WORD,dic_vars=0;
	int num_sent_words=0;
	char c1,sent_c;
	char *sentence=NULL,*token=NULL;
	char line[LINE_LEN+1];
	word_t dic[DIC_SIZE];
	queue_t *sent_words;
	node_t *cur_word = NULL;
	word_t *stem_found=NULL;
	noindex_var_t var_dic[VAR_DIC_SIZE];
	noindex_var_t *word_found=NULL;
	
	
	init_dic(dic);
	init_var_dic(var_dic);

	while(scanf("%c",&c1)==1 && (c1 != '%')) {
		read_dic(line,c1);
		
		if(c1 == HASH) {
			add_word(&dic[num_words],line+1);
		}
		else if(c1 == DOLLAR) {
			add_vars(&dic[num_words],line+1);
			num_words++;
		}
		else{
			add_POS(&dic[num_words],line);
		}
	}	
	/* This while loop below is to go over the line with percent signs*/
	while(c1 != ENTER){
		scanf("%c",&c1);
	}
	
	sentence = (char *)malloc(sent_size);
	/* The if statement asserts that the pointer "sentence" is not NULL 
	 * so that it can point to a character.
	 */
	if(sentence == NULL){
				printf("No more memory available");
				exit(EXIT_FAILURE);
	}
	/*while loop reads in the sentence*/
	while(scanf("%c",&sent_c)==1 && sent_c != ENTER && sent_c != EOF){
		if(sent_chars+1 == sent_size){
			sent_size *= 2;
			sentence = (char *)realloc(sentence,sent_size);
			if(sentence == NULL){
				printf("No more memory available");
				exit(EXIT_FAILURE);
			}
			sentence[sent_chars] = sent_c;
		}
		else {
			sentence[sent_chars] = sent_c;
		}
		sent_chars++;
	}
	sentence[sent_chars] = NULL_BYTE;
	
	sent_words = make_empty_queue();
	
	/* The strtok function will split up the sentence in words with an 
	 * identifier that indicates where a word ends and another begins 
	 * (in this case a space()
	 */
	const char tok[2] = " ";
	token = strtok(sentence,tok);
	while(token != NULL){
		sent_words = insert_at_end(sent_words,token);
		token = strtok(NULL,tok);
		num_sent_words++;
	}
	
	print_stage(1);
	printf("Word 0: ");
	printf("%s\n",dic[0].stem);
	printf("POS: ");
	print_POS(&dic[0]);
	print_vars(&dic[0]);
	
	print_stage(2);
	sum_chars = find_total_chars(dic,num_words);
	printf("Number of words: %d\n", num_words);
	printf("Average number of characters per word: %.2f\n",
		1.0*sum_chars/num_words);
	
	print_stage(3);
	cur_word = sent_words->start;
	while(cur_word){
		printf("%s\n",cur_word->word);
		cur_word = cur_word->next;
	}
	
	print_stage(4);
	cur_word = sent_words->start;
	while(cur_word){
		printf("%s",cur_word->word);
		print_space(strlen(cur_word->word));
		stem_found = (word_t *)bsearch(cur_word->word,dic,num_words,
			sizeof(word_t),compare_stage4);
		if(stem_found == NULL){
			printf("NOT_FOUND\n");
		}
		else {
			print_POS(stem_found);
		}
		cur_word = cur_word->next;
	}
	
	print_stage(5);
	assign_var_dic(dic,var_dic,num_words,&dic_vars);
	cur_word = sent_words->start;
	while(cur_word){
		printf("%s",cur_word->word);
		print_space(strlen(cur_word->word));
		word_found = (noindex_var_t *)bsearch(cur_word->word,var_dic,dic_vars,
			sizeof(noindex_var_t),compare_stage5);
		if(word_found == NULL){
			printf("%s",cur_word->word);
			print_space(strlen(cur_word->word));
			printf("NOT_FOUND\n");
		}
		else{
			printf("%s",word_found->pstem->stem);
			print_space(strlen(word_found->pstem->stem));
			print_POS(word_found->pstem);
		}
		cur_word = cur_word->next;
	}
	
	return 0;
}

/*Initialises dictionary. Sets all characters in the strings to null bytes.*/
void
init_dic(word_t dic[]){
	int i,j,k;
	for(i=0;i < DIC_SIZE;i++){
		dic[i].len_stem = 0;
		dic[i].num_pos = 0;
		dic[i].num_vars = 0;
		for(j=0; j <= LEN_STEM ; j++){
			dic[i].stem[j] = NULL_BYTE;
		}
		for(j=0; j < NUM_POS; j++) {
			for(k=0; k <= LEN_POS; k++){
				dic[i].pos_tags[j][k] = NULL_BYTE;
			}
		}
		for(j=0; j < NUM_VAR; j++) {
			for(k=0; k <= (LEN_VAR+1); k++){
				dic[i].variations[j][k] = NULL_BYTE;
			}
		}
	}
	return;
}

/*function used for reading in the lines containing the dictionary*/
void
read_dic(char *line, char c){
	int i=0;
	while((c != EOF) && (c != ENTER)){
		line[i] = c;
		scanf("%c",&c);
		i++;
	}
	line[i] = NULL_BYTE;
	return;
}

/*Prints 25 equal signs*/
void
print_eq(){
	int i;
	for(i=0; i<NUM_EQ;i++){
		printf("=");
	}
	return;
}

/*prints 25 equals signs, the stage and another 25 equal signs*/
void
print_stage(int stage_num){
	print_eq();
	printf("Stage %d",stage_num);
	print_eq();
	printf("\n");
	return;
}

/*adds the stem of word to the dictionary*/
void
add_word(word_t *w, char *line){
	int i=0;
	while(*line != NULL_BYTE){
		w->stem[i] = *line;
		line ++;
		i++;
	}
	w->stem[i] = NULL_BYTE;
	w->len_stem = i;
	return;
}

void
add_POS(word_t *w,char *line){
	int i=0,j=0;
	while(*line != NULL_BYTE){
		if(*line == SPACE){
			w->pos_tags[i][j]=NULL_BYTE;
			i++;
			j=0;
		}
		else {
			w->pos_tags[i][j] = *line;
			j++;
		}
		line++;
	}
	w->pos_tags[i][j]=NULL_BYTE;
	w->num_pos = i+1;
	return;
}

void
print_POS(word_t *w){
	int i;
	for(i=0; i < w->num_pos; i++){
		printf("%s",w->pos_tags[i]);
		if((w->num_pos) != i+1){
			printf(" ");
		}
	}
	printf("\n");
	return;
}

void
add_vars(word_t *w,char *line){
	int i=-1,j=0;
	while(*line != NULL_BYTE){
		if(isdigit(*line)){
			i++;
			j=0;
			w->variations[i][j]=*line;
		}
		else {
			j++;
			w->variations[i][j] = *line;
		}
		line++;
	}
	w->num_vars = i+1;
	return;
}

void
print_vars(word_t *w){
	int i;
	printf("Form: ");
	for(i=0; i < w->num_vars; i++){
		printf("%s",w->variations[i]);
	}
	printf("\n");
	return;
}

int
find_total_chars(word_t dic[],int num_words){
	int i, sum = 0;
	for(i = 0; i<num_words; i++){
		sum += dic[i].len_stem;
	}
	return sum;
}

/* Edited version of the function list_t *make_empty_list(void) from 
 * the textbook in figure 10.4 on page 172
 */
queue_t
*make_empty_queue(void) {
	queue_t *sent_words;
	sent_words = (queue_t *)malloc(sizeof(*sent_words));
	assert(sent_words != NULL);
	sent_words->start = sent_words->end = NULL;
	return sent_words;
}

/* Edited version of the function 
 * list_t *insert_at_foot(list_t *list, data_t value)
 * from the textbook in figure 10.5 on page 173
 */
queue_t
*insert_at_end(queue_t *queue,sent_t new_word){
	node_t *new;
	new = (node_t *)malloc(sizeof(*new));
	assert(new != NULL);
	strcpy(new->word,new_word);
	new->next = NULL;
	if(queue->end == NULL){
		queue->start = queue->end = new;
	}
	else{
		queue->end->next = new;
		queue->end = new;
	}
	return queue;
}

/*Prints spaces such that the next word is below "Stage"*/
void
print_space(int word_length){
	int num_spaces,i;
	num_spaces = NUM_EQ - word_length;
	for(i=0; i < num_spaces; i++){
		printf(" ");
	}
}

int 
compare_stage4(const void *p1, const void *p2){
	const char *w1 = p1;
	const word_t *w2 = p2;
	return strcmp(w1,w2->stem);
}

void 
init_var_dic(noindex_var_t var_dic[]){
	int i,k;
	for(i=0; i<VAR_DIC_SIZE; i++){
		var_dic[i].pstem = NULL;
		for(k=0; k<=LEN_VAR; k++){
			var_dic[i].nind_var[k] = NULL_BYTE;
		}
	}
	return;
}

/* Assigns all stems and variations of words in the dictionary to var_dic[], 
 * which are then alphabetically sorted by qsort()
 */
void 
assign_var_dic(word_t dic[], noindex_var_t var_dic[], int num_words,
	int *dic_vars){
	int i,j=0,k=0;
	for(i=0; i<num_words; i++){
		k=0;
		var_dic[*dic_vars].pstem = dic+i;
		while(dic[i].stem[k] != NULL_BYTE){
			var_dic[*dic_vars].nind_var[k] = dic[i].stem[k];
			k++;
		}
		*dic_vars += 1;
		for(j=0; j<dic[i].num_vars; j++){
			k=0;
			var_dic[*dic_vars].pstem = dic+i;
			while(dic[i].variations[j][k] != NULL_BYTE){
				var_dic[*dic_vars].nind_var[k] = dic[i].variations[j][k+1];
				k++;
			}
			*dic_vars += 1;
		}
	}
	qsort(var_dic,*dic_vars,sizeof(noindex_var_t),compare_qsort);
	return;
}

int
compare_qsort(const void *p1, const void *p2){
	const noindex_var_t *var1 = p1;
	const noindex_var_t *var2 = p2;
	return strcmp(var1->nind_var,var2->nind_var);
}

int
compare_stage5(const void *p1, const void *p2){
	const char *word = p1;
	const noindex_var_t *vars = p2;
	
	return strcmp(word,vars->nind_var);
}
/* First and foremost: Algorithms are fun!!
 * From the textbook on page 207, we are told that asymptotic cost of 
 * binary search is O(log(n)) where n is the number of items searched over or 
 * in this case the number of structs in the var_dic array. 
 * This is the total number of forms of all words in the dictionary, which is 
 * the sum of the number of stems or the number of words in the dictionary(n1)
 * and the total number of variations of these words (n2). Furthermore as I 
 * use the strcmp function, which I presume will compare every character in 
 * the strings to each other. As such if the length of the 
 * strings(LEN_VAR) is increased, the time taken to compare the strings will 
 * increase linearly. Therefore the overall time complexity in stage 5 will be 
 * O(m*log(n1+n2)) where m is the maximum length of the variation forms.
 */
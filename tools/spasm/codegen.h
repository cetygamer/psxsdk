#ifndef _SPASM_CODEGEN_H
#define _SPASM_CODEGEN_H

typedef struct
{
	char name[128];
	unsigned int pc;
	unsigned int pass;
}asm_label;

extern asm_label *labels;

extern volatile unsigned int curPc;
extern int curPass;
extern unsigned int numLabels;
extern unsigned int startAddress;
extern unsigned int copn;
extern int first_instruction;
extern int org_found;
extern void (*INSFUNC)(void);

void codegen_init(void);
void add_label(char *label, unsigned int pc);
void add_label_equ(char *label, unsigned int pc);
unsigned int find_label(char *label);
void find_label_reset();
int find_label_ok();
int label_was_not_found_once(char *name);
void add_not_found_label(char *name);
//void resolve_labels();


#endif

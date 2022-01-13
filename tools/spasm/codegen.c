#include "spasm.h"

char curIns[128];
unsigned int curInsArg;
unsigned int curInsArgT;
unsigned int insArgv[64];
unsigned int insArgc;
unsigned int insArgt[64];
unsigned int copn;
int org_found;

void (*INSFUNC)(void);

volatile unsigned int curPc = 0;
int curPass = 0;
unsigned int startAddress = 0;
unsigned int numLabels;
unsigned int numLabelsAlloc;
int first_instruction;
asm_label *labels;
static int find_label_status = 1;

void codegen_init(void)
{
	curPc = startAddress;
	curPass = 0;
	numLabels = 0;
	numLabelsAlloc = 0;
	labels = NULL;
}
		
static asm_label *find_label_internal(char *name)
{
	int i;
	
	for(i = 0; i < numLabels; i++)
	{
		if(strcmp(name, labels[i].name) == 0)
			return &labels[i];
	}
	
	return NULL;
}

static void add_label_internal(char *name, unsigned int pc)
{
	// add labels only if current pass >= 1!
	asm_label *l;
	
/*	if(curPass == )
		return;
	
	if(curPass >= 2)
		return;*/

	//printf("Name = %s\n", name);
	
	l = find_label_internal(name);	
		
	if(l)
	{
		if(l->pc != pc)
		{
			//if(l->pass == curPass)
			//	assembler_error("Impossible to redefine label %s", name);
			
			//printf("Redefining, [%s] = %08X, pass %d\n", l->name, pc, curPass);
			l->pc = pc;
		}
				
		return;
	}
	
	if(numLabels == numLabelsAlloc)
	{
		numLabelsAlloc += 128;
		labels = realloc(labels, sizeof(asm_label) * numLabelsAlloc);
	}
	
	strncpy(labels[numLabels].name, name, 127);
	labels[numLabels].pass = curPass;
	
	labels[numLabels].pc = pc;
		
	numLabels++;
	
	//printf("label #%d, [%s] = %08X, pass = %d\n", numLabels, name, pc, curPass);
	
	/*while(*name)
	{
		printf("%x, \'%c\'\n", *name, *name);
		name++;
	}*/
}

	

void add_label(char *name, unsigned int pc)
{
	if(curPass == -1)
		return;
	
	return add_label_internal(name, pc);
}

void add_label_equ(char *name, unsigned int pc)
{
	return add_label_internal(name, pc);
}

unsigned int find_label(char *name)
{
	//printf("find_label(%s)\n", name);
	
	asm_label *l = find_label_internal(name);
	
	if(l)
	{
		//find_label_status = 1;
		return l->pc;
	}
	
// remember! if pass >= 1, abort if you can't find a label, because that means it was really
// impossible to find.	
	
//	printf(">>DEBUG, PASS = %d << Couldn't find label %s$\n",  curPass, name);
		
	find_label_status = 0;
	
	if(curPass == 1)
		instruction_error("Cannot find label %s", name);
	
	return 0xFFFFFFFF;
}

void find_label_reset()
{
	find_label_status = 1;
}

int find_label_ok()
{
	return find_label_status;
}

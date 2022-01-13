#include "spasm.h"

int atoiT[64];

static char *reg_names[] =
{
	"zero", 
	"at",
	"v0", "v1",
	"a0", "a1", "a2", "a3",
	"t0", "t1", "t2", "t3", "t4", "t5", "t6", "t7",
	"s0", "s1", "s2", "s3", "s4", "s5", "s6", "s7",
	"t8", "t9",
	"k0", "k1",
	"gp", "sp",
	"fp",
	"ra",
	NULL
};

static int regtoi(char *arg)
{
	int i;

	if(strcmp(arg, "s8") == 0)
		arg = "fp";
	
	for(i = 0; reg_names[i]; i++)
	{
		if(strcmp(arg, reg_names[i]) == 0)
			return i;
	}
	
	return -1;
}

unsigned int asm_atoi(char *arg)
{    
	unsigned int i;
    
	if((i = regtoi(arg)) != -1)
	{
		atoiT[insArgc] = T_REGISTER;
		return i;
	}
	else if(*arg == '-' && *(arg+1) == '$' && isxdigit((unsigned int)*(arg+2)) )
	{
		sscanf(arg+2, "%x", &i);
		atoiT[insArgc] = T_INTEGER;
		return -i;
	}
	else if(*arg == '$' )
	{
		if((i = regtoi(arg+1)) == -1)
		{
			sscanf(arg+1, "%x", &i);
			atoiT[insArgc] = T_INTEGER;
		}
		else
			atoiT[insArgc] = T_REGISTER;
		
		return i;
	}
	else if(strcmp(arg, "*") == 0)
	{
		return curPc;
	}
	else if(isalpha((unsigned int)*arg))
	{
		atoiT[insArgc] = T_LABEL;
		return find_label(arg);
	}
	
	sscanf(arg, "%i", &i);
	atoiT[insArgc] = T_INTEGER;
	
	return i;
}

enum
{
	INITIAL, ARG_ENTER, COMMENT
};

char *spasm_parser(char *text, int pass)
{
	int i, j, l;
	char linebuf[1024];
	char linebuf2[1024];
	char linebuf3[1024];
	char argbuf[1024];
	char *tok[256];
	int state = INITIAL;
	int num_of_tok=0;
	char *t;
	curText = text;
	unsigned int v;

theBeginning:
	i = 0;
	curPass = pass;
	org_found = 0;
	first_instruction = 1;
	line_number = 0;
	text = curText;
	
	while(text[i])
	{
		state = INITIAL;
		
		for(j = 0; text[i] && text[i] != '\n'; i++)
		{
			if(j < 1023 && text[i] != '\r')
				linebuf[j++] = text[i];
		}
		
		line_number++;
		rawArgc = insArgc = 0;
		INSFUNC = NULL;
		
		if(text[i] == '\n')
			i++;
		
		linebuf[j] = '\0';
		
		strcpy(linebuf2, linebuf); // Keep a second copy, we will need it later.
		strcpy(linebuf3, linebuf);		
		//if(curPass==1)printf("linebuf[%d] %d:= %s\n",  curPass, line_number, linebuf);
		curLine = linebuf3;
		
		char *a = linebuf;
		char *s;
		j = 0;
	
		num_of_tok = 0;
		
		while((s = strtok(a, " \t")))
		{
			tok[num_of_tok++] = s;
			a = NULL;
		}
		
		tok[num_of_tok] = NULL;
		
		j = 0;
		
		while((s = tok[j]))
		{
			if(strlen(s) == 0)
			{ // A token with zero length is garbage, skip it
				j++;
				continue;
			}
				
			//printf("s = ^%s\n", s);
			
			switch(state)
			{
				case INITIAL: // Initial case
					
					// Is this token a comment?
				
					if(*s == ';')
					{
						state = COMMENT;
						break;
					}
				
					// Is the token a label?
				
					if((INSFUNC = get_instruction(s)))
					{
						strncpy(curIns, s, 127);
						s[127] = '\0';
						insArgc = 0;
						state = ARG_ENTER;
						argbuf[0] = '\0';
						
						break;
					}
					
					// Now we know it's a label
					// There are two possible cases now
					// - It's a label with a specified value: i.e. label EQU value
					// - It's a label which has the current value of the program counter
					
					// First, we will check for EQU
					
					if((j + 2) < num_of_tok)
					{  // If there are not enough tokens in the line, don't bother checking for EQU.
						if(strcasecmp(tok[j+1], "equ") == 0 || strcasecmp(tok[j+1], "=") == 0)
						{ // EQU found! Set label value to the one specified.
							
							// Set current instruction to EQU
							strcpy(curIns, "equ");
							
							// Remove quotes. Yes, in SPASM, values in EQU statements can have quotes
							// even if they are numerical values!
							
							if( (t = strchr(tok[j+2], '"')) )
							{
								*t = '\0';
								if( (t = strrchr(tok[j+2], '"')) ) 
									*t = '\0';
							}
							
							find_label_reset();
							
							v = spasm_eval(tok[j+2]);
						
							if(!find_label_ok())
								assembler_error("Can't resolve expression for EQU statement");
							
							//add_label(tok[j], asm_atoi(tok[j+2]));
							//printf("tok[j+2] = %s, %d\n", tok[j+2], v);
							add_label(tok[j], v);
							j+=2;// As we have processed the EQU, we need to jump two tokens ahead
							break;
						}
					}
					
					// At this point, it is a label which has the current value of the program counter
					
					if((t = strrchr(tok[j], ':')))
						*t='\0'; // Remove trailing colon, if any.
					
					add_label(tok[j], curPc);
				break;

				case ARG_ENTER: // Inside instruction
					// Is this token a comment?
					// If so, we do not have arguments anymore.
				
					if(*s == ';')
					{
						state = COMMENT;
						break;
					}

					strcat(argbuf, s);
					
					int was_sp = 0;
					int in_string = 0;
					int stringt = 0;
					char *argPlace = &linebuf2[ tok[j] - tok[0] ];

					a = linebuf2;
					
				//	printf("j = %d\n", j);
					
					
					l = 0;
					while(l < j && (argPlace = strtok(a, " \t")))
					{
						a = NULL;
						l++;
					}
					
					while(*argPlace)argPlace++;
					while(!(*argPlace))argPlace++;
					
					/*printf("argPlace = %s\n", argPlace);
					
					printf("argPlace = ^%s$\n", argPlace);*/
					
					char arg[64];
					char *argp = arg;
					int is_ok = 0;
					int esc = 0;
					
					rawArgc = 0;
										
					for(l = 0; argPlace[l] && rawArgc < 64; l++)
					{
						char c = argPlace[l];
						
						if(in_string)
						{
							*(argp++) = c;
							//printf("ARGP == %c\n", *(argp-1));
							
							if(!esc)
							{
								if(stringt == 0 && c == '"')
									in_string = 0;
								else if(stringt == 1 && c == '\'')
									in_string = 0;
								else if(c == '\\')
									esc = 1;
							}
							else
								esc = 0;
						}
						else
						{
							if(isalnum((unsigned int)c) || c == '_' || c == '$' || c == '.' || c == '*')
							{
								is_ok = 0;
								
								if(was_sp && (isalnum((unsigned int)*(argp-1)) || (*(argp-1) == '_') ||
										(*(argp-1) == '$') || (*(argp-1) == '"')
											|| (*(argp-1) == '\'') || (*(argp-1) == '.') || (*(argp-1) == '*')))
									goto noMoreArgs;
								
								*(argp++) = c;
								was_sp = 0;
							}
							else if(c == '"')
							{
								if(was_sp && (isalnum((unsigned int)*(argp-1)) || (*(argp-1) == '_') ||
										(*(argp-1) == '$') || (*(argp-1) == '"')
											|| (*(argp-1) == '\'') || (*(argp-1) == '.') || (*(argp-1) == '*')))
									goto noMoreArgs;
								
								*(argp++) = c;
								was_sp = 0;
								in_string = 1;
								stringt = 0;
								esc = 0;
							}
							else if(c == '\'')
							{
								if(was_sp && (isalnum((unsigned int)*(argp-1)) || (*(argp-1) == '_') ||
										(*(argp-1) == '$') || (*(argp-1) == '"')
											|| (*(argp-1) == '\'') || (*(argp-1) == '.') || (*(argp-1) == '*')))
									goto noMoreArgs;
								
								*(argp++) = c;
								was_sp = 0;
								in_string = 1;
								stringt = 1;
								esc = 0;
							}
							else if(c == ' ' || c == '\t')
							{
								is_ok = 0;
								was_sp = 1;
							}
							else if(c == '+' || c == '-' || c == '>' || c == '<'
								|| c == '(' || c == ')' || c == '&' || c == '|')
							{
								is_ok = 0;
								*(argp++) = c;
							}
							else if(c == ',')
							{
								*argp = '\0';
								insArgt[rawArgc] = 0;
								strcpy(rawArgv[rawArgc++], arg);
								argp = arg;
								was_sp = 0;
								is_ok = 1;
							}
							else if(c == ';' || c == '/')
							{
// '/' added in order to emulate a very buggy behavior of SPASM that is needed
// in order to assemble the imbNES 1.3.2 sources without modifications.
// yes, imbNES has some UNDENOTATED comments!
//
// pearls such as  
//								
// lw      v0,$1074(v1)    // this line would be at $DFAC where the jump goes from the patch								
//
// and...
//
// lw	v0,$DFFC(v0)	load the address to jump back to that was set in _patch_card
//
// It makes no sense, but hey it works in the original SPASM!								

								
								goto noMoreArgs;
							}
							else
							{
								instruction_error("Invalid character!\n");
								break;
							}
						}
					}
					
noMoreArgs:			
					if(!is_ok)
					{
						*argp = '\0';
						
						
						//strcpy(argv[argc], arg);
						
						char *fb = strchr(arg, '(');
						char *sb = strrchr(arg, ')');

						if(*arg != '"' && *arg != '\'' && fb && sb && fb<sb)
						{
							*fb = '\0';
							*sb = '\0';
						}
						
						insArgt[rawArgc] = 0;
						strcpy(rawArgv[rawArgc++], arg);
						
						if(*arg != '"' && *arg != '\'' && fb && sb && fb<sb)
						{
							insArgt[rawArgc] = 1;
							strcpy(rawArgv[rawArgc++], fb+1);
						}
					}
					
					insArgc = 0;
					
					find_label_reset();
					
					for(l = 0; l < rawArgc; l++, insArgc++)
					{
					//	printf("argv[%d] = %s\n", l, rawArgv[l]);
						
						insArgv[l] = spasm_eval(rawArgv[l]);
						
					//	printf("insArgv[%d] = %08x\n", l, insArgv[l]);
					}
					
					if(curPass == 1 && !find_label_ok())
						instruction_error("Can't resolve expression");
				
					//for(l = 0; l < insArgc; l++)
					//	printf("atoiT[%d] = %d\n", l, atoiT[l]);
					
					goto theNextLine;
				break;
				
				case COMMENT: // Inside comment
					
				break;
			}
			
			a = NULL;
			j++;
		}
theNextLine:
		if(INSFUNC)
		{
			INSFUNC();
			if(strcasecmp(curIns, "include") == 0 && curPass == 0)
				goto theBeginning;
			
			first_instruction = 0;
		}
	}
	
	return curText;
}

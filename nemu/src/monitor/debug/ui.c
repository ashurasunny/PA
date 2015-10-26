#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <errno.h>

void cpu_exec(uint32_t);

/* We use the ``readline'' library to provide more flexibility to read from stdin. */
char* rl_gets()
{
    static char *line_read = NULL;

    if (line_read)
    {
        free(line_read);
        line_read = NULL;
    }

    line_read = readline("(nemu) ");

    if (line_read && *line_read)
    {
        add_history(line_read);
    }

    return line_read;
}

static int cmd_c(char *args)
{
    cpu_exec(-1);
    return 0;
}

static int cmd_q(char *args)
{
    return -1;
}

static int cmd_help(char *args);
static int cmd_p(char *args);
int calculate(char * express, int length);
int all_number(char* express, int length);
static int cmd_info(char *args);
static int cmd_w(char *args);
static int cmd_d(char *args);
static int cmd_x(char *args);
static int pre_deal(char *express, int length);

static int cmd_si(char* args)
{
    if(args == NULL)
        cpu_exec(1);
    else
        cpu_exec(atoi(args));
    return 0;
}
static struct
{
    char *name;
    char *description;
    int (*handler) (char *);
} cmd_table [] =
{
    { "help", "Display informations about all supported commands", cmd_help },
    { "c", "Continue the execution of the program", cmd_c },
    { "q", "Exit NEMU", cmd_q },
    { "si","Excute program step by step", cmd_si},
    { "p","Calculate expression",cmd_p},
    { "info","Print information of register(please input r) or watch point(please input w)",cmd_info},
    { "w","Config a watchpoint",cmd_w},
    { "d", "delete a watchpoint ", cmd_d},
    { "x", "print some memory value", cmd_x},
    /* TODO: Add more commands */

};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

static int cmd_help(char *args)
{
    /* extract the first argument */
    char *arg = strtok(NULL, " ");
    int i;

    if(arg == NULL)
    {
        /* no argument given */
        for(i = 0; i < NR_CMD; i ++)
        {
            printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        }
    }
    else
    {
        for(i = 0; i < NR_CMD; i ++)
        {
            if(strcmp(arg, cmd_table[i].name) == 0)
            {
                printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
                return 0;
            }
        }
        printf("Unknown command '%s'\n", arg);
    }
    return 0;
}

static int cmd_p(char *args)
{//swaddr_write(1111,1,'9');
    printf("%d\n",pre_deal(args,strlen(args)));
    return 0;
}
static int cmd_info(char *args)
{
    int i;
    if('r' == *args)
    {
        for(i = 0;i < 8;i++)
        {
            if(i < 4)
                printf("%s\t0x%x\t%d\n",regsl[i],cpu.gpr[i]._32,cpu.gpr[i]._32);
           else
                printf("%s\t0x%x\t0x%x\n",regsl[i],cpu.gpr[i]._32,cpu.gpr[i]._32);
        }
        printf("eip\t0x%x\t0x%x\n ",cpu.eip,cpu.eip);
        printf("eflags\t0x%x\t0x%x\n",cpu.EFLAGS.val,cpu.EFLAGS.val);
        printf("ZF:%d CF:%d OF:%d  SF:%d AF:%d PF:%d \n",cpu.EFLAGS.ZF,cpu.EFLAGS.CF,cpu.EFLAGS.OF,cpu.EFLAGS.SF,cpu.EFLAGS.AF,cpu.EFLAGS.PF);
    }
    else if('w' == *args)
    {
        int i;
        WP *temp =* get_wp_head();
        for(i=0;i<32;i++)
        {
            if(temp->exist)
                printf("%d.\t0x%x\t%u\n",(*temp).NO,(*temp).watchaddr,swaddr_read(temp->watchaddr,1));
            temp = temp ->next;
        }
    }
    else printf("Unknown command '%s'\n",args);
    return 0;
}

static int cmd_w(char *args)
{
   if(*args != '*')
    {
        printf("wrong set\n");
        return 0;
    }
    else
    {
        args++;
        WP* temp = *get_wp_head();
        int j;
        for (j=0;j<32;j++)
        {
            if(temp->exist)
            {
                temp = temp->next;
                continue;
            }
            else
            {
                temp->watchaddr = calculate(args,strlen(args));
                temp->old_data = swaddr_read(temp->watchaddr,1);
                temp ->exist = 1;
                printf("set OK\n");
                break;
            }
        }
    }
    return 0;
}


static int cmd_d(char *args)
{
    int i;
    WP *temp = *get_wp_head();
    for(i=0;i<32;i++)
    {
        if(temp->NO == atoi(args))
        {
            temp->exist =0;
            return 0;
        }
        temp = temp->next;
    }
    printf("not found\n");
    return 0;
}

static int cmd_x(char *args)
{
    strtok(args ," ");
    int i = atoi(args);
    args = args + strlen(args)+1;
    uint32_t addr = pre_deal(args,strlen(args));
    int j;
    addr = addr&0xfffc;
    for(j = 0; j< i; j++)
    {
        printf("0x%x : 0x%x \n",addr,swaddr_read(addr,4));
        addr += 4;
    }
    printf("\n");
    return 0;
}

void ui_mainloop()
{
    while(1)
    {
        char *str = rl_gets();
        char *str_end = str + strlen(str);

        /* extract the first token as the command */
        char *cmd = strtok(str, " ");
        if(cmd == NULL)
        {
            continue;
        }

        /* treat the remaining string as the arguments,
         * which may need further parsing
         */
        char *args = cmd + strlen(cmd) + 1;
        if(args >= str_end)
        {
            args = NULL;
        }

#ifdef HAS_DEVICE
        extern void sdl_clear_event_queue(void);
        sdl_clear_event_queue();
#endif

        int i;
        for(i = 0; i < NR_CMD; i ++)
        {
            if(strcmp(cmd, cmd_table[i].name) == 0)
            {
                if(cmd_table[i].handler(args) < 0)
                {
                    return;
                }
                break;
            }
        }

        if(i == NR_CMD)
        {
            printf("Unknown command '%s'\n", cmd);
        }
    }
}

/**********************表达式求值函数**********************/
int all_number(char* express, int length)
{
    int i;
    for(i = 0;i < length; i++)
        if(express[i]<'0'|| ((express[i]>'9')&&(express[i]<'A')) || ((express[i]>'Z')&&(express[i]<'a')) ||express[i]>'z')
            return 0;
    return 1;
}

int calculate(char * express, int length)
{
	if(all_number(express,length))
        {
            int j;
            int ret;
            char a[length+1];
            for(j = 0;j < length ;j++)
                a[j] = express[j];
            a[length] = 0;
            ret = strtol(a,NULL,0);
            if(errno != 0)
            {
                printf("unknown number\n");
                exit(-1);
            }
            else
                return ret;
        }
	if(express[0] == '('&& express[length-1] == ')')
        return calculate(express+1,length-2);
    if(express[1] == '('&& express[length-1] == ')'&&express[0] == '*')
        return swaddr_read( calculate(express + 2, length - 3),4);
	int i = 0;
	int cur_pre = 65535;
	int basic_pre = 0;
	int cur_place = 0;
	if(*express == '*')
    {
        cur_pre =5;
        i=1;
    }
	for(;i<length;i++)
		{
			switch(express[i])
				{
					case '(':basic_pre += 10;break;
					case ')':basic_pre -= 10;break;
					case '+':if(basic_pre + 1 < cur_pre)
                                {cur_pre = basic_pre+1; cur_place = i;}
                                break;
					case '-':if(basic_pre + 1 < cur_pre)
                                {cur_pre = basic_pre+1; cur_place = i;}
                                break;
					case '*':if(basic_pre + 3 < cur_pre)
                                {
                                    cur_pre = basic_pre+3;
                                    cur_place = i;
                                }
                                break;
					case '/':if(basic_pre + 3 < cur_pre)
                                {cur_pre = basic_pre+3; cur_place = i;}
                                break;
					default:;
				}
		}
//		if(basic_pre != 0)
 //           return -1;
        switch(express[cur_place])
        {
            case '+' :return calculate(express,cur_place)+calculate(express+cur_place+1,length - cur_place-1);
            case '-' :return calculate(express,cur_place)-calculate(express+cur_place+1,length - cur_place-1);
            case '*' :
                if(cur_place != 0)
                return calculate(express,cur_place)*calculate(express+cur_place+1,length - cur_place-1);
                else
                    return swaddr_read(calculate(express+1,length - 1),4);
            case '/' :return calculate(express,cur_place)/calculate(express+cur_place+1,length - cur_place-1);
            default:return -1;
        }
}



/*************预处理表达式中的寄存器*************/
static int pre_deal(char *express, int length)
{
    int i = 0,j = 0;
    //uint32_t reg_value_temp;
    char  new_express[256];

    for (;i<length;i++,j++)
    {
        if(express[i]=='$')
        {
            if(express[i+1] == 'e')
            {
                if(express[i+2]=='a')
                {
                    if(express[i+3]=='x')
                        {
                            i+=3;
                            j+=sprintf(new_express,"%d",cpu.eax);
                            j-=1;
                            continue;
                        }
                }
                else if(express[i+2]=='b')
                {
                    if(express[i+3]=='x')
                        {
                            i+=3;
                            j+=sprintf(new_express,"%d",cpu.ebx);
                            j-=1;
                            continue;
                        }
                        else if(express[i+3]=='p')
                        {
                            i+=3;
                            j+=sprintf(new_express,"%d",cpu.ebp);
                            j-=1;
                            continue;
                        }
                }
                else if(express[i+2]=='c')
                {
                    if(express[i+3]=='x')
                        {
                            i+=3;
                            j+=sprintf(new_express,"%d",cpu.ecx);
                            j-=1;
                            continue;
                        }
                }
                else if(express[i+2]=='d')
                {
                    if(express[i+3]=='x')
                        {
                            i+=3;
                            j+=sprintf(new_express,"%d",cpu.edx);
                            j-=1;
                            continue;
                        }
                        else if(express[i+3]=='i')
                        {
                            i+=3;
                            j+=sprintf(new_express,"%d",cpu.edi);
                            j-=1;
                            continue;
                        }
                }
            }
            else if(express[1] == 'a')
            {
                if(express[2] == 'x')
                {
                    i+=2;
                    j+=sprintf(new_express,"%d",cpu.gpr[0]._16);
                    j--;
                    continue;
                }
                else if(express[2] == 'h')
                {
                    i+=2;
                    j+=sprintf(new_express,"%d",cpu.gpr[0]._8[1]);
                    j--;
                    continue;
                }
                else if(express[2] == 'l')
                {
                    i+=2;
                    j+=sprintf(new_express,"%d",cpu.gpr[0]._8[0]);
                    j--;
                    continue;
                }
            }
            else if(express[1] == 'b')
            {
                if(express[2] == 'x')
                {
                    i+=2;
                    j+=sprintf(new_express,"%d",cpu.gpr[3]._16);
                    j--;
                    continue;
                }
                else if(express[2] == 'h')
                {
                    i+=2;
                    j+=sprintf(new_express,"%d",cpu.gpr[3]._8[1]);
                    j--;
                    continue;
                }
                else if(express[2] == 'l')
                {
                    i+=2;
                    j+=sprintf(new_express,"%d",cpu.gpr[3]._8[0]);
                    j--;
                    continue;
                }
            }
            else if(express[1] == 'c')
            {
                if(express[2] == 'x')
                {
                    i+=2;
                    j+=sprintf(new_express,"%d",cpu.gpr[1]._16);
                    j--;
                    continue;
                }
                else if(express[2] == 'h')
                {
                    i+=2;
                    j+=sprintf(new_express,"%d",cpu.gpr[1]._8[1]);
                    j--;
                    continue;
                }
                else if(express[2] == 'l')
                {
                    i+=2;
                    j+=sprintf(new_express,"%d",cpu.gpr[1]._8[0]);
                    j--;
                    continue;
                }
            }
            else if(express[1] == 'd')
            {
                if(express[2] == 'x')
                {
                    i+=2;
                    j+=sprintf(new_express,"%d",cpu.gpr[2]._16);
                    j--;
                    continue;
                }
                else if(express[2] == 'h')
                {
                    i+=2;
                    j+=sprintf(new_express,"%d",cpu.gpr[2]._8[1]);
                    j--;
                    continue;
                }
                else if(express[2] == 'l')
                {
                    i+=2;
                    j+=sprintf(new_express,"%d",cpu.gpr[2]._8[0]);
                    j--;
                    continue;
                }
            }
        }
        else
        {
            new_express[j] = express[i];
        }
    }
    return calculate(new_express,strlen(new_express));
}

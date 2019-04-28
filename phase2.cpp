#include <bits/stdc++.h>
#include <stdlib.h>
#include <stdio.h>
using namespace std;

//Register file
static int reg[32];

//memory
static unsigned char memory[4000];

//CLOCK CYCLES
static int clock_cycles = 0;

//Stack pointer
static  int sp = 4000;

//Instruction register & program counter & interstage registers;
static int IR,PC,Ra,Rb,imm,Rd,Rz, Control_line, temp_PC, Ry, Rm, Rm_temp;

// it is used to set the reset values reset all registers and memory content to 0
void reset()
{
	for(int i=0;i<32;i++)
		reg[i]=0x00000000;
	reg[2] = sp;		//Stack pointer
	for(int i=0;i<4000;i++)
		memory[i]=0x00;
}

void write_word(unsigned char *mem, unsigned int address, unsigned int data)
{
  	mem+=address;
  	int *p = (int*)mem;
  	*p = data;
}

int read_word(unsigned char *mem, unsigned int address)
{
	mem+=address;
	int *p = (int*)mem;
	return *p;
}

//load_program_memory reads the input memory, and pupulates the instruction memory
void load_program_memory()
{
	FILE *fp;
  	unsigned int address,word;
  	fp = fopen("machine_code.mc", "r");
  	if(fp == NULL)
  	{
    	printf("Error opening input mem file\n");
    	exit(1);
  	}
  	while(fscanf(fp, "%x %x", &address, &word) != EOF)
  	{
  		if(word== 0xffffffff)
  			continue;
  		write_word(memory, address, word);	
  	} 
    	
  	fclose(fp);
  	for(int i=0;i<32;i++)
  	{

  		reg[i] = 0x0;
  					// initialising registers with zero
  	}
  	reg[2]=sp;
}

//writes the data memory in "data_out.mem" file
void write_data_memory() 
{

  	FILE * fp;
  	unsigned int i;
  	fp = fopen("data_out.mem", "w");
  	if(fp == NULL) 
  	{
    	printf("Error opening dataout.mem file for writing\n");
    	return;
  	}
  	for(int i=0;i<32;i++)
	{
		fprintf(fp,"reg%d:%d\n",i,reg[i]);
	}
  	for(i=0; i < 4000; i = i+4)
  	{
  		//int word = read_word(memory,i);
  		fprintf(fp, "%x %x\n", i, read_word(memory, i));
  		/*fprintf(fp,"%x ",i);
  		fprintf(fp,"%x",memory[i]);
  		fprintf(fp,"%x",memory[i+1]);
  		fprintf(fp,"%x",memory[i+2]);
  		fprintf(fp,"%x\n",memory[i+3]);*/
  	}	
    	
  	fclose(fp);
}

void fetch()
{
	unsigned char *mem = memory;
	int *p = (int*)(mem+PC);
	IR = 0x00000000;
	IR = *p;
	PC+=4;
}

/************ DECODE **************************/
unsigned int opcode(unsigned int word)
{
	unsigned int temp = 0x0000007F;
	temp = temp & word;
	return temp;
}

unsigned int rd(unsigned int word)
{
	unsigned int temp = 0x00000F80;
	temp = temp &  word;
	temp = temp/128;
	return temp;
}

unsigned int func3(unsigned int word)
{
	unsigned int temp = 0x00007000;
	temp = temp & word;
	temp = temp/4096;
	return temp;
}

unsigned int rs1(unsigned int word)
{
	unsigned int temp = 0x000F8000;
	temp = temp & word;
	temp = temp/32768;
	return temp;
}

unsigned int rs2(unsigned int word)
{
	unsigned int temp = 0x01F00000;
	temp = temp & word;
	temp = temp/1048576;
	return temp;
}

unsigned int func7(unsigned int word)
{
	unsigned int temp = 0xFE000000;
	temp = temp & word;
	temp = temp/33554432;
	return temp;
}

int imm_11_0 (unsigned int word)
{
	int temp = 0xFFF00000;
	temp = temp & word;
	temp = temp/1048576;
	return temp;
}

int imm_4_0 (unsigned int word)
{
	return rd(word);
}

int imm_11_5 (unsigned int word)
{
	return func7(word);
}

int imm_4_1_11 (unsigned int word)
{
	return rd(word);
}

int imm_12_10_5 (unsigned int word)
{
	return func7(word);
}

int imm_31_12 (unsigned int word)
{
	int temp = 0xFFFFF000;
	temp = temp&word;
	temp = temp/4096;
	return temp;
}

int imm_20_10_1_11_19_12 (unsigned int word)
{
	return imm_31_12(word);
}
/**********************************************************/
void extract_r()
{
	cout << "rs1:" << rs1(IR) << " rs2:" << rs2(IR) << " rd:" << rd(IR) << endl; 
	Ra = reg[rs1(IR)];
	Rb = reg[rs2(IR)];
	Rd = rd(IR);
	int opc = opcode(IR);
	int f3 = func3(IR);
	int f7 = func7(IR);		
	if(opc==51)
	{
		if(f3== 0 && f7==0)
			Control_line = 1;			//add inst

		else if(f3== 0 && f7==32)
			Control_line = 2;			//sub inst

		else if(f3==1 && f7==0)	
			Control_line = 3;			//sll inst

		else if(f3==2 && f7==0)	
			Control_line = 4;	//slt inst

		else if(f3==3 && f7==0)	
			Control_line = 5;		//sltu inst

		else if(f3==4 && f7==0)			
			Control_line = 6;			//xor inst

		else if(f3==5 && f7==0)
			Control_line = 7;			//srl inst

		else if(f3==5 && f7==32)
			Control_line = 8;			//sra inst

		else if(f3==6 && f7==0)
			Control_line = 9;			//or inst

		else if(f3==7 && f7==0)
			Control_line = 10;			//and inst

		else if(f3==0 && f7==1)
			Control_line = 11;			//mul inst

		else if(f3==1 && f7==1)
			Control_line = 12;			//mulh inst

		else if(f3==2 && f7==1)
			Control_line = 13;			//mulhsu inst
		
		else if(f3==3 && f7==1)
			Control_line = 14;			//mulhu inst
		
		else if(f3==4 && f7==1)
			Control_line = 15;			//div inst
		
		else if(f3==5 && f7==1)
			Control_line = 16;			//divu inst
		
		else if(f3==6 && f7==1)
			Control_line = 17;			//rem inst
		
		else if(f3==7 && f7==1)
			Control_line = 18;			//remu inst
	}

	else if(opc==59)
	{
		if(f3==0 && f7==0)			//addw inst
			Control_line = 21;
		
		else if(f3==0 && f7==32)			//subw inst
			Control_line = 22;

		else if(f3==1 && f7==0)			//sllw inst
			Control_line = 23;
		
		else if(f3==5 && f7==0)			//srlw inst
			Control_line = 24;

		else if(f3==5 && f7==48)
			Control_line = 25;			//sraw inst
		
		else if(f3==0 && f7==1 )
			Control_line = 26;				//mulw inst
		
		else if(f3==4 && f7==1 )
			Control_line = 27;					//divw inst
		
		else if(f3==7 && f7==1)
			Control_line = 19;			//remw inst
		
		else if(f3==7 && f7==1)
			Control_line = 20;			//remuw inst	
	}		
}

void extract_i()
{
	cout<<"rs1:"<<rs1(IR)<<endl;
	Ra = reg[rs1(IR)];
	cout<<"Ra:"<<Ra<<endl;
	imm = imm_11_0(IR);
	int temp = 0x00000800;
	temp = temp & imm;
	if(temp==0x800)
	{
		temp = 0xfffff000;
		imm = imm | temp;
	}
	Rd = rd(IR);
	int opc = opcode(IR);
	int f3 = func3(IR);

	if(opc==3)
	{	
		if(f3==0)
			Control_line = 28;			//lb inst
		
		else if(f3==1)
			Control_line = 29;			//lh inst		
		
		else if(f3==2)
			Control_line = 30;			//lw inst		
		
		else if(f3==3)
			Control_line = 31;			//ld inst		

		else if(f3==4)
			Control_line = 32;			//lbu inst		

		else if(f3==5)
			Control_line = 33;			//lhu inst		

		else if(f3==6)
			Control_line = 34;			//lwu inst	
	}

	else if(opc==19)
	{	
		if(f3==0)
			Control_line = 35;			//addi inst
		
		else if(f3==1)
			Control_line = 36;			//slli inst DOUBTFUL		

		else if(f3==2)
			Control_line = 37;		//slti inst		
		
		else if(f3==3)
			Control_line = 38;		//sltiu inst					
		
		else if(f3==4)
			Control_line = 39;			//xori inst		
		
		else if(f3==5)
		{		
			int f7 = func7(IR);
			if(f7==0)
				Control_line = 40;			//srli inst		DOUBTFULL			
		
			else if(f7==32)
				Control_line = 41;			//srai inst DOUBTFULL
		}
		
		else if(f3==6)
			Control_line = 42;			//ori	inst			
		
		else if(f3==7)
			Control_line = 43;		//andi inst			
	}

	else if(opc==27)
	{
		if(f3==0)
			Control_line = 44;		//addiw inst				
		
		else if(f3==1)
		{
			int f7 = func7(IR);
			if(f7==0)
				Control_line = 45;		//slliw inst
		}
		
		else if(f3==5)
		{
			int f7 = func7(IR);
			if(f7==0)
				Control_line = 46;			//srliw inst
	
			else if(f7==32)
				Control_line = 47;		//sraiw inst
		}
		
	}

	else if(opc==103)
	{
		if(f3==0)
			Control_line = 48;		//jalr inst
	}
}

void extract_s()
{
	Ra = reg[rs1(IR)];
	Rb = reg[rs2(IR)];
	imm = (imm_11_5(IR) * 32) + imm_4_0(IR) ;
	int temp = 0x00000800;
	temp = temp & imm;
	if(temp==0x800)
	{
		temp = 0xfffff000;
		imm = imm | temp;
	}
	int f3 = func3(IR);

	if(f3==0)
		Control_line = 49;		//sb inst

	else if(f3==1)
		Control_line = 50;  		//sh inst
	
	else if(f3==2)
		Control_line = 51;  		//sw inst	
}

void extract_sb()
{
	Ra = reg[rs1(IR)];
	Rb = reg[rs2(IR)];
	cout<<"Ra:"<<Ra<<" Rb:"<<Rb<<endl;
	unsigned int temp = imm_12_10_5(IR);
	imm = temp& 0b1000000;
	imm = imm * 32;
	temp = temp & 0b0111111;
	temp = temp * 16;
	imm = imm | temp;
	temp = imm_4_1_11(IR);
	temp = temp & 0b11110;
	temp = temp/2;
	imm = imm | temp;
	temp = imm_4_1_11(IR);
	temp = temp& 0b00001;
	temp = temp * 1024;
	imm = imm | temp;
	temp = 0x00000800;		// for negative immediate
	temp = temp & imm;
	if(temp==0x800)
	{
		temp = 0xfffff000;
		imm = imm | temp;
	}
	imm = imm << 1;
	cout<<"extracted imm:"<<imm<<endl;
	printf("extracted imm:%x\n",imm);

	int f3 = func3(IR);
	if(f3==0)
		Control_line = 52;		//beq
	
	else if(f3==1)
		Control_line = 53;		//bne
	
	else if(f3==4)
		Control_line = 54;		//blt
	
	else if(f3==5)
		Control_line = 55;		//bge
	
	else if(f3==6)
		Control_line = 56;		//bltu
	
	else if(f3==7)
		Control_line = 57;		//bgeu
}

void extract_u()
{
	imm = imm_31_12(IR);
	int temp = 0x00080000;
	temp = temp & imm;
	if(temp == 0x80000)
	{
		temp = 0xfff00000;
		imm = imm | temp;
	}
	Rd = rd(IR);
	int opc = opcode(IR);

	if(opc==23)
		Control_line = 58;		//auipc
	
	else if(opc == 55)
		Control_line = 59;		//lui 
}

void extract_uj()
{
	Rd = rd(IR);
	unsigned int temp = imm_20_10_1_11_19_12(IR);
	imm = temp & 0b00000000000011111111;
	imm = imm * 2048;
	temp = temp & 0b01111111111000000000;
	temp = temp/512;
	imm = imm | temp;
	temp = imm_20_10_1_11_19_12(IR);
	temp = temp & 0b00000000000100000000;
	temp = temp * 4;
	imm = imm | temp;
	temp = imm_20_10_1_11_19_12(IR);
	temp = temp & 0b10000000000000000000;
	imm = temp | imm;
	temp = 0x00080000;
	temp = temp & imm;
	if(temp == 0x80000)
	{
		temp = 0xfff00000;
		imm = imm | temp;
	}
	imm = imm << 1;

	Control_line = 60;		//jal
} 

void decode()
{
	int opc = opcode(IR);
	int f3 = func3(IR);
	if( opc==3 || opc==19 || opc==27 || opc == 103)
		extract_i();
	else if( opc==51 || opc==59)
		extract_r();
	else if(opc==35)			//In Reference sheet sd is given I Format.    ?????
		extract_s();
	else if(opc==99)
		extract_sb();
	else if(opc==23 || opc ==55)
		extract_u();
	else if(opc==111)
		extract_uj();
}

void execute()
{
	if(Control_line==1)
	{
		Rz = Ra + Rb;		//add
	}
	else if(Control_line==2)
	{
		Rz = Ra - Rb;		//sub
	}
	else if(Control_line==3)
	{
		Rz = Ra << Rb;      //sll
	}
	else if(Control_line==4)
	{
		Rz=(Ra<Rb)?1:0;		//slt
	}
	else if(Control_line==5)
	{
		unsigned int Ra_temp,Rb_temp;
		Ra_temp = Ra;
		Rb_temp = Rb;
		Rz=(Ra_temp<Rb_temp)?1:0;		//sltu
	}
	else if(Control_line==6)
	{
		Rz=Ra^Rb;		//xor
	}
	else if(Control_line==7)
	{
				Rz=(int)((unsigned int)Ra >> Rb);		//srl
	}
	else if(Control_line==8)
	{

		Rz=Ra>>Rb;		//sra
	}
	else if(Control_line==9)
	{
		Rz=Ra|Rb;		//or
	}
	else if(Control_line==10)
	{
		Rz=Ra&Rb;		//and
	}
	else if(Control_line==11)
	{
		Rz=Ra*Rb;		//mul
	}
	else if(Control_line==12)
	{
				//mulh
	}
	else if(Control_line==13)
	{
				//mulhsu
	}
	else if(Control_line==14)
	{
				//mulhu
	}
	else if(Control_line==15)
	{
		Rz = Ra/Rb;		//div
	}
	else if(Control_line==16)
	{
		unsigned int Ra_temp,Rb_temp;
		Ra_temp = Ra;
		Rb_temp = Rb;
		Rz = Ra_temp/Rb_temp;		//divu
	}
	else if(Control_line==17)
	{
		Rz = Ra%Rb;		//rem
	}
	else if(Control_line==18)
	{
		unsigned int Ra_temp,Rb_temp;
		Ra_temp = Ra;
		Rb_temp = Rb;		//remu
		Rz = Ra_temp%Rb_temp;
	}
	else if(Control_line==21)
	{
		Rz = Ra + Rb;		//addw
	}
	else if(Control_line==22)
	{
		Rz = Ra - Rb;		//subw
	}
	else if(Control_line==19)
	{
		Rz = Ra%Rb;		//remw
	}
	else if(Control_line==20)
	{
		unsigned int Ra_temp,Rb_temp;
		Ra_temp = Ra;
		Rb_temp = Rb;		
		Rz = Ra_temp%Rb_temp;		//remuw
	}
	else if(Control_line == 23)
	{
		Rz=Ra<<Rb;			//sllw inst	
	}	
	else if(Control_line == 24)
	{
				Rz=(int)((unsigned int)Ra >> Rb);		
			//srlw inst
	}
	else if(Control_line == 25)
	{
		Rz=Ra>>Rb;				//sraw inst
	}
	else if(Control_line == 26)
	{
		Rz = Ra * Rb;					//mulw inst
	}
	else if(Control_line == 27)
	{
		Rz = Ra/Rb;					//divw inst
	}
	else if(Control_line == 28)
	{
		Rz = Ra + imm;				//lb inst
	}
	else if(Control_line == 29)
	{
		Rz = Ra + imm;				//lh inst
	}
	else if(Control_line == 30)
	{
		Rz = Ra + imm;
cout << "Rz:"<<Rz << endl;				//lw inst
	}
	else if(Control_line==31)
	{
							//ld inst
	}
	else if(Control_line==32)
	{
		Rz=imm+Ra;		//lbu
	}
	else if(Control_line==33)
	{
		Rz=imm+Ra;	//lhu
	}
	else if(Control_line==34)
	{
		Rz=imm+Ra;	//lwu
	}
	else if(Control_line==35)
	{
		Rz= Ra + imm;
		cout<<"Rz:"<<Rz<<" Ra:"<<Ra<<" imm:"<<imm<<endl;			// addi
	}
	else if(Control_line==36)
	{
		Rz=Ra<<imm;				//slli
	}
	else if(Control_line==37)
	{
		Rz=(Ra<imm)?1:0;			//slti
	}
	else if(Control_line==38)
	{
		unsigned int Ra_temp = Ra;
		unsigned int imm_temp = imm & 0x00000fff;
		Rz=(Ra_temp<imm_temp)?1:0;			//sltiu
	}
	else if(Control_line==39)
	{
		Rz=Ra^imm;				//xori	
	}
	else if(Control_line==40)
	{
		Rz=(int)((unsigned int)Ra >> imm);				//srli inst		DOUBTFULL
	}
	else if(Control_line==41)
	{
		Rz=Ra>>imm;					//srai inst DOUBTFULL
	}
	else if(Control_line==42)
	{
		Rz=Ra|imm;				//ori
	}
	else if(Control_line==43)
	{
		Rz=Ra & imm;			//andi
	}
	else if(Control_line==44)
	{
		Rz=Ra + imm;			//addi
	}
	else if(Control_line==45)
	{
		Rz=(int)((unsigned int)Ra >> imm);			//srliw	
	}
	else if(Control_line==46)
	{
		Rz=Ra<<imm;				//slliw
	}
	else if(Control_line==47)
	{
		Rz=Ra>>imm;					//sraiw inst
	}
	else if(Control_line==48)
	{	
		Rz= Ra+imm;				//jalr inst
	}
	else if(Control_line==49)
	{
		Rz = Ra + imm;					
		Rm = Rb;					//sb inst
	}
	else if(Control_line==50)
	{
		Rz = Ra + imm;					
		Rm = Rb;					//sh inst
	}
	else if(Control_line==51)
	{
		Rz = Ra + imm;					//sw inst
		Rm = Rb;
		cout<<"Rz:"<<Rz<<" Ra:"<<Ra<<" imm:"<<imm<<endl;
	}
	else if(Control_line==52)
	{
		//cout<<"YES---------------\n";
		if(Ra==Rb)
		{
			Rz=1;
			//PC=PC+imm;
		}					//beq
		else
		{
			Rz=0;
			//PC=PC;
		}
	}
	else if(Control_line==53)
	{
		if(Ra!=Rb)
		{
			Rz=1;
			//PC=PC+imm;
		}					//bne
		else
		{
			Rz=0;
			//PC=PC;
		}
	}
	else if(Control_line==54)
	{
		if(Ra<Rb)
		{
			Rz=1;
			//PC=PC+imm;
		}					//blt
		else
		{
			Rz=0;
			//PC=PC;
		}
	}
	else if(Control_line==55)
	{
		if(Ra>=Rb)
		{
			Rz=1;
			//PC=PC+imm;
		}					//bge					
		else
		{
			Rz=0;
			//PC=PC;
		}
	}	
	else if(Control_line==56)
	{
		unsigned int Ra_temp = Ra, Rb_temp = Rb;
		if(Ra_temp<Rb_temp)
		{
			Rz=1;
			//PC=PC+imm;
		}					//bltu
		else
		{
			Rz=0;
			//PC=PC;
		}
	}	
	else if(Control_line==57)
	{
		unsigned int Ra_temp = Ra, Rb_temp = Rb;
		if(Ra_temp>=Rb_temp)
		{
			Rz=1;
			//PC=PC+imm;
		}					//bgeu					
		else
		{
			Rz=0;
			//PC=PC;
		}
	}
	else if(Control_line==58)
	{
		Rz = imm;
		cout <<"Rz:"<< Rz << endl;			//auipc
	}
	else if(Control_line==59)
	{
		Rz=imm<<12;					//lui
	}
	else if(Control_line==60)
	{
							//jal
	}
}

void memory_access()
{
    if(Control_line==28)
    {
        Ry=memory[Rz];
	int temp = 0x00000080;
	temp = temp & Ry;
	if(temp==0x80)
	{
		temp = 0xffffff00;
		Ry = Ry | temp;
	}
    }

    else if(Control_line==29)
    {
        //Ry=256*memory[Rz]+memory[Rz+1];
	/*int temp = 0x00008000;
	temp = temp & Ry;
	if(temp==0x8000)
	{
		temp = 0xffff0000;
		Ry = Ry | temp;
	}*/
    	int temp=read_word(memory,Rz);
    	Ry=temp & 0x0000ffff;
    	printf("Ry:%x\n",Ry);
    	int temp2 = 0x00008000;
	temp2 = temp2 & Ry;
	printf("temp2:%x\n",temp2);
	if(temp2==0x8000)
	{
		temp2 = 0xffff0000;
		Ry = Ry | temp2;
	}
	printf("this is Ry:%x\n",Ry);
	//cout<<"Ry:"<<Ry<<endl;

    }

	else if(Control_line==30)
    {
       //Ry=pow(256,3)*memory[Rz]+pow(256,2)*memory[Rz+1]+pow(256,1)*memory[Rz+2]+memory[Rz+3];
	Ry = read_word(memory,Rz); 
	cout << "read word Ry:"<<Ry << " " << endl;
    }

    /*else if(Control_line==31)
    {
        //Not to be used(requires 64 bit)
        //Ry=memory[Rz+7]+pow(256,1)*memory[Rz+6]+pow(256,2)*memory[Rz+5]+pow(256,3)*memory[Rz+4]+pow(256,4)*memory[Rz+3]+pow(256,5)*memory[Rz+2]+pow(256,6)*memory[Rz+1]+power(256,7)*memory[Rz];
    }*/

    else if(Control_line==32)
    {
        Ry=memory[Rz];
    }

    else if(Control_line==33)
    {
    	int temp=read_word(memory,Rz);
    	Ry=temp & 0x0000ffff;
        //Ry=256*memory[Rz]+memory[Rz+1];
    }

    else if(Control_line==34)
    {
       //Ry=pow(256,3)*memory[Rz]+pow(256,2)*memory[Rz+1]+pow(256,1)*memory[Rz+2]+memory[Rz+3];
	Ry = read_word(memory,Rz); 
    }

    else if(Control_line==49)
    {
        memory[Rz]=Rm;
    }

    else if(Control_line==50)
    {
        memory[Rz+1]=Rm%256;
        Rm_temp=Rm/256;
        memory[Rz]=Rm_temp%256;
    }

    else if(Control_line==51)
    {
        Rm_temp=Rm;
        /*memory[Rz+3]=Rm_temp%256;
        Rm_temp=Rm_temp/256;
        memory[Rz+2]=Rm_temp%256;
        Rm_temp=Rm_temp/256;
        memory[Rz+1]=Rm_temp%256;
        Rm_temp=Rm_temp/256;
        memory[Rz]=Rm_temp%256;*/
	write_word(memory,Rz,Rm);
    }

    else if(Control_line==48)
    {
    	temp_PC=PC;
    	PC=Rz;
    }

    else if(Control_line==60)
    {
    	temp_PC=PC;
    	PC -= 4;
    	PC+=imm;
    }

    else if(Control_line==52||Control_line==53||Control_line==54||Control_line==55||Control_line==56||Control_line==57)
    {
    	cout<<"Rz:"<<Rz<<endl;
    	if(Rz==1)
    	{
    		cout<<"PC jump"<<" imm:"<<imm<<endl;
   			PC -= 4;
    		PC+=imm;
    	}
    }

    else
    {
    	Ry=Rz;
    }
}

// Registry update
void writeback()
{
	if(Control_line==49||Control_line==50||Control_line==51||Control_line==52||Control_line==53||Control_line==54||Control_line==55||Control_line==56||Control_line==57)
	{
		return;
	}

	else if(Control_line==48||Control_line==60)
	{
		reg[Rd]=temp_PC;
	}
	else
	{
		reg[Rd] = Ry;
	}
	return;
}

int main()
{
	PC = 0x0;reg[2]=sp;
	load_program_memory();
	
	while(1)
	{
		fetch();
		if(IR == 0xEF000011)
			break;
		decode();
		//cout<<"Control_line:"<<Control_line<<endl;
		//cout << "Ra:" << Ra << " Rb:" << Rb << endl;
		execute();
		memory_access();
		writeback();
		cout << reg[10]<<" Ry:"<<Ry<<" PC:"<<PC<<" Rz:"<<Rz<<" Control:"<<Control_line<<"reg2: "<<reg[2]<<endl;
		clock_cycles++;	
	}
	write_data_memory();
	cout << "No. of clock cycles: " << clock_cycles << endl;
}

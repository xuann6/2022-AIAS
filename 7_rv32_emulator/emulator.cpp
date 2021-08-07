#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

#include "linenoise.hpp"

// 64 KB
#define MEM_BYTES 0x100000000//64k bytes memory
#define TEXT_OFFSET 0
#define DATA_OFFSET 8192*4*4

#define MAX_LABEL_COUNT 128
#define MAX_LABEL_LEN 32
#define MAX_SRC_LEN (1024*1024)

typedef struct {
	char* src;
	int offset;
} source;

bool streq(char* s, const char* q) {
	if ( strcmp(s,q) == 0 ) return true;
	//if equal return zero, streq==true
	return false;
}

uint32_t signextend(uint32_t in, int bits) {
	if ( in & (1<<(bits-1)) ) return ((-1)<<bits)|in;
	//if裡面確認最高位數，如果是1進行extend之後回傳
	//-1的二補數為全1，不影響in的情況下將前面全部補滿1
	return in;
	//如果不是1直接回傳即可
}

void print_syntax_error(int line, const char* msg) {
	printf( "Line %4d: Syntax error! %s\n", line, msg );
	exit(1);
}

void print_regfile(uint32_t rf[32]) {
	for ( int i = 0; i < 32; i++ ) {
		printf( "x%02d:0x%08x ", i, rf[i] );
		if ( (i+1) % 8 == 0 ) printf( "\n" );
	}
}


typedef enum {
	UNIMPL = 0,

//******
//add operation DIV  MUL
	DIV, 
	MUL, 
	REM, 
	REMU, 

//add operation ANDN  BCLR  BEXT
//**********
	ANDN, 
	BCLR, 
	BCLRI, 
	BEXT, 
//**********

	ADD,
	ADDI,
	AND,
	ANDI,
	AUIPC,
	BEQ,
	BGE,
	BGEU,
	BLT,
	BLTU,
	BNE,
	JAL,
	JALR,
	LB,
	LBU,
	LH,
	LHU,
	LUI,
	LW,
	OR,
	ORI,
	SB,
	SH,
	SLL,
	SLLI,
	SLT,
	SLTI,
	SLTIU,
	SLTU,
	SRA,
	SRAI,
	SRL,
	SRLI,
	SUB,
	SW,
	XOR,
	XORI,
	SYSCALL,
	HCF
} instr_type;

instr_type parse_instr(char* tok) {

	//******
	//add operation : MUL  DIV
	if ( streq(tok , "mul")) return MUL;
	if ( streq(tok , "div")) return DIV;
	if ( streq(tok , "rem")) return REM;
	if ( streq(tok , "remu")) return REMU;



	//add operation : ANDN  BCLR  BCLRI  BEXT
	if ( streq(tok  ,"andn")) return ANDN;
	if ( streq(tok  ,"bclr")) return BCLR;
	if ( streq(tok  ,"bclri")) return BCLRI;
	if ( streq(tok  ,"bext")) return BEXT;

	//******


	// 2r->1r
	if ( streq(tok, "add") ) return ADD;
	if ( streq(tok, "sub") ) return SUB;
	if ( streq(tok, "slt") ) return SLT;
	if ( streq(tok, "sltu") ) return SLTU;
	if ( streq(tok, "and") ) return AND;
	if ( streq(tok, "or") ) return OR;
	if ( streq(tok, "xor") ) return XOR;
	if ( streq(tok, "sll") ) return SLL;
	if ( streq(tok, "srl") ) return SRL;
	if ( streq(tok, "sra") ) return SRA;

	// 1r, imm -> 1r
	if ( streq(tok, "addi") ) return ADDI;
	if ( streq(tok, "slti") ) return SLTI;
	if ( streq(tok, "sltiu") ) return SLTIU;
	if ( streq(tok, "andi") ) return ANDI;
	if ( streq(tok, "ori") ) return ORI;
	if ( streq(tok, "xori") ) return XORI;
	if ( streq(tok, "slli") ) return SLLI;
	if ( streq(tok, "srli") ) return SRLI;
	if ( streq(tok, "srai") ) return SRAI;

	//load/store
	if ( streq(tok, "lb") ) return LB;
	if ( streq(tok, "lbu") ) return LBU;
	if ( streq(tok, "lh") ) return LH;
	if ( streq(tok, "lhu") ) return LHU;
	if ( streq(tok, "lw") ) return LW;
	if ( streq(tok, "sb") ) return SB;
	if ( streq(tok, "sh") ) return SH;
	if ( streq(tok, "sw") ) return SW;

	//branch
	if ( streq(tok, "beq") ) return BEQ;
	if ( streq(tok, "bge") ) return BGE;
	if ( streq(tok, "bgeu") ) return BGEU;
	if ( streq(tok, "blt") ) return BLT;
	if ( streq(tok, "bltu") ) return BLTU;
	if ( streq(tok, "bne") ) return BNE;

	// jal
	if ( streq(tok, "jal") ) return JAL;
	if ( streq(tok, "jalr") ) return JALR;

	// lui
	if ( streq(tok, "auipc") ) return AUIPC;
	if ( streq(tok, "lui") ) return LUI;

	// unimpl
	//if ( streq(tok, "unimpl") ) return UNIMPL;
	if ( streq(tok, "syscall") ) return SYSCALL;	
	if ( streq(tok, "hcf") ) return HCF;
	return UNIMPL;
}

int parse_reg(char* tok, int line, bool strict = true) {
	//reg x01 = 
	if ( tok[0] == 'x' ) {
		int ri = atoi(tok+1);
		//turn (pointer+1)(neglecting char "x") into integer
		//in order to translate register form "x01" "x02", etc.
		if ( ri < 0 || ri > 32 ) {
			if ( strict ) print_syntax_error(line, "Malformed register name");
			return -1;
		}
		return ri;
	}
	if ( streq(tok, "zero") ) return 0; 
	if ( streq(tok, "ra") ) return 1;
	if ( streq(tok, "sp") ) return 2;
	if ( streq(tok, "gp") ) return 3;
	if ( streq(tok, "tp") ) return 4;
	if ( streq(tok, "t0") ) return 5;
	if ( streq(tok, "t1") ) return 6;
	if ( streq(tok, "t2") ) return 7;
	if ( streq(tok, "s0") ) return 8;
	if ( streq(tok, "s1") ) return 9;
	if ( streq(tok, "a0") ) return 10;
	if ( streq(tok, "a1") ) return 11;
	if ( streq(tok, "a2") ) return 12;
	if ( streq(tok, "a3") ) return 13;
	if ( streq(tok, "a4") ) return 14;
	if ( streq(tok, "a5") ) return 15;
	if ( streq(tok, "a6") ) return 16;
	if ( streq(tok, "a7") ) return 17;
	if ( streq(tok, "s2") ) return 18;
	if ( streq(tok, "s3") ) return 19;
	if ( streq(tok, "s4") ) return 20;
	if ( streq(tok, "s5") ) return 21;
	if ( streq(tok, "s6") ) return 22;
	if ( streq(tok, "s7") ) return 23;
	if ( streq(tok, "s8") ) return 24;
	if ( streq(tok, "s9") ) return 25;
	if ( streq(tok, "s10") ) return 26;
	if ( streq(tok, "s11") ) return 27;
	if ( streq(tok, "t3") ) return 28;
	if ( streq(tok, "t4") ) return 29;
	if ( streq(tok, "t5") ) return 30;
	if ( streq(tok, "t6") ) return 31;
	//if register form isn't "x01" "x02"...
	//then return relative int value  

	if ( strict ) print_syntax_error(line, "Malformed register name");
	//out od default reg range(32), return malformed error
	//strict boolean value always 1
	return -1;//function failed
}

uint32_t parse_imm(char* tok, int bits, int line, bool strict = true) {
	//表示第一個char可以是0-9 or 負號
	//輸入範圍內的正負數，只要是數字都不會是malformed value
	if ( !(tok[0]>='0'&&tok[0]<='9') && tok[0] != '-' && strict) {
		print_syntax_error(line, "Malformed immediate value" );
	}

	//確認是可以轉換成long int的
	//進行str->long int(32 bits)轉換
	long int imml = strtol(tok, NULL, 0);//base zero 採用10進制

	//imm out of range
	if (imml > ((1<<bits)-1) || imml < -(1<<(bits-1)) ) {
		printf( "Syntax error at token %s\n", tok);
		exit(1);
	}
	uint64_t uv = *(uint64_t*)&imml;//轉成64 bits
	uint32_t hv = (uv&UINT32_MAX);  //和0xFFFFFFFF做bit-wise and

	return hv;
}

void parse_mem(char* tok, int* reg, uint32_t* imm, int bits, int line) {
	char* imms = strtok(tok, "(");
	char* regs = strtok(NULL, ")");
	//?這樣分割表示mem的形式是1000(x2)之類的?
	
	*imm = parse_imm(imms, bits, line);
	*reg = parse_reg(regs, line);
}


void mem_write(uint8_t* mem, uint32_t addr, uint32_t data, instr_type op) {
	//printf( "Storing %x to %d\n", data, addr );
	int bytes = 0;
	//sw sh sb rs2, rs1, simm12 , mem addr= rs1+simm12
	//sw 32 bits 全寫入
	//sh lower 16 bits
	//sb lowest 8 bits
	switch (op ) {
		case SB: bytes = 1; break;//1 bytes
		case SH: bytes = 2; break;//2 bytes
		case SW: bytes = 4; break;//4 bytes
	}
	if ( addr < MEM_BYTES && addr + bytes <= MEM_BYTES ) {
		switch (op ) {
			case SB: mem[addr] = *(uint8_t*)&(data); break;
			//***********************************************************************
			case SH: *(uint16_t*)&(mem[addr]) = *(uint16_t*)&(data); break;
			//***********************************************************************
			case SW: 
				*(uint32_t*)&(mem[addr]) = data;
				//printf( "Writing %x to addr %x\n", rf[i.a1.reg], rf[i.a2.reg]+i.a3.imm );
			break;
			//***********************************************************************
		}
	} else if ( addr == MEM_BYTES ) {
		printf( "[System output]: 0x%x\n", data );
	} else {
		printf( "0x%x -- 0x%x\n", addr, data);
	}
}
uint32_t mem_read(uint8_t* mem, uint32_t addr, instr_type op) {
	uint32_t ret = 0;;
	int bytes = 0;

	switch(op) {
		//load_op rd, rs1, simm12 
		case LB: 
		case LBU: bytes = 1; break;
		case LH:
		case LHU: bytes = 2; break;
		case LW:  bytes = 4; break;
	}
	if ( addr + bytes <= MEM_BYTES ) {
		switch(op) {
			//因為是load指令所以要注意讀出資料為32 bits，如果是有號數需作signextension
			case LB: ret = signextend(mem[addr], 8); break;
			case LBU: ret = mem[addr]; break;
			case LH: {
				int16_t rv = *(int16_t*)&(mem[addr]); //先轉型成16 bits 
				//***********************************************************************
				int32_t rvv = (int32_t)rv;//為甚麼需要分成兩次轉型，不能直接8->16 bits?
				//***********************************************************************
				ret = *(uint32_t*)&rvv;
				break;
				}
			case LHU: {
				uint16_t rv = *(int16_t*)&(mem[addr]); 
				ret = (uint32_t)rv;
				break;
				}
			case LW: 
				ret = *(uint32_t*)&(mem[addr]); 
				//printf( "Reading %x from addr %x\n", rf[i.a1.reg], rf[i.a2.reg]+i.a3.imm );
			break;
		}
	}
	//printf( "Reading %x from %d\n", ret, addr );

	return ret;
}

typedef enum {
	OPTYPE_NONE, // more like "don't care"
	OPTYPE_REG,
	OPTYPE_IMM,
	OPTYPE_LABEL,
} operand_type;
typedef struct {
	operand_type type = OPTYPE_NONE;
	char label[MAX_LABEL_LEN];
	int reg;
	uint32_t imm;

} operand;
typedef struct {
	instr_type op;
	operand a1;
	operand a2;
	operand a3;
	char* psrc = NULL;
	int orig_line=-1;
	bool breakpoint = false;
} instr;

void append_source(const char* op, const char* a1, const char* a2, const char* a3, source* src, instr* i) {
	char tbuf[128]; // not safe... static size... but should be okay since label length enforced
	if ( op && a1 && !a2 && !a3 ) {
		sprintf(tbuf, "%s %s", op, a1);
	} else if ( op && a1 && a2 && !a3 ) {
		sprintf(tbuf, "%s %s, %s", op, a1, a2);
	} else if ( op && a1 && a2 && a3 ) {
		sprintf(tbuf, "%s %s, %s, %s", op, a1, a2, a3);
	} else {
		return;
	}

	//根據.txt 所提供的指令print在螢幕上同時存入tbuf中供等等使用
	int slen = strlen(tbuf);

	//基本上src->offset 和(*src).offset 結果會一樣
	//作為運算子或者是overload時除外
	if ( slen + src->offset < MAX_SRC_LEN ) {
		//***********************************************************************
		strncpy(src->src+src->offset, tbuf, strlen(tbuf));
		//***********************************************************************

		//將tbuf中長度為strlen(tbuf)的部分copy
		//?? line 342, src->src+src->offset ?? 

		//i->psrc也就是(*i).psrc

		i->psrc = src->src+src->offset;
		src->offset += slen+1;
	}
}


typedef struct {
	char label[MAX_LABEL_LEN];
	int loc = -1;
} label_loc;

uint32_t label_addr(char* label , label_loc* labels, int label_count, int orig_line) {
	for ( int i = 0; i < label_count; i++ ) {
		//***********************************************************************
		if (streq(labels[i].label, label)) return labels[i].loc;
		//***********************************************************************
		//label_loc*可是還是用dot的表示方法，到底怎麼區分
	}
	print_syntax_error(orig_line, "Undefined label" );
}



//typedef enum {SECTION_NONE, SECTION_TEXT, SECTION_DATA} sectionType;
int parse_data_element(int line, int size, uint8_t* mem, int offset) {
	while (char* t = strtok(NULL, " \t\r\n") ) {
		errno = 0;
		//errno，在<errno.h>中記錄錯誤代碼，不同的錯誤會有不同的代碼
		long int v = strtol(t, NULL, 0);
		if ( errno == ERANGE || (v>>(size*8)) ) {
			//將v右移且最高位補０，可以知道v是否超過最高位數（八位）
			printf( "Value out of bounds at line %d : %s\n", line, t);
			exit(2);
		}
		memcpy(&mem[offset], &v, size);
		offset += size;
		//strtok(NULL, ",");
	}
	return offset;
}
int parse_assembler_directive(int line, char* ftok, uint8_t* mem, int memoff) {
	//printf( "assembler directive %s\n", ftok );
	if ( 0 == memcmp(ftok, ".text", strlen(ftok) ) ) {
		//等號成立如果ftok和.text的strlen(ftok)個char比較結果都一樣的話
		if (strtok(NULL, " \t\r\n") ) {
			print_syntax_error(line, "Tokens after assembler directive");
		}
		//cur_section = SECTION_TEXT;
		memoff = TEXT_OFFSET;
		//printf( "starting text section\n" );
	} else if ( 0 == memcmp(ftok, ".data", strlen(ftok) ) ) {
		//cur_section = SECTION_TEXT;
		memoff = DATA_OFFSET;
		//printf( "starting data section\n" );
	} else if ( 0 == memcmp(ftok, ".byte", strlen(ftok)) ) memoff = parse_data_element(line, 1, mem, memoff);
	else if ( 0 == memcmp(ftok, ".half", strlen(ftok)) ) memoff = parse_data_element(line, 2, mem, memoff);
	else if ( 0 == memcmp(ftok, ".word", strlen(ftok)) ) memoff = parse_data_element(line, 4, mem, memoff);
	else {
		printf( "Undefined assembler directive at line %d: %s\n", line, ftok );
		exit(3);
	}
	return memoff;
}

int parse_pseudoinstructions(int line, char* ftok, instr* imem, int ioff, label_loc* labels, char* o1, char* o2, char* o3, char* o4, source* src) {
	//pseudoinstruction會用其他基本指令cover他的功能
	//回傳int 表示當前的pseudoinstruction 動作需要多少個instruction 去實現

	if ( streq(ftok, "li" ) ) {
	//"load immediate" instruction
	//可以load 32 bits immediate
	//可以利用lui+ori達到一樣效果
	//??這裡是利用lui+addi達成??待確認
		if ( !o1 || !o2 || o3 ) print_syntax_error(line, "Invalid format");

		int reg = parse_reg(o1, line);
		long int imml = strtol(o2, NULL, 0);


		if (reg < 0 || imml > UINT32_MAX || imml < INT32_MIN ) {
			printf( "Syntax error at line %d -- %lx, %x\n", line, imml, INT32_MAX);
			exit(1);
		}
		uint64_t uv = *(uint64_t*)&imml;
		uint32_t hv = (uv&UINT32_MAX);

		char areg[4]; sprintf(areg, "x%02d", reg);
		char immu[12]; sprintf(immu, "0x%08x" , (hv>>12));
		char immd[12]; sprintf(immd, "0x%08x" , (hv&((1<<12)-1)));


		instr* i = &imem[ioff];
		i->op = LUI;
		i->a1.type = OPTYPE_REG; i->a1.reg = reg;
		i->a2.type = OPTYPE_IMM; i->a2.imm = hv>>12;
		i->orig_line = line;
		append_source("lui", areg, immu, NULL, src, i);
		instr* i2 = &imem[ioff+1];

		i2->op = ADDI;
		i2->a1.type = OPTYPE_REG; i2->a1.reg = reg;
		i2->a2.type = OPTYPE_REG; i2->a2.reg = reg;
		i2->a3.type = OPTYPE_IMM; i2->a3.imm = (hv&((1<<12)-1)); 
		i2->orig_line = line;
		append_source("addi", areg, areg, immd, src, i2);
		//printf( ">> %d %x %d\n", reg, i->a2.imm, i->a2.imm );
		//printf( ">> %d %x %d\n", reg, i2->a3.imm, i2->a3.imm );
		return 2;
	}
	if ( streq(ftok, "la") ) {
	//"load address" instruction
	//"la" 也可以用來load標籤
		if ( !o1 || !o2 || o3 ) print_syntax_error(line, "Invalid format");

		int reg = parse_reg(o1, line);

		instr* i = &imem[ioff];
		i->op = LUI;
		i->a1.type = OPTYPE_REG; i->a1.reg = reg;
		i->a2.type = OPTYPE_LABEL; strncpy(i->a2.label, o2, MAX_LABEL_LEN);
		i->orig_line = line;
		//append_source(ftok, o1, o2, o3, src, i); // done in normalize
		instr* i2 = &imem[ioff+1];
		i2->op = ADDI;
		i2->a1.type = OPTYPE_REG; i2->a1.reg = reg;
		i2->a2.type = OPTYPE_REG; i2->a2.reg = reg;
		i2->a3.type = OPTYPE_LABEL; strncpy(i2->a3.label, o2, MAX_LABEL_LEN);
		i2->orig_line = line;
		//append_source(ftok, o1, o2, o3, src, i2); // done in normalize
		return 2;
	}
	if ( streq(ftok, "ret" )) {
		if ( o1 ) print_syntax_error(line, "Invalid format");

		instr* i = &imem[ioff];
		i->op = JALR;
		i->a1.type = OPTYPE_REG; i->a1.reg = 0;
		i->a2.type = OPTYPE_REG; i->a2.reg = 1;
		i->a3.type = OPTYPE_IMM; i->a3.imm = 0;
		i->orig_line = line;
		append_source("jalr", "x0", "x1", "x0", src, i);
		return 1;
	}
	if ( streq(ftok, "j" )) {
		if ( !o1 || o2 ) print_syntax_error(line, "Invalid format");

		instr* i = &imem[ioff];
		i->op = JAL;
		i->a1.type = OPTYPE_REG; i->a1.reg = 0;
		i->a2.type = OPTYPE_LABEL; strncpy(i->a2.label, o1, MAX_LABEL_LEN);
		i->orig_line = line;
		append_source("j", "x0", o1, NULL, src, i);
		return 1;
	}
	if ( streq(ftok, "mv" )) {
		if ( !o1 || !o2 || o3 ) print_syntax_error(line, "Invalid format");
		instr* i = &imem[ioff];
		i->op = ADDI;
		i->a1.type = OPTYPE_REG; i->a1.reg = parse_reg(o1, line);
		i->a2.type = OPTYPE_REG; i->a2.reg = parse_reg(o2, line);
		i->a3.type = OPTYPE_IMM; i->a3.imm = 0;
		i->orig_line = line;
		append_source("addi",o1, o2, NULL, src, i);
		return 1;
	}
	if ( streq(ftok, "bnez" )) {
	//"branch not equal zero"
	//example : bnez $t0, Loop，如果站存說$t0不等於0->跳到Loop 
		if ( !o1 || !o2 || o3 ) print_syntax_error(line, "Invalid format");
		instr* i = &imem[ioff];
		i->op = BNE;
		i->a1.type = OPTYPE_REG; i->a1.reg = parse_reg(o1, line);
		i->a2.type = OPTYPE_REG; i->a2.reg = 0;
		i->a3.type = OPTYPE_LABEL; strncpy(i->a3.label, o2, MAX_LABEL_LEN);
		i->orig_line = line;
		append_source("bne", "x0", o1, o2, src, i);
		return 1;
	}
	if ( streq(ftok, "beqz" )) {
	//"branch equal zero"
	//example : beqz $t0, Loop，如果站存說$t0等於0->跳到Loop 
		if ( !o1 || !o2 || o3 ) print_syntax_error(line, "Invalid format");
		instr* i = &imem[ioff];
		i->op = BEQ;
		i->a1.type = OPTYPE_REG; i->a1.reg = parse_reg(o1, line);
		i->a2.type = OPTYPE_REG; i->a2.reg = 0;
		i->a3.type = OPTYPE_LABEL; strncpy(i->a3.label, o2, MAX_LABEL_LEN);
		i->orig_line = line;
		append_source("beq", "x0", o1, o2, src, i);
		return 1;
	}
	return 0;
}

int parse_instr(int line, char* ftok, instr* imem, int memoff, label_loc* labels, source* src) {
	if ( memoff +4 > DATA_OFFSET ) {
		printf( "Instructions in data segment!\n" );
		//超出instruction memory 的範圍
		exit(1);
	}
	char* o1 = strtok(NULL, " \t\r\n,");
	char* o2 = strtok(NULL, " \t\r\n,");
	char* o3 = strtok(NULL, " \t\r\n,");
	char* o4 = strtok(NULL, " \t\r\n,");

	int ioff = memoff/4;
	int pscnt = parse_pseudoinstructions(line, ftok, imem, ioff, labels, o1, o2, o3, o4, src);
	if ( pscnt > 0 ) {
		return pscnt;//表示instruction 為pseudoinstruction 
	} else {
		//instruction 不是pseudoinstruction 所以要根據不同的instruction 去parse
		instr* i = &imem[ioff];
		instr_type op = parse_instr(ftok);
		i->op = op;
		i->orig_line = line;
		append_source(ftok, o1, o2, o3, src, i);

		switch( op ) {
			case UNIMPL: return 1;
			case JAL:
				if ( o2 ) { // two operands, reg, label
					if ( !o1 || !o2 || o3 || o4 ) print_syntax_error( line, "Invalid format" );
					i->a1.type = OPTYPE_REG; i->a1.reg = parse_reg(o1, line);
					i->a2.type = OPTYPE_LABEL; strncpy(i->a2.label, o2, MAX_LABEL_LEN);
				} else { // one operand, label
					if ( !o1 || o2 || o3 || o4 ) print_syntax_error( line, "Invalid format" );

					i->a1.type = OPTYPE_REG; i->a1.reg = 1;
					i->a2.type = OPTYPE_LABEL; strncpy(i->a2.label, o1, MAX_LABEL_LEN);
				}
				return 1;
			case JALR:
				if ( !o1 || !o2 || o3 || o4 ) print_syntax_error( line, "Invalid format" );
				i->a1.reg = parse_reg(o1, line);
				parse_mem(o2, &i->a2.reg, &i->a3.imm, 12, line);
				return 1;






			//***************
			//adding DIV  MUL
			case REM: case REMU:
				if ( !o1 || !o2 || !o3 || o4 ) print_syntax_error( line,  "Invalid format" );
				i->a1.reg = parse_reg(o1 , line);
				i->a2.reg = parse_reg(o2 , line);
				i->a3.reg = parse_reg(o3 , line);
			return 1;



			case DIV: case MUL:
				if ( !o1 || !o2 || !o3 || o4 ) print_syntax_error( line,  "Invalid format" );
				i->a1.reg = parse_reg(o1 , line);
				i->a2.reg = parse_reg(o2 , line);
				i->a3.reg = parse_reg(o3 , line);
				return 1;

			//adding ANDN  BCLR  BCLRI  BEXT operation
			case ANDN: case BCLR: case BEXT:
				if ( !o1 || !o2 || !o3 || o4 ) print_syntax_error( line,  "Invalid format" );
				i->a1.reg = parse_reg(o1 , line);
				i->a2.reg = parse_reg(o2 , line);
				i->a3.reg = parse_reg(o3 , line);
				return 1;
			case BCLRI:
				if ( !o1 || !o2 || !o3 || o4 ) print_syntax_error( line,  "Invalid format" );
				i->a1.reg = parse_reg(o1 , line);
				i->a2.reg = parse_reg(o2 , line);
				i->a3.imm = signextend(parse_imm(o3 , 5 , line),  5);
				//instruction BCLRI's format is equal to ANDI/XORI/ORI
				return 1;




			//******
			//******





			case ADD: case SUB: case SLT: case SLTU: case AND: case OR: case XOR: case SLL: case SRL: case SRA:
				if ( !o1 || !o2 || !o3 || o4 ) print_syntax_error( line, "Invalid format" );
				i->a1.reg = parse_reg(o1, line);
				i->a2.reg = parse_reg(o2, line);
				i->a3.reg = parse_reg(o3, line);
				return 1;
			case LB: case LBU: case LH: case LHU: case LW: case SB: case SH: case SW:
				if ( !o1 || !o2 || o3 || o4 ) print_syntax_error( line, "Invalid format" );
				i->a1.reg = parse_reg(o1, line);
				parse_mem(o2, &i->a2.reg, &i->a3.imm, 12, line);
				return 1;
			case ADDI: case SLTI: case SLTIU: case ANDI: case ORI: case XORI: case SLLI: case SRLI: case SRAI:
				if ( !o1 || !o2 || !o3 || o4 ) print_syntax_error( line, "Invalid format" );

				i->a1.reg = parse_reg(o1, line);
				i->a2.reg = parse_reg(o2, line);
				i->a3.imm = signextend(parse_imm(o3, 12, line), 12);
				return 1;
			case BEQ: case BGE: case BGEU: case BLT: case BLTU: case BNE:
				if ( !o1 || !o2 || !o3 || o4 ) print_syntax_error( line, "Invalid format" );
				i->a1.reg = parse_reg(o1, line);
				i->a2.reg = parse_reg(o2, line);
				i->a3.type = OPTYPE_LABEL; strncpy(i->a3.label, o3, MAX_LABEL_LEN);
				return 1;
			case LUI: 
			case AUIPC: // how to deal with LSB correctly? FIXME
				if ( !o1 || !o2 || o3 || o4 ) print_syntax_error( line, "Invalid format" );
				i->a1.reg = parse_reg(o1, line);
				i->a2.imm = (parse_imm(o2, 20, line));
				return 1;
			case HCF:case SYSCALL: return 1;
		}

	}
	return 1;
}

void parse(FILE* fin, uint8_t* mem, instr* imem, int& memoff, label_loc* labels, int& label_count, source* src) {
	int line = 0;

	printf( "Parsing input file\n" );

	//sectionType cur_section = SECTION_NONE;

	char rbuf[1024];
	while(!feof(fin)) {
		//feof, 用來檢測stream上的結束標誌。判斷是不是讀到了結尾
		if ( !fgets(rbuf, 1024, fin) ) break;
		for (char* p = rbuf; *p; ++p) *p = tolower(*p);
		line++;

		char* ftok = strtok(rbuf, " \t\r\n");
		if ( !ftok ) continue;

		if ( ftok[0] == '#' ) continue;
		if ( ftok[0] == '.' ) {
			memoff = parse_assembler_directive(line, ftok, mem, memoff);
		} else if ( ftok[strlen(ftok)-1] == ':' ) {
			ftok[strlen(ftok)-1] = 0;
			if ( strlen(ftok) >= MAX_LABEL_LEN ) {
				printf( "Exceeded maximum length of label: %s\n", ftok );
				exit(3);
			}
			if ( label_count >= MAX_LABEL_COUNT ) {
				printf( "Exceeded maximum number of supported labels" );
				exit(3);
			}
			strncpy(labels[label_count].label, ftok, MAX_LABEL_LEN);
			labels[label_count].loc = memoff;
			label_count++;
			//printf( "Parsing label %s at mem %x\n", ftok, memoff );

			char* ntok = strtok(NULL, " \t\r\n");
			// there is more code after label
			if ( ntok ) {
				if ( ntok[0] == '.' ) {
					memoff = parse_assembler_directive(line, ntok, mem, memoff);
				} else {
					int count = parse_instr(line, ntok, imem, memoff, labels, src);
					for ( int i = 0; i < count; i++ ) *(uint32_t*)&mem[memoff+(i*4)] = 0xcccccccc;
					memoff += count*4;
				}
			}
		}
		else {
			int count = parse_instr(line, ftok, imem, memoff, labels, src);
			for ( int i = 0; i < count; i++ ) *(uint32_t*)&mem[memoff+(i*4)] = 0xcccccccc;
			memoff += count*4;
		}
	}
}

void execute(uint8_t* mem, instr* imem, label_loc* labels, int label_count, bool start_immediate) {
	uint32_t rf[32];
	uint32_t rf_mirror[32];
	uint32_t pc = 0;
	uint32_t inst_cnt = 0;
	for ( int i = 0; i < 32; i++ ) {
		rf[i] = 0;
		rf_mirror[i] = 0;
	}
	//clean up register file

	bool stepping = !start_immediate;
	int stepcnt = 0;
	char keybuf[128];
	char* kbp = keybuf;
	//key buffer pointer


	bool dexit = false;
	while(!dexit) {
		uint32_t iid = pc/4;
		instr i = imem[iid];
		inst_cnt ++;

		if ( stepping || i.breakpoint ) {
			if ( stepcnt > 0 ) {
				stepcnt -= 1;
			} 

			if ( stepcnt == 0 || i.breakpoint ) {
				stepping = true;
				printf( "\n" );
				if ( i.psrc ) printf( "Next: %s\n", i.psrc );
				while (true) {
					printf( "[inst: %6d pc: %6d, src line %4d]\n", inst_cnt, pc, i.orig_line );

					std::string linebuf;
					fflush(stdout);
					linenoise::Readline(">>", linebuf);
					memcpy(keybuf, linebuf.c_str(), 128);
					linenoise::AddHistory(linebuf.c_str());
					//while ((kbp = linenoise?::Readline(">>")) == NULL);
					//fgets(keybuf, 128, stdin);


					for ( int i = 0; i < strlen(keybuf); i++ ) if (keybuf[i] == '\n') keybuf[i] = '\0';

					if ( keybuf[0] == 'q' ) {
						printf( "Quit command input! Exiting...\n" );
						exit(0);
					}
					if ( keybuf[0] == 'c' ) {
						stepping = false;
						break;
					}
					if ( strlen(keybuf) == 0 ) {
						break;
					}
					if ( keybuf[0] == 's' ) {
						stepcnt = parse_imm(keybuf+1, 16, 0, false);
						break;
					}
					if ( keybuf[0] == 'b' ) { // todo breakpoint!
						uint32_t break_line = parse_imm(keybuf+1, 16, 0, false);
						if ( strlen(keybuf+1) == 0 ) {
							for ( int i = 0; i < DATA_OFFSET/4; i++ ) {
								if ( imem[i].breakpoint ) printf( "Break at line %d: %s\n", imem[i].orig_line, imem[i].psrc );
							}
						} else {
							for ( int i = 0; i < DATA_OFFSET/4; i++ ) {
								if ( imem[i].orig_line >= break_line ) {
									printf( "Break point added to line %d\n", break_line );
									imem[i].breakpoint = true;
									break;
								}
							}
						}
					}
					if ( keybuf[0] == 'B' ) { // breakpoint remv
						uint32_t break_line = parse_imm(keybuf+1, 16, 0, false);
						for ( int i = 0; i < DATA_OFFSET/4; i++ ) {
							if ( imem[i].orig_line == break_line && imem[i].breakpoint ) {
								printf( "Break point remvd from line %d\n", break_line );
								imem[i].breakpoint = false;
								break;
							}
						}
					}
					if ( keybuf[0] == 'r' ) {
						int reg = parse_reg(keybuf+1, 0, false);
						if ( reg >= 0 ) printf( "rf[%2d] = 0x%x\n", reg, rf[reg] );
						if ( reg < 0 ) print_regfile(rf);
					}
					if ( keybuf[0] == 'm' ) {
						uint32_t addr = parse_imm(keybuf+1, 31, 0, false); // just for simplicity
						int cnt = 1;
						char* ftok = strtok(keybuf, " \t\r\n");
						ftok = strtok(NULL, " \t\r\n");
						if ( ftok ) {
							cnt = parse_imm(ftok, 16, 0, false);
						}

						for ( int w = 0; w < cnt; w++ ) {
							printf( "0x%04x: ", addr+(w*4) );
							for ( int i = 0; i < 4; i++ ) {
								printf( "%02x ", mem[addr+(w*4)+i] );
							}
							printf( "\n" );
						}
					}
					if ( keybuf[0] == 'l' ) {
						printf( "Listing compiled isntructions\n" );
						printf( " srcline : Compiled instruction\n" );
						for ( int i = 0; i < DATA_OFFSET/4; i++ ) {
							instr* ii = &imem[i];
							if ( ii->orig_line >= 0 && ii->psrc ) {
								printf( "%9d: %s\n", ii->orig_line, ii->psrc );
							}
						}
					}
				}
			}
		}
		
		int pc_next = pc + 4;
		switch (i.op) {



			//add operation : DIV  MUL

			case DIV: rf[i.a1.reg] = rf[i.a2.reg] / rf[i.a3.reg]; break;
			case MUL: rf[i.a1.reg] = rf[i.a2.reg] * rf[i.a3.reg]; break;
			case REM: rf[i.a1.reg] = rf[i.a2.reg] * rf[i.a3.reg]; break;
			case REMU:rf[i.a1.reg] = rf[i.a2.reg] * rf[i.a3.reg]; break;

			//add operation : ANDN  BCLR  BCLRI  BEXT

			case ANDN: rf[i.a1.reg] = rf[i.a2.reg] & (~rf[i.a3.reg]); break;
			case BCLR: rf[i.a1.reg] = rf[i.a2.reg] & ~(1 << rf[i.a3.reg]); break;
			case BCLRI: rf[i.a1.reg] = rf[i.a2.reg] & ~(1 << i.a3.imm); break;
			case BEXT: rf[i.a1.reg] = (rf[i.a2.reg] >> rf[i.a3.reg]) & 1;break;

			//**********



			case ADD: rf[i.a1.reg] = rf[i.a2.reg] + rf[i.a3.reg]; break;
			case SUB: rf[i.a1.reg] = rf[i.a2.reg] - rf[i.a3.reg]; break;
			case SLT: rf[i.a1.reg] = (*(int32_t*)&rf[i.a2.reg]) < (*(int32_t*)&rf[i.a3.reg]) ? 1 : 0; break;
			case SLTU: rf[i.a1.reg] = rf[i.a2.reg] + rf[i.a3.reg]; break;
			case AND: rf[i.a1.reg] = rf[i.a2.reg] & rf[i.a3.reg]; break;
			case OR: rf[i.a1.reg] = rf[i.a2.reg] | rf[i.a3.reg]; break;
			case XOR: rf[i.a1.reg] = rf[i.a2.reg] ^ rf[i.a3.reg]; break;
			case SLL: rf[i.a1.reg] = rf[i.a2.reg] << rf[i.a3.reg]; break;
			case SRL: rf[i.a1.reg] = rf[i.a2.reg] >> rf[i.a3.reg]; break;
			case SRA: rf[i.a1.reg] = (*(int32_t*)&rf[i.a2.reg]) >> rf[i.a3.reg]; break;


			case ADDI: rf[i.a1.reg] = rf[i.a2.reg] + i.a3.imm; break;
			case SLTI: rf[i.a1.reg] = (*(int32_t*)&rf[i.a2.reg]) < (*(int32_t*)&(i.a3.imm)) ? 1 : 0; break;
			case SLTIU: rf[i.a1.reg] = rf[i.a2.reg] < i.a3.imm ? 1 : 0; break;
			case ANDI: rf[i.a1.reg] = rf[i.a2.reg] & i.a3.imm; break;
			case ORI: rf[i.a1.reg] = rf[i.a2.reg] | i.a3.imm; break;
			case XORI: rf[i.a1.reg] = rf[i.a2.reg] ^ i.a3.imm; break;
			case SLLI: rf[i.a1.reg] = rf[i.a2.reg] << i.a3.imm; break;
			case SRLI: rf[i.a1.reg] = rf[i.a2.reg] >> i.a3.imm; break;
			case SRAI: rf[i.a1.reg] = (*(int32_t*)&rf[i.a2.reg]) >> i.a3.imm; break;

			case LB:
			case LBU: 
			case LH:
			case LHU:
			case LW: rf[i.a1.reg] = mem_read(mem, rf[i.a2.reg]+i.a3.imm, i.op); break;
			
			case SB: 
			case SH: 
			case SW: mem_write(mem, rf[i.a2.reg]+i.a3.imm, rf[i.a1.reg], i.op); break;
			/*

			case SB: mem[rf[i.a2.reg]+i.a3.imm] = *(uint8_t*)&(rf[i.a1.reg]); break;
			case SH: *(uint16_t*)&(mem[rf[i.a2.reg]+i.a3.imm]) = *(uint16_t*)&(rf[i.a1.reg]); break;
			case SW: 
				*(uint32_t*)&(mem[rf[i.a2.reg]+i.a3.imm]) = rf[i.a1.reg]; 
				//printf( "Writing %x to addr %x\n", rf[i.a1.reg], rf[i.a2.reg]+i.a3.imm );
			break;
			*/

			case BEQ: if ( rf[i.a1.reg] == rf[i.a2.reg] ) pc_next = i.a3.imm; break;
			case BGE: if ( *(int32_t*)&rf[i.a1.reg] >= *(int32_t*)&rf[i.a2.reg] ) pc_next = i.a3.imm; break;
			case BGEU: if ( rf[i.a1.reg] >= rf[i.a2.reg] ) pc_next = i.a3.imm; 
				break;
			case BLT: if ( *(int32_t*)&rf[i.a1.reg] < *(int32_t*)&rf[i.a2.reg] ) pc_next = i.a3.imm; break;
			case BLTU: if ( rf[i.a1.reg] < rf[i.a2.reg] ) pc_next = i.a3.imm; break;
			case BNE: if ( rf[i.a1.reg] != rf[i.a2.reg] ) pc_next = i.a3.imm; break;

			case JAL:
				rf[i.a1.reg] = pc + 4;
				pc_next = i.a2.imm;
				//printf( "jal %d %x\n", pc+4, pc_next );
				break;
			case JALR:
				rf[i.a1.reg] = pc + 4;
				pc_next = rf[i.a2.reg] + i.a3.imm;
				//printf( "jalr %d %d(%d)\n", i.a1.reg, i.a3.imm, i.a2.reg );
				break;
			case AUIPC:
				rf[i.a1.reg] = pc + (i.a2.imm<<12);
				//printf( "auipc %x \n", rf[i.a1.reg] );
				break;
			case LUI:
				rf[i.a1.reg] = (i.a2.imm<<12);
				//printf( "lui %x \n", rf[i.a1.reg] );
				break;
			
			case HCF:
				printf( "\n\n----------\n\n" );
				printf( "Reached Halt and Catch Fire instruction!\n" );
				printf( "inst: %6d pc: %6d src line: %d\n", inst_cnt, pc, i.orig_line );
				print_regfile(rf);
				dexit = true;
				break;

			//******************************************
			//****************************************** 
			case SYSCALL:
				if (rf[10]==0) {
					printf( " +---+---+---+---+---+---+---+---+---+\n" );
				}
				else if (rf[10]==1) {
					printf( " | " );
				}
				else if (rf[10]==2) {
					printf( " %x ", rf[12]);
				}
				else if (rf[10]==3) {
					printf( " |\n" );
				}
				break;
			//***************************************** *
			//******************************************


			case UNIMPL:
			default:
				printf( "Reached an unimplemented instruction!\n" );
				if ( i.psrc ) printf( "Instruction: %s\n", i.psrc );
				printf( "inst: %6d pc: %6d src line: %d\n", inst_cnt, pc, i.orig_line );
				print_regfile(rf);
				dexit = true;
				break;
		}
		pc = pc_next % MEM_BYTES;
		rf[0] = 0;// cleaner way to do this?
		if ( stepping ) {
			for ( int i = 0; i < 32; i++ ) {
				if ( rf[i] != rf_mirror[i] ) printf( ">> rf[x%02d] %x -> %x\n", i, rf_mirror[i], rf[i] );
				rf_mirror[i] = rf[i];
			}
		}

		//printf( "reg dst %d -> %x %d\n", i.a1.reg, rf[i.a1.reg], rf[i.a1.reg] );

		fflush(stdout);
		
	}
}

void normalize_labels(instr* imem, label_loc* labels, int label_count, source* src) {
	for ( int i = 0; i < DATA_OFFSET/4; i++ ) {
		instr* ii = &imem[i];
		if ( ii->op == UNIMPL ) continue;

		if ( ii->a1.type == OPTYPE_LABEL ) {
			ii->a1.type = OPTYPE_IMM;
			ii->a1.imm = label_addr(ii->a1.label, labels, label_count, ii->orig_line);
		}
		if ( ii->a2.type == OPTYPE_LABEL ) {
			ii->a2.type = OPTYPE_IMM;
			ii->a2.imm = label_addr(ii->a2.label, labels, label_count, ii->orig_line);
			switch (ii->op) {
				case LUI: {
					ii->a2.imm = (ii->a2.imm>>12); 
					char areg[4]; sprintf(areg, "x%02d", ii->a1.reg);
					char immu[12]; sprintf(immu, "0x%08x" , ii->a2.imm);
					//printf( "LUI %d 0x%x %s\n", ii->a1.reg, ii->a2.imm, immu );
					append_source("lui", areg, immu, NULL, src, ii);
					break;
				}
				case JAL:
				int pc = (i*4);
				int target = ii->a3.imm;
				int diff = pc - target;
				if ( diff < 0 ) diff = -diff;

				if ( diff >= (1<<21) ) {
					printf( "JAL instruction target out of bounds\n" );
					exit(3);
				}
				break;
			}
		}
		if ( ii->a3.type == OPTYPE_LABEL ) {
			ii->a3.type = OPTYPE_IMM;
			ii->a3.imm = label_addr(ii->a3.label, labels, label_count, ii->orig_line);
			switch(ii->op) {
				case ADDI: {
					ii->a3.imm = ii->a3.imm&((1<<12)-1);
					char a1reg[4]; sprintf(a1reg, "x%02d", ii->a1.reg);
					char a2reg[4]; sprintf(a2reg, "x%02d", ii->a2.reg);
					char immd[12]; sprintf(immd, "0x%08x" , ii->a3.imm);
					//printf( "ADDI %d %d 0x%x %s\n", ii->a1.reg, ii->a2.reg, ii->a3.imm, immd );
					append_source("addi", a1reg, a2reg, immd, src, ii);
					break;
				}
				case BEQ: case BGE: case BGEU: case BLT: case BLTU: case BNE: {
					int pc = (i*4);
					int target = ii->a3.imm;
					int diff = pc - target;
					if ( diff < 0 ) diff = -diff;

					if ( diff >= (1<<13) ) {
						printf( "Branch instruction target out of bounds\n" );
						exit(3);
					}
					break;
				}
			}
		}
	}
}

int
main(int argc, char** argv) {
	if ( argc < 2 ) {
		printf( "usage: %s asmfile\n", argv[0] );
		exit(1);
	}

	FILE* fin = fopen(argv[1], "r" );
	if ( !fin ) {
		printf( "%s: No such file\n", argv[1] );
		exit(2);
	}

	bool start_immediate = false;
	if ( argc >= 3 ) {
		start_immediate = true;
	}
	//如果執行./obj/emulator [assembly file path]的話argc值只會有2，出現3的可能性是...？
	//??什麼時候會觸發start_immediate


	//ProcessorState* ps = new ProcessorState();
	
	//將所有記憶體位置分配好
	int memoff = 0;
	uint8_t* mem = (uint8_t*)malloc(MEM_BYTES);
	instr* imem = (instr*)malloc(DATA_OFFSET*sizeof(instr)/4);
	//sizeof(instr/4)這個部分怎麼計算他的size
	//是會把裡面所有東西相加？
	label_loc* labels = (label_loc*)malloc(MAX_LABEL_COUNT*sizeof(label_loc));
	int label_count = 0;
	source src;
	src.offset = 0;
	src.src = (char*)malloc(sizeof(char)*MAX_SRC_LEN);

	//要確認malloc指令是否順利完成（記憶體的位置夠不夠）
	if ( !mem || !labels || !imem || !src.src ) {
		printf( "Memory allocation failed\n" );
		exit(2);
	}

	for ( int i = 0; i < DATA_OFFSET/4; i++ ) {
		imem[i].op = UNIMPL;
		imem[i].a1.type = OPTYPE_NONE;
		imem[i].a2.type = OPTYPE_NONE;
		imem[i].a3.type = OPTYPE_NONE;
	}

	parse(fin, mem, imem, memoff, labels, label_count, &src);
	normalize_labels(imem, labels, label_count, &src);
	
	execute(mem, imem, labels, label_count, start_immediate);

	printf( "Execution done!\n" );
	exit(0);

}


//2021/08/06
//**********************************

//add instruction : DIV
//format          : div rd rs1 rs2
//discription     : division  result(quotient) -> rd
//operation       : X(rd) = X(rs1) / X(rs2)

//**********************************

//add instruction : MUL
//format          : mul rd rs1 rs2
//discription     : multiplication  result -> rd **check overflow problem**
//operation       : X(rd) = X(rs1) * X(rs2)

//**********************************

//add instruction : REM
//format          : rem rd rs1 rs2
//discription     : remainder  set rd value to rs1%rs2. Notice that we need to signextend rs1 and rs2 before % operation
//operation       : X(rd) = X(rs1) % X(rs2)

//**********************************

//add instruction : REMU
//format          : remu rd rs1 rs2
//discription     : remainder unsigned  set rd value to rs1%rs2. Padding zeros before % operation
//operation       : X(rd) = X(rs1) % X(rs2)

//**********************************
//beqz/bnez為 pseudoinstruction  會被拆解為beq/bne

//add instruction : BEQZ
//format          : beqz rs offset
//discription     : branch if rd value is equal to zero
//operation       : (pc=pc+offset)?(X(rd)==0):(pc=pc+4)

//**********************************

//add instruction : BNEZ
//format          : bnez rs offset
//discription     : branch if rd value isn't equal to zero
//operation       : (pc=pc+offset)?(X(rd)==0):(pc=pc+4)

//**********************************

//add instruction : syscall / ECALL
//format          : 
//discription     : 
//operation       : 

//**********************************









//2021/07/30 
//**********************************

//add instruction : ANDN
//format 		  : andn rd rs1 rs2
//discription     : AND operation with inverted operand 
//operation       : X(rd) = X(rs1) & (~X(rs2))

//**********************************   

//add instruction : BCLR
//format   		  : bclr rd rs1 rs2
//discription     : clear the "rs2" bit in rs1 register
//operation       : X(rd) = X(rs1) & ~(1 << index)
//					index = X(rs2) & (XLEN-1)

//**********************************

//add instruction : BCLRI
//format  		  : bclri rd rs1 shamt
//discription     : clear the "shamt(5 bits)" bit in rs1 register
//operation       : X(rd) = X(rs1) & ~(1 << shamt) 

//**********************************

//add instruction : BEXT
//format          : bext rd  rs1  rs2
//discription     : return "rs2's" bit in rs1 (only single bit  0 or 1)
//operation       : X(rd) = (X(rs1) >> index) & 1

//**********************************



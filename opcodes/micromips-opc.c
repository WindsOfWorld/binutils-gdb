/* micromips-opc.c.  microMIPS opcode table.
   Copyright 2008, 2012 Free Software Foundation, Inc.
   Contributed by Chao-ying Fu, MIPS Technologies, Inc.

   This file is part of the GNU opcodes library.

   This library is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.

   It is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
   or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
   License for more details.

   You should have received a copy of the GNU General Public License
   along with this file; see the file COPYING.  If not, write to the
   Free Software Foundation, 51 Franklin Street - Fifth Floor, Boston,
   MA 02110-1301, USA.  */

#include "sysdep.h"
#include <stdio.h>
#include "opcode/mips.h"

#define UBD	INSN_UNCOND_BRANCH_DELAY
#define CBD	INSN_COND_BRANCH_DELAY
#define NODS	INSN_NO_DELAY_SLOT
#define TRAP	INSN_NO_DELAY_SLOT
#define SM	INSN_STORE_MEMORY
#define BD16	INSN2_BRANCH_DELAY_16BIT	/* Used in pinfo2.  */
#define BD32	INSN2_BRANCH_DELAY_32BIT	/* Used in pinfo2.  */

/* For 16-bit/32-bit microMIPS instructions.  They are used in pinfo2.  */
#define UBR	INSN2_UNCOND_BRANCH
#define CBR	INSN2_COND_BRANCH
#define WR_mb	INSN2_WRITE_GPR_MB
#define RD_mc	INSN2_READ_GPR_MC
#define RD_md	INSN2_MOD_GPR_MD
#define WR_md	INSN2_MOD_GPR_MD
#define RD_me	INSN2_READ_GPR_ME
#define RD_mf	INSN2_MOD_GPR_MF
#define WR_mf	INSN2_MOD_GPR_MF
#define RD_mg	INSN2_READ_GPR_MG
#define WR_mh	INSN2_WRITE_GPR_MH
#define RD_mj	INSN2_READ_GPR_MJ
#define WR_mj	INSN2_WRITE_GPR_MJ
#define RD_ml	RD_mc	/* Reuse, since the bit position is the same.  */
#define RD_mmn	INSN2_READ_GPR_MMN
#define RD_mp	INSN2_READ_GPR_MP
#define WR_mp	INSN2_WRITE_GPR_MP
#define RD_mq	INSN2_READ_GPR_MQ
#define RD_sp	INSN2_MOD_SP
#define WR_sp	INSN2_MOD_SP
#define RD_31	INSN2_READ_GPR_31
#define RD_gp	INSN2_READ_GP
#define RD_pc	INSN2_READ_PC

/* For 32-bit microMIPS instructions.  */
#define WR_s	INSN_WRITE_GPR_S
#define WR_d	INSN_WRITE_GPR_D
#define WR_t	INSN_WRITE_GPR_T
#define WR_31	INSN_WRITE_GPR_31
#define WR_D	INSN_WRITE_FPR_D
#define WR_T	INSN_WRITE_FPR_T
#define WR_S	INSN_WRITE_FPR_S
#define WR_CC	INSN_WRITE_COND_CODE

#define RD_s	INSN_READ_GPR_S
#define RD_b	INSN_READ_GPR_S
#define RD_t	INSN_READ_GPR_T
#define RD_T	INSN_READ_FPR_T
#define RD_S	INSN_READ_FPR_S
#define RD_R	INSN_READ_FPR_R
#define RD_D	INSN2_READ_FPR_D	/* Used in pinfo2.  */
#define RD_CC	INSN_READ_COND_CODE
#define RD_C0	INSN_COP
#define RD_C1	INSN_COP
#define RD_C2	INSN_COP
#define WR_C0	INSN_COP
#define WR_C1	INSN_COP
#define WR_C2	INSN_COP
#define CP	INSN_COP

#define WR_HI	INSN_WRITE_HI
#define RD_HI	INSN_READ_HI

#define WR_LO	INSN_WRITE_LO
#define RD_LO	INSN_READ_LO

#define WR_HILO	WR_HI|WR_LO
#define RD_HILO	RD_HI|RD_LO
#define MOD_HILO WR_HILO|RD_HILO

/* Reuse INSN_ISA1 for 32-bit microMIPS ISA.  All instructions in I1
   are accepted as 32-bit microMIPS ISA.
   Reuse INSN_ISA3 for 64-bit microMIPS ISA.  All instructions in I3
   are accepted as 64-bit microMIPS ISA.  */
#define I1	INSN_ISA1
#define I3	INSN_ISA3

/* MIPS DSP ASE support.  */
#define WR_a	WR_HILO		/* Write DSP accumulators (reuse WR_HILO).  */
#define RD_a	RD_HILO		/* Read DSP accumulators (reuse RD_HILO).  */
#define MOD_a	WR_a|RD_a
#define DSP_VOLA INSN_NO_DELAY_SLOT
#define D32	ASE_DSP
#define D33	ASE_DSPR2

/* MIPS MCU (MicroController) ASE support.  */
#define MC	ASE_MCU

/* MIPS Enhanced VA Scheme.  */
#define EVA	ASE_EVA

/* TLB invalidate instruction support.  */
#define TLBINV	ASE_EVA

/* MIPS Virtualization ASE.  */
#define IVIRT	ASE_VIRT
#define IVIRT64	ASE_VIRT64

const struct mips_opcode micromips_opcodes[] =
{
/* These instructions appear first so that the disassembler will find
   them first.  The assemblers uses a hash table based on the
   instruction name anyhow.  */
/* name,    args,	match,      mask,	pinfo,			pinfo2,		membership,	[ase],	[exclusions] */
{"pref",    "k,~(b)",	0x60002000, 0xfc00f000,	RD_b,			0,		I1	},
{"pref",    "k,A(b)",	0,    (int) M_PREF_AB,	INSN_MACRO,		0,		I1	},
{"prefx",   "h,t(b)",	0x540001a0, 0xfc0007ff,	RD_b|RD_t|FP_S,		0,		I1	},
{"nop",     "",		    0x0c00,     0xffff,	0,			INSN2_ALIAS,	I1	},
{"nop",     "",		0x00000000, 0xffffffff,	0,			INSN2_ALIAS,	I1	}, /* sll */
{"ssnop",   "",		0x00000800, 0xffffffff,	0,			INSN2_ALIAS,	I1	}, /* sll */
{"ehb",     "",		0x00001800, 0xffffffff,	0,			INSN2_ALIAS,	I1	}, /* sll */
{"pause",   "",		0x00002800, 0xffffffff,	0,			INSN2_ALIAS,	I1	}, /* sll */
{"li",      "md,mI",	    0xec00,     0xfc00,	0,			WR_md,		I1	},
{"li",      "t,j",	0x30000000, 0xfc1f0000,	WR_t,			INSN2_ALIAS,	I1	}, /* addiu */
{"li",      "t,i",	0x50000000, 0xfc1f0000,	WR_t,			INSN2_ALIAS,	I1	}, /* ori */
#if 0
/* Disabled until we can handle 48-bit opcodes.  */
{"li",      "s,I",	0x7c0000010000, 0xfc00001f0000,	WR_t,		0,		I3	}, /* li48 */
#endif
{"li",      "t,I",	0,    (int) M_LI,	INSN_MACRO,		0,		I1	},
{"move",    "d,s",	0,    (int) M_MOVE,	INSN_MACRO,		0,		I1	},
{"move",    "mp,mj",	    0x0c00,     0xfc00,	0,			WR_mp|RD_mj,	I1	},
{"move",    "d,s",	0x58000150, 0xffe007ff,	WR_d|RD_s,		INSN2_ALIAS,	I3	}, /* daddu */
{"move",    "d,s",	0x00000150, 0xffe007ff,	WR_d|RD_s,		INSN2_ALIAS,	I1	}, /* addu */
{"move",    "d,s",	0x00000290, 0xffe007ff,	WR_d|RD_s,		INSN2_ALIAS,	I1	}, /* or */
{"b",       "mD",	    0xcc00,     0xfc00,	UBD,			0,		I1	},
{"b",       "p",	0x94000000, 0xffff0000,	UBD,			INSN2_ALIAS,	I1	}, /* beq 0, 0 */
{"b",       "p",	0x40400000, 0xffff0000,	UBD,			INSN2_ALIAS,	I1	}, /* bgez 0 */
{"bal",     "p",	0x40600000, 0xffff0000,	UBD|WR_31,		INSN2_ALIAS|BD32,	I1	}, /* bgezal 0 */
{"bals",    "p",	0x42600000, 0xffff0000,	UBD|WR_31,		INSN2_ALIAS|BD16,	I1	}, /* bgezals 0 */
{"bc",      "p",	0x40e00000, 0xffff0000,	NODS,			INSN2_ALIAS|UBR,	I1	}, /* beqzc 0 */

{"abs",     "d,v",	0,    (int) M_ABS,	INSN_MACRO,		0,		I1	},
{"abs.d",   "T,V",	0x5400237b, 0xfc00ffff,	WR_T|RD_S|FP_D,		0,		I1	},
{"abs.s",   "T,V",	0x5400037b, 0xfc00ffff,	WR_T|RD_S|FP_S,		0,		I1	},
{"abs.ps",  "T,V",	0x5400437b, 0xfc00ffff,	WR_T|RD_S|FP_D,		0,		I1	},
{"aclr",    "\\,~(b)",	0x2000b000, 0xff00f000,	SM|RD_b|NODS,		0,		0,	MC	},
{"aclr",    "\\,A(b)",	0,    (int) M_ACLR_AB,	INSN_MACRO,		0,		0,	MC	},
{"add",     "d,v,t",	0x00000110, 0xfc0007ff,	WR_d|RD_s|RD_t,		0,		I1	},
{"add",     "t,r,I",	0,    (int) M_ADD_I,	INSN_MACRO,		0,		I1	},
{"add.d",   "D,V,T",	0x54000130, 0xfc0007ff,	WR_D|RD_S|RD_T|FP_D,	0,		I1	},
{"add.s",   "D,V,T",	0x54000030, 0xfc0007ff,	WR_D|RD_S|RD_T|FP_S,	0,		I1	},
{"add.ps",  "D,V,T",	0x54000230, 0xfc0007ff,	WR_D|RD_S|RD_T|FP_D,	0,		I1	},
{"addi",    "t,r,j",	0x10000000, 0xfc000000,	WR_t|RD_s,		0,		I1	},
{"addiu",   "mp,mj,mZ",	    0x0c00,     0xfc00,	0,			WR_mp|RD_mj,	I1	}, /* move */
{"addiu",   "md,ms,mW",	    0x6c01,     0xfc01,	0,			WR_md|RD_sp,	I1	}, /* addiur1sp */
{"addiu",   "md,mc,mB",	    0x6c00,     0xfc01,	0,			WR_md|RD_mc,	I1	}, /* addiur2 */
{"addiu",   "ms,mt,mY",	    0x4c01,     0xfc01,	0,			WR_sp|RD_sp,		I1	}, /* addiusp */
{"addiu",   "mp,mt,mX",	    0x4c00,     0xfc01,	0,			WR_mp|RD_mp,		I1	}, /* addius5 */
{"addiu",   "mb,mr,mQ",	0x78000000, 0xfc000000,	0,			WR_mb|RD_pc,	I1	}, /* addiupc */
{"addiu",   "t,r,j",	0x30000000, 0xfc000000,	WR_t|RD_s,		0,		I1	},
{"addiupc", "mb,mQ",	0x78000000, 0xfc000000,	0,			WR_mb|RD_pc,	I1	},
{"addiur1sp", "md,mW",	    0x6c01,     0xfc01,	0,			WR_md|RD_sp,	I1	},
{"addiur2", "md,mc,mB",	    0x6c00,     0xfc01,	0,			WR_md|RD_mc,	I1	},
{"addiusp", "mY",	    0x4c01,     0xfc01,	0,			WR_sp|RD_sp,		I1	},
{"addius5", "mp,mX",	    0x4c00,     0xfc01,	0,			WR_mp|RD_mp,		I1	},
{"addu",    "mp,mj,mz",	    0x0c00,     0xfc00,	0,			WR_mp|RD_mj,	I1	}, /* move */
{"addu",    "mp,mz,mj",	    0x0c00,     0xfc00,	0,			WR_mp|RD_mj,	I1	}, /* move */
{"addu",    "md,me,ml",	    0x0400,     0xfc01,	0,			WR_md|RD_me|RD_ml,	I1	},
{"addu",    "d,v,t",	0x00000150, 0xfc0007ff,	WR_d|RD_s|RD_t,		0,		I1	},
{"addu",    "t,r,I",	0,    (int) M_ADDU_I,	INSN_MACRO,		0,		I1	},
/* We have no flag to mark the read from "y", so we use NODS to disable
   delay slot scheduling of ALNV.PS altogether.  */
{"alnv.ps", "D,V,T,y",	0x54000019, 0xfc00003f,	NODS|WR_D|RD_S|RD_T|FP_D, 0,		I1	},
{"and",     "mf,mt,mg",	    0x4480,     0xffc0,	0,			WR_mf|RD_mf|RD_mg,	I1	},
{"and",     "mf,mg,mx",	    0x4480,     0xffc0,	0,			WR_mf|RD_mf|RD_mg,	I1	},
{"and",     "d,v,t",	0x00000250, 0xfc0007ff,	WR_d|RD_s|RD_t,		0,		I1	},
{"and",     "t,r,I",	0,    (int) M_AND_I,	INSN_MACRO,		0,		I1	},
{"andi",    "md,mc,mC",	    0x2c00,     0xfc00,	0,			WR_md|RD_mc,	I1	},
{"andi",    "t,r,i",	0xd0000000, 0xfc000000,	WR_t|RD_s,		0,		I1	},
{"aset",    "\\,~(b)",	0x20003000, 0xff00f000,	SM|RD_b|NODS,		0,		0,	MC	},
{"aset",    "\\,A(b)",	0,    (int) M_ASET_AB,	INSN_MACRO,		0,		0,	MC	},
/* b is at the top of the table.  */
/* bal is at the top of the table.  */
{"bc1f",    "p",	0x43800000, 0xffff0000,	CBD|RD_CC|FP_S,		0,		I1	},
{"bc1f",    "N,p",	0x43800000, 0xffe30000,	CBD|RD_CC|FP_S,		0,		I1	},
{"bc1fl",   "p",	0,    (int) M_BC1FL,	INSN_MACRO,		INSN2_M_FP_S,		I1	},
{"bc1fl",   "N,p",	0,    (int) M_BC1FL,	INSN_MACRO,		INSN2_M_FP_S,		I1	},
{"bc2f",    "p",	0x42800000, 0xffff0000,	CBD|RD_CC,		0,		I1	},
{"bc2f",    "N,p",	0x42800000, 0xffe30000,	CBD|RD_CC,		0,		I1	},
{"bc2fl",   "p",	0,    (int) M_BC2FL,	INSN_MACRO,		0,		I1	},
{"bc2fl",   "N,p",	0,    (int) M_BC2FL,	INSN_MACRO,		0,		I1	},
{"bc1t",    "p",	0x43a00000, 0xffff0000,	CBD|RD_CC|FP_S,		0,		I1	},
{"bc1t",    "N,p",	0x43a00000, 0xffe30000,	CBD|RD_CC|FP_S,		0,		I1	},
{"bc1tl",   "p",	0,    (int) M_BC1TL,	INSN_MACRO,		INSN2_M_FP_S,		I1	},
{"bc1tl",   "N,p",	0,    (int) M_BC1TL,	INSN_MACRO,		INSN2_M_FP_S,		I1	},
{"bc2t",    "p",	0x42a00000, 0xffff0000,	CBD|RD_CC,		0,		I1	},
{"bc2t",    "N,p",	0x42a00000, 0xffe30000,	CBD|RD_CC,		0,		I1	},
{"bc2tl",   "p",	0,    (int) M_BC2TL,	INSN_MACRO,		0,		I1	},
{"bc2tl",   "N,p",	0,    (int) M_BC2TL,	INSN_MACRO,		0,		I1	},
{"beqz",    "md,mE",	    0x8c00,     0xfc00,	CBD,			RD_md,		I1	},
{"beqz",    "s,p",	0x94000000, 0xffe00000,	CBD|RD_s,		0,		I1	},
{"beqzc",   "s,p",	0x40e00000, 0xffe00000,	NODS|RD_s,		CBR,		I1	},
{"beqzl",   "s,p",	0,    (int) M_BEQL,	INSN_MACRO,		0,		I1	},
{"beq",     "md,mz,mE",	    0x8c00,     0xfc00,	CBD,			RD_md,		I1	}, /* beqz */
{"beq",     "mz,md,mE",	    0x8c00,     0xfc00,	CBD,			RD_md,		I1	}, /* beqz */
{"beq",     "s,t,p",	0x94000000, 0xfc000000,	CBD|RD_s|RD_t,		0,		I1	},
{"beq",     "s,I,p",	0,    (int) M_BEQ_I,	INSN_MACRO,		0,		I1	},
{"beql",    "s,t,p",	0,    (int) M_BEQL,	INSN_MACRO,		0,		I1	},
{"beql",    "s,I,p",	0,    (int) M_BEQL_I,	INSN_MACRO,		0,		I1	},
{"bge",     "s,t,p",	0,    (int) M_BGE,	INSN_MACRO,		0,		I1	},
{"bge",     "s,I,p",	0,    (int) M_BGE_I,	INSN_MACRO,		0,		I1	},
{"bgel",    "s,t,p",	0,    (int) M_BGEL,	INSN_MACRO,		0,		I1	},
{"bgel",    "s,I,p",	0,    (int) M_BGEL_I,	INSN_MACRO,		0,		I1	},
{"bgeu",    "s,t,p",	0,    (int) M_BGEU,	INSN_MACRO,		0,		I1	},
{"bgeu",    "s,I,p",	0,    (int) M_BGEU_I,	INSN_MACRO,		0,		I1	},
{"bgeul",   "s,t,p",	0,    (int) M_BGEUL,	INSN_MACRO,		0,		I1	},
{"bgeul",   "s,I,p",	0,    (int) M_BGEUL_I,	INSN_MACRO,		0,		I1	},
{"bgez",    "s,p",	0x40400000, 0xffe00000,	CBD|RD_s,		0,		I1	},
{"bgezl",   "s,p",	0,    (int) M_BGEZL,	INSN_MACRO,		0,		I1	},
{"bgezal",  "s,p",	0x40600000, 0xffe00000,	CBD|RD_s|WR_31,		BD32,		I1	},
{"bgezals", "s,p",	0x42600000, 0xffe00000,	CBD|RD_s|WR_31,		BD16,		I1	},
{"bgezall", "s,p",	0,    (int) M_BGEZALL,	INSN_MACRO,		0,		I1	},
{"bgt",     "s,t,p",	0,    (int) M_BGT,	INSN_MACRO,		0,		I1	},
{"bgt",     "s,I,p",	0,    (int) M_BGT_I,	INSN_MACRO,		0,		I1	},
{"bgtl",    "s,t,p",	0,    (int) M_BGTL,	INSN_MACRO,		0,		I1	},
{"bgtl",    "s,I,p",	0,    (int) M_BGTL_I,	INSN_MACRO,		0,		I1	},
{"bgtu",    "s,t,p",	0,    (int) M_BGTU,	INSN_MACRO,		0,		I1	},
{"bgtu",    "s,I,p",	0,    (int) M_BGTU_I,	INSN_MACRO,		0,		I1	},
{"bgtul",   "s,t,p",	0,    (int) M_BGTUL,	INSN_MACRO,		0,		I1	},
{"bgtul",   "s,I,p",	0,    (int) M_BGTUL_I,	INSN_MACRO,		0,		I1	},
{"bgtz",    "s,p",	0x40c00000, 0xffe00000,	CBD|RD_s,		0,		I1	},
{"bgtzl",   "s,p",	0,    (int) M_BGTZL,	INSN_MACRO,		0,		I1	},
{"ble",     "s,t,p",	0,    (int) M_BLE,	INSN_MACRO,		0,		I1	},
{"ble",     "s,I,p",	0,    (int) M_BLE_I,	INSN_MACRO,		0,		I1	},
{"blel",    "s,t,p",	0,    (int) M_BLEL,	INSN_MACRO,		0,		I1	},
{"blel",    "s,I,p",	0,    (int) M_BLEL_I,	INSN_MACRO,		0,		I1	},
{"bleu",    "s,t,p",	0,    (int) M_BLEU,	INSN_MACRO,		0,		I1	},
{"bleu",    "s,I,p",	0,    (int) M_BLEU_I,	INSN_MACRO,		0,		I1	},
{"bleul",   "s,t,p",	0,    (int) M_BLEUL,	INSN_MACRO,		0,		I1	},
{"bleul",   "s,I,p",	0,    (int) M_BLEUL_I,	INSN_MACRO,		0,		I1	},
{"blez",    "s,p",	0x40800000, 0xffe00000,	CBD|RD_s,		0,		I1	},
{"blezl",   "s,p",	0,    (int) M_BLEZL,	INSN_MACRO,		0,		I1	},
{"blt",     "s,t,p",	0,    (int) M_BLT,	INSN_MACRO,		0,		I1	},
{"blt",     "s,I,p",	0,    (int) M_BLT_I,	INSN_MACRO,		0,		I1	},
{"bltl",    "s,t,p",	0,    (int) M_BLTL,	INSN_MACRO,		0,		I1	},
{"bltl",    "s,I,p",	0,    (int) M_BLTL_I,	INSN_MACRO,		0,		I1	},
{"bltu",    "s,t,p",	0,    (int) M_BLTU,	INSN_MACRO,		0,		I1	},
{"bltu",    "s,I,p",	0,    (int) M_BLTU_I,	INSN_MACRO,		0,		I1	},
{"bltul",   "s,t,p",	0,    (int) M_BLTUL,	INSN_MACRO,		0,		I1	},
{"bltul",   "s,I,p",	0,    (int) M_BLTUL_I,	INSN_MACRO,		0,		I1	},
{"bltz",    "s,p",	0x40000000, 0xffe00000,	CBD|RD_s,		0,		I1	},
{"bltzl",   "s,p",	0,    (int) M_BLTZL,	INSN_MACRO,		0,		I1	},
{"bltzal",  "s,p",	0x40200000, 0xffe00000,	CBD|RD_s|WR_31,		BD32,		I1	},
{"bltzals", "s,p",	0x42200000, 0xffe00000,	CBD|RD_s|WR_31,		BD16,		I1	},
{"bltzall", "s,p",	0,    (int) M_BLTZALL,	INSN_MACRO,		0,		I1	},
{"bnez",    "md,mE",	    0xac00,     0xfc00,	CBD,			RD_md,		I1	},
{"bnez",    "s,p",	0xb4000000, 0xffe00000,	CBD|RD_s,		0,		I1	},
{"bnezc",   "s,p",	0x40a00000, 0xffe00000,	NODS|RD_s,		CBR,		I1	},
{"bnezl",   "s,p",	0,    (int) M_BNEL,	INSN_MACRO,		0,		I1	},
{"bne",     "md,mz,mE",	    0xac00,     0xfc00,	CBD,			RD_md,		I1	}, /* bnez */
{"bne",     "mz,md,mE",	    0xac00,     0xfc00,	CBD,			RD_md,		I1	}, /* bnez */
{"bne",     "s,t,p",	0xb4000000, 0xfc000000,	CBD|RD_s|RD_t,		0,		I1	},
{"bne",     "s,I,p",	0,    (int) M_BNE_I,	INSN_MACRO,		0,		I1	},
{"bnel",    "s,t,p",	0,    (int) M_BNEL,	INSN_MACRO,		0,		I1	},
{"bnel",    "s,I,p",	0,    (int) M_BNEL_I,	INSN_MACRO,		0,		I1	},
{"break",   "",		    0x4680,     0xffff,	TRAP,			0,		I1	},
{"break",   "",		0x00000007, 0xffffffff,	TRAP,			0,		I1	},
{"break",   "mF",	    0x4680,     0xfff0,	TRAP,			0,		I1	},
{"break",   "c",	0x00000007, 0xfc00ffff,	TRAP,			0,		I1	},
{"break",   "c,q",	0x00000007, 0xfc00003f,	TRAP,			0,		I1	},
{"c.f.d",   "S,T",	0x5400043c, 0xfc00ffff,	RD_S|RD_T|WR_CC|FP_D,	0,		I1	},
{"c.f.d",   "M,S,T",	0x5400043c, 0xfc001fff,	RD_S|RD_T|WR_CC|FP_D,	0,		I1	},
{"c.f.s",   "S,T",	0x5400003c, 0xfc00ffff,	RD_S|RD_T|WR_CC|FP_S,	0,		I1	},
{"c.f.s",   "M,S,T",	0x5400003c, 0xfc001fff,	RD_S|RD_T|WR_CC|FP_S,	0,		I1	},
{"c.f.ps",  "S,T",	0x5400083c, 0xfc00ffff,	RD_S|RD_T|WR_CC|FP_D,	0,		I1	},
{"c.f.ps",  "M,S,T",	0x5400083c, 0xfc001fff,	RD_S|RD_T|WR_CC|FP_D,	0,		I1	},
{"c.un.d",  "S,T",	0x5400047c, 0xfc00ffff,	RD_S|RD_T|WR_CC|FP_D,	0,		I1	},
{"c.un.d",  "M,S,T",	0x5400047c, 0xfc001fff,	RD_S|RD_T|WR_CC|FP_D,	0,		I1	},
{"c.un.s",  "S,T",	0x5400007c, 0xfc00ffff,	RD_S|RD_T|WR_CC|FP_S,	0,		I1	},
{"c.un.s",  "M,S,T",	0x5400007c, 0xfc001fff,	RD_S|RD_T|WR_CC|FP_S,	0,		I1	},
{"c.un.ps", "S,T",	0x5400087c, 0xfc00ffff,	RD_S|RD_T|WR_CC|FP_D,	0,		I1	},
{"c.un.ps", "M,S,T",	0x5400087c, 0xfc001fff,	RD_S|RD_T|WR_CC|FP_D,	0,		I1	},
{"c.eq.d",  "S,T",	0x540004bc, 0xfc00ffff,	RD_S|RD_T|WR_CC|FP_D,	0,		I1	},
{"c.eq.d",  "M,S,T",	0x540004bc, 0xfc001fff,	RD_S|RD_T|WR_CC|FP_D,	0,		I1	},
{"c.eq.s",  "S,T",	0x540000bc, 0xfc00ffff,	RD_S|RD_T|WR_CC|FP_S,	0,		I1	},
{"c.eq.s",  "M,S,T",	0x540000bc, 0xfc001fff,	RD_S|RD_T|WR_CC|FP_S,	0,		I1	},
{"c.eq.ps", "S,T",	0x540008bc, 0xfc00ffff,	RD_S|RD_T|WR_CC|FP_D,	0,		I1	},
{"c.eq.ps", "M,S,T",	0x540008bc, 0xfc001fff,	RD_S|RD_T|WR_CC|FP_D,	0,		I1	},
{"c.ueq.d", "S,T",	0x540004fc, 0xfc00ffff,	RD_S|RD_T|WR_CC|FP_D,	0,		I1	},
{"c.ueq.d", "M,S,T",	0x540004fc, 0xfc001fff,	RD_S|RD_T|WR_CC|FP_D,	0,		I1	},
{"c.ueq.s", "S,T",	0x540000fc, 0xfc00ffff,	RD_S|RD_T|WR_CC|FP_S,	0,		I1	},
{"c.ueq.s", "M,S,T",	0x540000fc, 0xfc001fff,	RD_S|RD_T|WR_CC|FP_S,	0,		I1	},
{"c.ueq.ps", "S,T",	0x540008fc, 0xfc00ffff,	RD_S|RD_T|WR_CC|FP_D,	0,		I1	},
{"c.ueq.ps", "M,S,T",	0x540008fc, 0xfc001fff,	RD_S|RD_T|WR_CC|FP_D,	0,		I1	},
{"c.olt.d", "S,T",	0x5400053c, 0xfc00ffff,	RD_S|RD_T|WR_CC|FP_D,	0,		I1	},
{"c.olt.d", "M,S,T",	0x5400053c, 0xfc001fff,	RD_S|RD_T|WR_CC|FP_D,	0,		I1	},
{"c.olt.s", "S,T",	0x5400013c, 0xfc00ffff,	RD_S|RD_T|WR_CC|FP_S,	0,		I1	},
{"c.olt.s", "M,S,T",	0x5400013c, 0xfc001fff,	RD_S|RD_T|WR_CC|FP_S,	0,		I1	},
{"c.olt.ps", "S,T",	0x5400093c, 0xfc00ffff,	RD_S|RD_T|WR_CC|FP_D,	0,		I1	},
{"c.olt.ps", "M,S,T",	0x5400093c, 0xfc001fff,	RD_S|RD_T|WR_CC|FP_D,	0,		I1	},
{"c.ult.d", "S,T",	0x5400057c, 0xfc00ffff,	RD_S|RD_T|WR_CC|FP_D,	0,		I1	},
{"c.ult.d", "M,S,T",	0x5400057c, 0xfc001fff,	RD_S|RD_T|WR_CC|FP_D,	0,		I1	},
{"c.ult.s", "S,T",	0x5400017c, 0xfc00ffff,	RD_S|RD_T|WR_CC|FP_S,	0,		I1	},
{"c.ult.s", "M,S,T",	0x5400017c, 0xfc001fff,	RD_S|RD_T|WR_CC|FP_S,	0,		I1	},
{"c.ult.ps", "S,T",	0x5400097c, 0xfc00ffff,	RD_S|RD_T|WR_CC|FP_D,	0,		I1	},
{"c.ult.ps", "M,S,T",	0x5400097c, 0xfc001fff,	RD_S|RD_T|WR_CC|FP_D,	0,		I1	},
{"c.ole.d", "S,T",	0x540005bc, 0xfc00ffff,	RD_S|RD_T|WR_CC|FP_D,	0,		I1	},
{"c.ole.d", "M,S,T",	0x540005bc, 0xfc001fff,	RD_S|RD_T|WR_CC|FP_D,	0,		I1	},
{"c.ole.s", "S,T",	0x540001bc, 0xfc00ffff,	RD_S|RD_T|WR_CC|FP_S,	0,		I1	},
{"c.ole.s", "M,S,T",	0x540001bc, 0xfc001fff,	RD_S|RD_T|WR_CC|FP_S,	0,		I1	},
{"c.ole.ps", "S,T",	0x540009bc, 0xfc00ffff,	RD_S|RD_T|WR_CC|FP_D,	0,		I1	},
{"c.ole.ps", "M,S,T",	0x540009bc, 0xfc001fff,	RD_S|RD_T|WR_CC|FP_D,	0,		I1	},
{"c.ule.d", "S,T",	0x540005fc, 0xfc00ffff,	RD_S|RD_T|WR_CC|FP_D,	0,		I1	},
{"c.ule.d", "M,S,T",	0x540005fc, 0xfc001fff,	RD_S|RD_T|WR_CC|FP_D,	0,		I1	},
{"c.ule.s", "S,T",	0x540001fc, 0xfc00ffff,	RD_S|RD_T|WR_CC|FP_S,	0,		I1	},
{"c.ule.s", "M,S,T",	0x540001fc, 0xfc001fff,	RD_S|RD_T|WR_CC|FP_S,	0,		I1	},
{"c.ule.ps", "S,T",	0x540009fc, 0xfc00ffff,	RD_S|RD_T|WR_CC|FP_D,	0,		I1	},
{"c.ule.ps", "M,S,T",	0x540009fc, 0xfc001fff,	RD_S|RD_T|WR_CC|FP_D,	0,		I1	},
{"c.sf.d",  "S,T",	0x5400063c, 0xfc00ffff,	RD_S|RD_T|WR_CC|FP_D,	0,		I1	},
{"c.sf.d",  "M,S,T",	0x5400063c, 0xfc001fff,	RD_S|RD_T|WR_CC|FP_D,	0,		I1	},
{"c.sf.s",  "S,T",	0x5400023c, 0xfc00ffff,	RD_S|RD_T|WR_CC|FP_S,	0,		I1	},
{"c.sf.s",  "M,S,T",	0x5400023c, 0xfc001fff,	RD_S|RD_T|WR_CC|FP_S,	0,		I1	},
{"c.sf.ps", "S,T",	0x54000a3c, 0xfc00ffff,	RD_S|RD_T|WR_CC|FP_D,	0,		I1	},
{"c.sf.ps", "M,S,T",	0x54000a3c, 0xfc001fff,	RD_S|RD_T|WR_CC|FP_D,	0,		I1	},
{"c.ngle.d", "S,T",	0x5400067c, 0xfc00ffff,	RD_S|RD_T|WR_CC|FP_D,	0,		I1	},
{"c.ngle.d", "M,S,T",	0x5400067c, 0xfc001fff,	RD_S|RD_T|WR_CC|FP_D,	0,		I1	},
{"c.ngle.s", "S,T",	0x5400027c, 0xfc00ffff,	RD_S|RD_T|WR_CC|FP_S,	0,		I1	},
{"c.ngle.s", "M,S,T",	0x5400027c, 0xfc001fff,	RD_S|RD_T|WR_CC|FP_S,	0,		I1	},
{"c.ngle.ps", "S,T",	0x54000a7c, 0xfc00ffff,	RD_S|RD_T|WR_CC|FP_D,	0,		I1	},
{"c.ngle.ps", "M,S,T",	0x54000a7c, 0xfc001fff,	RD_S|RD_T|WR_CC|FP_D,	0,		I1	},
{"c.seq.d", "S,T",	0x540006bc, 0xfc00ffff,	RD_S|RD_T|WR_CC|FP_D,	0,		I1	},
{"c.seq.d", "M,S,T",	0x540006bc, 0xfc001fff,	RD_S|RD_T|WR_CC|FP_D,	0,		I1	},
{"c.seq.s", "S,T",	0x540002bc, 0xfc00ffff,	RD_S|RD_T|WR_CC|FP_S,	0,		I1	},
{"c.seq.s", "M,S,T",	0x540002bc, 0xfc001fff,	RD_S|RD_T|WR_CC|FP_S,	0,		I1	},
{"c.seq.ps", "S,T",	0x54000abc, 0xfc00ffff,	RD_S|RD_T|WR_CC|FP_D,	0,		I1	},
{"c.seq.ps", "M,S,T",	0x54000abc, 0xfc001fff,	RD_S|RD_T|WR_CC|FP_D,	0,		I1	},
{"c.ngl.d", "S,T",	0x540006fc, 0xfc00ffff,	RD_S|RD_T|WR_CC|FP_D,	0,		I1	},
{"c.ngl.d", "M,S,T",	0x540006fc, 0xfc001fff,	RD_S|RD_T|WR_CC|FP_D,	0,		I1	},
{"c.ngl.s", "S,T",	0x540002fc, 0xfc00ffff,	RD_S|RD_T|WR_CC|FP_S,	0,		I1	},
{"c.ngl.s", "M,S,T",	0x540002fc, 0xfc001fff,	RD_S|RD_T|WR_CC|FP_S,	0,		I1	},
{"c.ngl.ps", "S,T",	0x54000afc, 0xfc00ffff,	RD_S|RD_T|WR_CC|FP_D,	0,		I1	},
{"c.ngl.ps", "M,S,T",	0x54000afc, 0xfc001fff,	RD_S|RD_T|WR_CC|FP_D,	0,		I1	},
{"c.lt.d",  "S,T",	0x5400073c, 0xfc00ffff,	RD_S|RD_T|WR_CC|FP_D,	0,		I1	},
{"c.lt.d",  "M,S,T",	0x5400073c, 0xfc001fff,	RD_S|RD_T|WR_CC|FP_D,	0,		I1	},
{"c.lt.s",  "S,T",	0x5400033c, 0xfc00ffff,	RD_S|RD_T|WR_CC|FP_S,	0,		I1	},
{"c.lt.s",  "M,S,T",	0x5400033c, 0xfc001fff,	RD_S|RD_T|WR_CC|FP_S,	0,		I1	},
{"c.lt.ps", "S,T",	0x54000b3c, 0xfc00ffff,	RD_S|RD_T|WR_CC|FP_D,	0,		I1	},
{"c.lt.ps", "M,S,T",	0x54000b3c, 0xfc001fff,	RD_S|RD_T|WR_CC|FP_D,	0,		I1	},
{"c.nge.d", "S,T",	0x5400077c, 0xfc00ffff,	RD_S|RD_T|WR_CC|FP_D,	0,		I1	},
{"c.nge.d", "M,S,T",	0x5400077c, 0xfc001fff,	RD_S|RD_T|WR_CC|FP_D,	0,		I1	},
{"c.nge.s", "S,T",	0x5400037c, 0xfc00ffff,	RD_S|RD_T|WR_CC|FP_S,	0,		I1	},
{"c.nge.s", "M,S,T",	0x5400037c, 0xfc001fff,	RD_S|RD_T|WR_CC|FP_S,	0,		I1	},
{"c.nge.ps", "S,T",	0x54000b7c, 0xfc00ffff,	RD_S|RD_T|WR_CC|FP_D,	0,		I1	},
{"c.nge.ps", "M,S,T",	0x54000b7c, 0xfc001fff,	RD_S|RD_T|WR_CC|FP_D,	0,		I1	},
{"c.le.d",  "S,T",	0x540007bc, 0xfc00ffff,	RD_S|RD_T|WR_CC|FP_D,	0,		I1	},
{"c.le.d",  "M,S,T",	0x540007bc, 0xfc001fff,	RD_S|RD_T|WR_CC|FP_D,	0,		I1	},
{"c.le.s",  "S,T",	0x540003bc, 0xfc00ffff,	RD_S|RD_T|WR_CC|FP_S,	0,		I1	},
{"c.le.s",  "M,S,T",	0x540003bc, 0xfc001fff,	RD_S|RD_T|WR_CC|FP_S,	0,		I1	},
{"c.le.ps", "S,T",	0x54000bbc, 0xfc00ffff,	RD_S|RD_T|WR_CC|FP_D,	0,		I1	},
{"c.le.ps", "M,S,T",	0x54000bbc, 0xfc001fff,	RD_S|RD_T|WR_CC|FP_D,	0,		I1	},
{"c.ngt.d", "S,T",	0x540007fc, 0xfc00ffff,	RD_S|RD_T|WR_CC|FP_D,	0,		I1	},
{"c.ngt.d", "M,S,T",	0x540007fc, 0xfc001fff,	RD_S|RD_T|WR_CC|FP_D,	0,		I1	},
{"c.ngt.s", "S,T",	0x540003fc, 0xfc00ffff,	RD_S|RD_T|WR_CC|FP_S,	0,		I1	},
{"c.ngt.s", "M,S,T",	0x540003fc, 0xfc001fff,	RD_S|RD_T|WR_CC|FP_S,	0,		I1	},
{"c.ngt.ps", "S,T",	0x54000bfc, 0xfc00ffff,	RD_S|RD_T|WR_CC|FP_D,	0,		I1	},
{"c.ngt.ps", "M,S,T",	0x54000bfc, 0xfc001fff,	RD_S|RD_T|WR_CC|FP_D,	0,		I1	},
{"cache",   "k,~(b)",	0x20006000, 0xfc00f000,	RD_b,			0,		I1	},
{"cache",   "k,A(b)",	0,    (int) M_CACHE_AB,	INSN_MACRO,		0,		I1	},
{"ceil.l.d", "T,S",	0x5400533b, 0xfc00ffff,	WR_T|RD_S|FP_D,		0,		I1	},
{"ceil.l.s", "T,S",	0x5400133b, 0xfc00ffff,	WR_T|RD_S|FP_S|FP_D,	0,		I1	},
{"ceil.w.d", "T,S",	0x54005b3b, 0xfc00ffff,	WR_T|RD_S|FP_S|FP_D,	0,		I1	},
{"ceil.w.s", "T,S",	0x54001b3b, 0xfc00ffff,	WR_T|RD_S|FP_S,		0,	I1	},
{"cfc1",    "t,G",	0x5400103b, 0xfc00ffff,	WR_t|RD_C1|FP_S,	0,		I1	},
{"cfc1",    "t,S",	0x5400103b, 0xfc00ffff,	WR_t|RD_C1|FP_S,	0,		I1	},
{"cfc2",    "t,G",	0x0000cd3c, 0xfc00ffff,	WR_t|RD_C2,		0,		I1	},
{"clo",     "t,s",	0x00004b3c, 0xfc00ffff,	WR_t|RD_s,		0,		I1	},
{"clz",     "t,s",	0x00005b3c, 0xfc00ffff,	WR_t|RD_s,		0,		I1	},
{"cop2",    "C",	0x00000002, 0xfc000007,	CP,			0,		I1	},
{"ctc1",    "t,G",	0x5400183b, 0xfc00ffff,	RD_t|WR_CC|FP_S,	0,		I1	},
{"ctc1",    "t,S",	0x5400183b, 0xfc00ffff,	RD_t|WR_CC|FP_S,	0,		I1	},
{"ctc2",    "t,G",	0x0000dd3c, 0xfc00ffff,	RD_t|WR_C2|WR_CC,	0,		I1	},
{"cvt.d.l", "T,S",	0x5400537b, 0xfc00ffff,	WR_T|RD_S|FP_D,		0,		I1	},
{"cvt.d.s", "T,S",	0x5400137b, 0xfc00ffff,	WR_T|RD_S|FP_S|FP_D,	0,		I1	},
{"cvt.d.w", "T,S",	0x5400337b, 0xfc00ffff,	WR_T|RD_S|FP_S|FP_D,	0,		I1	},
{"cvt.l.d", "T,S",	0x5400413b, 0xfc00ffff,	WR_T|RD_S|FP_D,		0,		I1	},
{"cvt.l.s", "T,S",	0x5400013b, 0xfc00ffff,	WR_T|RD_S|FP_S|FP_D,	0,		I1	},
{"cvt.s.l", "T,S",	0x54005b7b, 0xfc00ffff,	WR_T|RD_S|FP_S|FP_D,	0,		I1	},
{"cvt.s.d", "T,S",	0x54001b7b, 0xfc00ffff,	WR_T|RD_S|FP_S|FP_D,	0,		I1	},
{"cvt.s.w", "T,S",	0x54003b7b, 0xfc00ffff,	WR_T|RD_S|FP_S,		0,		I1	},
{"cvt.s.pl", "T,S",	0x5400213b, 0xfc00ffff,	WR_T|RD_S|FP_S|FP_D,	0,		I1	},
{"cvt.s.pu", "T,S",	0x5400293b, 0xfc00ffff,	WR_T|RD_S|FP_S|FP_D,	0,		I1	},
{"cvt.w.d", "T,S",	0x5400493b, 0xfc00ffff,	WR_T|RD_S|FP_S|FP_D,	0,		I1	},
{"cvt.w.s", "T,S",	0x5400093b, 0xfc00ffff,	WR_T|RD_S|FP_S,		0,		I1	},
{"cvt.ps.s", "D,V,T",	0x54000180, 0xfc0007ff,	WR_D|RD_S|RD_T|FP_S|FP_D, 0,		I1	},
{"dabs",    "d,v",	0,    (int) M_DABS,	INSN_MACRO,		0,		I3	},
{"dadd",    "d,v,t",	0x58000110, 0xfc0007ff,	WR_d|RD_s|RD_t,		0,		I3	},
{"dadd",    "t,r,I",	0,    (int) M_DADD_I,	INSN_MACRO,		0,		I3	},
{"daddi",   "t,r,.",	0x5800001c, 0xfc00003f,	WR_t|RD_s,		0,		I3	},
{"daddi",   "t,r,I",	0,    (int) M_DADD_I,	INSN_MACRO,		0,		I3	},
{"daddiu",  "t,r,j",	0x5c000000, 0xfc000000,	WR_t|RD_s,		0,		I3	},
{"daddu",   "d,v,t",	0x58000150, 0xfc0007ff,	WR_d|RD_s|RD_t,		0,		I3	},
{"daddu",   "t,r,I",	0,    (int) M_DADDU_I,	INSN_MACRO,		0,		I3	},
{"dclo",    "t,s",	0x58004b3c, 0xfc00ffff,	WR_t|RD_s,		0,		I3	},
{"dclz",    "t,s",	0x58005b3c, 0xfc00ffff,	WR_t|RD_s,		0,		I3	},
{"deret",   "",		0x0000e37c, 0xffffffff,	NODS,			0,		I1	},
{"dext",    "t,r,I,+I",	0,    (int) M_DEXT,	INSN_MACRO,		0,		I3	},
{"dext",    "t,r,+A,+C",0x5800002c, 0xfc00003f, WR_t|RD_s,		0,		I3	},
{"dextm",   "t,r,+A,+G",0x58000024, 0xfc00003f, WR_t|RD_s,		0,		I3	},
{"dextu",   "t,r,+E,+H",0x58000014, 0xfc00003f, WR_t|RD_s,		0,		I3	},
/* For ddiv, see the comments about div.  */
{"ddiv",    "z,s,t",	0x5800ab3c, 0xfc00ffff,	RD_s|RD_t|WR_HILO,	0,		I3	},
{"ddiv",    "z,t",	0x5800ab3c, 0xfc1fffff,	RD_s|RD_t|WR_HILO,	0,		I3	},
{"ddiv",    "d,v,t",	0,    (int) M_DDIV_3,	INSN_MACRO,		0,		I3	},
{"ddiv",    "d,v,I",	0,    (int) M_DDIV_3I,	INSN_MACRO,		0,		I3	},
/* For ddivu, see the comments about div.  */
{"ddivu",   "z,s,t",	0x5800bb3c, 0xfc00ffff,	RD_s|RD_t|WR_HILO,	0,		I3	},
{"ddivu",   "z,t",	0x5800bb3c, 0xfc1fffff,	RD_s|RD_t|WR_HILO,	0,		I3	},
{"ddivu",   "d,v,t",	0,    (int) M_DDIVU_3,	INSN_MACRO,		0,		I3	},
{"ddivu",   "d,v,I",	0,    (int) M_DDIVU_3I,	INSN_MACRO,		0,		I3	},
{"di",      "",		0x0000477c, 0xffffffff,	WR_s|RD_C0,		0,		I1	},
{"di",      "s",	0x0000477c, 0xffe0ffff,	WR_s|RD_C0,		0,		I1	},
{"dins",    "t,r,I,+I",	0,    (int) M_DINS,	INSN_MACRO,		0,		I3	},
{"dins",    "t,r,+A,+B",0x5800000c, 0xfc00003f, WR_t|RD_s,		0,		I3	},
{"dinsm",   "t,r,+A,+F",0x58000004, 0xfc00003f, WR_t|RD_s,		0,		I3	},
{"dinsu",   "t,r,+E,+F",0x58000034, 0xfc00003f, WR_t|RD_s,		0,		I3	},
/* The MIPS assembler treats the div opcode with two operands as
   though the first operand appeared twice (the first operand is both
   a source and a destination).  To get the div machine instruction,
   you must use an explicit destination of $0.  */
{"div",     "z,s,t",	0x0000ab3c, 0xfc00ffff,	RD_s|RD_t|WR_HILO,	0,		I1	},
{"div",     "z,t",	0x0000ab3c, 0xfc1fffff,	RD_s|RD_t|WR_HILO,	0,		I1	},
{"div",     "d,v,t",	0,    (int) M_DIV_3,	INSN_MACRO,		0,		I1	},
{"div",     "d,v,I",	0,    (int) M_DIV_3I,	INSN_MACRO,		0,		I1	},
{"div.d",   "D,V,T",	0x540001f0, 0xfc0007ff,	WR_D|RD_S|RD_T|FP_D,	0,		I1	},
{"div.s",   "D,V,T",	0x540000f0, 0xfc0007ff,	WR_D|RD_S|RD_T|FP_S,	0,		I1	},
/* For divu, see the comments about div.  */
{"divu",    "z,s,t",	0x0000bb3c, 0xfc00ffff,	RD_s|RD_t|WR_HILO,	0,		I1	},
{"divu",    "z,t",	0x0000bb3c, 0xfc1fffff,	RD_s|RD_t|WR_HILO,	0,		I1	},
{"divu",    "d,v,t",	0,    (int) M_DIVU_3,	INSN_MACRO,		0,		I1	},
{"divu",    "d,v,I",	0,    (int) M_DIVU_3I,	INSN_MACRO,		0,		I1	},
{"dla",     "t,A(b)",	0,    (int) M_DLA_AB,	INSN_MACRO,		0,		I3	},
{"dlca",    "t,A(b)",	0,    (int) M_DLCA_AB,	INSN_MACRO,		0,		I3	},
{"dli",     "t,j",	0x30000000, 0xfc1f0000,	WR_t,			0,		I3	}, /* addiu */
{"dli",     "t,i",	0x50000000, 0xfc1f0000,	WR_t,			0,		I3	}, /* ori */
{"dli",     "t,I",	0,    (int) M_DLI,	INSN_MACRO,		0,		I3	},
{"dmfc0",   "t,G",	0x580000fc, 0xfc00ffff,	WR_t|RD_C0,		0,		I3	},
{"dmfc0",   "t,G,H",	0x580000fc, 0xfc00c7ff,	WR_t|RD_C0,		0,		I3	},
{"dmfgc0",  "t,G",	0x580000e7, 0xfc00ffff,	WR_t|RD_C0,		0,		0,	IVIRT64	},
{"dmfgc0",  "t,G,H",	0x580000e7, 0xfc00c7ff,	WR_t|RD_C0,		0,		0,	IVIRT64	},
{"dmtc0",   "t,G",	0x580002fc, 0xfc00ffff,	RD_t|WR_C0|WR_CC,	0,		I3	},
{"dmtc0",   "t,G,H",	0x580002fc, 0xfc00c7ff,	RD_t|WR_C0|WR_CC,	0,		I3	},
{"dmtgc0",  "t,G",	0x580002e7, 0xfc00ffff,	RD_t|WR_C0|WR_CC,	0,		0,	IVIRT64	},
{"dmtgc0",  "t,G,H",	0x580002e7, 0xfc00c7ff,	RD_t|WR_C0|WR_CC,	0,		0,	IVIRT64	},
{"dmfc1",   "t,S",	0x5400243b, 0xfc00ffff,	WR_t|RD_S|FP_S,		0,		I3	},
{"dmfc1",   "t,G",	0x5400243b, 0xfc00ffff,	WR_t|RD_S|FP_S,		0,		I3	},
{"dmtc1",   "t,G",	0x54002c3b, 0xfc00ffff,	RD_t|WR_S|FP_S,		0,		I3	},
{"dmtc1",   "t,S",	0x54002c3b, 0xfc00ffff,	RD_t|WR_S|FP_S,		0,		I3	},
{"dmfc2",   "t,G",	0x00006d3c, 0xfc00ffff,	WR_t|RD_C2,		0,		I3	},
/*{"dmfc2",   "t,G,H",	0x58000283, 0xfc001fff,	WR_t|RD_C2,		0,		I3	},*/
{"dmtc2",   "t,G",	0x00007d3c, 0xfc00ffff,	RD_t|WR_C2|WR_CC,	0,		I3	},
/*{"dmtc2",   "t,G,H",	0x58000683, 0xfc001fff,	RD_t|WR_C2|WR_CC,	0,		I3	},*/
{"dmul",    "d,v,t",	0,    (int) M_DMUL,	INSN_MACRO,		0,		I3	},
{"dmul",    "d,v,I",	0,    (int) M_DMUL_I,	INSN_MACRO,		0,		I3	},
{"dmulo",   "d,v,t",	0,    (int) M_DMULO,	INSN_MACRO,		0,		I3	},
{"dmulo",   "d,v,I",	0,    (int) M_DMULO_I,	INSN_MACRO,		0,		I3	},
{"dmulou",  "d,v,t",	0,    (int) M_DMULOU,	INSN_MACRO,		0,		I3	},
{"dmulou",  "d,v,I",	0,    (int) M_DMULOU_I,	INSN_MACRO,		0,		I3	},
{"dmult",   "s,t",	0x58008b3c, 0xfc00ffff,	RD_s|RD_t|WR_HILO,	0,		I3	},
{"dmultu",  "s,t",	0x58009b3c, 0xfc00ffff,	RD_s|RD_t|WR_HILO,	0,		I3	},
{"dneg",    "d,w",	0x58000190, 0xfc1f07ff,	WR_d|RD_t,		0,		I3	}, /* dsub 0 */
{"dnegu",   "d,w",	0x580001d0, 0xfc1f07ff,	WR_d|RD_t,		0,		I3	}, /* dsubu 0 */
{"drem",    "z,s,t",	0x5800ab3c, 0xfc00ffff,	RD_s|RD_t|WR_HILO,	0,		I3	},
{"drem",    "d,v,t",	0,    (int) M_DREM_3,	INSN_MACRO,		0,		I3	},
{"drem",    "d,v,I",	0,    (int) M_DREM_3I,	INSN_MACRO,		0,		I3	},
{"dremu",   "z,s,t",	0x5800bb3c, 0xfc00ffff,	RD_s|RD_t|WR_HILO,	0,		I3	},
{"dremu",   "d,v,t",	0,    (int) M_DREMU_3,	INSN_MACRO,		0,		I3	},
{"dremu",   "d,v,I",	0,    (int) M_DREMU_3I,	INSN_MACRO,		0,		I3	},
{"drol",    "d,v,t",	0,    (int) M_DROL,	INSN_MACRO,		0,		I3	},
{"drol",    "d,v,I",	0,    (int) M_DROL_I,	INSN_MACRO,		0,		I3	},
{"dror",    "d,v,t",	0,    (int) M_DROR,	INSN_MACRO,		0,		I3	},
{"dror",    "d,v,I",	0,    (int) M_DROR_I,	INSN_MACRO,		0,		I3	},
{"dror",    "t,r,<",	0x580000c0, 0xfc0007ff,	WR_t|RD_s,		0,		I3	},
{"drorv",   "d,t,s",	0x580000d0, 0xfc0007ff,	RD_t|RD_s|WR_d,		0,		I3	},
{"dror32",  "t,r,<",	0x580000c8, 0xfc0007ff,	WR_t|RD_s,		0,		I3	},
{"drotl",   "d,v,t",	0,    (int) M_DROL,	INSN_MACRO,		0,		I3	},
{"drotl",   "d,v,I",	0,    (int) M_DROL_I,	INSN_MACRO,		0,		I3	},
{"drotr",   "d,v,t",	0,    (int) M_DROR,	INSN_MACRO,		0,		I3	},
{"drotr",   "d,v,I",	0,    (int) M_DROR_I,	INSN_MACRO,		0,		I3	},
{"drotrv",  "d,t,s",	0x580000d0, 0xfc0007ff,	RD_t|RD_s|WR_d,		0,		I3	},
{"drotr32", "t,r,<",	0x580000c8, 0xfc0007ff,	WR_t|RD_s,		0,		I3	},
{"dsbh",    "t,r",	0x58007b3c, 0xfc00ffff,	WR_t|RD_s,		0,		I3	},
{"dshd",    "t,r",	0x5800fb3c, 0xfc00ffff,	WR_t|RD_s,		0,		I3	},
{"dsllv",   "d,t,s",	0x58000010, 0xfc0007ff,	WR_d|RD_t|RD_s,		0,		I3	},
{"dsll32",  "t,r,<",	0x58000008, 0xfc0007ff,	WR_t|RD_s,		0,		I3	},
{"dsll",    "d,t,s",	0x58000010, 0xfc0007ff,	WR_d|RD_t|RD_s,		0,		I3	}, /* dsllv */
{"dsll",    "t,r,>",	0x58000008, 0xfc0007ff,	WR_t|RD_s,		0,		I3	}, /* dsll32 */
{"dsll",    "t,r,<",	0x58000000, 0xfc0007ff,	WR_t|RD_s,		0,		I3	},
{"dsrav",   "d,t,s",	0x58000090, 0xfc0007ff,	WR_d|RD_t|RD_s,		0,		I3	},
{"dsra32",  "t,r,<",	0x58000088, 0xfc0007ff,	WR_t|RD_s,		0,		I3	},
{"dsra",    "d,t,s",	0x58000090, 0xfc0007ff,	WR_d|RD_t|RD_s,		0,		I3	}, /* dsrav */
{"dsra",    "t,r,>",	0x58000088, 0xfc0007ff,	WR_t|RD_s,		0,		I3	}, /* dsra32 */
{"dsra",    "t,r,<",	0x58000080, 0xfc0007ff,	WR_t|RD_s,		0,		I3	},
{"dsrlv",   "d,t,s",	0x58000050, 0xfc0007ff,	WR_d|RD_t|RD_s,		0,		I3	},
{"dsrl32",  "t,r,<",	0x58000048, 0xfc0007ff,	WR_t|RD_s,		0,		I3	},
{"dsrl",    "d,t,s",	0x58000050, 0xfc0007ff,	WR_d|RD_t|RD_s,		0,		I3	}, /* dsrlv */
{"dsrl",    "t,r,>",	0x58000048, 0xfc0007ff,	WR_t|RD_s,		0,		I3	}, /* dsrl32 */
{"dsrl",    "t,r,<",	0x58000040, 0xfc0007ff,	WR_t|RD_s,		0,		I3	},
{"dsub",    "d,v,t",	0x58000190, 0xfc0007ff,	WR_d|RD_s|RD_t,		0,		I3	},
{"dsub",    "d,v,I",	0,    (int) M_DSUB_I,	INSN_MACRO,		0,		I3	},
{"dsubu",   "d,v,t",	0x580001d0, 0xfc0007ff,	WR_d|RD_s|RD_t,		0,		I3	},
{"dsubu",   "d,v,I",	0,    (int) M_DSUBU_I,	INSN_MACRO,		0,		I3	},
{"ei",      "",		0x0000577c, 0xffffffff,	WR_s|WR_C0,		0,		I1	},
{"ei",      "s",	0x0000577c, 0xffe0ffff,	WR_s|WR_C0,		0,		I1	},
{"eret",    "",		0x0000f37c, 0xffffffff,	NODS,			0,		I1	},
{"ext",     "t,r,+A,+C", 0x0000002c, 0xfc00003f, WR_t|RD_s,		0,		I1	},
{"floor.l.d", "T,V",	0x5400433b, 0xfc00ffff,	WR_T|RD_S|FP_D,		0,		I1	},
{"floor.l.s", "T,V",	0x5400033b, 0xfc00ffff,	WR_T|RD_S|FP_S|FP_D,	0,		I1	},
{"floor.w.d", "T,V",	0x54004b3b, 0xfc00ffff,	WR_T|RD_S|FP_S|FP_D,	0,		I1	},
{"floor.w.s", "T,V",	0x54000b3b, 0xfc00ffff,	WR_T|RD_S|FP_S,		0,		I1	},
{"hypcall",   "",	0x0000c37c, 0xffffffff,	TRAP,			0,		0,	IVIRT	},
{"hypcall",   "B",	0x0000c37c, 0xfc00ffff,	TRAP,			0,		0,	IVIRT	},
{"ins",     "t,r,+A,+B", 0x0000000c, 0xfc00003f, WR_t|RD_s,		0,		I1	},
{"iret",    "",		0x0000d37c, 0xffffffff,	NODS,			0,		0,	MC	},
{"jr",      "mj",	    0x4580,     0xffe0,	UBD,			RD_mj,		I1	},
{"jr",      "s",	0x00000f3c, 0xffe0ffff,	UBD|RD_s,		BD32,		I1	}, /* jalr */
{"jrs",     "s",	0x00004f3c, 0xffe0ffff,	UBD|RD_s,		BD16,		I1	}, /* jalrs */
{"jraddiusp", "mP",	    0x4700,     0xffe0,	NODS,			UBR|RD_31|WR_sp|RD_sp,	I1	},
/* This macro is after the real instruction so that it only matches with
   -minsn32.  */
{"jraddiusp", "mP",	0,   (int) M_JRADDIUSP,	INSN_MACRO,		0,		I1	},
{"jrc",     "mj",	    0x45a0,     0xffe0,	NODS,			UBR|RD_mj,		I1	},
/* This macro is after the real instruction so that it only matches with
   -minsn32.  */
{"jrc",     "s",	0,    (int) M_JRC,	INSN_MACRO,		0,		I1	},
{"jr.hb",   "s",	0x00001f3c, 0xffe0ffff,	UBD|RD_s,		BD32,		I1	}, /* jalr.hb */
{"jrs.hb",  "s",	0x00005f3c, 0xffe0ffff,	UBD|RD_s,		BD16,		I1	}, /* jalrs.hb */
{"j",       "mj",	    0x4580,     0xffe0,	UBD,			RD_mj,		I1	}, /* jr */
{"j",       "s",	0x00000f3c, 0xffe0ffff,	UBD|RD_s,		BD32,		I1	}, /* jr */
/* SVR4 PIC code requires special handling for j, so it must be a
   macro.  */
{"j",       "a",	0,    (int) M_J_A,	INSN_MACRO,		0,		I1	},
/* This form of j is used by the disassembler and internally by the
   assembler, but will never match user input (because the line above
   will match first).  */
{"j",       "a",	0xd4000000, 0xfc000000,	UBD,			0,		I1	},
{"jalr",    "mj",	    0x45c0,     0xffe0,	UBD|WR_31,		RD_mj|BD32,	I1	},
{"jalr",    "my,mj",	    0x45c0,     0xffe0,	UBD|WR_31,		RD_mj|BD32,	I1	},
{"jalr",    "s",	0x03e00f3c, 0xffe0ffff,	UBD|RD_s|WR_t,		BD32,		I1	},
{"jalr",    "t,s",	0x00000f3c, 0xfc00ffff,	UBD|RD_s|WR_t,		BD32,		I1	},
{"jalr.hb", "s",	0x03e01f3c, 0xffe0ffff,	UBD|RD_s|WR_t,		BD32,		I1	},
{"jalr.hb", "t,s",	0x00001f3c, 0xfc00ffff,	UBD|RD_s|WR_t,		BD32,		I1	},
{"jalrs",   "mj",	    0x45e0,     0xffe0,	UBD|WR_31,		RD_mj|BD16,	I1	},
{"jalrs",   "my,mj",	    0x45e0,     0xffe0,	UBD|WR_31,		RD_mj|BD16,	I1	},
{"jalrs",   "s",	0x03e04f3c, 0xffe0ffff,	UBD|RD_s|WR_t,		BD16,		I1	},
{"jalrs",   "t,s",	0x00004f3c, 0xfc00ffff,	UBD|RD_s|WR_t,		BD16,		I1	},
{"jalrs.hb", "s",	0x03e05f3c, 0xffe0ffff,	UBD|RD_s|WR_t,		BD16,		I1	},
{"jalrs.hb", "t,s",	0x00005f3c, 0xfc00ffff,	UBD|RD_s|WR_t,		BD16,		I1	},
/* SVR4 PIC code requires special handling for jal, so it must be a
   macro.  */
{"jal",     "d,s",	0,    (int) M_JAL_2,	INSN_MACRO,		0,		I1	},
{"jal",     "s",	0,    (int) M_JAL_1,	INSN_MACRO,		0,		I1	},
{"jal",     "a",	0,    (int) M_JAL_A,	INSN_MACRO,		0,		I1	},
/* This form of jal is used by the disassembler and internally by the
   assembler, but will never match user input (because the line above
   will match first).  */
{"jal",     "a",	0xf4000000, 0xfc000000,	UBD|WR_31,		BD32,		I1	},
{"jals",    "d,s",	0,    (int) M_JALS_2,	INSN_MACRO,		0,		I1	},
{"jals",    "s",	0,    (int) M_JALS_1,	INSN_MACRO,		0,		I1	},
{"jals",    "a",	0,    (int) M_JALS_A,	INSN_MACRO,		0,		I1	},
{"jals",    "a",	0x74000000, 0xfc000000,	UBD|WR_31,		BD16,		I1	},
{"jalx",    "+i",	0xf0000000, 0xfc000000,	UBD|WR_31,		BD32,		I1	},
{"la",      "t,A(b)",	0,    (int) M_LA_AB,	INSN_MACRO,		0,		I1	},
{"lb",      "t,o(b)",	0x1c000000, 0xfc000000,	RD_b|WR_t,		0,		I1	},
{"lb",      "t,A(b)",	0,    (int) M_LB_AB,	INSN_MACRO,		0,		I1	},
{"lbu",     "md,mG(ml)",    0x0800,     0xfc00,	0,			WR_md|RD_ml,	I1	},
{"lbu",     "t,o(b)",	0x14000000, 0xfc000000,	RD_b|WR_t,		0,		I1	},
{"lbu",     "t,A(b)",	0,    (int) M_LBU_AB,	INSN_MACRO,		0,		I1	},
{"lca",     "t,A(b)",	0,    (int) M_LCA_AB,	INSN_MACRO,		0,		I1	},
/* The macro has to be first to handle o32 correctly.  */
{"ld",      "t,A(b)",	0,    (int) M_LD_AB,	INSN_MACRO,		0,		I1	},
{"ld",      "t,o(b)",	0xdc000000, 0xfc000000,	RD_b|WR_t,		0,		I3	},
{"ldc1",    "T,o(b)",	0xbc000000, 0xfc000000,	RD_b|WR_T|FP_D,		0,		I1	},
{"ldc1",    "E,o(b)",	0xbc000000, 0xfc000000,	RD_b|WR_T|FP_D,		0,		I1	},
{"ldc1",    "T,A(b)",	0,    (int) M_LDC1_AB,	INSN_MACRO,		INSN2_M_FP_D,	I1	},
{"ldc1",    "E,A(b)",	0,    (int) M_LDC1_AB,	INSN_MACRO,		INSN2_M_FP_D,	I1	},
{"ldc2",    "E,~(b)",	0x20002000, 0xfc00f000,	RD_b|WR_CC,		0,		I1	},
{"ldc2",    "E,A(b)",	0,    (int) M_LDC2_AB,	INSN_MACRO,		0,		I1	},
{"l.d",     "T,o(b)",	0xbc000000, 0xfc000000,	RD_b|WR_T|FP_D,	0,		I1	}, /* ldc1 */
{"l.d",     "T,A(b)",	0,    (int) M_LDC1_AB,	INSN_MACRO,		INSN2_M_FP_D,	I1	},
{"ldl",     "t,~(b)",	0x60004000, 0xfc00f000,	WR_t|RD_b,		0,		I3	},
{"ldl",     "t,A(b)",	0,    (int) M_LDL_AB,	INSN_MACRO,		0,		I3	},
{"ldm",     "n,~(b)",	0x20007000, 0xfc00f000,	RD_b,			0,		I3	},
{"ldm",     "n,A(b)",	0,    (int) M_LDM_AB,	INSN_MACRO,		0,		I3	},
{"ldp",     "t,~(b)",	0x20004000, 0xfc00f000,	RD_b|WR_t,		0,		I3	},
{"ldp",     "t,A(b)",	0,    (int) M_LDP_AB,	INSN_MACRO,		0,		I3	},
{"ldr",     "t,~(b)",	0x60005000, 0xfc00f000,	WR_t|RD_b,		0,		I3	},
{"ldr",     "t,A(b)",	0,    (int) M_LDR_AB,	INSN_MACRO,		0,		I3	},
{"ldxc1",   "D,t(b)",	0x540000c8, 0xfc0007ff,	WR_D|RD_t|RD_b|FP_D,	0,		I1	},
{"lh",      "t,o(b)",	0x3c000000, 0xfc000000,	RD_b|WR_t,		0,		I1	},
{"lh",      "t,A(b)",	0,    (int) M_LH_AB,	INSN_MACRO,		0,		I1	},
{"lhu",     "md,mH(ml)",    0x2800,     0xfc00,	0,			WR_md|RD_ml,	I1	},
{"lhu",     "t,o(b)",	0x34000000, 0xfc000000,	RD_b|WR_t,		0,		I1	},
{"lhu",     "t,A(b)",	0,    (int) M_LHU_AB,	INSN_MACRO,		0,		I1	},
/* li is at the start of the table.  */
{"li.d",    "t,F",	0,    (int) M_LI_D,	INSN_MACRO,		INSN2_M_FP_D,	I1	},
{"li.d",    "T,L",	0,    (int) M_LI_DD,	INSN_MACRO,		INSN2_M_FP_D,	I1	},
{"li.s",    "t,f",	0,    (int) M_LI_S,	INSN_MACRO,		INSN2_M_FP_S,	I1	},
{"li.s",    "T,l",	0,    (int) M_LI_SS,	INSN_MACRO,		INSN2_M_FP_S,	I1	},
{"ll",      "t,~(b)",	0x60003000, 0xfc00f000,	RD_b|WR_t,		0,		I1	},
{"ll",      "t,A(b)",	0,    (int) M_LL_AB,	INSN_MACRO,		0,		I1	},
{"lld",     "t,~(b)",	0x60007000, 0xfc00f000,	RD_b|WR_t,		0,		I3	},
{"lld",     "t,A(b)",	0,    (int) M_LLD_AB,	INSN_MACRO,		0,		I3	},
{"lui",     "s,u",	0x41a00000, 0xffe00000,	WR_s,			0,		I1	},
{"luxc1",   "D,t(b)",	0x54000148, 0xfc0007ff,	WR_D|RD_t|RD_b|FP_D,	0,		I1	},
{"lw",      "md,mJ(ml)",    0x6800,     0xfc00,	0,			WR_md|RD_ml,	I1	},
{"lw",      "mp,mU(ms)",    0x4800,     0xfc00,	0,			WR_mp|RD_sp,	I1	}, /* lwsp */
{"lw",      "md,mA(ma)",    0x6400,     0xfc00,	0,			WR_md|RD_gp,	I1	}, /* lwgp */
{"lw",      "t,o(b)",	0xfc000000, 0xfc000000,	RD_b|WR_t,		0,		I1	},
{"lw",      "t,A(b)",	0,    (int) M_LW_AB,	INSN_MACRO,		0,		I1	},
{"lwc1",    "T,o(b)",	0x9c000000, 0xfc000000,	RD_b|WR_T|FP_S,		0,		I1	},
{"lwc1",    "E,o(b)",	0x9c000000, 0xfc000000,	RD_b|WR_T|FP_S,		0,		I1	},
{"lwc1",    "T,A(b)",	0,    (int) M_LWC1_AB,	INSN_MACRO,		INSN2_M_FP_S,	I1	},
{"lwc1",    "E,A(b)",	0,    (int) M_LWC1_AB,	INSN_MACRO,		INSN2_M_FP_S,	I1	},
{"lwc2",    "E,~(b)",	0x20000000, 0xfc00f000,	RD_b|WR_CC,		0,		I1	},
{"lwc2",    "E,A(b)",	0,    (int) M_LWC2_AB,	INSN_MACRO,		0,		I1	},
{"l.s",     "T,o(b)",	0x9c000000, 0xfc000000,	RD_b|WR_T|FP_S,		0,		I1	}, /* lwc1 */
{"l.s",     "T,A(b)",	0,    (int) M_LWC1_AB,	INSN_MACRO,		INSN2_M_FP_S,	I1	},
{"lwl",     "t,~(b)",	0x60000000, 0xfc00f000,	RD_b|WR_t,		0,		I1	},
{"lwl",     "t,A(b)",	0,    (int) M_LWL_AB,	INSN_MACRO,		0,		I1	},
{"lcache",  "t,~(b)",	0x60000000, 0xfc00f000,	RD_b|WR_t,		0,		I1	}, /* same */
{"lcache",  "t,A(b)",	0,    (int) M_LWL_AB,	INSN_MACRO,		0,		I1	},
{"lwm",     "mN,mJ(ms)",    0x4500,     0xffc0,	NODS,			RD_sp,		I1	},
{"lwm",     "n,~(b)",	0x20005000, 0xfc00f000,	RD_b|NODS,		0,		I1	},
{"lwm",     "n,A(b)",	0,    (int) M_LWM_AB,	INSN_MACRO,		0,		I1	},
{"lwp",     "t,~(b)",	0x20001000, 0xfc00f000,	RD_b|WR_t|NODS,		0,		I1	},
{"lwp",     "t,A(b)",	0,    (int) M_LWP_AB,	INSN_MACRO,		0,		I1	},
{"lwr",     "t,~(b)",	0x60001000, 0xfc00f000,	RD_b|WR_t,		0,		I1	},
{"lwr",     "t,A(b)",	0,    (int) M_LWR_AB,	INSN_MACRO,		0,		I1	},
{"lwu",     "t,~(b)",	0x6000e000, 0xfc00f000,	RD_b|WR_t,		0,		I3	},
{"lwu",     "t,A(b)",	0,    (int) M_LWU_AB,	INSN_MACRO,		0,		I3	},
{"lwxc1",   "D,t(b)",	0x54000048, 0xfc0007ff,	WR_D|RD_t|RD_b|FP_S,	0,		I1	},
{"flush",   "t,~(b)",	0x60001000, 0xfc00f000,	RD_b|WR_t,		0,		I1	}, /* same */
{"flush",   "t,A(b)",	0,    (int) M_LWR_AB,	INSN_MACRO,		0,		I1	},
{"lwxs",    "d,t(b)",	0x00000118, 0xfc0007ff,	RD_b|RD_t|WR_d,		0,		I1	},
{"madd",    "s,t",	0x0000cb3c, 0xfc00ffff,	RD_s|RD_t|MOD_HILO,	0,		I1	},
{"madd",    "7,s,t",	0x00000abc, 0xfc003fff,	MOD_a|RD_s|RD_t,	0,		0,	D32	},
{"madd.d",  "D,R,S,T",	0x54000009, 0xfc00003f,	RD_R|RD_S|RD_T|WR_D|FP_D, 0,		I1	},
{"madd.s",  "D,R,S,T",	0x54000001, 0xfc00003f,	RD_R|RD_S|RD_T|WR_D|FP_S, 0,		I1	},
{"madd.ps", "D,R,S,T",	0x54000011, 0xfc00003f,	RD_R|RD_S|RD_T|WR_D|FP_D, 0,		I1	},
{"maddu",   "s,t",	0x0000db3c, 0xfc00ffff,	RD_s|RD_t|MOD_HILO,	0,		I1	},
{"maddu",   "7,s,t",	0x00001abc, 0xfc003fff,	MOD_a|RD_s|RD_t,	0,		0,	D32	},
{"mfc0",    "t,G",	0x000000fc, 0xfc00ffff,	WR_t|RD_C0,		0,		I1	},
{"mfc0",    "t,G,H",	0x000000fc, 0xfc00c7ff,	WR_t|RD_C0,		0,		I1	},
{"mfc1",    "t,S",	0x5400203b, 0xfc00ffff,	WR_t|RD_S|FP_S,		0,		I1	},
{"mfc1",    "t,G",	0x5400203b, 0xfc00ffff,	WR_t|RD_S|FP_S,		0,		I1	},
{"mfc2",    "t,G",	0x00004d3c, 0xfc00ffff,	WR_t|RD_C2,		0,		I1	},
{"mfgc0",   "t,G",	0x000004fc, 0xfc00ffff,	WR_t|RD_C0,		0,		0,	IVIRT	},
{"mfgc0",   "t,G,H",	0x000004fc, 0xfc00c7ff,	WR_t|RD_C0,		0,		0,	IVIRT	},
{"mfhc1",   "t,S",	0x5400303b, 0xfc00ffff,	WR_t|RD_S|FP_D,		0,		I1	},
{"mfhc1",   "t,G",	0x5400303b, 0xfc00ffff,	WR_t|RD_S|FP_D,		0,		I1	},
{"mfhc2",   "t,G",	0x00008d3c, 0xfc00ffff,	WR_t|RD_C2,		0,		I1	},
{"mfhi",    "mj",	    0x4600,     0xffe0,	RD_HI,			WR_mj,		I1	},
{"mfhi",    "s",	0x00000d7c, 0xffe0ffff,	WR_s|RD_HI,		0,		I1	},
{"mfhi",    "s,7",	0x0000007c, 0xffe03fff,	WR_s|RD_HI,		0,		0,	D32	},
{"mflo",    "mj",	    0x4640,     0xffe0,	RD_LO,			WR_mj,		I1	},
{"mflo",    "s",	0x00001d7c, 0xffe0ffff,	WR_s|RD_LO,		0,		I1	},
{"mflo",    "s,7",	0x0000107c, 0xffe03fff,	WR_s|RD_LO,		0,		0,	D32	},
{"mov.d",   "T,S",	0x5400207b, 0xfc00ffff,	WR_T|RD_S|FP_D,		0,		I1	},
{"mov.s",   "T,S",	0x5400007b, 0xfc00ffff,	WR_T|RD_S|FP_S,		0,		I1	},
{"mov.ps",  "T,S",	0x5400407b, 0xfc00ffff,	WR_T|RD_S|FP_D,		0,		I1	},
{"movep",   "mh,mm,mn",     0x8400,     0xfc01,	NODS,			WR_mh|RD_mmn,	I1	},
/* This macro is after the real instruction so that it only matches with
   -minsn32.  */
{"movep",   "mh,mm,mn", 0, (int) M_MOVEP,	INSN_MACRO,		0,		I1	},
{"movf",    "t,s,M",	0x5400017b, 0xfc001fff,	WR_t|RD_s|RD_CC|FP_S|FP_D, 0,		I1	},
{"movf.d",  "T,S,M",	0x54000220, 0xfc001fff,	WR_T|RD_S|RD_CC|FP_D,	0,		I1	},
{"movf.s",  "T,S,M",	0x54000020, 0xfc001fff,	WR_T|RD_S|RD_CC|FP_S,	0,		I1	},
{"movf.ps", "T,S,M",	0x54000420, 0xfc001fff,	WR_T|RD_S|RD_CC|FP_D,	0,		I1	},
{"movn",    "d,v,t",	0x00000018, 0xfc0007ff,	WR_d|RD_s|RD_t,		0,		I1	},
{"movn.d",  "D,S,t",	0x54000138, 0xfc0007ff,	WR_D|RD_S|RD_t|FP_D,	0,		I1	},
{"movn.s",  "D,S,t",	0x54000038, 0xfc0007ff,	WR_D|RD_S|RD_t|FP_S,	0,		I1	},
{"movn.ps", "D,S,t",	0x54000238, 0xfc0007ff,	WR_D|RD_S|RD_t|FP_D,	0,		I1	},
{"movt",    "t,s,M",	0x5400097b, 0xfc001fff,	WR_t|RD_s|RD_CC|FP_S|FP_D, 0,		I1	},
{"movt.d",  "T,S,M",	0x54000260, 0xfc001fff,	WR_T|RD_S|RD_CC|FP_D,	0,		I1	},
{"movt.s",  "T,S,M",	0x54000060, 0xfc001fff,	WR_T|RD_S|RD_CC|FP_S,	0,		I1	},
{"movt.ps", "T,S,M",	0x54000460, 0xfc001fff,	WR_T|RD_S|RD_CC|FP_D,	0,		I1	},
{"movz",    "d,v,t",	0x00000058, 0xfc0007ff,	WR_d|RD_s|RD_t,		0,		I1	},
{"movz.d",  "D,S,t",	0x54000178, 0xfc0007ff,	WR_D|RD_S|RD_t|FP_D,	0,		I1	},
{"movz.s",  "D,S,t",	0x54000078, 0xfc0007ff,	WR_D|RD_S|RD_t|FP_S,	0,		I1	},
{"movz.ps", "D,S,t",	0x54000278, 0xfc0007ff,	WR_D|RD_S|RD_t|FP_D,	0,		I1	},
{"msub",    "s,t",	0x0000eb3c, 0xfc00ffff,	RD_s|RD_t|MOD_HILO,	0,		I1	},
{"msub",    "7,s,t",	0x00002abc, 0xfc003fff,	MOD_a|RD_s|RD_t,	0,		0,	D32	},
{"msub.d",  "D,R,S,T",	0x54000029, 0xfc00003f,	RD_R|RD_S|RD_T|WR_D|FP_D, 0,		I1	},
{"msub.s",  "D,R,S,T",	0x54000021, 0xfc00003f,	RD_R|RD_S|RD_T|WR_D|FP_S, 0,		I1	},
{"msub.ps", "D,R,S,T",	0x54000031, 0xfc00003f,	RD_R|RD_S|RD_T|WR_D|FP_D, 0,		I1	},
{"msubu",   "s,t",	0x0000fb3c, 0xfc00ffff,	RD_s|RD_t|MOD_HILO,	0,		I1	},
{"msubu",   "7,s,t",	0x00003abc, 0xfc003fff,	MOD_a|RD_s|RD_t,	0,		0,	D32	},
{"mtc0",    "t,G",	0x000002fc, 0xfc00ffff,	RD_t|WR_C0|WR_CC,	0,		I1	},
{"mtc0",    "t,G,H",	0x000002fc, 0xfc00c7ff,	RD_t|WR_C0|WR_CC,	0,		I1	},
{"mtc1",    "t,S",	0x5400283b, 0xfc00ffff,	RD_t|WR_S|FP_S,		0,		I1	},
{"mtc1",    "t,G",	0x5400283b, 0xfc00ffff,	RD_t|WR_S|FP_S,		0,		I1	},
{"mtc2",    "t,G",	0x00005d3c, 0xfc00ffff,	RD_t|WR_C2|WR_CC,	0,		I1	},
{"mtgc0",   "t,G",	0x000006fc, 0xfc00ffff,	RD_t|WR_C0|WR_CC,	0,		0,	IVIRT	},
{"mtgc0",   "t,G,H",	0x000006fc, 0xfc00c7ff,	RD_t|WR_C0|WR_CC,	0,		0,	IVIRT	},
{"mthc1",   "t,S",	0x5400383b, 0xfc00ffff,	RD_t|WR_S|FP_D,		0,		I1	},
{"mthc1",   "t,G",	0x5400383b, 0xfc00ffff,	RD_t|WR_S|FP_D,		0,		I1	},
{"mthc2",   "t,G",	0x00009d3c, 0xfc00ffff,	RD_t|WR_C2|WR_CC,	0,		I1	},
{"mthi",    "s",	0x00002d7c, 0xffe0ffff,	RD_s|WR_HI,		0,		I1	},
{"mthi",    "s,7",	0x0000207c, 0xffe03fff,	RD_s|WR_HI,		0,		0,	D32	},
{"mtlo",    "s",	0x00003d7c, 0xffe0ffff,	RD_s|WR_LO,		0,		I1	},
{"mtlo",    "s,7",	0x0000307c, 0xffe03fff,	RD_s|WR_LO,		0,		0,	D32	},
{"mul",     "d,v,t",	0x00000210, 0xfc0007ff,	WR_d|RD_s|RD_t|WR_HILO,	0,		I1	},
{"mul",     "d,v,I",	0,    (int) M_MUL_I,	INSN_MACRO,		0,		I1	},
{"mul.d",   "D,V,T",	0x540001b0, 0xfc0007ff,	WR_D|RD_S|RD_T|FP_D,	0,		I1	},
{"mul.s",   "D,V,T",	0x540000b0, 0xfc0007ff,	WR_D|RD_S|RD_T|FP_S,	0,		I1	},
{"mul.ps",  "D,V,T",	0x540002b0, 0xfc0007ff,	WR_D|RD_S|RD_T|FP_D,	0,		I1	},
{"mulo",    "d,v,t",	0,    (int) M_MULO,	INSN_MACRO,		0,		I1	},
{"mulo",    "d,v,I",	0,    (int) M_MULO_I,	INSN_MACRO,		0,		I1	},
{"mulou",   "d,v,t",	0,    (int) M_MULOU,	INSN_MACRO,		0,		I1	},
{"mulou",   "d,v,I",	0,    (int) M_MULOU_I,	INSN_MACRO,		0,		I1	},
{"mult",    "s,t",	0x00008b3c, 0xfc00ffff,	RD_s|RD_t|WR_HILO,	0,		I1	},
{"mult",    "7,s,t",	0x00000cbc, 0xfc003fff,	WR_a|RD_s|RD_t,		0,		0,	D32	},
{"multu",   "s,t",	0x00009b3c, 0xfc00ffff,	RD_s|RD_t|WR_HILO,	0,		I1	},
{"multu",   "7,s,t",	0x00001cbc, 0xfc003fff,	WR_a|RD_s|RD_t,		0,		0,	D32	},
{"neg",     "d,w",	0x00000190, 0xfc1f07ff,	WR_d|RD_t,		0,		I1	}, /* sub 0 */
{"negu",    "d,w",	0x000001d0, 0xfc1f07ff,	WR_d|RD_t,		0,		I1	}, /* subu 0 */
{"neg.d",   "T,V",	0x54002b7b, 0xfc00ffff,	WR_T|RD_S|FP_D,		0,		I1	},
{"neg.s",   "T,V",	0x54000b7b, 0xfc00ffff,	WR_T|RD_S|FP_S,		0,		I1	},
{"neg.ps",  "T,V",	0x54004b7b, 0xfc00ffff,	WR_T|RD_S|FP_D,		0,		I1	},
{"nmadd.d", "D,R,S,T",	0x5400000a, 0xfc00003f,	RD_R|RD_S|RD_T|WR_D|FP_D, 0,		I1	},
{"nmadd.s", "D,R,S,T",	0x54000002, 0xfc00003f,	RD_R|RD_S|RD_T|WR_D|FP_S, 0,		I1	},
{"nmadd.ps", "D,R,S,T",	0x54000012, 0xfc00003f,	RD_R|RD_S|RD_T|WR_D|FP_D, 0,		I1	},
{"nmsub.d", "D,R,S,T",	0x5400002a, 0xfc00003f,	RD_R|RD_S|RD_T|WR_D|FP_D, 0,		I1	},
{"nmsub.s", "D,R,S,T",	0x54000022, 0xfc00003f,	RD_R|RD_S|RD_T|WR_D|FP_S, 0,		I1	},
{"nmsub.ps", "D,R,S,T",	0x54000032, 0xfc00003f,	RD_R|RD_S|RD_T|WR_D|FP_D, 0,		I1	},
/* nop is at the start of the table.  */
{"not",     "mf,mg",	    0x4400,     0xffc0,	0,			WR_mf|RD_mg,	I1	}, /* put not before nor */
{"not",     "d,v",	0x000002d0, 0xffe007ff,	WR_d|RD_s|RD_t,		0,		I1	}, /* nor d,s,0 */
{"nor",     "mf,mz,mg",	    0x4400,     0xffc0,	0,			WR_mf|RD_mg,	I1	}, /* not */
{"nor",     "mf,mg,mz",	    0x4400,     0xffc0,	0,			WR_mf|RD_mg,	I1	}, /* not */
{"nor",     "d,v,t",	0x000002d0, 0xfc0007ff,	WR_d|RD_s|RD_t,		0,		I1	},
{"nor",     "t,r,I",	0,    (int) M_NOR_I,	INSN_MACRO,		0,		I1	},
{"or",      "mp,mj,mz",	    0x0c00,     0xfc00,	0,			WR_mp|RD_mj,	I1	}, /* move */
{"or",      "mp,mz,mj",	    0x0c00,     0xfc00,	0,			WR_mp|RD_mj,	I1	}, /* move */
{"or",      "mf,mt,mg",	    0x44c0,     0xffc0,	0,			WR_mf|RD_mf|RD_mg,	I1	},
{"or",      "mf,mg,mx",	    0x44c0,     0xffc0,	0,			WR_mf|RD_mf|RD_mg,	I1	},
{"or",      "d,v,t",	0x00000290, 0xfc0007ff,	WR_d|RD_s|RD_t,		0,		I1	},
{"or",      "t,r,I",	0,    (int) M_OR_I,	INSN_MACRO,		0,		I1	},
{"ori",     "mp,mj,mZ",	    0x0c00,     0xfc00,	0,			WR_mp|RD_mj,	I1	}, /* move */
{"ori",     "t,r,i",	0x50000000, 0xfc000000,	WR_t|RD_s,		0,		I1	},
{"pll.ps",  "D,V,T",	0x54000080, 0xfc0007ff,	WR_D|RD_S|RD_T|FP_D,	0,		I1	},
{"plu.ps",  "D,V,T",	0x540000c0, 0xfc0007ff,	WR_D|RD_S|RD_T|FP_D,	0,		I1	},
{"pul.ps",  "D,V,T",	0x54000100, 0xfc0007ff,	WR_D|RD_S|RD_T|FP_D,	0,		I1	},
{"puu.ps",  "D,V,T",	0x54000140, 0xfc0007ff,	WR_D|RD_S|RD_T|FP_D,	0,		I1	},
/* pref is at the start of the table.  */
{"recip.d", "T,S",	0x5400523b, 0xfc00ffff,	WR_T|RD_S|FP_D,		0,		I1	},
{"recip.s", "T,S",	0x5400123b, 0xfc00ffff,	WR_T|RD_S|FP_S,		0,		I1	},
{"rem",     "z,s,t",	0x0000ab3c, 0xfc00ffff,	RD_s|RD_t|WR_HILO,	0,		I1	},
{"rem",     "d,v,t",	0,    (int) M_REM_3,	INSN_MACRO,		0,		I1	},
{"rem",     "d,v,I",	0,    (int) M_REM_3I,	INSN_MACRO,		0,		I1	},
{"remu",    "z,s,t",	0x0000bb3c, 0xfc00ffff,	RD_s|RD_t|WR_HILO,	0,		I1	},
{"remu",    "d,v,t",	0,    (int) M_REMU_3,	INSN_MACRO,		0,		I1	},
{"remu",    "d,v,I",	0,    (int) M_REMU_3I,	INSN_MACRO,		0,		I1	},
{"rdhwr",   "t,K",	0x00006b3c, 0xfc00ffff,	0,			WR_t,		I1	},
{"rdpgpr",  "t,r",	0x0000e17c, 0xfc00ffff,	WR_t,			0,		I1	},
{"rol",     "d,v,t",	0,    (int) M_ROL,	INSN_MACRO,		0,		I1	},
{"rol",     "d,v,I",	0,    (int) M_ROL_I,	INSN_MACRO,		0,		I1	},
{"ror",     "d,v,t",	0,    (int) M_ROR,	INSN_MACRO,		0,		I1	},
{"ror",     "d,v,I",	0,    (int) M_ROR_I,	INSN_MACRO,		0,		I1	},
{"ror",     "t,r,<",	0x000000c0, 0xfc0007ff,	WR_t|RD_s,		0,		I1	},
{"rorv",    "d,t,s",	0x000000d0, 0xfc0007ff,	RD_t|RD_s|WR_d,		0,		I1	},
{"rotl",    "d,v,t",	0,    (int) M_ROL,	INSN_MACRO,		0,		I1	},
{"rotl",    "d,v,I",	0,    (int) M_ROL_I,	INSN_MACRO,		0,		I1	},
{"rotr",    "d,v,t",	0,    (int) M_ROR,	INSN_MACRO,		0,		I1	},
{"rotr",    "t,r,<",	0x000000c0, 0xfc0007ff,	WR_t|RD_s,		0,		I1	},
{"rotrv",   "d,t,s",	0x000000d0, 0xfc0007ff,	RD_t|RD_s|WR_d,		0,		I1	},
{"round.l.d", "T,S",	0x5400733b, 0xfc00ffff,	WR_T|RD_S|FP_D,		0,		I1	},
{"round.l.s", "T,S",	0x5400333b, 0xfc00ffff,	WR_T|RD_S|FP_S|FP_D,	0,		I1	},
{"round.w.d", "T,S",	0x54007b3b, 0xfc00ffff,	WR_T|RD_S|FP_S|FP_D,	0,		I1	},
{"round.w.s", "T,S",	0x54003b3b, 0xfc00ffff,	WR_T|RD_S|FP_S,		0,		I1	},
{"rsqrt.d", "T,S",	0x5400423b, 0xfc00ffff,	WR_T|RD_S|FP_D,		0,		I1	},
{"rsqrt.s", "T,S",	0x5400023b, 0xfc00ffff,	WR_T|RD_S|FP_S,		0,		I1	},
{"sb",      "mq,mL(ml)",    0x8800,     0xfc00,	SM,			RD_mq|RD_ml,		I1	},
{"sb",      "t,o(b)",	0x18000000, 0xfc000000,	SM|RD_t|RD_b,		0,		I1	},
{"sb",      "t,A(b)",	0,    (int) M_SB_AB,	INSN_MACRO,		0,		I1	},
{"sc",      "t,~(b)",	0x6000b000, 0xfc00f000,	SM|RD_t|WR_t|RD_b,	0,		I1	},
{"sc",      "t,A(b)",	0,    (int) M_SC_AB,	INSN_MACRO,		0,		I1	},
{"scd",     "t,~(b)",	0x6000f000, 0xfc00f000,	SM|RD_t|WR_t|RD_b,	0,		I3	},
{"scd",     "t,A(b)",	0,    (int) M_SCD_AB,	INSN_MACRO,		0,		I3	},
/* The macro has to be first to handle o32 correctly.  */
{"sd",      "t,A(b)",	0,    (int) M_SD_AB,	INSN_MACRO,		0,		I1	},
{"sd",      "t,o(b)",	0xd8000000, 0xfc000000,	SM|RD_t|RD_b,		0,		I3	},
{"sdbbp",   "",		    0x46c0,     0xffff,	TRAP,			0,		I1	},
{"sdbbp",   "",		0x0000db7c, 0xffffffff,	TRAP,			0,		I1	},
{"sdbbp",   "mO",	    0x46c0,     0xfff0,	TRAP,			0,		I1	},
{"sdbbp",   "B",	0x0000db7c, 0xfc00ffff,	TRAP,			0,		I1	},
{"sdc1",    "T,o(b)",	0xb8000000, 0xfc000000,	SM|RD_T|RD_b|FP_D,	0,		I1	},
{"sdc1",    "E,o(b)",	0xb8000000, 0xfc000000,	SM|RD_T|RD_b|FP_D,	0,		I1	},
{"sdc1",    "T,A(b)",	0,    (int) M_SDC1_AB,	INSN_MACRO,		INSN2_M_FP_D,	I1	},
{"sdc1",    "E,A(b)",	0,    (int) M_SDC1_AB,	INSN_MACRO,		INSN2_M_FP_D,	I1	},
{"sdc2",    "E,~(b)",	0x2000a000, 0xfc00f000,	SM|RD_C2|RD_b,		0,		I1	},
{"sdc2",    "E,A(b)",	0,    (int) M_SDC2_AB,	INSN_MACRO,		0,		I1	},
{"s.d",     "T,o(b)",	0xb8000000, 0xfc000000,	SM|RD_T|RD_b|FP_D,	0,		I1	}, /* sdc1 */
{"s.d",     "T,A(b)",	0,    (int) M_SDC1_AB,	INSN_MACRO,		INSN2_M_FP_D,	I1	},
{"sdl",     "t,~(b)",	0x6000c000, 0xfc00f000,	SM|RD_t|RD_b,		0,		I3	},
{"sdl",     "t,A(b)",	0,    (int) M_SDL_AB,	INSN_MACRO,		0,		I3	},
{"sdm",     "n,~(b)",	0x2000f000, 0xfc00f000,	SM|RD_b,		0,		I3	},
{"sdm",     "n,A(b)",	0,    (int) M_SDM_AB,	INSN_MACRO,		0,		I3	},
{"sdp",     "t,~(b)",	0x2000c000, 0xfc00f000,	SM|RD_t|RD_b,		0,		I3	},
{"sdp",     "t,A(b)",	0,    (int) M_SDP_AB,	INSN_MACRO,		0,		I3	},
{"sdr",     "t,~(b)",	0x6000d000, 0xfc00f000,	SM|RD_t|RD_b,		0,		I3	},
{"sdr",     "t,A(b)",	0,    (int) M_SDR_AB,	INSN_MACRO,		0,		I3	},
{"sdxc1",   "D,t(b)",	0x54000108, 0xfc0007ff,	SM|RD_t|RD_b|FP_D,	RD_D,		I1	},
{"seb",     "t,r",	0x00002b3c, 0xfc00ffff,	WR_t|RD_s,		0,		I1	},
{"seh",     "t,r",	0x00003b3c, 0xfc00ffff,	WR_t|RD_s,		0,		I1	},
{"seq",     "d,v,t",	0,    (int) M_SEQ,	INSN_MACRO,		0,		I1	},
{"seq",     "d,v,I",	0,    (int) M_SEQ_I,	INSN_MACRO,		0,		I1	},
{"sge",     "d,v,t",	0,    (int) M_SGE,	INSN_MACRO,		0,		I1	},
{"sge",     "d,v,I",	0,    (int) M_SGE_I,	INSN_MACRO,		0,		I1	},
{"sgeu",    "d,v,t",	0,    (int) M_SGEU,	INSN_MACRO,		0,		I1	},
{"sgeu",    "d,v,I",	0,    (int) M_SGEU_I,	INSN_MACRO,		0,		I1	},
{"sgt",     "d,v,t",	0,    (int) M_SGT,	INSN_MACRO,		0,		I1	},
{"sgt",     "d,v,I",	0,    (int) M_SGT_I,	INSN_MACRO,		0,		I1	},
{"sgtu",    "d,v,t",	0,    (int) M_SGTU,	INSN_MACRO,		0,		I1	},
{"sgtu",    "d,v,I",	0,    (int) M_SGTU_I,	INSN_MACRO,		0,		I1	},
{"sh",      "mq,mH(ml)",    0xa800,     0xfc00,	SM,			RD_mq|RD_ml,	I1	},
{"sh",      "t,o(b)",	0x38000000, 0xfc000000,	SM|RD_t|RD_b,		0,		I1	},
{"sh",      "t,A(b)",	0,    (int) M_SH_AB,	INSN_MACRO,		0,		I1	},
{"sle",     "d,v,t",	0,    (int) M_SLE,	INSN_MACRO,		0,		I1	},
{"sle",     "d,v,I",	0,    (int) M_SLE_I,	INSN_MACRO,		0,		I1	},
{"sleu",    "d,v,t",	0,    (int) M_SLEU,	INSN_MACRO,		0,		I1	},
{"sleu",    "d,v,I",	0,    (int) M_SLEU_I,	INSN_MACRO,		0,		I1	},
{"sllv",    "d,t,s",	0x00000010, 0xfc0007ff,	WR_d|RD_s|RD_t,		0,		I1	},
{"sll",     "md,mc,mM",	    0x2400,     0xfc01,	0,			WR_md|RD_mc,	I1	},
{"sll",     "d,w,s",	0x00000010, 0xfc0007ff,	WR_d|RD_s|RD_t,		0,		I1	}, /* sllv */
{"sll",     "t,r,<",	0x00000000, 0xfc0007ff,	WR_t|RD_s,		0,		I1	},
{"slt",     "d,v,t",	0x00000350, 0xfc0007ff,	WR_d|RD_s|RD_t,		0,		I1	},
{"slt",     "d,v,I",	0,    (int) M_SLT_I,	INSN_MACRO,		0,		I1	},
{"slti",    "t,r,j",	0x90000000, 0xfc000000,	WR_t|RD_s,		0,		I1	},
{"sltiu",   "t,r,j",	0xb0000000, 0xfc000000,	WR_t|RD_s,		0,		I1	},
{"sltu",    "d,v,t",	0x00000390, 0xfc0007ff,	WR_d|RD_s|RD_t,		0,		I1	},
{"sltu",    "d,v,I",	0,    (int) M_SLTU_I,	INSN_MACRO,		0,		I1	},
{"sne",     "d,v,t",	0,    (int) M_SNE,	INSN_MACRO,		0,		I1	},
{"sne",     "d,v,I",	0,    (int) M_SNE_I,	INSN_MACRO,		0,		I1	},
{"sqrt.d",  "T,S",	0x54004a3b, 0xfc00ffff,	WR_T|RD_S|FP_D,		0,		I1	},
{"sqrt.s",  "T,S",	0x54000a3b, 0xfc00ffff,	WR_T|RD_S|FP_S,		0,		I1	},
{"srav",    "d,t,s",	0x00000090, 0xfc0007ff,	WR_d|RD_t|RD_s,		0,		I1	},
{"sra",     "d,w,s",	0x00000090, 0xfc0007ff,	WR_d|RD_t|RD_s,		0,		I1	}, /* srav */
{"sra",     "t,r,<",	0x00000080, 0xfc0007ff,	WR_t|RD_s,		0,		I1	},
{"srlv",    "d,t,s",	0x00000050, 0xfc0007ff,	WR_d|RD_t|RD_s,		0,		I1	},
{"srl",     "md,mc,mM",	    0x2401,     0xfc01,	0,			WR_md|RD_mc,	I1	},
{"srl",     "d,w,s",	0x00000050, 0xfc0007ff,	WR_d|RD_t|RD_s,		0,		I1	}, /* srlv */
{"srl",     "t,r,<",	0x00000040, 0xfc0007ff,	WR_t|RD_s,		0,		I1	},
/* ssnop is at the start of the table.  */
{"sub",     "d,v,t",	0x00000190, 0xfc0007ff,	WR_d|RD_s|RD_t,		0,		I1	},
{"sub",     "d,v,I",	0,    (int) M_SUB_I,	INSN_MACRO,		0,		I1	},
{"sub.d",   "D,V,T",	0x54000170, 0xfc0007ff,	WR_D|RD_S|RD_T|FP_D,	0,		I1	},
{"sub.s",   "D,V,T",	0x54000070, 0xfc0007ff,	WR_D|RD_S|RD_T|FP_S,	0,		I1	},
{"sub.ps",  "D,V,T",	0x54000270, 0xfc0007ff,	WR_D|RD_S|RD_T|FP_D,	0,		I1	},
{"subu",    "md,me,ml",	    0x0401,     0xfc01,	0,			WR_md|RD_me|RD_ml,	I1	},
{"subu",    "d,v,t",	0x000001d0, 0xfc0007ff,	WR_d|RD_s|RD_t,		0,		I1	},
{"subu",    "d,v,I",	0,    (int) M_SUBU_I,	INSN_MACRO,		0,		I1	},
{"suxc1",   "D,t(b)",	0x54000188, 0xfc0007ff,	SM|RD_t|RD_b|FP_D,	RD_D,		I1	},
{"sw",      "mq,mJ(ml)",    0xe800,     0xfc00,	SM,			RD_mq|RD_ml,	I1	},
{"sw",      "mp,mU(ms)",    0xc800,     0xfc00,	SM,			RD_mp|RD_sp,	I1	}, /* swsp */
{"sw",      "t,o(b)",	0xf8000000, 0xfc000000,	SM|RD_t|RD_b,		0,		I1	},
{"sw",      "t,A(b)",	0,    (int) M_SW_AB,	INSN_MACRO,		0,		I1	},
{"swc1",    "T,o(b)",	0x98000000, 0xfc000000,	SM|RD_T|RD_b|FP_S,	0,		I1	},
{"swc1",    "E,o(b)",	0x98000000, 0xfc000000,	SM|RD_T|RD_b|FP_S,	0,		I1	},
{"swc1",    "T,A(b)",	0,    (int) M_SWC1_AB,	INSN_MACRO,		INSN2_M_FP_S,	I1	},
{"swc1",    "E,A(b)",	0,    (int) M_SWC1_AB,	INSN_MACRO,		INSN2_M_FP_S,	I1	},
{"swc2",    "E,~(b)",	0x20008000, 0xfc00f000,	SM|RD_C2|RD_b,		0,		I1	},
{"swc2",    "E,A(b)",	0,    (int) M_SWC2_AB,	INSN_MACRO,		0,		I1	},
{"s.s",     "T,o(b)",	0x98000000, 0xfc000000,	SM|RD_T|RD_b|FP_S,	0,		I1	}, /* swc1 */
{"s.s",     "T,A(b)",	0,    (int) M_SWC1_AB,	INSN_MACRO,		INSN2_M_FP_S,	I1	},
{"swl",     "t,~(b)",	0x60008000, 0xfc00f000,	SM|RD_t|RD_b,		0,		I1	},
{"swl",     "t,A(b)",	0,    (int) M_SWL_AB,	INSN_MACRO,		0,		I1	},
{"scache",  "t,~(b)",	0x60008000, 0xfc00f000,	SM|RD_t|RD_b,		0,		I1	}, /* same */
{"scache",  "t,A(b)",	0,    (int) M_SWL_AB,	INSN_MACRO,		0,		I1	},
{"swm",     "mN,mJ(ms)",    0x4540,     0xffc0,	NODS,			RD_sp,		I1	},
{"swm",     "n,~(b)",	0x2000d000, 0xfc00f000,	SM|RD_b|NODS,		0,		I1	},
{"swm",     "n,A(b)",	0,    (int) M_SWM_AB,	INSN_MACRO,		0,		I1	},
{"swp",     "t,~(b)",	0x20009000, 0xfc00f000,	SM|RD_t|RD_b|NODS,	0,		I1	},
{"swp",     "t,A(b)",	0,    (int) M_SWP_AB,	INSN_MACRO,		0,		I1	},
{"swr",     "t,~(b)",	0x60009000, 0xfc00f000,	SM|RD_b|RD_t,		0,		I1	},
{"swr",     "t,A(b)",	0,    (int) M_SWR_AB,	INSN_MACRO,		0,		I1	},
{"invalidate", "t,~(b)",0x60009000, 0xfc00f000,	SM|RD_b|RD_t,		0,		I1	}, /* same */
{"invalidate", "t,A(b)",0,    (int) M_SWR_AB,	INSN_MACRO,		0,		I1	},
{"swxc1",   "D,t(b)",	0x54000088, 0xfc0007ff,	SM|RD_t|RD_b|FP_S,	RD_D,		I1	},
{"sync_acquire", "",	0x00116b7c, 0xffffffff,	NODS,			0,		I1	},
{"sync_mb", "",		0x00106b7c, 0xffffffff,	NODS,			0,		I1	},
{"sync_release", "",	0x00126b7c, 0xffffffff,	NODS,			0,		I1	},
{"sync_rmb", "",	0x00136b7c, 0xffffffff,	NODS,			0,		I1	},
{"sync_wmb", "",	0x00046b7c, 0xffffffff,	NODS,			0,		I1	},
{"sync",    "",		0x00006b7c, 0xffffffff,	NODS,			0,		I1	},
{"sync",    "1",	0x00006b7c, 0xffe0ffff,	NODS,			0,		I1	},
{"synci",   "o(b)",	0x42000000, 0xffe00000,	SM|RD_b,		0,		I1	},
{"syscall", "",		0x00008b7c, 0xffffffff,	TRAP,			0,		I1	},
{"syscall", "B",	0x00008b7c, 0xfc00ffff,	TRAP,			0,		I1	},
{"teqi",    "s,j",	0x41c00000, 0xffe00000,	RD_s|TRAP,		0,		I1	},
{"teq",     "s,t",	0x0000003c, 0xfc00ffff,	RD_s|RD_t|TRAP,		0,		I1	},
{"teq",     "s,t,|",	0x0000003c, 0xfc000fff,	RD_s|RD_t|TRAP,		0,		I1	},
{"teq",     "s,j",	0x41c00000, 0xffe00000,	RD_s|TRAP,		0,		I1	}, /* teqi */
{"teq",     "s,I",	0,    (int) M_TEQ_I,	INSN_MACRO,		0,		I1	},
{"tgei",    "s,j",	0x41200000, 0xffe00000,	RD_s|TRAP,		0,		I1	},
{"tge",     "s,t",	0x0000023c, 0xfc00ffff,	RD_s|RD_t|TRAP,		0,		I1	},
{"tge",     "s,t,|",	0x0000023c, 0xfc000fff,	RD_s|RD_t|TRAP,		0,		I1	},
{"tge",     "s,j",	0x41200000, 0xffe00000,	RD_s|TRAP,		0,		I1	}, /* tgei */
{"tge",     "s,I",	0,    (int) M_TGE_I,	INSN_MACRO,		0,		I1	},
{"tgeiu",   "s,j",	0x41600000, 0xffe00000,	RD_s|TRAP,		0,		I1	},
{"tgeu",    "s,t",	0x0000043c, 0xfc00ffff,	RD_s|RD_t|TRAP,		0,		I1	},
{"tgeu",    "s,t,|",	0x0000043c, 0xfc000fff,	RD_s|RD_t|TRAP,		0,		I1	},
{"tgeu",    "s,j",	0x41600000, 0xffe00000,	RD_s|TRAP,		0,		I1	}, /* tgeiu */
{"tgeu",    "s,I",	0,    (int) M_TGEU_I,	INSN_MACRO,		0,		I1	},
{"tlbinv",  "",		0x0000437c, 0xffffffff,	INSN_TLB,		0,		0,	TLBINV	},
{"tlbinvf", "",		0x0000537c, 0xffffffff,	INSN_TLB,		0,		0,	TLBINV	},
{"tlbginv", "",		0x0000417c, 0xffffffff,	INSN_TLB,		0,		0,	IVIRT	},
{"tlbginvf","",		0x0000517c, 0xffffffff,	INSN_TLB,		0,		0,	IVIRT	},
{"tlbgp",   "",		0x0000017c, 0xffffffff,	INSN_TLB,		0,		0,	IVIRT	},
{"tlbgr",   "",		0x0000117c, 0xffffffff,	INSN_TLB,		0,		0,	IVIRT	},
{"tlbgwi",  "",		0x0000217c, 0xffffffff,	INSN_TLB,		0,		0,	IVIRT	},
{"tlbgwr",  "",		0x0000317c, 0xffffffff,	INSN_TLB,		0,		0,	IVIRT	},
{"tlbp",    "",		0x0000037c, 0xffffffff,	INSN_TLB,		0,		I1	},
{"tlbr",    "",		0x0000137c, 0xffffffff,	INSN_TLB,		0,		I1	},
{"tlbwi",   "",		0x0000237c, 0xffffffff,	INSN_TLB,		0,		I1	},
{"tlbwr",   "",		0x0000337c, 0xffffffff,	INSN_TLB,		0,		I1	},
{"tlti",    "s,j",	0x41000000, 0xffe00000,	RD_s|TRAP,		0,		I1	},
{"tlt",     "s,t",	0x0000083c, 0xfc00ffff,	RD_s|RD_t|TRAP,		0,		I1	},
{"tlt",     "s,t,|",	0x0000083c, 0xfc000fff,	RD_s|RD_t|TRAP,		0,		I1	},
{"tlt",     "s,j",	0x41000000, 0xffe00000,	RD_s|TRAP,		0,		I1	}, /* tlti */
{"tlt",     "s,I",	0,    (int) M_TLT_I,	INSN_MACRO,		0,		I1	},
{"tltiu",   "s,j",	0x41400000, 0xffe00000,	RD_s|TRAP,		0,		I1	},
{"tltu",    "s,t",	0x00000a3c, 0xfc00ffff,	RD_s|RD_t|TRAP,		0,		I1	},
{"tltu",    "s,t,|",	0x00000a3c, 0xfc000fff,	RD_s|RD_t|TRAP,		0,		I1	},
{"tltu",    "s,j",	0x41400000, 0xffe00000,	RD_s|TRAP,		0,		I1	}, /* tltiu */
{"tltu",    "s,I",	0,    (int) M_TLTU_I,	INSN_MACRO,		0,		I1	},
{"tnei",    "s,j",	0x41800000, 0xffe00000,	RD_s|TRAP,		0,		I1	},
{"tne",     "s,t",	0x00000c3c, 0xfc00ffff,	RD_s|RD_t|TRAP,		0,		I1	},
{"tne",     "s,t,|",	0x00000c3c, 0xfc000fff,	RD_s|RD_t|TRAP,		0,		I1	},
{"tne",     "s,j",	0x41800000, 0xffe00000,	RD_s|TRAP,		0,		I1	}, /* tnei */
{"tne",     "s,I",	0,    (int) M_TNE_I,	INSN_MACRO,		0,		I1	},
{"trunc.l.d", "T,S",	0x5400633b, 0xfc00ffff,	WR_T|RD_S|FP_D,		0,		I1	},
{"trunc.l.s", "T,S",	0x5400233b, 0xfc00ffff,	WR_T|RD_S|FP_S|FP_D,	0,		I1	},
{"trunc.w.d", "T,S",	0x54006b3b, 0xfc00ffff,	WR_T|RD_S|FP_S|FP_D,	0,		I1	},
{"trunc.w.s", "T,S",	0x54002b3b, 0xfc00ffff,	WR_T|RD_S|FP_S,		0,		I1	},
{"uld",     "t,A(b)",	0,    (int) M_ULD_AB,	INSN_MACRO,		0,		I3	},
{"ulh",     "t,A(b)",	0,    (int) M_ULH_AB,	INSN_MACRO,		0,		I1	},
{"ulhu",    "t,A(b)",	0,    (int) M_ULHU_AB,	INSN_MACRO,		0,		I1	},
{"ulw",     "t,A(b)",	0,    (int) M_ULW_AB,	INSN_MACRO,		0,		I1	},
{"usd",     "t,A(b)",	0,    (int) M_USD_AB,	INSN_MACRO,		0,		I1	},
{"ush",     "t,A(b)",	0,    (int) M_USH_AB,	INSN_MACRO,		0,		I1	},
{"usw",     "t,A(b)",	0,    (int) M_USW_AB,	INSN_MACRO,		0,		I1	},
{"wait",    "",		0x0000937c, 0xffffffff,	NODS,			0,		I1	},
{"wait",    "B",	0x0000937c, 0xfc00ffff,	NODS,			0,		I1	},
{"wrpgpr",  "t,r",	0x0000f17c, 0xfc00ffff,	RD_s,			0,		I1	},
{"wsbh",    "t,r",	0x00007b3c, 0xfc00ffff,	WR_t|RD_s,		0,		I1	},
{"xor",     "mf,mt,mg",	    0x4440,     0xffc0,	0,			WR_mf|RD_mf|RD_mg,	I1	},
{"xor",     "mf,mg,mx",	    0x4440,     0xffc0,	0,			WR_mf|RD_mf|RD_mg,	I1	},
{"xor",     "d,v,t",	0x00000310, 0xfc0007ff,	WR_d|RD_s|RD_t,		0,		I1	},
{"xor",     "t,r,I",	0,    (int) M_XOR_I,	INSN_MACRO,		0,		I1	},
{"xori",    "t,r,i",	0x70000000, 0xfc000000,	WR_t|RD_s,		0,		I1	},
/* microMIPS Enhanced VA Scheme */
{"lbue",   "t,+j(b)",	0x60006000, 0xfc00fe00, RD_b|WR_t,	0,      0,	EVA	},
{"lbue",   "t,A(b)",	0,    (int) M_LBUE_AB,	INSN_MACRO,	0,      0,	EVA	},
{"lhue",   "t,+j(b)",	0x60006200, 0xfc00fe00, RD_b|WR_t,	0,      0,	EVA	},
{"lhue",   "t,A(b)",	0,    (int) M_LHUE_AB,	INSN_MACRO,	0,      0,	EVA	},
{"lbe",    "t,+j(b)",	0x60006800, 0xfc00fe00, RD_b|WR_t,	0,      0,	EVA	},
{"lbe",    "t,A(b)",	0,    (int) M_LBE_AB,	INSN_MACRO,	0,      0,	EVA	},
{"lhe",    "t,+j(b)",	0x60006a00, 0xfc00fe00, RD_b|WR_t,	0,      0,	EVA	},
{"lhe",    "t,A(b)",	0,    (int) M_LHE_AB,	INSN_MACRO,	0,      0,	EVA	},
{"lle",    "t,+j(b)",	0x60006c00, 0xfc00fe00, RD_b|WR_t,	0,      0,	EVA	},
{"lle",    "t,A(b)",	0,    (int) M_LLE_AB,	INSN_MACRO,	0,      0,	EVA	},
{"lwe",    "t,+j(b)",	0x60006e00, 0xfc00fe00, RD_b|WR_t,	0,      0,	EVA	},
{"lwe",    "t,A(b)",	0,    (int) M_LWE_AB,	INSN_MACRO,	0,      0,	EVA	},
{"lwle",   "t,+j(b)",	0x60006400, 0xfc00fe00, RD_b|WR_t,	0,      0,	EVA	},
{"lwle",   "t,A(b)",	0,    (int) M_LWLE_AB,	INSN_MACRO,	0,      0,	EVA	},
{"lwre",   "t,+j(b)",	0x60006600, 0xfc00fe00, RD_b|WR_t,	0,      0,	EVA	},
{"lwre",   "t,A(b)",	0,    (int) M_LWRE_AB,	INSN_MACRO,	0,      0,	EVA	},
{"sbe",    "t,+j(b)",	0x6000a800, 0xfc00fe00, SM|RD_b|WR_t,	0,      0,	EVA	},
{"sbe",    "t,A(b)",	0,    (int) M_SBE_AB,	INSN_MACRO,	0,      0,	EVA	},
{"sce",    "t,+j(b)",	0x6000ac00, 0xfc00fe00, SM|RD_t|WR_t|RD_b,	0,      0,	EVA	},
{"sce",    "t,A(b)",	0,    (int) M_SCE_AB,	INSN_MACRO,	0,      0,	EVA	},
{"she",    "t,+j(b)",	0x6000aa00, 0xfc00fe00, SM|RD_b|WR_t,	0,      0,	EVA	},
{"she",    "t,A(b)",	0,    (int) M_SHE_AB,	INSN_MACRO,	0,      0,	EVA	},
{"swe",    "t,+j(b)",	0x6000ae00, 0xfc00fe00, SM|RD_b|WR_t,	0,      0,	EVA	},
{"swe",    "t,A(b)",	0,    (int) M_SWE_AB,	INSN_MACRO,	0,      0,	EVA	},
{"swle",   "t,+j(b)",	0x6000a000, 0xfc00fe00, SM|RD_b|WR_t,	0,      0,	EVA	},
{"swle",   "t,A(b)",	0,    (int) M_SWLE_AB,	INSN_MACRO,	0,      0,	EVA	},
{"swre",   "t,+j(b)",	0x6000a200, 0xfc00fe00, SM|RD_b|WR_t,	0,      0,	EVA	},
{"swre",   "t,A(b)",	0,    (int) M_SWRE_AB,	INSN_MACRO,	0,      0,	EVA	},
{"cachee", "k,+j(b)",	0x6000a600, 0xfc00fe00, RD_b,		0,      0,	EVA	},
{"cachee", "k,A(b)",	0,    (int) M_CACHEE_AB,INSN_MACRO,	0,      0,	EVA	},
{"prefe",  "k,+j(b)",	0x6000a400, 0xfc00fe00, RD_b,		0,      0,	EVA	},
{"prefe",  "k,A(b)",	0,    (int) M_PREFE_AB,	INSN_MACRO,	0,      0,	EVA	},
/* MIPS DSP ASE.  */
{"absq_s.ph", "t,s",	0x0000113c, 0xfc00ffff,	WR_t|RD_s,		0,		0,	D32	},
{"absq_s.w", "t,s",	0x0000213c, 0xfc00ffff,	WR_t|RD_s,		0,		0,	D32	},
{"addq.ph", "d,s,t",	0x0000000d, 0xfc0007ff,	WR_d|RD_s|RD_t,		0,		0,	D32	},
{"addq_s.ph", "d,s,t",	0x0000040d, 0xfc0007ff,	WR_d|RD_s|RD_t,		0,		0,	D32	},
{"addq_s.w", "d,s,t",	0x00000305, 0xfc0007ff,	WR_d|RD_s|RD_t,		0,		0,	D32	},
{"addsc",   "d,s,t",	0x00000385, 0xfc0007ff,	WR_d|RD_s|RD_t,		0,		0,	D32	},
{"addu.qb", "d,s,t",	0x000000cd, 0xfc0007ff,	WR_d|RD_s|RD_t,		0,		0,	D32	},
{"addu_s.qb", "d,s,t",	0x000004cd, 0xfc0007ff,	WR_d|RD_s|RD_t,		0,		0,	D32	},
{"addwc",   "d,s,t",	0x000003c5, 0xfc0007ff,	WR_d|RD_s|RD_t,		0,		0,	D32	},
{"bitrev",  "t,s",	0x0000313c, 0xfc00ffff,	WR_t|RD_s,		0,		0,	D32	},
{"bposge32", "p",	0x43600000, 0xffff0000,	CBD,			0,		0,	D32	},
{"cmp.eq.ph", "s,t",	0x00000005, 0xfc00ffff,	RD_s|RD_t,		0,		0,	D32	},
{"cmpgu.eq.qb", "d,s,t", 0x000000c5, 0xfc0007ff, WR_d|RD_s|RD_t,	0,		0,	D32	},
{"cmp.le.ph", "s,t",	0x00000085, 0xfc00ffff,	RD_s|RD_t,		0,		0,	D32	},
{"cmpgu.le.qb", "d,s,t", 0x00000145, 0xfc0007ff, WR_d|RD_s|RD_t,	0,		0,	D32	},
{"cmp.lt.ph", "s,t",	0x00000045, 0xfc00ffff,	RD_s|RD_t,		0,		0,	D32	},
{"cmpgu.lt.qb", "d,s,t", 0x00000105, 0xfc0007ff, WR_d|RD_s|RD_t,	0,		0,	D32	},
{"cmpu.eq.qb", "s,t",	0x00000245, 0xfc00ffff,	RD_s|RD_t,		0,		0,	D32	},
{"cmpu.le.qb", "s,t",	0x000002c5, 0xfc00ffff,	RD_s|RD_t,		0,		0,	D32	},
{"cmpu.lt.qb", "s,t",	0x00000285, 0xfc00ffff,	RD_s|RD_t,		0,		0,	D32	},
{"dpaq_sa.l.w", "7,s,t", 0x000012bc, 0xfc003fff, MOD_a|RD_s|RD_t,	0,		0,	D32	},
{"dpaq_s.w.ph", "7,s,t", 0x000002bc, 0xfc003fff, MOD_a|RD_s|RD_t,	0,		0,	D32	},
{"dpau.h.qbl", "7,s,t", 0x000020bc, 0xfc003fff,	MOD_a|RD_s|RD_t,	0,		0,	D32	},
{"dpau.h.qbr", "7,s,t", 0x000030bc, 0xfc003fff,	MOD_a|RD_s|RD_t,	0,		0,	D32	},
{"dpsq_sa.l.w", "7,s,t", 0x000016bc, 0xfc003fff, MOD_a|RD_s|RD_t,	0,		0,	D32	},
{"dpsq_s.w.ph", "7,s,t", 0x000006bc, 0xfc003fff, MOD_a|RD_s|RD_t,	0,		0,	D32	},
{"dpsu.h.qbl", "7,s,t",	0x000024bc, 0xfc003fff,	MOD_a|RD_s|RD_t,	0,		0,	D32	},
{"dpsu.h.qbr", "7,s,t",	0x000034bc, 0xfc003fff,	MOD_a|RD_s|RD_t,	0,		0,	D32	},
{"extpdp",  "t,7,6",	0x0000367c, 0xfc003fff,	WR_t|RD_a|DSP_VOLA,	0,		0,	D32	},
{"extpdpv", "t,7,s",	0x000038bc, 0xfc003fff,	WR_t|RD_a|RD_s|DSP_VOLA, 0,		0,	D32	},
{"extp",    "t,7,6",	0x0000267c, 0xfc003fff,	WR_t|RD_a,		0,		0,	D32	},
{"extpv",   "t,7,s",	0x000028bc, 0xfc003fff,	WR_t|RD_a|RD_s,		0,		0,	D32	},
{"extr_rs.w", "t,7,6",	0x00002e7c, 0xfc003fff,	WR_t|RD_a,		0,		0,	D32	},
{"extr_r.w", "t,7,6",	0x00001e7c, 0xfc003fff,	WR_t|RD_a,		0,		0,	D32	},
{"extr_s.h", "t,7,6",	0x00003e7c, 0xfc003fff,	WR_t|RD_a,		0,		0,	D32	},
{"extrv_rs.w", "t,7,s",	0x00002ebc, 0xfc003fff,	WR_t|RD_a|RD_s,		0,		0,	D32	},
{"extrv_r.w", "t,7,s",	0x00001ebc, 0xfc003fff,	WR_t|RD_a|RD_s,		0,		0,	D32	},
{"extrv_s.h", "t,7,s",	0x00003ebc, 0xfc003fff,	WR_t|RD_a|RD_s,		0,		0,	D32	},
{"extrv.w", "t,7,s",	0x00000ebc, 0xfc003fff,	WR_t|RD_a|RD_s,		0,		0,	D32	},
{"extr.w",  "t,7,6",	0x00000e7c, 0xfc003fff,	WR_t|RD_a,		0,		0,	D32	},
{"insv",    "t,s",	0x0000413c, 0xfc00ffff,	WR_t|RD_s,		0,		0,	D32	},
{"lbux",    "d,t(b)",	0x00000225, 0xfc0007ff,	WR_d|RD_b|RD_t,		0,		0,	D32	},
{"lhx",     "d,t(b)",	0x00000165, 0xfc0007ff,	WR_d|RD_b|RD_t,		0,		0,	D32	},
{"lwx",     "d,t(b)",	0x000001a5, 0xfc0007ff,	WR_d|RD_b|RD_t,		0,		0,	D32	},
{"maq_sa.w.phl", "7,s,t", 0x00003a7c, 0xfc003fff, MOD_a|RD_s|RD_t,	0,		0,	D32	},
{"maq_sa.w.phr", "7,s,t", 0x00002a7c, 0xfc003fff, MOD_a|RD_s|RD_t,	0,		0,	D32	},
{"maq_s.w.phl", "7,s,t", 0x00001a7c, 0xfc003fff, MOD_a|RD_s|RD_t,	0,		0,	D32	},
{"maq_s.w.phr", "7,s,t", 0x00000a7c, 0xfc003fff, MOD_a|RD_s|RD_t,	0,		0,	D32	},
{"modsub",  "d,s,t",	0x00000295, 0xfc0007ff,	WR_d|RD_s|RD_t,		0,		0,	D32	},
{"mthlip",  "s,7",	0x0000027c, 0xffe03fff,	RD_s|MOD_a|DSP_VOLA,	0,		0,	D32	},
{"muleq_s.w.phl", "d,s,t", 0x00000025, 0xfc0007ff, WR_d|RD_s|RD_t|WR_HILO, 0,		0,	D32	},
{"muleq_s.w.phr", "d,s,t", 0x00000065, 0xfc0007ff, WR_d|RD_s|RD_t|WR_HILO, 0,		0,	D32	},
{"muleu_s.ph.qbl", "d,s,t", 0x00000095, 0xfc0007ff, WR_d|RD_s|RD_t|WR_HILO, 0,		0,	D32	},
{"muleu_s.ph.qbr", "d,s,t", 0x000000d5, 0xfc0007ff, WR_d|RD_s|RD_t|WR_HILO, 0,		0,	D32	},
{"mulq_rs.ph", "d,s,t",	0x00000115, 0xfc0007ff,	WR_d|RD_s|RD_t|WR_HILO,	0,		0,	D32	},
{"mulsaq_s.w.ph", "7,s,t", 0x00003cbc, 0xfc003fff, MOD_a|RD_s|RD_t,	0,		0,	D32	},
{"packrl.ph", "d,s,t",	0x000001ad, 0xfc0007ff,	WR_d|RD_s|RD_t,		0,		0,	D32	},
{"pick.ph", "d,s,t",	0x0000022d, 0xfc0007ff,	WR_d|RD_s|RD_t,		0,		0,	D32	},
{"pick.qb", "d,s,t",	0x000001ed, 0xfc0007ff,	WR_d|RD_s|RD_t,		0,		0,	D32	},
{"precequ.ph.qbla", "t,s", 0x0000733c, 0xfc00ffff, WR_t|RD_s,		0,		0,	D32	},
{"precequ.ph.qbl", "t,s", 0x0000713c, 0xfc00ffff, WR_t|RD_s,		0,		0,	D32	},
{"precequ.ph.qbra", "t,s", 0x0000933c, 0xfc00ffff, WR_t|RD_s,		0,		0,	D32	},
{"precequ.ph.qbr", "t,s", 0x0000913c, 0xfc00ffff, WR_t|RD_s,		0,		0,	D32	},
{"preceq.w.phl", "t,s",	0x0000513c, 0xfc00ffff,	WR_t|RD_s,		0,		0,	D32	},
{"preceq.w.phr", "t,s",	0x0000613c, 0xfc00ffff,	WR_t|RD_s,		0,		0,	D32	},
{"preceu.ph.qbla", "t,s", 0x0000b33c, 0xfc00ffff, WR_t|RD_s,		0,		0,	D32	},
{"preceu.ph.qbl", "t,s", 0x0000b13c, 0xfc00ffff, WR_t|RD_s,		0,		0,	D32	},
{"preceu.ph.qbra", "t,s",0x0000d33c, 0xfc00ffff, WR_t|RD_s,		0,		0,	D32	},
{"preceu.ph.qbr", "t,s", 0x0000d13c, 0xfc00ffff, WR_t|RD_s,		0,		0,	D32	},
{"precrq.ph.w", "d,s,t", 0x000000ed, 0xfc0007ff, WR_d|RD_s|RD_t,	0,		0,	D32	},
{"precrq.qb.ph", "d,s,t", 0x000000ad, 0xfc0007ff, WR_d|RD_s|RD_t,	0,		0,	D32	},
{"precrq_rs.ph.w", "d,s,t", 0x0000012d, 0xfc0007ff, WR_d|RD_s|RD_t,	0,		0,	D32	},
{"precrqu_s.qb.ph", "d,s,t", 0x0000016d, 0xfc0007ff, WR_d|RD_s|RD_t,	0,		0,	D32	},
{"raddu.w.qb", "t,s",	0x0000f13c, 0xfc00ffff,	WR_t|RD_s,		0,		0,	D32	},
{"rddsp",   "t",	0x000fc67c, 0xfc1fffff,	WR_t,			0,		0,	D32	},
{"rddsp",   "t,8",	0x0000067c, 0xfc103fff,	WR_t,			0,		0,	D32	},
{"repl.ph", "d,@",	0x0000003d, 0xfc0007ff,	WR_d,			0,		0,	D32	},
{"repl.qb", "t,5",	0x000005fc, 0xfc001fff,	WR_t,			0,		0,	D32	},
{"replv.ph", "t,s",	0x0000033c, 0xfc00ffff,	WR_t|RD_s,		0,		0,	D32	},
{"replv.qb", "t,s",	0x0000133c, 0xfc00ffff,	WR_t|RD_s,		0,		0,	D32	},
{"shilo",   "7,0",	0x0000001d, 0xffc03fff,	MOD_a,			0,		0,	D32	},
{"shilov",  "7,s",	0x0000127c, 0xffe03fff,	MOD_a|RD_s,		0,		0,	D32	},
{"shll.ph", "t,s,4",	0x000003b5, 0xfc000fff,	WR_t|RD_s,		0,		0,	D32	},
{"shll.qb", "t,s,3",	0x0000087c, 0xfc001fff,	WR_t|RD_s,		0,		0,	D32	},
{"shll_s.ph", "t,s,4",	0x00000bb5, 0xfc000fff,	WR_t|RD_s,		0,		0,	D32	},
{"shll_s.w", "t,s,^",	0x000003f5, 0xfc0007ff,	WR_t|RD_s,		0,		0,	D32	},
{"shllv.ph", "d,t,s",	0x0000038d, 0xfc0007ff,	WR_d|RD_s|RD_t,		0,		0,	D32	},
{"shllv.qb", "d,t,s",	0x00000395, 0xfc0007ff,	WR_d|RD_s|RD_t,		0,		0,	D32	},
{"shllv_s.ph", "d,t,s",	0x0000078d, 0xfc0007ff,	WR_d|RD_s|RD_t,		0,		0,	D32	},
{"shllv_s.w", "d,t,s",	0x000003d5, 0xfc0007ff,	WR_d|RD_s|RD_t,		0,		0,	D32	},
{"shra.ph", "t,s,4",	0x00000335, 0xfc000fff,	WR_t|RD_s,		0,		0,	D32	},
{"shra_r.ph", "t,s,4",	0x00000735, 0xfc000fff,	WR_t|RD_s,		0,		0,	D32	},
{"shra_r.w", "t,s,^",	0x000002f5, 0xfc0007ff,	WR_t|RD_s,		0,		0,	D32	},
{"shrav.ph", "d,t,s",	0x0000018d, 0xfc0007ff,	WR_d|RD_s|RD_t,		0,		0,	D32	},
{"shrav_r.ph", "d,t,s",	0x0000058d, 0xfc0007ff,	WR_d|RD_s|RD_t,		0,		0,	D32	},
{"shrav_r.w", "d,t,s",	0x000002d5, 0xfc0007ff,	WR_d|RD_s|RD_t,		0,		0,	D32	},
{"shrl.qb", "t,s,3",	0x0000187c, 0xfc001fff,	WR_t|RD_s,		0,		0,	D32	},
{"shrlv.qb", "d,t,s",	0x00000355, 0xfc0007ff,	WR_d|RD_s|RD_t,		0,		0,	D32	},
{"subq.ph", "d,s,t",	0x0000020d, 0xfc0007ff,	WR_d|RD_s|RD_t,		0,		0,	D32	},
{"subq_s.ph", "d,s,t",	0x0000060d, 0xfc0007ff,	WR_d|RD_s|RD_t,		0,		0,	D32	},
{"subq_s.w", "d,s,t",	0x00000345, 0xfc0007ff,	WR_d|RD_s|RD_t,		0,		0,	D32	},
{"subu.qb", "d,s,t",	0x000002cd, 0xfc0007ff,	WR_d|RD_s|RD_t,		0,		0,	D32	},
{"subu_s.qb", "d,s,t",	0x000006cd, 0xfc0007ff,	WR_d|RD_s|RD_t,		0,		0,	D32	},
{"wrdsp",   "t",	0x000fd67c, 0xfc1fffff,	RD_t|DSP_VOLA,		0,		0,	D32	},
{"wrdsp",   "t,8",	0x0000167c, 0xfc103fff,	RD_t|DSP_VOLA,		0,		0,	D32	},
/* MIPS DSP ASE Rev2.  */
{"absq_s.qb", "t,s",	0x0000013c, 0xfc00ffff,	WR_t|RD_s,		0,		0,	D33	},
{"addqh.ph", "d,s,t",	0x0000004d, 0xfc0007ff,	WR_d|RD_s|RD_t,		0,		0,	D33	},
{"addqh_r.ph", "d,s,t",	0x0000044d, 0xfc0007ff,	WR_d|RD_s|RD_t,		0,		0,	D33	},
{"addqh.w", "d,s,t",	0x0000008d, 0xfc0007ff,	WR_d|RD_s|RD_t,		0,		0,	D33	},
{"addqh_r.w", "d,s,t",	0x0000048d, 0xfc0007ff,	WR_d|RD_s|RD_t,		0,		0,	D33	},
{"addu.ph", "d,s,t",	0x0000010d, 0xfc0007ff,	WR_d|RD_s|RD_t,		0,		0,	D33	},
{"addu_s.ph", "d,s,t",	0x0000050d, 0xfc0007ff,	WR_d|RD_s|RD_t,		0,		0,	D33	},
{"adduh.qb", "d,s,t",	0x0000014d, 0xfc0007ff,	WR_d|RD_s|RD_t,		0,		0,	D33	},
{"adduh_r.qb", "d,s,t",	0x0000054d, 0xfc0007ff,	WR_d|RD_s|RD_t,		0,		0,	D33	},
{"append",  "t,s,h",	0x00000215, 0xfc0007ff,	WR_t|RD_t|RD_s,		0,		0,	D33	},
{"balign",  "t,s,I",	0,    (int) M_BALIGN,	INSN_MACRO,		0,		0,	D33	},
{"balign",  "t,s,2",	0x000008bc, 0xfc003fff,	WR_t|RD_t|RD_s,		0,		0,	D33	},
{"cmpgdu.eq.qb", "d,s,t", 0x00000185, 0xfc0007ff, WR_d|RD_s|RD_t,	0,		0,	D33	},
{"cmpgdu.lt.qb", "d,s,t", 0x000001c5, 0xfc0007ff, WR_d|RD_s|RD_t,	0,		0,	D33	},
{"cmpgdu.le.qb", "d,s,t", 0x00000205, 0xfc0007ff, WR_d|RD_s|RD_t,	0,		0,	D33	},
{"dpa.w.ph", "7,s,t",	0x000000bc, 0xfc003fff,	MOD_a|RD_s|RD_t,	0,		0,	D33	},
{"dpaqx_s.w.ph", "7,s,t", 0x000022bc, 0xfc003fff, MOD_a|RD_s|RD_t,	0,		0,	D33	},
{"dpaqx_sa.w.ph", "7,s,t", 0x000032bc, 0xfc003fff, MOD_a|RD_s|RD_t,	0,		0,	D33	},
{"dpax.w.ph", "7,s,t",	0x000010bc, 0xfc003fff,	MOD_a|RD_s|RD_t,	0,		0,	D33	},
{"dps.w.ph", "7,s,t",	0x000004bc, 0xfc003fff,	MOD_a|RD_s|RD_t,	0,		0,	D33	},
{"dpsqx_s.w.ph", "7,s,t", 0x000026bc, 0xfc003fff, MOD_a|RD_s|RD_t,	0,		0,	D33	},
{"dpsqx_sa.w.ph", "7,s,t", 0x000036bc, 0xfc003fff, MOD_a|RD_s|RD_t,	0,		0,	D33	},
{"dpsx.w.ph", "7,s,t",	0x000014bc, 0xfc003fff,	MOD_a|RD_s|RD_t,	0,		0,	D33	},
{"mul.ph",  "d,s,t",	0x0000002d, 0xfc0007ff,	WR_d|RD_s|RD_t|WR_HILO,	0,		0,	D33	},
{"mul_s.ph", "d,s,t",	0x0000042d, 0xfc0007ff,	WR_d|RD_s|RD_t|WR_HILO,	0,		0,	D33	},
{"mulq_rs.w", "d,s,t",	0x00000195, 0xfc0007ff,	WR_d|RD_s|RD_t|WR_HILO,	0,		0,	D33	},
{"mulq_s.ph", "d,s,t",	0x00000155, 0xfc0007ff,	WR_d|RD_s|RD_t|WR_HILO,	0,		0,	D33	},
{"mulq_s.w", "d,s,t",	0x000001d5, 0xfc0007ff,	WR_d|RD_s|RD_t|WR_HILO,	0,		0,	D33	},
{"mulsa.w.ph", "7,s,t",	0x00002cbc, 0xfc003fff,	MOD_a|RD_s|RD_t,	0,		0,	D33	},
{"precr.qb.ph", "d,s,t", 0x0000006d, 0xfc0007ff, WR_d|RD_s|RD_t,	0,		0,	D33	},
{"precr_sra.ph.w", "t,s,h", 0x000003cd, 0xfc0007ff, WR_t|RD_t|RD_s,	0,		0,	D33	},
{"precr_sra_r.ph.w", "t,s,h", 0x000007cd, 0xfc0007ff, WR_t|RD_t|RD_s,	0,		0,	D33	},
{"prepend", "t,s,h",	0x00000255, 0xfc0007ff,	WR_t|RD_t|RD_s,		0,		0,	D33	},
{"shra.qb", "t,s,3",	0x000001fc, 0xfc001fff,	WR_t|RD_s,		0,		0,	D33	},
{"shra_r.qb", "t,s,3",	0x000011fc, 0xfc001fff,	WR_t|RD_s,		0,		0,	D33	},
{"shrav.qb", "d,t,s",	0x000001cd, 0xfc0007ff,	WR_d|RD_s|RD_t,		0,		0,	D33	},
{"shrav_r.qb", "d,t,s",	0x000005cd, 0xfc0007ff,	WR_d|RD_s|RD_t,		0,		0,	D33	},
{"shrl.ph", "t,s,4",	0x000003fc, 0xfc000fff,	WR_t|RD_s,		0,		0,	D33	},
{"shrlv.ph", "d,t,s",	0x00000315, 0xfc0007ff,	WR_d|RD_s|RD_t,		0,		0,	D33	},
{"subu.ph", "d,s,t",	0x0000030d, 0xfc0007ff,	WR_d|RD_s|RD_t,		0,		0,	D33	},
{"subu_s.ph", "d,s,t",	0x0000070d, 0xfc0007ff,	WR_d|RD_s|RD_t,		0,		0,	D33	},
{"subuh.qb", "d,s,t",	0x0000034d, 0xfc0007ff,	WR_d|RD_s|RD_t,		0,		0,	D33	},
{"subuh_r.qb", "d,s,t",	0x0000074d, 0xfc0007ff,	WR_d|RD_s|RD_t,		0,		0,	D33	},
{"subqh.ph", "d,s,t",	0x0000024d, 0xfc0007ff,	WR_d|RD_s|RD_t,		0,		0,	D33	},
{"subqh_r.ph", "d,s,t",	0x0000064d, 0xfc0007ff,	WR_d|RD_s|RD_t,		0,		0,	D33	},
{"subqh.w", "d,s,t",	0x0000028d, 0xfc0007ff,	WR_d|RD_s|RD_t,		0,		0,	D33	},
{"subqh_r.w", "d,s,t",	0x0000068d, 0xfc0007ff,	WR_d|RD_s|RD_t,		0,		0,	D33	},
};

const int bfd_micromips_num_opcodes =
  ((sizeof micromips_opcodes) / (sizeof (micromips_opcodes[0])));

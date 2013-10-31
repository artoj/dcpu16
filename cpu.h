/*
 * Copyright (c) 2012-2013 Arto Jonsson <artoj@iki.fi>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#ifndef CPU_H
#define CPU_H

#include <stdint.h>

/* DCPU16 values */
#define DCPU16_REG_A	0x00
#define DCPU16_REG_B	0x01
#define DCPU16_REG_C	0x02
#define DCPU16_REG_X	0x03
#define DCPU16_REG_Y	0x04
#define DCPU16_REG_Z	0x05
#define DCPU16_REG_I	0x06
#define DCPU16_REG_J	0x07

#define DCPU16_REGV_A	0x08
#define DCPU16_REGV_B	0x09
#define DCPU16_REGV_C	0x0a
#define DCPU16_REGV_X	0x0b
#define DCPU16_REGV_Y	0x0c
#define DCPU16_REGV_Z	0x0d
#define DCPU16_REGV_I	0x0e
#define DCPU16_REGV_J	0x0f

#define DCPU16_REGVNW_A	0x10
#define DCPU16_REGVNW_B	0x11
#define DCPU16_REGVNW_C	0x12
#define DCPU16_REGVNW_X	0x13
#define DCPU16_REGVNW_Y	0x14
#define DCPU16_REGVNW_Z	0x15
#define DCPU16_REGVNW_I	0x16
#define DCPU16_REGVNW_J	0x17

#define DCPU16_PUSHPOP	0x18
#define DCPU16_PEEK	0x19
#define DCPU16_PICK	0x1a

#define DCPU16_SP	0x1b
#define DCPU16_PC	0x1c
#define DCPU16_EX	0x1d

#define DCPU16_NWORDV	0x1e
#define DCPU16_NWORD	0x1f

/* normal opcodes */
#define DCPU16_OP_SPC	0x00
#define DCPU16_OP_SET	0x01
#define DCPU16_OP_ADD	0x02
#define DCPU16_OP_SUB	0x03
#define DCPU16_OP_MUL	0x04
#define DCPU16_OP_MLI	0x05
#define DCPU16_OP_DIV	0x06
#define DCPU16_OP_DVI	0x07
#define DCPU16_OP_MOD	0x08
#define DCPU16_OP_MDI	0x09
#define DCPU16_OP_AND	0x0a
#define DCPU16_OP_BOR	0x0b
#define DCPU16_OP_XOR	0x0c
#define DCPU16_OP_SHR	0x0d
#define DCPU16_OP_ASR	0x0e
#define DCPU16_OP_SHL	0x0f
#define DCPU16_OP_IFB	0x10
#define DCPU16_OP_IFC	0x11
#define DCPU16_OP_IFE	0x12
#define DCPU16_OP_IFN	0x13
#define DCPU16_OP_IFG	0x14
#define DCPU16_OP_IFA	0x15
#define DCPU16_OP_IFL	0x16
#define DCPU16_OP_IFU	0x17
#define DCPU16_OP_ADX	0x1a
#define DCPU16_OP_SBX	0x1b
#define DCPU16_OP_STI	0x1e
#define DCPU16_OP_STD	0x1f

/* special opcodes */
#define DCPU16_OP_JSR	0x01
#define DCPU16_OP_INT	0x08
#define DCPU16_OP_IAG	0x09
#define DCPU16_OP_IAS	0x0a
#define DCPU16_OP_RFI	0x0b
#define DCPU16_OP_IAQ	0x0c
#define DCPU16_OP_HWN	0x10
#define DCPU16_OP_HWQ	0x11
#define DCPU16_OP_HWI	0x12

/* cpu context */
struct dcpu16_ctx
{
	uint16_t	sp;           /* stack pointer */
	uint16_t	pc;           /* program counter */
	uint16_t	ex;           /* extra / excess */
	uint16_t	reg[8];       /* registers */
	uint16_t	mem[0x10000]; /* memory */
};

struct dcpu16_val
{
#define VAL_TYPE_REG	0x1
#define VAL_TYPE_PC	0x2
#define VAL_TYPE_SP	0x3
#define VAL_TYPE_EX	0x4
#define VAL_TYPE_MEM	0x5
#define VAL_TYPE_LIT	0x6
	int		type;
	uint16_t	v;
};

struct dcpu16_ctx *dcpu16_init(void);
void dcpu16_free(struct dcpu16_ctx *);
void dcpu16_dump(struct dcpu16_ctx *);
void dcpu16_run(struct dcpu16_ctx *);

#endif /* CPU_H */

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

#include <stdio.h>
#include <stdlib.h>

#include "cpu.h"

static uint8_t
dcpu16_opcode(uint16_t ins)
{
	return (ins & 0x1f);
}

static uint8_t
dcpu16_spc_opcode(uint16_t ins)
{
	return (ins & 0x3e0) >> 5;
}

static uint8_t
dcpu16_val_a(uint16_t ins)
{
	return (ins & 0xfc00) >> 10;
}

static uint8_t
dcpu16_val_b(uint16_t ins)
{
	return (ins & 0x3e0) >> 5;
}

/* calculate instruction length */
static uint8_t
dcpu16_ins_length(uint16_t ins)
{
	uint8_t	vals[2], length;
	size_t	i;

	length = 1;
	/* XXX: needs a-b order? */
	vals[0] = dcpu16_val_b(ins);
	vals[1] = dcpu16_val_a(ins);

	/* special ops have only value "a" */
	i = dcpu16_opcode(ins) == DCPU16_OP_SPC ? 1 : 0;

	for (; i < sizeof(vals); i++) {
		switch (vals[i]) {
		case DCPU16_REGVNW_A:
		case DCPU16_REGVNW_B:
		case DCPU16_REGVNW_C:
		case DCPU16_REGVNW_X:
		case DCPU16_REGVNW_Y:
		case DCPU16_REGVNW_Z:
		case DCPU16_REGVNW_I:
		case DCPU16_REGVNW_J:
		case DCPU16_PICK:
		case DCPU16_NWORDV:
		case DCPU16_NWORD:
			length++;
			break;
		}
	}
	return (length);
}

struct dcpu16_ctx *
dcpu16_init(void)
{
	struct dcpu16_ctx	*ctx;
	int			 i;

	if ((ctx = malloc(sizeof(struct dcpu16_ctx))) == NULL)
		return NULL;

	ctx->sp = ctx->pc = ctx->ex = 0;

	for (i = 0; i <= DCPU16_REG_J; i++)
		ctx->reg[i] = 0;

	return ctx;
}

void
dcpu16_free(struct dcpu16_ctx *ctx)
{
	free(ctx);
}

void
dcpu16_dump(struct dcpu16_ctx *ctx)
{
	printf("pc: %04x addr: %04x\n\n", ctx->pc, ctx->mem[ctx->pc]);
	printf("ra: %04x   rz: %04x\n", ctx->reg[DCPU16_REG_A],
	    ctx->reg[DCPU16_REG_Z]);
	printf("rb: %04x   ri: %04x\n", ctx->reg[DCPU16_REG_B],
	    ctx->reg[DCPU16_REG_I]);
	printf("rc: %04x   rj: %04x\n", ctx->reg[DCPU16_REG_C],
	    ctx->reg[DCPU16_REG_J]);
	printf("rx: %04x   sp: %04x\n", ctx->reg[DCPU16_REG_X], ctx->sp);
	printf("ry: %04x   ex: %04x\n", ctx->reg[DCPU16_REG_Y], ctx->ex);
}

/* read value from given location */
uint16_t
dcpu16_read(struct dcpu16_ctx *ctx, struct dcpu16_val *loc)
{
	printf("READ  ");
	switch (loc->type) {
	case VAL_TYPE_REG:
		printf(" REG(%u)\n", loc->v);
		return ctx->reg[loc->v];
	case VAL_TYPE_PC:
		printf(" PC\n");
		return ctx->pc;
	case VAL_TYPE_SP:
		printf(" SP\n");
		return ctx->sp;
	case VAL_TYPE_EX:
		printf(" EX\n");
		return ctx->ex;
	case VAL_TYPE_MEM:
		printf(" MEM(%04x)\n", loc->v);
		return ctx->mem[loc->v];
	case VAL_TYPE_LIT:
		printf(" %04x\n", loc->v);
		return loc->v;
	default:
		fprintf(stderr, "read: unknown type: %u\n", loc->type);
		exit(1);
	}
}

/* write value to given location */
void
dcpu16_write(struct dcpu16_ctx *ctx, struct dcpu16_val *loc, uint16_t val)
{
	printf("WRITE ");
	switch (loc->type) {
	case VAL_TYPE_REG:
		printf(" REG(%u), %04x\n", loc->v, val);
		ctx->reg[loc->v] = val;
		break;
	case VAL_TYPE_PC:
		printf(" PC, %04x\n", val);
		ctx->pc = val;
		break;
	case VAL_TYPE_SP:
		printf(" SP, %04x\n", val);
		ctx->sp = val;
		break;
	case VAL_TYPE_EX:
		printf(" EX, %04x\n", val);
		ctx->ex = val;
		break;
	case VAL_TYPE_MEM:
		printf(" MEM(%04x), %04x\n", loc->v, val);
		ctx->mem[loc->v] = val;
		break;
	case VAL_TYPE_LIT:
		/* XXX: ignore silently */
		fprintf(stderr, "attempt to write literal");
		exit(1);
	default:
		fprintf(stderr, "write: unknown type: %u\n", loc->type);
		exit(1);
	}
}

/* convert value to exact memory location */
static void
val2loc(struct dcpu16_ctx *ctx, struct dcpu16_val *loc, uint8_t val, int is_a)
{
	switch (val) {
	case DCPU16_REG_A:
	case DCPU16_REG_B:
	case DCPU16_REG_C:
	case DCPU16_REG_X:
	case DCPU16_REG_Y:
	case DCPU16_REG_Z:
	case DCPU16_REG_I:
	case DCPU16_REG_J:
		loc->type = VAL_TYPE_REG;
		loc->v = val;
		break;
	case DCPU16_SP:
		loc->type = VAL_TYPE_SP;
		break;
	case DCPU16_PC:
		loc->type = VAL_TYPE_PC;
		break;
	case DCPU16_EX:
		loc->type = VAL_TYPE_EX;
		break;
	case DCPU16_REGV_A:
	case DCPU16_REGV_B:
	case DCPU16_REGV_C:
	case DCPU16_REGV_X:
	case DCPU16_REGV_Y:
	case DCPU16_REGV_Z:
	case DCPU16_REGV_I:
	case DCPU16_REGV_J:
		loc->type = VAL_TYPE_MEM;
		loc->v = val - DCPU16_REGV_A;
		break;
	case DCPU16_REGVNW_A:
	case DCPU16_REGVNW_B:
	case DCPU16_REGVNW_C:
	case DCPU16_REGVNW_X:
	case DCPU16_REGVNW_Y:
	case DCPU16_REGVNW_Z:
	case DCPU16_REGVNW_I:
	case DCPU16_REGVNW_J:
		loc->type = VAL_TYPE_LIT;
		loc->v = ctx->reg[val - DCPU16_REGVNW_A] +
		    ctx->mem[ctx->pc++];
		break;
	case DCPU16_PUSHPOP:
		loc->type = VAL_TYPE_MEM;
		if (is_a)
			loc->v = ctx->sp++;
		else
			loc->v = --ctx->sp;
		break;
	case DCPU16_PEEK:
		loc->type = VAL_TYPE_MEM;
		loc->v = ctx->sp;
		break;
	case DCPU16_PICK:
		loc->type = VAL_TYPE_LIT;
		loc->v = ctx->sp + ctx->mem[ctx->pc++];
		break;
	case DCPU16_NWORDV:
		loc->type = VAL_TYPE_MEM;
		loc->v = ctx->mem[ctx->pc++];
		break;
	case DCPU16_NWORD:
		loc->type = VAL_TYPE_LIT;
		loc->v = ctx->mem[ctx->pc++];
		break;
	default:
		/* literal */
		loc->type = VAL_TYPE_LIT;
		if (val == 0x20)
			loc->v = 0xffff;
		else
			loc->v = val - 0x21;
	}
}

void
dcpu16_run(struct dcpu16_ctx *ctx)
{
	uint16_t		ins, res;
	uint8_t			opcode;
	struct dcpu16_val	a, b;
	int			quit;

	quit = 0;

	while (!quit) {
		ins = ctx->mem[ctx->pc++];
		opcode = dcpu16_opcode(ins);

		val2loc(ctx, &a, dcpu16_val_a(ins), 1);
		val2loc(ctx, &b, dcpu16_val_b(ins), 0);

		switch (opcode) {
		case DCPU16_OP_SPC:
			opcode = dcpu16_spc_opcode(ins);

			switch (opcode) {
			case DCPU16_OP_JSR:
				ctx->mem[--ctx->sp] = ctx->pc;
				ctx->pc = a.v;
				break;
			default:
				fprintf(stderr,
				    "unknown special op code: %x\n", opcode);
				quit = 1;
				break;
			}
			break;
		case DCPU16_OP_SET:
			dcpu16_write(ctx, &b, dcpu16_read(ctx, &a));
			break;
		case DCPU16_OP_ADD:
			/* XXX: overflow */
			res = dcpu16_read(ctx, &b) + dcpu16_read(ctx, &a);
			dcpu16_write(ctx, &b, res);
			break;
		case DCPU16_OP_SUB:
			/* XXX: underflow */
			res = dcpu16_read(ctx, &b) - dcpu16_read(ctx, &a);
			dcpu16_write(ctx, &b, res);
			break;
		case DCPU16_OP_IFB:
			if ((dcpu16_read(ctx, &b) & dcpu16_read(ctx, &a)) == 0)
				ctx->pc += dcpu16_ins_length(ins);
			break;

		case DCPU16_OP_IFC:
			if ((dcpu16_read(ctx, &b) & dcpu16_read(ctx, &a)) != 0)
				ctx->pc += dcpu16_ins_length(ins);
			break;

		case DCPU16_OP_IFE:
			if (dcpu16_read(ctx, &b) != dcpu16_read(ctx, &a))
				ctx->pc += dcpu16_ins_length(ins);
			break;

		case DCPU16_OP_IFN:
			if (dcpu16_read(ctx, &b) == dcpu16_read(ctx, &a))
				ctx->pc += dcpu16_ins_length(ins);
			break;

		case DCPU16_OP_IFG:
			if (dcpu16_read(ctx, &b) < dcpu16_read(ctx, &a))
				ctx->pc += dcpu16_ins_length(ins);
			break;

		case DCPU16_OP_IFA:
			if ((int16_t)dcpu16_read(ctx, &b) <
			    (int16_t)dcpu16_read(ctx, &a))
				ctx->pc += dcpu16_ins_length(ins);
			break;

		case DCPU16_OP_IFL:
			if (dcpu16_read(ctx, &b) > dcpu16_read(ctx, &a))
				ctx->pc += dcpu16_ins_length(ins);
			break;

		case DCPU16_OP_IFU:
			if ((int16_t)dcpu16_read(ctx, &b) >
			    (int16_t)dcpu16_read(ctx, &a))
				ctx->pc += dcpu16_ins_length(ins);
			break;

		default:
			fprintf(stderr, "unknown op code: %x\n", opcode);
			quit = 1;
			break;
		}
	}
}

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

#include <err.h>
#include <stdio.h>

#include "cpu.h"

int
main(int argc, char *argv[])
{
	struct dcpu16_ctx	*ctx;
	FILE			*input;
	uint16_t		op;
	int			i;

	if (argc != 2) {
		fprintf(stderr, "usage: %s <image>\n", argv[0]);
		return (1);
	}

	if (!(ctx = dcpu16_init()))
		errx(1, "failed to initialise cpu");

	if ((input = fopen(argv[1], "r")) == NULL)
		err(1, "fopen");

	for (i = 0; fread(&op, sizeof(uint16_t), 1, input) == 1; i++)
		ctx->mem[i] = op;

	fclose(input);

	dcpu16_run(ctx);
	dcpu16_dump(ctx);
	dcpu16_free(ctx);

	return (0);
}

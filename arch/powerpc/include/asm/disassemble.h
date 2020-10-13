/* SPDX-License-Identifier: GPL-2.0-only */
/*
 *
 * Copyright IBM Corp. 2008
 *
 * Authors: Hollis Blanchard <hollisb@us.ibm.com>
 */

#ifndef __ASM_PPC_DISASSEMBLE_H__
#define __ASM_PPC_DISASSEMBLE_H__

#include <linux/types.h>
#include <asm/inst.h>

static inline unsigned int get_op(unsigned long inst)
{
	return inst >> 26;
}

static inline unsigned int get_xop(unsigned long inst)
{
	return (inst >> 1) & 0x3ff;
}

static inline unsigned int get_sprn(unsigned long inst)
{
	return ((inst >> 16) & 0x1f) | ((inst >> 6) & 0x3e0);
}

static inline unsigned int get_dcrn(unsigned long inst)
{
	return ((inst >> 16) & 0x1f) | ((inst >> 6) & 0x3e0);
}

static inline unsigned int get_tmrn(unsigned long inst)
{
	return ((inst >> 16) & 0x1f) | ((inst >> 6) & 0x3e0);
}

static inline unsigned int get_rt(unsigned long inst)
{
	return (inst >> 21) & 0x1f;
}

static inline unsigned int get_rs(unsigned long inst)
{
	return (inst >> 21) & 0x1f;
}

static inline unsigned int get_ra(unsigned long inst)
{
	return (inst >> 16) & 0x1f;
}

static inline unsigned int get_rb(unsigned long inst)
{
	return (inst >> 11) & 0x1f;
}

static inline unsigned int get_rc(unsigned long inst)
{
	return inst & 0x1;
}

static inline unsigned int get_ws(unsigned long inst)
{
	return (inst >> 11) & 0x1f;
}

static inline unsigned int get_d(unsigned long inst)
{
	return inst & 0xffff;
}

static inline unsigned int get_oc(unsigned long inst)
{
	return (inst >> 11) & 0x7fff;
}

static inline unsigned int get_tx_or_sx(unsigned long inst)
{
	return (inst) & 0x1;
}

#define IS_XFORM(inst)	(get_op(inst)  == 31)
#define IS_DSFORM(inst)	(get_op(inst) >= 56)

/*
 * Create a DSISR value from the instruction
 */
static inline unsigned make_dsisr(unsigned long instr)
{
	unsigned dsisr;


	/* bits  6:15 --> 22:31 */
	dsisr = (instr & 0x03ff0000) >> 16;

	if (IS_XFORM(instr)) {
		/* bits 29:30 --> 15:16 */
		dsisr |= (instr & 0x00000006) << 14;
		/* bit     25 -->    17 */
		dsisr |= (instr & 0x00000040) << 8;
		/* bits 21:24 --> 18:21 */
		dsisr |= (instr & 0x00000780) << 3;
	} else {
		/* bit      5 -->    17 */
		dsisr |= (instr & 0x04000000) >> 12;
		/* bits  1: 4 --> 18:21 */
		dsisr |= (instr & 0x78000000) >> 17;
		/* bits 30:31 --> 12:13 */
		if (IS_DSFORM(instr))
			dsisr |= (instr & 0x00000003) << 18;
	}

	return dsisr;
}
#endif /* __ASM_PPC_DISASSEMBLE_H__ */

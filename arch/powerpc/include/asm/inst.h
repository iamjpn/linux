/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ASM_POWERPC_INST_H
#define _ASM_POWERPC_INST_H

#include <asm/ppc-opcode.h>

/*
 * Instruction data type for POWER
 */



#ifdef CONFIG_PPC64
#define ppc_inst(x) (x)

#define ppc_inst_prefix(x, y) (((unsigned long)(x) << 32) | ((y) & 0xffffffff))

static inline bool ppc_inst_prefixed(unsigned long x)
{
	return ((x >> 32) >> 26 == 1);
}

static inline u32 ppc_inst_val(unsigned long x)
{
	if (ppc_inst_prefixed(x))
		return x >> 32;
	else
		return x & 0xffffffff;
}

static inline u32 ppc_inst_suffix(unsigned long x)
{
	return x & 0xffffffff;
}


static inline unsigned long ppc_inst_swab(unsigned long x)
{
	return ppc_inst_prefix(swab32(ppc_inst_val(x)),
			       swab32(ppc_inst_suffix(x)));
}

static inline unsigned long ppc_inst_read(const unsigned long *ptr)
{
	u32 val, suffix;

	val = *(u32 *)ptr;
	if ((val >> 26) == OP_PREFIX) {
		suffix = *((u32 *)ptr + 1);
		return ppc_inst_prefix(val, suffix);
	} else {
		return ppc_inst(val);
	}
}

#else

#define ppc_inst(x) (x)

static inline u32 ppc_inst_val(unsigned long x)
{
	return x;
}

static inline bool ppc_inst_prefixed(unsigned long x)
{
	return false;
}

static inline u32 ppc_inst_suffix(unsigned long x)
{
	return 0;
}

static inline unsigned long ppc_inst_swab(unsigned long x)
{
	return ppc_inst(swab32(ppc_inst_val(x)));
}

static inline unsigned long ppc_inst_read(const unsigned long *ptr)
{
	return *ptr;
}

#endif /* CONFIG_PPC64 */

static inline int ppc_inst_primary_opcode(unsigned long x)
{
	return ppc_inst_val(x) >> 26;
}

static inline bool ppc_inst_equal(unsigned long x, unsigned long y)
{
	return x == y;
}

static inline int ppc_inst_len(unsigned long x)
{
	return ppc_inst_prefixed(x) ? 8 : 4;
}

/*
 * Return the address of the next instruction, if the instruction @value was
 * located at @location.
 */
static inline unsigned long *ppc_inst_next(void *location, unsigned long *value)
{
	unsigned long tmp;

	tmp = ppc_inst_read(value);

	return location + ppc_inst_len(tmp);
}

static inline u64 ppc_inst_as_u64(unsigned long x)
{
#ifdef CONFIG_CPU_LITTLE_ENDIAN
	return (u64)ppc_inst_suffix(x) << 32 | ppc_inst_val(x);
#else
	return (u64)ppc_inst_val(x) << 32 | ppc_inst_suffix(x);
#endif
}

#define PPC_INST_STR_LEN sizeof("00000000 00000000")

static inline char *__ppc_inst_as_str(char str[PPC_INST_STR_LEN], unsigned long x)
{
	if (ppc_inst_prefixed(x))
		sprintf(str, "%08x %08x", ppc_inst_val(x), ppc_inst_suffix(x));
	else
		sprintf(str, "%08x", ppc_inst_val(x));

	return str;
}

#define ppc_inst_as_str(x)		\
({					\
	char __str[PPC_INST_STR_LEN];	\
	__ppc_inst_as_str(__str, x);	\
	__str;				\
})

int probe_user_read_inst(unsigned long *inst,
			 unsigned long __user *nip);

int probe_kernel_read_inst(unsigned long *inst,
			   unsigned long *src);

#endif /* _ASM_POWERPC_INST_H */

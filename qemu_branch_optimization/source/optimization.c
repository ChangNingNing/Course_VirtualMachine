/*
 *  (C) 2010 by Computer System Laboratory, IIS, Academia Sinica, Taiwan.
 *      See COPYRIGHT in top-level directory.
 */

#include <stdlib.h>
#include "exec-all.h"
#include "tcg-op.h"
#include "helper.h"
#define GEN_HELPER 1
#include "helper.h"
#include "optimization.h"

extern uint8_t *optimization_ret_addr;

/*
 * Shadow Stack
 */
list_t *shadow_hash_list;

static inline void shack_init(CPUState *env)
{
	env->shack = (uint64_t *)malloc(SHACK_SIZE * sizeof(uint64_t));
	env->shack_top = env->shack;
	env->shack_end = env->shack + SHACK_SIZE - 1;
	env->shadow_hash_list = (void *)malloc(S_HASH_SIZE * sizeof(shadow_pair));
	{
		shadow_pair *slot = (shadow_pair *)env->shadow_hash_list;
		int _i;
		for (_i=0; _i<S_HASH_SIZE; _i++, slot++){
			slot->next = NULL;
			slot->guest_eip = 0;
			slot->tpc_ptr = NULL;
		}
	}
}

/*
 * shack_set_shadow()
 *  Insert a guest eip to host eip pair if it is not yet created.
 */
 void shack_set_shadow(CPUState *env, target_ulong guest_eip, unsigned long *host_eip)
{
	uint32_t h = guest_eip % S_HASH_SIZE;
	shadow_pair *cur = (shadow_pair *)env->shadow_hash_list + h;
	for (; cur!=NULL; cur = cur->next){
		if (cur->guest_eip == guest_eip){
			cur->tpc_ptr = host_eip;
			break;
		}
	}
}

/*
 * helper_shack_flush()
 *  Reset shadow stack.
 */
void helper_shack_flush(CPUState *env)
{
//	puts("helper_shack_flush");
}

/*
 * push_shack()
 *  Push next guest eip into shadow stack.
 */
void push_shack(CPUState *env, TCGv_ptr cpu_env, target_ulong next_eip)
{
	// Find the tb address of next_eip.
	TranslationBlock *tb;
	tb_page_addr_t phys_pc, phys_page;

	phys_pc = get_page_addr_code(env, next_eip);
	phys_page = phys_pc & TARGET_PAGE_MASK;
	uint32_t h = tb_phys_hash_func(phys_pc), isfound = 0;
	for (tb = tb_phys_hash[h]; tb; tb = tb->phys_hash_next){
		if (tb->pc == next_eip && tb->page_addr[0] == phys_page){
			isfound = 1;
			break;
		}
	}

	// Record the corresponding shadow_pair.
	shadow_pair *shadow_pair_ptr = NULL;
	uint32_t _h = next_eip % S_HASH_SIZE;
	shadow_pair *cur = (shadow_pair *)env->shadow_hash_list + _h, *pre = NULL;
	for (; cur!=NULL; pre = cur, cur = cur->next){
		if (cur->guest_eip == next_eip){
			shadow_pair_ptr = cur;
			break;
		}
	}
	if (cur == NULL){
		assert(pre != NULL);
		pre->next = (shadow_pair *)malloc(sizeof(shadow_pair));
		cur = pre->next;
		cur->next = NULL;
		cur->guest_eip = next_eip;
		shadow_pair_ptr = cur;
	}

	if (!isfound)
		cur->tpc_ptr = NULL;
	else
		cur->tpc_ptr = (uint64_t *)tb->tc_ptr;

	// TODO: shack_flush, when stack is full.
	// Push in Shack.
	TCGv_ptr tmp_shack_top = tcg_temp_local_new_ptr();
	tcg_gen_ld_ptr(tmp_shack_top, cpu_env, offsetof(CPUState, shack_top));
	tcg_gen_st_ptr(tcg_const_ptr((uint64_t)shadow_pair_ptr), tmp_shack_top, 0);
	tcg_gen_addi_ptr(tmp_shack_top, tmp_shack_top, sizeof(uint64_t));
	tcg_gen_st_ptr(tmp_shack_top, cpu_env, offsetof(CPUState, shack_top));
	tcg_temp_free_ptr(tmp_shack_top);
}

/*
 * pop_shack()
 *  Pop next host eip from shadow stack.
 */
void pop_shack(TCGv_ptr cpu_env, TCGv next_eip)
{
	/* Load shadow_pair_ptr <=> tmp_shack_top */
	TCGv_ptr tmp_shack_top = tcg_temp_local_new_ptr();
	tcg_gen_ld_ptr(tmp_shack_top, cpu_env, offsetof(CPUState, shack_top));
	/* Load shack */
	TCGv_ptr tmp_shack = tcg_temp_local_new_ptr();
	tcg_gen_ld_ptr(tmp_shack, cpu_env, offsetof(CPUState, shack));
	/* Make sure shack_top > shack */
	int label_exit = gen_new_label();
	tcg_gen_brcond_ptr(TCG_COND_EQ, tmp_shack_top, tmp_shack, label_exit);
	/* if shack_top > shack */
		/* Load slot from shack_top - 1. */
		tcg_gen_addi_ptr(tmp_shack_top, tmp_shack_top, -sizeof(uint64_t));
		TCGv_ptr tmp_slot = tcg_temp_local_new();
		tcg_gen_ld_ptr(tmp_slot, tmp_shack_top, 0);
		/* Load guest_eip <=> tmp_spc */
		TCGv tmp_spc = tcg_temp_local_new();
		tcg_gen_ld_tl(tmp_spc, tmp_slot, offsetof(shadow_pair, guest_eip));

//		tcg_gen_brcond_tl(TCG_COND_NE, tmp_spc, next_eip, label_exit);
		/* if next_eip == tmp_spc */
			/* Load tpc_ptr <=> tmp_tpc */
			TCGv_ptr tmp_tpc = tcg_temp_local_new_ptr();
			tcg_gen_ld_ptr(tmp_tpc, tmp_slot, offsetof(shadow_pair, tpc_ptr));
			/* shack_top = shack_top - 1 */
			tcg_gen_st_ptr(tmp_shack_top, cpu_env, offsetof(CPUState, shack_top));

			tcg_gen_brcond_ptr(TCG_COND_EQ, tmp_tpc, tcg_const_ptr(0), label_exit);
			/* if tpc != NULL */
				*gen_opc_ptr++ = INDEX_op_jmp;
				*gen_opparam_ptr++ = tmp_tpc;		
	/* exit */
	gen_set_label(label_exit);
		tcg_temp_free_ptr(tmp_shack_top);
		tcg_temp_free_ptr(tmp_shack);
		tcg_temp_free_ptr(tmp_slot);
		tcg_temp_free(tmp_spc);
		tcg_temp_free_ptr(tmp_tpc);
}

/*
 * Indirect Branch Target Cache
 */
__thread int update_ibtc;
struct ibtc_table my_ibtc_table;

/*
 * helper_lookup_ibtc()
 *  Look up IBTC. Return next host eip if cache hit or
 *  back-to-dispatcher stub address if cache miss.
 */
void *helper_lookup_ibtc(target_ulong guest_eip)
{
	uint32_t index = guest_eip & IBTC_CACHE_MASK;
	if (guest_eip == my_ibtc_table.htable[index].guest_eip)
		return my_ibtc_table.htable[index].tb->tc_ptr;
   	return optimization_ret_addr;
}

/*
 * update_ibtc_entry()
 *  Populate eip and tb pair in IBTC entry.
 */
void update_ibtc_entry(TranslationBlock *tb)
{
	uint32_t index = tb->pc & IBTC_CACHE_MASK;
	my_ibtc_table.htable[index].guest_eip = tb->pc;
	my_ibtc_table.htable[index].tb = tb;
}

/*
 * ibtc_init()
 *  Create and initialize indirect branch target cache.
 */
static inline void ibtc_init(CPUState *env)
{
	update_ibtc = 1;
	memset(my_ibtc_table.htable, 0, IBTC_CACHE_SIZE*sizeof(struct jmp_pair));
}

/*
 * init_optimizations()
 *  Initialize optimization subsystem.
 */
int init_optimizations(CPUState *env)
{
    shack_init(env);
    ibtc_init(env);
    return 0;
}

/*
 * vim: ts=8 sts=4 sw=4 expandtab
 */

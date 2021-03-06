#include "monitor/watchpoint.h"
#include "monitor/expr.h"

#define NR_WP 32

static WP wp_list[NR_WP];
static WP *head, *free_;

void init_wp_list() {
	int i;
	for(i = 0; i < NR_WP; i ++) {
		wp_list[i].NO = i;
		wp_list[i].next = &wp_list[i + 1];
		wp_list[i].exist = 0;
		wp_list[i].old_data = 0;
	}
	wp_list[NR_WP - 1].next = NULL;


	head = NULL;
	free_ = wp_list;
}

/* TODO: Implement the functionality of watchpoint */

WP ** get_wp_head(void)
{
    return &free_;
}

#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include "common.h"


typedef struct watchpoint {
	int NO;
	struct watchpoint *next;

	/* TODO: Add more members if necessary */
    uint32_t watchaddr;
    uint8_t old_data;
    int exist;

} WP;
WP ** get_wp_head(void);

#endif

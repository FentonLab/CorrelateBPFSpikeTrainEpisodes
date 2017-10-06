
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "size_types.h"


typedef struct node {
	ui1		val;
	struct node	*next_node, *prev_node;
} NODE;


void	median_smooth(n_samps, wind, data, ra_factor)
ui4	n_samps, wind;
ui1	*data;
si4	ra_factor;
{
	register NODE	*free_node, *current_node, *median_node, *link_node, *temp_node;
	register ui1	med_val, free_val, new_val;
	si4		compare();
	si4		current_idx;
	ui4		i, j, half_wind;
	ui1		*temp;
	sf8		sum, len;
	extern void	*gmem();
	NODE		*nodes, link;

	/* median smoothing part */

	temp = (ui1 *) gmem((size_t) wind, sizeof(ui1));
	nodes = (NODE *) gmem((size_t) wind, sizeof(NODE));

	(void) memcpy((void *) temp, (void *) data, (size_t) wind);
	(void) qsort((void *) temp, (size_t) wind, sizeof(ui1), compare);

	for (i = 1; i < wind; ++i) {
		nodes[i].prev_node = (nodes + i - 1);
		nodes[i - 1].next_node = (nodes + i);
		nodes[i].val = temp[i];
	}
	link_node = &link;
	nodes[0].val = temp[0];
	nodes[0].prev_node = link_node;
	nodes[wind - 1].next_node = link_node;
	link_node->next_node = nodes;
	link_node->prev_node = (nodes + wind - 1);
	
	half_wind = wind >> 1;
	current_node = median_node = nodes + half_wind;
	current_idx = wind - 1;
	med_val = median_node->val;
	for (i = half_wind, j = wind; j < n_samps; ++i, ++j) {

		if ((++current_idx) == wind)
			current_idx = 0;
		
		data[i] = med_val;
		free_val = (free_node = nodes + current_idx)->val;
		new_val = free_node->val = data[j];

		if (free_val > med_val) {
			median_node = median_node->prev_node;
			med_val = median_node->val;
		} else if (free_val == med_val) {
			if (free_node == median_node) {
				median_node = median_node->prev_node;
				med_val = median_node->val;
			} else {
				temp_node = median_node;
				link_node->val = ~med_val;
				do {
					if ((temp_node = temp_node->next_node) == free_node) {
						median_node = median_node->prev_node;
						med_val = median_node->val;
						break;
					}	
				} while (temp_node->val == med_val);
			}
		}

		if (free_node == current_node) {
			if (free_node->next_node != link_node)
				current_node = free_node->next_node;
			else
				current_node = free_node->prev_node;
		}

		free_node->prev_node->next_node = free_node->next_node;
		free_node->next_node->prev_node = free_node->prev_node;

		if (current_node->val > new_val) {
			do {
				current_node = current_node->prev_node;
			} while (current_node->val > new_val && current_node != link_node);
			free_node->next_node = current_node->next_node;
			free_node->prev_node = current_node;
			free_node->next_node->prev_node = free_node;
			current_node->next_node = free_node;
			if (current_node == link_node)
				current_node = current_node->next_node;
		} else  {
			while (current_node->val < new_val && current_node != link_node)
				current_node = current_node->next_node;
			free_node->next_node = current_node;
			free_node->prev_node = current_node->prev_node;
			free_node->prev_node->next_node = free_node;
			current_node->prev_node = free_node;
			if (current_node == link_node)
				current_node = current_node->prev_node;
		}

		if (new_val > med_val) {
			median_node = median_node->next_node;
			med_val = median_node->val;
		} else if (new_val == med_val) {
			temp_node = median_node;
			link_node->val = ~med_val;
			do {
				if ((temp_node = temp_node->next_node) == free_node) {
					median_node = median_node->next_node;
					med_val = median_node->val;
					break;
				}	
			} while (temp_node->val == med_val);
		}
	}

	free((void *) nodes);

	/* running average part */

	if (ra_factor) {
		wind = (ui4) (((sf8) wind / (sf8) ra_factor) + 0.5);
		half_wind = wind >> 1;
		(void) memcpy((void *) temp, (void *) data, (size_t) wind);
		len = (sf8) wind;
		for (sum = 0.0, i = 0; i < wind; ++i)
			sum += (sf8) temp[i];
		current_idx = wind - 1;
		for (i = half_wind, j = wind; j < n_samps; ++i, ++j) {
			if ((++current_idx) == wind)
				current_idx = 0;
			sum -= (sf8) temp[current_idx];
			sum += (temp[current_idx] = data[j]);
			data[i] = (ui1) ((sum / len) + 0.5);
		}
	}

	free((void *) temp);

	return;
}


si4	compare(a, b)
ui1		*a, *b;
{
	return((si4) *a - (si4) *b);
}


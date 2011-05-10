/*
 * FDDL : The Free Decision Diagram Library 
 * Copyright (C) 2004 Robert Marmorstein
 * 
 * This program is released under the GNU Public License, version 2.  Please
 * see the file "LICENSE" in the root directory of this repository for more
 * information.
 */

#include "caches.h"

node_idx Cache::hit(node_idx p, node_idx q, node_idx s)
{
    int idx;
    cache_node *cur;

    assert(p >= 0);
    idx = p;

    if (q>=0){
	idx = idx << 8 + q;
    	if (s >= 0){
		idx = idx << 8 + s;
	}
    }

    idx %= m_size;

    cur = m_list[idx];
    while (cur != NULL) {
	if (cur->p == p && cur->q == q && cur->s)
	    return cur->r;
	cur = cur->m_next;
    }
    return -1;
}

void Cache::add(node_idx r, node_idx p, node_idx q, node_idx s)
{
    int idx;
    
    assert(p >= 0);
    idx = p;

    if (q>=0){
	idx = idx << 8 + q;
    	if (s >= 0){
		idx = idx << 8 + s;
	}
    }

    idx %= m_size;

    cache_node *newNode;
    newNode = new cache_node;

    newNode->p = p;
    newNode->q = q;
    newNode->s = s;
    newNode->r = r;
    newNode->m_next = m_list[idx];
    m_list[idx] = newNode;
}

void Cache::clear()
{
    cache_node *prev;

    for (int i = 0; i < m_size; i++) {
	while (m_list[i] != NULL) {
	    prev = m_list[i];
	    m_list[i] = m_list[i]->m_next;
	    delete prev;
	}
    }
}

node_idx TupleCache::hit(node_idx p, node_idx * vals, int numvals)
{
    int idx;
    cache_node *cur;

    idx = p;
    for (int i = 0; i < numvals; i++) {
	idx = idx << 8;
	idx += vals[i];
	idx %= m_size;
    }
    cur = m_list[idx];
    while (cur != NULL) {
	if (cur->p == p && cur->numvals == numvals) {
	    int i;

	    for (i = 0; i < numvals; i++) {
		if (vals[i] != cur->vals[i])
		    break;
	    }
	    if (i == numvals)
		return cur->r;
	}
	cur = cur->m_next;
    }
    return -1;
}

node_idx TupleCache::hit(node_idx * vals, int numvals)
{
    int idx;
    cache_node *cur;

    idx = 0;
    for (int i = 0; i < numvals; i++) {
	idx = idx << 8;
	idx += vals[i];
	idx %= m_size;
    }
    cur = m_list[idx];
    while (cur != NULL) {
	if (cur->numvals != numvals) {
	    cur = cur->m_next;
	    continue;
	}
	for (int i = 0; i < numvals; i++) {
	    if (cur->vals[i] != vals[i]) {
		cur = cur->m_next;
		continue;
	    }
	}
	return cur->r;
    }
    return -1;
}

void TupleCache::add(node_idx r, node_idx p, node_idx * vals, int numvals)
{
    int idx;
    cache_node *newNode;

    newNode = new cache_node;
    newNode->p = p;
    newNode->vals = new node_idx[numvals];
    newNode->numvals = numvals;

    for (int i = 0; i < numvals; i++)
	newNode->vals[i] = vals[i];
    newNode->r = r;

    idx = p % m_size;
    for (int i = 0; i < numvals; i++) {
	idx = idx << 8;
	idx += vals[i];
    }
    idx %= m_size;
    newNode->m_next = m_list[idx];
    m_list[idx] = newNode;
}

void TupleCache::add(node_idx r, node_idx * vals, int numvals)
{
    int idx;
    cache_node *newNode;

    newNode = new cache_node;
    newNode->p = 0;
    newNode->vals = new node_idx[numvals];

    for (int i = 0; i < numvals; i++)
	newNode->vals[i] = vals[i];
    newNode->r = r;

    idx = 0;
    for (int i = 0; i < numvals; i++) {
	idx = idx << 8;
	idx += vals[i];
    }
    idx %= m_size;
    newNode->m_next = m_list[idx];
    m_list[idx] = newNode;
}

void TupleCache::clear()
{
    cache_node *prev;

    for (int i = 0; i < m_size; i++) {
	while (m_list[i] != NULL) {
	    prev = m_list[i];
	    m_list[i] = m_list[i]->m_next;
	    delete prev;
	}
    }
}

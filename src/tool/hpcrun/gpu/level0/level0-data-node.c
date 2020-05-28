// -*-Mode: C++;-*- // technically C99

// * BeginRiceCopyright *****************************************************
//
// --------------------------------------------------------------------------
// Part of HPCToolkit (hpctoolkit.org)
//
// Information about sources of support for research and development of
// HPCToolkit is at 'hpctoolkit.org' and in 'README.Acknowledgments'.
// --------------------------------------------------------------------------
//
// Copyright ((c)) 2002-2020, Rice University
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
// * Redistributions of source code must retain the above copyright
//   notice, this list of conditions and the following disclaimer.
//
// * Redistributions in binary form must reproduce the above copyright
//   notice, this list of conditions and the following disclaimer in the
//   documentation and/or other materials provided with the distribution.
//
// * Neither the name of Rice University (RICE) nor the names of its
//   contributors may be used to endorse or promote products derived from
//   this software without specific prior written permission.
//
// This software is provided by RICE and contributors "as is" and any
// express or implied warranties, including, but not limited to, the
// implied warranties of merchantability and fitness for a particular
// purpose are disclaimed. In no event shall RICE or contributors be
// liable for any direct, indirect, incidental, special, exemplary, or
// consequential damages (including, but not limited to, procurement of
// substitute goods or services; loss of use, data, or profits; or
// business interruption) however caused and on any theory of liability,
// whether in contract, strict liability, or tort (including negligence
// or otherwise) arising in any way out of the use of this software, even
// if advised of the possibility of such damage.
//
// ******************************************************* EndRiceCopyright *

//*****************************************************************************
// system includes
//*****************************************************************************

#include <string.h>

//*****************************************************************************
// local includes
//*****************************************************************************

#include "level0-data-node.h"
#include "hpcrun/memory/hpcrun-malloc.h"

//******************************************************************************
// local data
//******************************************************************************

static level0_data_node_t *free_list = NULL;

//*****************************************************************************
// interface operations
//*****************************************************************************

// Allocate a node for the linked list representing a command list
level0_data_node_t*
level0_data_node_new
(
)
{
  level0_data_node_t *first = free_list; 

  if (first) { 
    free_list = first->next;
  } else {
    first = (level0_data_node_t *) hpcrun_malloc_safe(sizeof(level0_data_node_t));
  }

  memset(first, 0, sizeof(level0_data_node_t)); 

  return first;  
}

// Return a node for the linked list to the free list
void
level0_data_node_return_free_list
(
  level0_data_node_t* node
)
{
  node->next = free_list;
  free_list = node;
}


void
level0_data_list_free
(
 level0_data_node_t* head
)
{
  level0_data_node_t* next;
  for (; head != NULL; head=next) {
    next = head->next;
    level0_data_node_return_free_list(head);
  }
}

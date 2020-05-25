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


#ifndef level0_commandlist_map
#define level0_commandlist_map

//*****************************************************************************
// system includes
//*****************************************************************************

#include <stdint.h>

//*****************************************************************************
// local includes
//*****************************************************************************


#include <level_zero/ze_api.h>
#include <level_zero/zet_api.h>


//******************************************************************************
// type declarations
//******************************************************************************

typedef struct level0_kernel_list_entry {
 ze_kernel_handle_t kernel;
 ze_event_handle_t event;
 struct level0_kernel_list_entry* next;
} level0_kernel_list_entry_t;

typedef struct level0_commandlist_map_entry_t level0_commandlist_map_entry_t;

//*****************************************************************************
// interface operations
//*****************************************************************************

level0_commandlist_map_entry_t *
level0_commandlist_map_lookup
(
 ze_command_list_handle_t command_list_handle
);


void
level0_commandlist_map_insert
(
 ze_command_list_handle_t command_list_handle
);


void
level0_commandlist_map_delete
(
 ze_command_list_handle_t command_list_handle
);

void
level0_commandlist_map_kernel_list_append
(
 level0_commandlist_map_entry_t *command_list,
 ze_kernel_handle_t kernel;
 ze_event_handle_t event;
);

level0_kernel_list_entry_t*
level0_commandlist_map_kernel_list_get
(
 level0_commandlist_map_entry_t *entry
);
#endif
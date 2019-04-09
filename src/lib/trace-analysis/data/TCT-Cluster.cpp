// -*-Mode: C++;-*-

// * BeginRiceCopyright *****************************************************
//
// $HeadURL$
// $Id$
//
// --------------------------------------------------------------------------
// Part of HPCToolkit (hpctoolkit.org)
//
// Information about sources of support for research and development of
// HPCToolkit is at 'hpctoolkit.org' and in 'README.Acknowledgments'.
// --------------------------------------------------------------------------
//
// Copyright ((c)) 2002-2017, Rice University
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

/* 
 * File:   TCT-Cluster.cpp
 * Author: Lai Wei <lai.wei@rice.edu>
 *
 * Created on June 30, 2018, 8:25 PM
 */

#include "TCT-Cluster.hpp"

#include <algorithm>
using std::max;
using std::min;

#include <string.h>
#include <limits.h>

namespace TraceAnalysis {
  bool TCTClusterMemberRSD::isIsomorphic(const TCTClusterMemberRSD* latter) const {
    // Not isomorphic if nested_level doesn't match.
    if (nested_level != latter->nested_level) return false;

    if (nested_level == 0) return true;

    // Not isomorphic if stride or length doesn't match.
    if (stride != latter->stride) return false; 
    if (length != latter->length) return false;

    if (nested_level > 1) // Check if their containing RSDs are isomorphic
      return first_rsd->isIsomorphic(latter->first_rsd);
    else 
      return true;
  }
  
  bool TCTClusterMemberRSD::canConcatenate(const TCTClusterMemberRSD* latter) const {
    // Check if the IDs can be concatenated.
    if (first_id + stride * length != latter->first_id) return false;

    // When two RSDs have same nested_level
    if (nested_level == latter->nested_level) {
      if (stride != latter->stride) return false; // Strides must match for concatenation

      if (nested_level > 1) // for PRSDs, their containing RSDs need to be isomorphic for concatenation
        return first_rsd->isIsomorphic(latter->first_rsd);
      else
        return true;
    }
    // When this RSD is one level higher than the latter RSD
    else if (nested_level == latter->nested_level + 1) {
      if (nested_level > 1) // for PRSDs, the latter RSD must be isomorphic to the first nested RSD
        return first_rsd->isIsomorphic(latter);
      else
        return true;
    }
    else
      return false;
  }
  
  void TCTClusterMemberRSD::concatenate(TCTClusterMemberRSD* latter) {
    if (!canConcatenate(latter)) 
      print_msg(MSG_PRIO_MAX, "ERROR: TCTClusterMemberRSD::concatenate() is called when TCTClusterMemberRSD::canConcatenate() returns false.\n");
    
    // When two RSDs have same nested_level, sum up their lengths
    if (nested_level == latter->nested_level)
      length += latter->length;
    else // When this RSD is one level higher than the latter RSD
      length += 1;
    
    last_id = latter->last_id;
    delete latter;
  }
  
  string TCTClusterMemberRSD::toString() const {
    if (nested_level == 0) return std::to_string(first_id);
    return "(" + first_rsd->toString() + "+" + std::to_string(stride) + "[x" + std::to_string(length) + "]:" + std::to_string(last_id) + ")";
  }

  // Helper function
  TCTClusterMemberRSD* getFirstInstance(int max_level, const vector<vector<TCTClusterMemberRSD*>>& members, size_t* idx) {
    long min_ID = LONG_MAX;
    TCTClusterMemberRSD* ret = NULL;
    
    for (int level = 0; level <= max_level; level++)
      if (idx[level] < members[level].size()
              && members[level][idx[level]]->getLastID() < min_ID) {
        ret = members[level][idx[level]];
        min_ID = ret->getLastID();
      }
    
    return ret;
  }
  
  TCTClusterMembers::TCTClusterMembers(TCTClusterMembers& other1, TCTClusterMembers& other2) :
    max_level(max(other1.max_level, other2.max_level)), members(max_level+1) {
    //TODO: optimization -- if last_ID of an input is greater than the first_ID of another input
    size_t* idx1 = new size_t[other1.max_level+1];
    size_t* idx2 = new size_t[other2.max_level+1];
    memset(idx1, 0, (other1.max_level+1) * sizeof(size_t));
    memset(idx2, 0, (other2.max_level+1) * sizeof(size_t));
    
    TCTClusterMemberRSD* first1 = getFirstInstance(other1.max_level, other1.members, idx1);
    TCTClusterMemberRSD* first2 = getFirstInstance(other2.max_level, other2.members, idx2);
    
    while (first1 != NULL || first2 != NULL)
      if ((first2 == NULL) ||
              (first1 != NULL && first1->getLastID() < first2->getLastID())) {
        idx1[first1->nested_level]++;
        addMember(first1);
        first1 = getFirstInstance(other1.max_level, other1.members, idx1);
      }
      else {
        idx2[first2->nested_level]++;
        addMember(first2);
        first2 = getFirstInstance(other2.max_level, other2.members, idx2);
      }
    
    delete idx1;
    delete idx2;
    
    other1.max_level = -1;
    other2.max_level = -1;
  }
  
  void TCTClusterMembers::addMember(TCTClusterMemberRSD* rsd) {
    // Check if rsd can be concatenated to an RSD that are one level higher.
    if (rsd->nested_level < max_level) {
      vector<TCTClusterMemberRSD*>& higherRSDs = members[rsd->nested_level+1];
      for (int idx = (int)higherRSDs.size() - 1; idx >= 0 && idx > (int)higherRSDs.size() - RSD_DETECTION_WINDOW; idx--)
        if (higherRSDs[idx]->canConcatenate(rsd)) {
          higherRSDs[idx]->concatenate(rsd);
          updateMember(rsd->nested_level+1, idx);
          return;
        }
    }
    // Check if rsd can be concatenated to an RSD at the same level.
    if (rsd->nested_level >= 1) {
      vector<TCTClusterMemberRSD*>& RSDs = members[rsd->nested_level];
      for (int idx = (int)RSDs.size() - 1; idx >= 0 && idx > (int)RSDs.size() - RSD_DETECTION_WINDOW; idx--)
        if (RSDs[idx]->canConcatenate(rsd)) {
          RSDs[idx]->concatenate(rsd);
          updateMember(rsd->nested_level, idx);
          return;
        }
    }
    
    // Insert the new member and detect potential RSDs.
    members[rsd->nested_level].push_back(rsd);
    detectRSD(rsd->nested_level, members[rsd->nested_level].size() - 1);
  }
  
  void TCTClusterMembers::updateMember(int nested_level, int idx) {
    // make sure members are still ranked by last_id
    TCTClusterMemberRSD* tmp = members[nested_level][idx];
    while ((idx < (int)members[nested_level].size() - 1) 
            && (tmp->getLastID() > members[nested_level][idx+1]->getLastID())) {
      members[nested_level][idx] = members[nested_level][idx+1];
      idx++;
    }
    members[nested_level][idx] = tmp;
    
    detectRSD(nested_level, idx);
  }
  
  void TCTClusterMembers::detectRSD(int nested_level, int last_idx) {
    vector<TCTClusterMemberRSD*>& members = this->members[nested_level];
    // Enumerate previous isomorphic members within RSD_DETECTION_WINDOW
    for (int prev_idx = last_idx - 1; prev_idx >= 0 && prev_idx > last_idx - RSD_DETECTION_WINDOW; prev_idx--)
      if (members[prev_idx]->isIsomorphic(members[last_idx])) {
        int first_idx = prev_idx;
        int length = 2;
        int stride = members[last_idx]->getLastID() - members[first_idx]->getLastID();
        if (stride <= 0) {
          print_msg(MSG_PRIO_MAX, "ERROR: TCTClusterMemberRSD::detectRSD() -- stride %d <= 0.\n", stride);
          continue;
        }
        // Check previous members to see if they are in the same sequence.
        long temp_id = members[first_idx]->getLastID() - stride;
        int temp_idx = findLastIDIndex(temp_id, nested_level, max(0, first_idx - RSD_DETECTION_WINDOW), first_idx - 1);
        while (temp_idx != -1 && members[temp_idx]->isIsomorphic(members[last_idx])) {
          first_idx = temp_idx;
          length++;
          temp_id -= stride;
          temp_idx = findLastIDIndex(temp_id, nested_level, max(0, first_idx - RSD_DETECTION_WINDOW), first_idx - 1);
        }
        
        // if length passes certain threshold, construct RSD.
        if ((nested_level == 0 && length >= RSD_DETECTION_LENGTH) ||
            (nested_level > 0  && length >= PRSD_DETECTION_LENGTH) ) {
          constructRSD(nested_level, first_idx, stride, length);
          return;
        }
      }
  }
  
  void TCTClusterMembers::constructRSD(int nested_level, int first_idx, int stride, int length) {
    // Construct and add RSD
    TCTClusterMemberRSD* rsd = new TCTClusterMemberRSD(members[nested_level][first_idx], stride, length);
    if (rsd->nested_level > max_level) increaseMaxLevel();
    addMember(rsd);
    
    // Remove members that are represented by the new RSD
    vector<TCTClusterMemberRSD*>& members = this->members[nested_level];
    long id = members[first_idx]->getLastID();
    int idx = first_idx;
    
    while (length > 0) {
      if (idx == -1) {
        print_msg(MSG_PRIO_MAX, "ERROR: TCTClusterMemberRSD::constructRSD() -- index not found.\n");
        break;
      }
      
      delete members[idx];
      members.erase(members.begin() + idx);
      length--;
      id += stride;
      idx = findLastIDIndex(id, nested_level, idx, min((int)members.size() - 1, idx + RSD_DETECTION_WINDOW));
    }
  }
  
  int TCTClusterMembers::findLastIDIndex(long id, int nested_level, int min_idx, int max_idx) {
    if (min_idx >= max_idx) {
      if (members[nested_level][min_idx]->getLastID() == id) return min_idx;
      else return -1;
    }
    
    int mid_idx = (min_idx + max_idx) / 2;
    
    if (members[nested_level][mid_idx]->getLastID() >= id) return findLastIDIndex(id, nested_level, min_idx, mid_idx);
    else return findLastIDIndex(id, nested_level, mid_idx+1, max_idx);
  }
  
  void TCTClusterMembers::increaseMaxLevel() {
    max_level++;
    members.push_back(vector<TCTClusterMemberRSD*>());
  }
  
  string TCTClusterMembers::toString() const {
    string ret = "";
    for (int level = max_level; level >= 0; level--)
      for (int idx = 0; idx < (int)members[level].size(); idx++)
        ret += members[level][idx]->toString() + ", ";
    return ret;
  }
}
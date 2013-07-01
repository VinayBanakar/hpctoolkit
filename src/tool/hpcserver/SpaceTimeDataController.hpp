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
// Copyright ((c)) 2002-2013, Rice University
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

//***************************************************************************
//
// File:
//   $HeadURL$
//
// Purpose:
//   [The purpose of this file]
//
// Description:
//   [The set of functions, macros, etc. defined in the file]
//
//***************************************************************************
#ifndef SpaceTimeDataController_H_
#define SpaceTimeDataController_H_
#include "FileData.hpp"
#include "ImageTraceAttributes.hpp"
#include "ProcessTimeline.hpp"
#include "FilteredBaseData.hpp"
#include "FilterSet.hpp"


#ifndef DEBUG
#define DEBUG 0
#endif
#define DEBUGCOUT(a) if (DEBUG > (a))\
						cout

namespace TraceviewerServer
{

	class SpaceTimeDataController
	{
	public:

		SpaceTimeDataController(FileData*);
		virtual ~SpaceTimeDataController();
		void setInfo(Long, Long, int);
		ProcessTimeline* getNextTrace();
		void addNextTrace(ProcessTimeline*);
		void fillTraces();
		ProcessTimeline* fillTrace(bool);
		void applyFilters(FilterSet filters);
		//The number of processes in the database, independent of the current display size
		int getNumRanks();

		 int* getValuesXProcessID();
		 short* getValuesXThreadID();

		std::string getExperimentXML();
		ImageTraceAttributes* attributes;
		ProcessTimeline** traces;
		int tracesLength;
	private:
		void resetTraces();

		ImageTraceAttributes* oldAttributes;

		FilteredBaseData* dataTrace;
		int headerSize;

		// The minimum beginning and maximum ending time stamp across all traces (in microseconds).
		Long maxEndTime, minBegTime;

		int height;
		string experimentXML;
		string fileTrace;

		bool tracesInitialized;

		static const int DEFAULT_HEADER_SIZE = 24;

	};

} /* namespace TraceviewerServer */
#endif /* SpaceTimeDataController_H_ */
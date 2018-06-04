// Copyright 2018 SYSU
// Author: Hongzheng Chen
// E-mail: chenhzh37@mail2.sysu.edu.cn

// This is the implement of Entropy-directed scheduling (EDS) algorithm for FPGA high-level synthesis.
// Our work has been contributed to ICCD 2018.

// This head file contains the implement of the basic graph operations and our EDS algorithm.

#include <iostream>
#include <fstream>
#include <vector>
#include <utility> // pairs
#include <map>
#include <string>
#include <regex> // regular expression for string split
#include <iterator>

#include <windows.h>
#include "watch.h" // for high-accuracy time counting

#include "graph.h"
using namespace std;

#define MUL_DELAY 2 // default MUL delay
#define MAXINT 0x3f3f3f3f

// for high-accuracy time counting
stop_watch watch;

graph::~graph()
{
	for (auto node : adjlist)
		delete node;
}

void graph::clearMark()
{
	mark.clear();
	for (int i = 0; i < vertex; ++i)
		mark.push_back(0);
}

void graph::initialize()
{
	cout << "Begin initializing..." << endl;
	pair<int,int> tpair(0,0);
	for (int i = 0; i < vertex; ++i)
		axap.push_back(tpair);
	clearMark();
	cout << "Initialized successfully!\n" << endl;
}

// read from dot file
graph::graph(ifstream& infile)
{
	string str;
	// The first two lines in dot file are useless info
	getline(infile,str);
	getline(infile,str);
	cout << "Begin parsing..." << endl;
	// operation nodes info
	while (getline(infile,str) && str.find("-") == std::string::npos)
	{
		vector<string> op = split(str," *\\[ *label *= *| *\\];| +"); // reg exp
		addVertex(op[1],op[2]); // op[0] = ""
	}
	// edges info
	do {
		vector<string> arc = split(str," *\\[ *name *= *| *\\];| *-> *| +"); // reg exp
		if (!addEdge(arc[1],arc[2]))
			cout << "Add edge wrong!" << endl;
	} while (getline(infile,str) && str.size() > 1);
	cout << "Parse dot file successfully!\n" << endl;
	initialize();
}

void graph::addVertex(string name,string type)
{
	int delay = 1;
	// set MUL delay
	if (mapResourceType(type) == "MUL")
		delay = MUL_DELAY;
	// be careful of the numbers!!! start labeling from 0
	VNode* v = new VNode(vertex++,name,type,delay);
	adjlist.push_back(v);
	if (nr.find(mapResourceType(type)) != nr.end())
		nr[mapResourceType(type)]++;
	else
	{
		typeNum++;
		nr[mapResourceType(type)] = 1;
	}
}

bool graph::addEdge(string vFrom,string vTo)
{
	VNode* vf = findVertex(vFrom);
	VNode* vt = findVertex(vTo);
	if (vf == nullptr || vt == nullptr)
		return false;
	vf->succ.push_back(vt);
	vt->pred.push_back(vf);
	edge++;
	vector<int> cons = {vf->num,vt->num,(-1)*vf->delay};
	ilp.push_back(cons);
	return true;
}

VNode* graph::findVertex(string name)
{
	for (auto pnode = adjlist.cbegin(); pnode != adjlist.cend(); ++pnode)
		if ((*pnode)->name == name)
			return (*pnode);
	return nullptr;
}

void graph::printAdjlist()
{
	cout << "Start printing adjlist..." << endl;
	for (auto pnode = adjlist.cbegin(); pnode != adjlist.cend(); ++pnode)
	{
		cout << (*pnode)->num << ": ";
		for (auto adjnode = (*pnode)->succ.cbegin(); adjnode != (*pnode)->succ.cend(); ++adjnode)
			cout << (*adjnode)->num << " ";
		cout << endl;
	}
	cout << "Done!" << endl;
}

string graph::mapResourceType(string type)
{
	// return "ALL";
	if (type == "mul" || type == "MUL" || type == "div" || type == "DIV")
		return "MUL";
	if (type == "sub" || type == "add" || type == "SUB" || type == "ADD" ||
		type == "NEG" || type == "AND" || type == "les" || type == "LSR" || type == "ASR" ||
		type == "imp" || type == "exp" || type == "MemR" || type == "MemW" ||
		type == "STR" || type == "LOD" || type == "BNE" || type == "BGE" || type == "LSL")
		return "ALU";
	return type;
}

void graph::dfsASAP(VNode* node)
{
	if (mark[node->num])
		return;
	int asapmax = 0;
	if (node->pred.empty())
		asapmax = 1;
	else for (auto pprec = node->pred.cbegin(); pprec != node->pred.cend(); ++pprec)
	{
		dfsASAP(*pprec);
		asapmax = max(axap[(*pprec)->num].first + (*pprec)->delay,asapmax);
	}
	axap[node->num].first = asapmax; // asap
	cdepth = max(asapmax + node->delay - 1,cdepth); // critical path delay
	mark[node->num] = 1;
	order.push_back(node);
}

void graph::dfsALAP(VNode* node) // different from asap
{
	if (mark[node->num])
		return;
	int alapmin = MAXINT;
	if (node->succ.empty())
		alapmin = int(cdepth*LC) - node->delay + 1; // dfsasap must be done first
	else for (auto psucc = node->succ.cbegin(); psucc != node->succ.cend(); ++psucc)
	{
		dfsALAP(*psucc);
		alapmin = min(axap[(*psucc)->num].second - node->delay,alapmin);
	}
	axap[node->num].second = alapmin; // alap
	mark[node->num] = 1;
}

void graph::topologicalSortingDFS()
{
	cout << "Begin topological sorting..." << endl;
	for (auto pnode = adjlist.cbegin(); pnode != adjlist.cend(); ++pnode) // asap
		if ((*pnode)->succ.empty() && !mark[(*pnode)->num]) // out-degree = 0
			dfsASAP(*pnode);
	clearMark();
	for (auto pnode = adjlist.cbegin(); pnode != adjlist.cend(); ++pnode) // alap
		if ((*pnode)->pred.empty() && !mark[(*pnode)->num]) // in-degree = 0
			dfsALAP(*pnode);
	cout << "Topological sorting done!" << endl;
}

void graph::topologicalSortingKahn()
{
	cout << "Begin topological sorting (Kahn)..." << endl;
	order.clear();
	clearMark();
	vector<VNode*> temp;
	for (auto pnode = adjlist.cbegin(); pnode != adjlist.cend(); ++pnode)
		if ((*pnode)->pred.empty() && !mark[(*pnode)->num]) // in-degree = 0
			temp.push_back(*pnode);
	for (auto pnode = adjlist.cbegin(); pnode != adjlist.cend(); ++pnode)
		(*pnode)->incoming = (*pnode)->pred.size();
	while (!temp.empty())
	{
		order.push_back(temp[0]);
		for (auto pnode = temp[0]->succ.cbegin(); pnode != temp[0]->succ.cend(); ++pnode)
		{
			(*pnode)->incoming--;
			if ((*pnode)->incoming == 0)
				temp.push_back(*pnode);
		}
		temp.erase(temp.begin());
	}
	cout << "Topological sorting (Kahn) done!" << endl;
}

bool graph::scheduleNodeStep(VNode* node,int step)
{
	if (step + node->delay - 1 > int(cdepth*LC)) // important to minus 1
	{
		cout << "Invalid schedule!" << endl;
		return false;
	}
	for (int i = step; i < step + node->delay; ++i)
		nrt[i][mapResourceType(node->type)]++;
	schedule[node->num] = step;
	maxLatency = max(maxLatency,step + node->delay - 1);
	return true;
}

bool graph::scheduleNodeStepResource(VNode* node,int step)
{
	for (int i = step; i < step + node->delay; ++i)
		nrt[i][mapResourceType(node->type)]++;
	schedule[node->num] = step;
	maxLatency = max(maxLatency,step + node->delay - 1); // important to minus 1
	return true;
}

void graph::placeCriticalPath()
{
	cout << "Begin placing critical path..." << endl;
	for (auto node : order)
		if (axap[node->num].first == axap[node->num].second)
			scheduleNodeStep(node,axap[node->num].first);
		else
			edsOrder.push_back(node);
	cout << "Placing critical path done!" << endl;
}

void graph::EDS()
{
	cout << "Begin EDS scheduling...\n" << endl;
	watch.restart();
	topologicalSortingDFS();
	if (MODE[1] == 1)
		topologicalSortingKahn();
	// initialize N_r(t)
	map<string,int> temp;
	for (auto pnr = nr.cbegin(); pnr != nr.cend(); ++pnr)
		temp[pnr->first] = 0;
	for (int i = 0; i <= int(cdepth*LC); ++i) // number+1
		nrt.push_back(temp);

	// placing operations on critical path
	placeCriticalPath();
	cout << "Critical path time delay: " << cdepth << endl;

	// main part of scheduling
	cout << "Begin placing other nodes..." << endl;
	for (auto pnode = edsOrder.cbegin(); pnode != edsOrder.cend(); ++pnode)
	{
		int a = axap[(*pnode)->num].first, b = axap[(*pnode)->num].second;
		// because of topo order, it's pred must have been scheduled
		if (!(*pnode)->pred.empty())
			for (auto pprec = (*pnode)->pred.cbegin(); pprec != (*pnode)->pred.cend(); ++pprec)
				a = max(a,schedule[(*pprec)->num] + (*pprec)->delay);
		int minnrt = MAXINT, minstep = a;
		// cout << (*pnode)->name << " " << a << " " << b << endl;
		for (int t = a; t <= b; ++t)
		{
			int sumNrt = 0;
			for (int d = 1; d <= (*pnode)->delay; ++d)
			{
				string tempType = mapResourceType((*pnode)->type);
				// if (tempType == "MUL" || tempType == "ALU")
				// 	sumNrt += 5*nrt[t+d-1]["MUL"] + nrt[t+d-1]["ALU"]; // cost
				// else
					sumNrt += nrt[t+d-1][tempType];
			}
			if (sumNrt <= minnrt) // "equal" place backwards
			{
				minnrt = sumNrt;
				minstep = t;
			}
		}
		scheduleNodeStep(*pnode,minstep);
	}
	watch.stop();
	cout << "Placing other nodes done!\n" << endl;
	cout << "Finish EDS scheduling!\n" << endl;
	cout << "Total time used: " << watch.elapsed() << " micro-seconds" << endl;
}

void graph::EDSrev()
{
	cout << "Begin EDS scheduling...\n" << endl;
	watch.restart();
	topologicalSortingDFS();
	if (MODE[1] == 1)
		topologicalSortingKahn();
	// initialize N_r(t)
	map<string,int> temp;
	for (auto pnr = nr.cbegin(); pnr != nr.cend(); ++pnr)
		temp[pnr->first] = 0;
	for (int i = 0; i <= int(cdepth*LC); ++i) // number+1
		nrt.push_back(temp);

	// placing operations on critical path
	placeCriticalPath();
	cout << "Critical path time delay: " << cdepth << endl;

	// main part of scheduling
	cout << "Begin placing other nodes..." << endl;
	for (auto pnode = edsOrder.crbegin(); pnode != edsOrder.crend(); ++pnode)
	{
		int a = axap[(*pnode)->num].first, b = axap[(*pnode)->num].second;
		if (!(*pnode)->succ.empty()) // because of topo order, it's pred must have been scheduled
			for (auto psucc = (*pnode)->succ.cbegin(); psucc != (*pnode)->succ.cend(); ++psucc)
				b = min(b,schedule[(*psucc)->num] - (*pnode)->delay);
		int minnrt = MAXINT, minstep = b;
		// cout << (*pnode)->name << " " << a << " " << b << endl;
		for (int t = b; t >= a; --t) // --?
		{
			int sumNrt = 0;
			for (int d = 1; d <= (*pnode)->delay; ++d)
			{
				string tempType = mapResourceType((*pnode)->type);
				// if (tempType == "MUL" || tempType == "ALU")
				// 	sumNrt += 5*nrt[t+d-1]["MUL"] + nrt[t+d-1]["ALU"]; // cost
				// else
					sumNrt += nrt[t+d-1][tempType];
			}
			if (sumNrt < minnrt) // "equal" place backwards
			{
				minnrt = sumNrt;
				minstep = t;
			}
		}
		scheduleNodeStep(*pnode,minstep);
	}
	watch.stop();
	cout << "Placing other nodes done!\n" << endl;
	cout << "Finish EDS scheduling!\n" << endl;
	cout << "Total time used: " << watch.elapsed() << " micro-seconds" << endl;
}

void graph::RCEDS() // ResourceConstrained
{
	cout << "Begin EDS resource-constrained scheduling...\n" << endl;
	topologicalSortingDFS();
	watch.restart();
	if (MODE[1] == 1)
		topologicalSortingKahn();
	// initialize N_r(t)
	map<string,int> temp,maxNr;
	for (auto pnr = nr.cbegin(); pnr != nr.cend(); ++pnr)
		if (pnr->first == "mul" || pnr->first == "MUL")
		{
			temp[pnr->first] = 0;
			maxNr[pnr->first] = MAXRESOURCE.first;
		}
		else
		{
			temp[pnr->first] = 0;
			maxNr[pnr->first] = MAXRESOURCE.second;
		}
	nrt.push_back(temp); // nrt[0]

	// NO nrt.push_back! NO placeCriticalPath!
	cout << "Begin placing operations..." << endl;
	for (auto pnode = order.cbegin(); pnode != order.cend(); ++pnode)
	{
		int a = axap[(*pnode)->num].first, b = MAXINT;
		if (!(*pnode)->pred.empty()) // because of topo order, it's pred must have been scheduled
			for (auto pprec = (*pnode)->pred.cbegin(); pprec != (*pnode)->pred.cend(); ++pprec)
				a = max(a,schedule[(*pprec)->num] + (*pprec)->delay);
		int maxstep = a, maxnrt = 0;
		// cout << (*pnode)->name << " " << a << " " << b << endl;
		for (int t = a; t <= b; ++t)
		{
			int flag = 1;
			for (int d = 1; d <= (*pnode)->delay; ++d)
			{
				if (t+d-1 >= nrt.size())
					nrt.push_back(temp); // important!
				if (nrt[t+d-1][mapResourceType((*pnode)->type)]+1 > maxNr[mapResourceType((*pnode)->type)])
					flag = 0;
			}
			if (flag == 1)
			{
				maxstep = t;
				break;
			}
		}
		scheduleNodeStepResource(*pnode,maxstep); // something difference
	}
	watch.stop();
	cout << "Placing operations done!\n" << endl;
	cout << "Finish EDS scheduling!\n" << endl;
	cout << "Total time used: " << watch.elapsed() << " micro-seconds" << endl;
}

void graph::countResource()
{
	for (auto ptype = nr.cbegin(); ptype != nr.cend(); ++ptype)
	{
		cout << ptype->first << ": ";
		int res = 0;
		for (int i = 1; i <= maxLatency; ++i) // int(cdepth*LC)
			res = max(res,nrt[i][mapResourceType(ptype->first)]);
		cout << res << endl;
		// if (ptype->first == "MUL" || ptype->first == "mul")
			for (int i = 1; i <= maxLatency; ++i) // int(cdepth*LC)
				cout << "Step " << i << ": " << nrt[i][mapResourceType(ptype->first)] << endl;
	}
}

void graph::printOutput()
{
	switch (MODE[0])
	{
		case 0: EDS();break;
		case 1: EDSrev();break;
		case 2: RCEDS();break;
	}
	cout << "Output as follows:" << endl;
	cout << "Topological order:" << endl;
	for (auto pnode : order)
		cout << pnode->name << " ";
	cout << "\n" << endl;
	cout << "Time frame:" << endl;
	int cnt = 1;
	for (auto frame : axap)
		cout << cnt++ << ": [ " << frame.first << " , " << frame.second << " ]" << endl;
	cout << endl;
	cout << "Final schedule:" << endl;
	for (int i = 0; i < vertex; ++i)
		cout << i+1 << ": " << schedule[i] << ((i+1)%5==0 ? "\n" : "\t");
	cout << endl;
	cout << "Total latency: " << maxLatency << endl;
	if (MODE[0] == 2)
		cout << "Constrained resource:\n"
				"MUL: " << MAXRESOURCE.first << endl <<
				"ALU: " << MAXRESOURCE.second << endl;
	cout << "Resource used:" << endl;
	countResource();
}

// generated in CPLEX form
void graph::generateTCSILP(ofstream& outfile)
{
	topologicalSortingDFS();
	cout << "Time frame:" << endl;
	int cnt = 1;
	for (auto frame : axap)
		cout << cnt++ << ": [ " << frame.first << " , " << frame.second << " ]" << endl;
	cout << endl;
	cout << "Start generating ILP formulas for latency-constrained problems..." << endl;

	outfile << "Minimize" << endl;
	outfile << "M1 + M2" << endl;

	outfile << "Subject To" << endl;
	cnt = 0;
	// Time frame constraints
	for (auto frame : axap)
	{
		for (int i = frame.first; i <= frame.second; ++i)
			outfile << "x" << cnt << "," << i << (i == frame.second ? " = 1\n" : " + ");
		cnt++;
	}
	cout << "Time frame constraints generated." << endl;

	// Resource constraints
	cnt = 0;
	for (cnt = 0; cnt < vertex; ++cnt)
		for (int i = axap[cnt].first; i <= axap[cnt].second + adjlist[cnt]->delay - 1; ++i)
			// cout << i << " " << adjlist[cnt]->type << endl;
			rowResource[i][mapResourceType(adjlist[cnt]->type)].push_back(cnt); // push delay
	cout << "Critical path delay: " << int(cdepth*LC) << endl;
	for (int i = 1; i <= int(cdepth*LC); ++i)
		for (auto ptype = nr.cbegin(); ptype != nr.cend(); ++ptype)
		{
			if (rowResource[i][ptype->first].size() < 2)
				continue;
			for (int j = 0; j < rowResource[i][ptype->first].size(); ++j)
				for (int d = 0; d < adjlist[rowResource[i][ptype->first][j]]->delay; ++d)
					if (i-d >= 1)
						outfile << "x" << rowResource[i][ptype->first][j]
								<< "," << i-d << ((j == rowResource[i][ptype->first].size()-1 && (d == adjlist[rowResource[i][ptype->first][j]]->delay-1 || i-d == 1)) ? "" : " + ");
					else
						break;
			if (ptype->first == "MUL") // ptype->first == "mul" || 
				outfile << " - M1 <= 0" << endl;
			else
				outfile << " - M2 <= 0" << endl;
		}
	cout << "Resource constraints generated." << endl;

	// Precedence constraints
	for (auto pnode = adjlist.cbegin(); pnode != adjlist.cend(); ++pnode)
		for (auto psucc = (*pnode)->succ.cbegin(); psucc != (*pnode)->succ.cend(); ++psucc)
		{
			for (int i = axap[(*pnode)->num].first; i <= axap[(*pnode)->num].second; ++i)
				outfile << i << " x" << (*pnode)->num << ","
						<< i << (i == axap[(*pnode)->num].second ? "" : " + ");
			outfile << " - ";
			for (int i = axap[(*psucc)->num].first; i <= axap[(*psucc)->num].second; ++i)
				outfile << i << " x" << (*psucc)->num << ","
						<< i << (i == axap[(*psucc)->num].second ? "" : " - ");
			outfile << " <= -" << (*pnode)->delay << endl;
		}
	// for (int i = 0; i < vertex; ++i)
	// 	for (int j = axap[i].first; j <= axap[i].second; ++j)
	// 		outfile << "x" << i << "," << j << " - "
	// 				<< ((adjlist[i]->type == "mul" || adjlist[i]->type == "MUL") ? "M2" : "M1") << " <= 0" << endl;
	cout << "Precedence constraints generated." << endl;

	// Bounds NO VARIABLES RHS!
	outfile << "Bounds" << endl;
	for (int i = 0; i < vertex; ++i)
		for (int j = axap[i].first; j <= axap[i].second; ++j)
			// outfile << "x" << i << "," << j << " >= 0" <<endl;
			outfile << "0 <= x" << i << "," << j << " <= 1" <<endl;
	outfile << "M1 >= 1" << endl;
	outfile << "M2 >= 1" << endl;
	cout << "Bounds generated." << endl;

	// Generals
	outfile << "Generals" << endl;
	for (int i = 0; i < vertex; ++i)
		for (int j = axap[i].first; j <= axap[i].second; ++j)
			outfile << "x" << i << "," << j << "\n";
	outfile << "M1\nM2" << endl;
	cout << "Generals generated." << endl;
	// *******SDC*******
	// cnt = 1;
	// for (auto cons : ilp)
	// 	outfile << "c" << cnt++ << ": x"
	// 			<< cons[0] << " - x"
	// 			<< cons[1] << " <= " << cons[2] << endl;
	// outfile << "Bounds" << endl;
	// for (int i = 0; i < vertex; ++i)
	// 	outfile << "x" << i << " >= 0" << endl;
	// outfile << "Generals" << endl;
	// for (int i = 0; i < vertex; ++i)
	// 	outfile << "x" << i << " ";
	// outfile << endl;
	outfile << "End" << endl;
	cout << "Finished ILP generation!" << endl;
}

// generated in CPLEX form
void graph::generateRCSILP(ofstream& outfile)
{
	topologicalSortingDFS();
	cout << "Time frame:" << endl;
	int cnt = 1;
	for (auto paxap = axap.begin(); paxap != axap.end(); ++paxap) // cannot use `auto frame : axap`
	{
		paxap->second = vertex; // set upper bound
		cout << cnt++ << ": [ " << paxap->first << " , " << paxap->second << " ]" << endl;
	}
	cout << endl;
	cout << "Start generating ILP formulas for resource-constrained problems..." << endl;

	outfile << "Minimize" << endl;
	outfile << "L" << endl;

	outfile << "Subject To" << endl;
	cnt = 0;
	// Time frame constraints
	for (auto frame : axap)
	{
		for (int i = frame.first; i <= frame.second; ++i)
			outfile << "x" << cnt << "," << i << (i == frame.second ? " = 1\n" : " + ");
		// (t+d-1) x
		for (int i = frame.first; i <= frame.second; ++i)
			outfile << (i + adjlist[cnt]->delay - 1) << " x" << cnt << "," << i << " - L <= 0" << endl;
		cnt++;
	}
	cout << "Time frame and upper latency constraints generated." << endl;

	// Resource constraints
	cnt = 0;
	for (cnt = 0; cnt < vertex; ++cnt)
		for (int i = axap[cnt].first; i <= axap[cnt].second + adjlist[cnt]->delay - 1; ++i)
			// cout << i << " " << adjlist[cnt]->type << endl;
			rowResource[i][mapResourceType(adjlist[cnt]->type)].push_back(cnt); // push delay
	// cout << "Critical path delay: " << int(cdepth*LC) << endl;
	for (int i = 1; i <= vertex; ++i) // int(cdepth*LC)
		for (auto ptype = nr.cbegin(); ptype != nr.cend(); ++ptype)
		{
			if (rowResource[i][ptype->first].size() < 2)
				continue;
			for (int j = 0; j < rowResource[i][ptype->first].size(); ++j)
				for (int d = 0; d < adjlist[rowResource[i][ptype->first][j]]->delay; ++d)
					if (i-d >= 1)
						outfile << "x" << rowResource[i][ptype->first][j]
								<< "," << i-d << ((j == rowResource[i][ptype->first].size()-1 && (d == adjlist[rowResource[i][ptype->first][j]]->delay-1 || i-d == 1)) ? "" : " + ");
					else
						break;
			if (ptype->first == "MUL") // ptype->first == "mul" || 
				outfile << " <= " << MAXRESOURCE.first << endl; // differenet
			else
				outfile << " <= " << MAXRESOURCE.second << endl;
		}
	cout << "Resource constraints generated." << endl;

	// Precedence constraints
	for (auto pnode = adjlist.cbegin(); pnode != adjlist.cend(); ++pnode)
		for (auto psucc = (*pnode)->succ.cbegin(); psucc != (*pnode)->succ.cend(); ++psucc)
		{
			for (int i = axap[(*pnode)->num].first; i <= axap[(*pnode)->num].second; ++i)
				outfile << i << " x" << (*pnode)->num << ","
						<< i << (i == axap[(*pnode)->num].second ? "" : " + ");
			outfile << " - ";
			for (int i = axap[(*psucc)->num].first; i <= axap[(*psucc)->num].second; ++i)
				outfile << i << " x" << (*psucc)->num << ","
						<< i << (i == axap[(*psucc)->num].second ? "" : " - ");
			outfile << " <= -" << (*pnode)->delay << endl;
		}
	cout << "Precedence constraints generated." << endl;

	// Bounds NO VARIABLES RHS!
	outfile << "Bounds" << endl;
	for (int i = 0; i < vertex; ++i)
		for (int j = axap[i].first; j <= axap[i].second; ++j)
			// outfile << "x" << i << "," << j << " >= 0" <<endl;
			outfile << "0 <= x" << i << "," << j << " <= 1" <<endl;
	outfile << "L >= 1" << endl;
	cout << "Bounds generated." << endl;

	// Generals
	outfile << "Generals" << endl;
	for (int i = 0; i < vertex; ++i)
		for (int j = axap[i].first; j <= axap[i].second; ++j)
			outfile << "x" << i << "," << j << "\n";
	outfile << "L" << endl;
	cout << "Generals generated." << endl;
	outfile << "End" << endl;
	cout << "Finished ILP generation!" << endl;
}
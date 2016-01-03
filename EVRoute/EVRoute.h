#pragma once
#include "stdafx.h"
#include <unordered_map>

struct RFSINFO
{
	int m_nid;
	int m_rftype;
};

struct LABEL
{
	double totalDistance;
	double subDistance;
	int nextNodeID;
};

struct StocLABEL
{
	double expCost;
	double expRange;
	double probOfArrival;
	int nextNodeID;
};

class RFSNODE
{
public:
	RFSNODE() { refuel = false; };
	virtual ~RFSNODE() {};
	map<int, LABEL*> Labels;
	map<int, vector<StocLABEL*>*> StocLabels;
	vector<StocLABEL*> *FinalStocLabels;
	TNM_SNODE *SNode;

	bool IsRefuelNode() { return refuel; }
	void SetRefuelNode() { refuel = true; }
	void ClearStocLabels(); 

private:
	bool refuel;
};

class TNM_SNETRF : public TNM_SNET
{
public:
	TNM_SNETRF(const string &name) : TNM_SNET(name) {}
	virtual ~TNM_SNETRF();
	int BuildTAPASRF(TNM_LINKTYPE lt = BPRLK);
	vector<RFSINFO*> rfsStations;
	vector<RFSNODE*> rfsNodes;
	double totalCPUTime;
	
	map<pair<int,int>,TNM_SPATH*> spRecord;
	vector<int> NewNodeID; // store the new node ID w.r.t. the old node ID
	vector<int> OldNodeID; // store the old node ID w.r.t. the new node ID
	vector<int> SNewNodeID; // store the new node ID w.r.t the old node ID for stochastic network
	vector<int> SOldNodeID; // store the old node ID w.r.t. the new node ID for stochastic network
	map<pair<int,int>, vector<StocLABEL*>*> sspRecord;
	TNM_SNET *metaNet;
	TNM_SNET *sMetaNet;

public:
	int ShortestPathWR_LS(RFSNODE *dest, double mLimit);
	int ShortestPathWR_MN(TNM_SNODE *dest, double mLimit);
	int StochasticShortestPathWR(RFSNODE *dest, double mLimit, int n, int method);
	int StochasticShortestPathWOR(RFSNODE *dest, double mLimit, int n, int method);
	int StochasticShortestPathToDest(RFSNODE *dest, int method);
	TNM_SPATH* GetSPWR_LS(int orgID, int destID);
	TNM_SPATH* GetSPWR_MN(int orgID, int destID);
	int GetOptimalChargingPath(int orgID, int destID, bool optimal, int method);
	int DeleteKeyPair(multimap<double, pair<int, int>, less<double>> *Q, double distance, int nodeID, int nextRefuelID);
	static int RSGenerator(string netFile, string rfsFile, double mLimit);
	double EnergyPerUnitDist(double v); 

	int BuildStocMetaNetwork(int numOfNodes);

	// Results output function
	int PrintLabelsAtFirstStage(int n);
	double AvgLabelSizeAtFirstStage(int n);
	int AvgLabelSizeAtSecondStage();
	StocLABEL *GetOptimalLabel(int orgID);
};
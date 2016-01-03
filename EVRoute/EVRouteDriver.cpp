//sample driver
#include "stdafx.h"
#include "EVRoute.h"

TNM_SNETRF::~TNM_SNETRF()
{
	for(int i = 0;i<rfsStations.size();i++) delete rfsStations[i];
	rfsStations.clear();

	for(int i = 0;i < rfsNodes.size();i++) delete rfsNodes[i];
	rfsNodes.clear();
}

int TNM_SNETRF::BuildTAPASRF(TNM_LINKTYPE lt)
{
	if(BuildTAPAS(false, lt)!=0)
	{
		return 1;
	}

	// Initialize RFSNODE vector
	int i = 0;
	for (i = 0; i < numOfNode; i++)
	{
		RFSNODE *rfsnode = new RFSNODE();
		rfsnode->SNode = nodeVector[i];
		rfsNodes.push_back(rfsnode);
	}

	string fname = networkName + "_rfs.dat";
	ifstream sfile;
	if(!TNM_OpenInFile(sfile, fname))
	{
		return 2;
	}
	string line;
	vector<string> words;

	while(getline(sfile, line))
	{
		TNM_GetWordsFromLine(line, words);
		if(words.size()==2)
		{
			RFSINFO *pinfo = new RFSINFO;
			if(TNM_FromString(pinfo->m_nid, words[0], std::dec))
			{
				if(CatchNodePtr(pinfo->m_nid))
				{
					TNM_FromString(pinfo->m_rftype, words[1], std::dec);
					rfsStations.push_back(pinfo);
					rfsNodes[pinfo->m_nid - 1]->SetRefuelNode();
				}
			}
		}
		else
		{
			cout<<"\tSomething is wrong in your file."<<endl;
			return 3;
		}
	}

	return 0;
}

int TNM_SNETRF::DeleteKeyPair(multimap<double, pair<int, int>, less<double>> *Q, double distance, int nodeID, int nextRefuelID)
{
	multimap<double, pair<int, int>, less<double>>::iterator lower = Q->lower_bound(distance - 1e-10), 
		upper = Q->upper_bound(distance + 1e-10), piter;
	piter = lower;
	while (piter != upper && (piter->second.first != nodeID || piter->second.second != nextRefuelID))
	{
		piter++;
	}

	if (piter == Q->end() || piter->second.first != nodeID || piter->second.second != nextRefuelID)
	{
		cout<<"failed to find node" <<nodeID<<"in the queue."<<endl;
		return 1;
	}
	else
	{
		Q->erase(piter);
	}
	return 0;
}

//int TNM_SNETRF::StochasticShortestPathWOR(RFSNODE *dest, double mLimit)
//{
//	clock_t start,end;
//	double cpuTime;
//	start = clock();
//
//	int n = 10;
//	double p[3] = {0.6, 0.3, 0.1};
//	double beta[3] = {1.2, 1.0, 0.8};
//	double gamma[3] = {1.2, 1.0, 1.5};
//	double alphaE = 1.0;
//	double alphaT = 2.0;
//	double delta = mLimit / n;
//	double refuelCost = 5.0;
//	//cout << "n= " << n << "delta " << delta << beta[0] << endl;
//	int destID = dest->SNode->id;
//	cout << "destination id is " << destID << endl;
//
//	for(int i = 1; i <= n; i++)
//	{
//		StocLABEL *label = new StocLABEL;
//		vector<StocLABEL*> *labelVec = new vector<StocLABEL*>();
//		label->expCost = 0;
//		label->probOfArrival = 1;
//		label->nextNodeID = 0;  // next node for destination is set to be 0
//		labelVec->push_back(label);
//		dest->StocLabels.insert(pair<int, vector<StocLABEL*>*>(i, labelVec));
//	}
//
//	//for(int i = 1; i <= n; i++)
//	//{
//	//	cout << "a" << (*(dest->StocLabels[i]->begin()))->expCost << endl;
//	//}
//	//vector<StocLABEL*> lV;
//	//for(int m = 1; m < 3; m++)
//	//{
//	//	StocLABEL lab = {m, 0, 2};
//	//	//lab->expCost = m+3;
//	//	//lab->nextNodeID = 0;
//	//	//lab->probOfArrival = 44;
//	//	lV.push_back(&lab);
//	//	cout << (*lV.begin())->expCost << "gui" << endl;
//	//}
//	//for(vector<StocLABEL*>::iterator iter = lV.begin(); iter != lV.end(); iter++)
//	//{
//	//	cout << "gao" << (*iter)->expCost << endl;
//	//}
//	
//
//	vector<RFSNODE*> Q;
//	Q.push_back(dest);
//	RFSNODE *curNode;
//	RFSNODE *tailNode;
//	TNM_SLINK *curLink;
//
//	vector<StocLABEL*> tempLabels;
//	vector<StocLABEL*> tempLabelsPlus;
//	double tempProb = 0.0;
//	int remainingK; 
//	double curLinkCost;
//	bool dominated = false;
//	bool needUpdate = false;
//	bool feasible = false;
//	
//	while(!Q.empty())
//	{
//		curNode = *Q.begin();
//		Q.erase(Q.begin());
//		// iterate over all the tail nodes connected to the current node
//		for(PTRTRACE pv = curNode->SNode->backStar.begin(); pv != curNode->SNode->backStar.end(); pv++)
//		{
//			curLink = *pv;
//			tailNode = rfsNodes[curLink->tail->id - 1];
//			cout << "current node is " << curNode->SNode->id << ",";
//			cout << "tail node " << curLink->tail->id << "visited" << endl; 
//			int minK = int(1.0 * curLink->length / delta + 0.5);
//			// iterate over all possible ranges at the tail node
//			for(int k = minK; k <= n; k++)
//			{
//				feasible = false;
//				StocLABEL *label = new StocLABEL;
//				label->expCost = 0;
//				label->probOfArrival = 0;
//				label->nextNodeID = curNode->SNode->id;
//				tempLabels.push_back(label);
//				tempProb = 0.0;
//				cout << "mileage left is " << k << endl;
//				// iterate over all possible traffic conditions
//				for(int l = 0; l < 3; l++)
//				{
//					remainingK = k - int(gamma[l] * curLink->length / delta + 0.5);
//					if(curNode->IsRefuelNode())
//					{
//						if(remainingK >= 0)
//						{
//							feasible = true;
//							tempProb = tempProb + p[l];
//							if(curNode->StocLabels.find(remainingK) != curNode->StocLabels.end())
//							{
//								// may change the implementation later, now assume that the vehicle always refuel at the refuel node
//								for(vector<StocLABEL*>::iterator iterI = tempLabels.begin(); iterI != tempLabels.end(); iterI++)
//								{
//									for(vector<StocLABEL*>::iterator iterJ = curNode->StocLabels[n]->begin(); iterJ != curNode->StocLabels[n]->end(); iterJ++)
//									{
//										StocLABEL *label = new StocLABEL;
//										curLinkCost = alphaE * gamma[l] * curLink->length + alphaT * beta[l] * curLink->length;
//										label->expCost = (*iterI)->expCost + p[l] * ( (*iterJ)->expCost + curLinkCost + refuelCost);
//										label->probOfArrival = (*iterI)->probOfArrival + p[l] * (*iterJ)->probOfArrival;
//										label->nextNodeID = curNode->SNode->id;
//										tempLabelsPlus.push_back(label);
//									}
//								}
//							}
//							else
//							{
//								for(vector<StocLABEL*>::iterator iterI = tempLabels.begin(); iterI != tempLabels.end(); iterI++)
//								{
//									for(vector<StocLABEL*>::iterator iterJ = curNode->StocLabels[n]->begin(); iterJ != curNode->StocLabels[n]->end(); iterJ++)
//									{
//										StocLABEL *label = new StocLABEL;
//										curLinkCost = alphaE * gamma[l] * curLink->length + alphaT * beta[l] * curLink->length;
//										label->expCost = (*iterI)->expCost + p[l] * ( (*iterJ)->expCost + curLinkCost + refuelCost);
//										label->probOfArrival = (*iterI)->probOfArrival + p[l] * (*iterJ)->probOfArrival;
//										label->nextNodeID = curNode->SNode->id;
//										tempLabelsPlus.push_back(label);
//									}
//								}
//							}
//							// release memory of tempLabels
//							for(vector<StocLABEL*>::iterator iter = tempLabels.begin(); iter != tempLabels.end(); iter++)
//							{
//								delete *iter;
//							}
//							tempLabels.clear();
//							for(vector<StocLABEL*>::iterator iter = tempLabelsPlus.begin(); iter != tempLabelsPlus.end(); iter++)
//							{
//								tempLabels.push_back(*iter);
//							}
//							tempLabelsPlus.clear();
//						}
//					}
//					else
//					{
//						if(remainingK > 0 && curNode->StocLabels.find(remainingK) != curNode->StocLabels.end())
//						{
//							feasible = true;
//							tempProb = tempProb + p[l];
//							for(vector<StocLABEL*>::iterator iterI = tempLabels.begin(); iterI != tempLabels.end(); iterI++)
//							{
//								for(vector<StocLABEL*>::iterator iterJ = curNode->StocLabels[remainingK]->begin(); iterJ != curNode->StocLabels[remainingK]->end(); iterJ++)
//								{
//									StocLABEL *label = new StocLABEL;
//									curLinkCost = alphaE * gamma[l] * curLink->length + alphaT * beta[l] * curLink->length;
//									label->expCost = (*iterI)->expCost + p[l] * ( (*iterJ)->expCost + curLinkCost);
//									label->probOfArrival = (*iterI)->probOfArrival + p[l] * (*iterJ)->probOfArrival;
//									label->nextNodeID = curNode->SNode->id;
//									tempLabelsPlus.push_back(label);
//								}
//							}
//							// release memory of tempLabels
//							for(vector<StocLABEL*>::iterator iter = tempLabels.begin(); iter != tempLabels.end(); iter++)
//							{
//								delete *iter;
//							}
//							tempLabels.clear();
//							for(vector<StocLABEL*>::iterator iter = tempLabelsPlus.begin(); iter != tempLabelsPlus.end(); iter++)
//							{
//								tempLabels.push_back(*iter);
//							}
//							tempLabelsPlus.clear();
//							cout << "size is " << tempLabelsPlus.size() << endl;
//						}
//					}
//				}
//
//				if(feasible)
//				{
//					// update the conditional expected cost
//					for(vector<StocLABEL*>::iterator iter = tempLabels.begin(); iter != tempLabels.end(); iter++)
//					{
//						(*iter)->expCost = (*iter)->expCost / tempProb;
//					}
//					// check dominated labels
//					if(tailNode->StocLabels.find(k) == tailNode->StocLabels.end())
//					{
//						tailNode->StocLabels[k] = new vector<StocLABEL*>();
//					}
//
//					for(vector<StocLABEL*>::iterator iterI = tempLabels.begin(); iterI != tempLabels.end();)
//					{
// 						dominated = false;
//						cout << "temp label probability " << (*iterI)->probOfArrival << endl;
//						if( (*iterI)->probOfArrival < 0.85 )
//						{
//							iterI++;
//							continue;
//						}
//						for(vector<StocLABEL*>::iterator iterJ = tailNode->StocLabels[k]->begin(); iterJ != tailNode->StocLabels[k]->end();)
//						{
//							if( (*iterJ)->expCost <= (*iterI)->expCost && (*iterJ)->probOfArrival >= (*iterI)->probOfArrival )
//							{
//								dominated = true;
//								break;
//							}
//							else if( (*iterI)->expCost <= (*iterJ)->expCost && (*iterI)->probOfArrival >= (*iterJ)->probOfArrival )
//							{
//								tailNode->StocLabels[k]->erase(iterJ);
//							}
//							else
//							{
//								iterJ++;
//							}
//						}
//						
//						if(!dominated)
//						{
//							needUpdate = true;
//							tailNode->StocLabels[k]->push_back(*iterI);
//							tempLabels.erase(iterI);
//						}
//						else
//						{
//							iterI++;
//						}
//					}
//					
//					if(tailNode->StocLabels[k]->size() == 0)
//					{
//						delete tailNode->StocLabels[k]; 
//						tailNode->StocLabels.erase(k);
//					}
//
//					if(tailNode->SNode->id == 7 && k == 10)
//					{
//						cout << "expected cost      probability" << endl;
//						for(vector<StocLABEL*>::iterator iter = tailNode->StocLabels[k]->begin(); iter != tailNode->StocLabels[k]->end(); iter++)
//						{
//							cout << "" << (*iter)->expCost << "    " << (*iter)->probOfArrival << endl;
//						}
//						cout << "dddd" << endl;
//					}
//
//					if(needUpdate)
//					{
//						if(find(Q.begin(), Q.end(), tailNode) == Q.end())
//						{
//							Q.push_back(tailNode);
//						}
//					}
//					needUpdate = false;
//				}
//
//			    // clear tempLabels
//				for(vector<StocLABEL*>::iterator iter = tempLabels.begin(); iter != tempLabels.end(); iter++)
//				{
//					delete *iter;
//				}
//				tempLabels.clear();
//			}
//		}
//	}
//	cout << "Finished!" << endl;
//	end = clock();
//	cpuTime = (end - start)/double(CLOCKS_PER_SEC);
//	cout << "CPU Time of StochasticSPPWR is " << cpuTime << endl;
//	return 1;
//}




void RFSNODE::ClearStocLabels()
{
	for(map<int, vector<StocLABEL*>*>::iterator iter = StocLabels.begin(); iter != StocLabels.end(); iter++)
		delete iter->second;
	StocLabels.clear();
}

double TNM_SNETRF::EnergyPerUnitDist(double v)
{
	double eta = 0.9;
	double rho = 1.2;
	double Cw = 0.29;
	double Af = 2.27;
	double m = 1500;
	double g = 9.81;
	double mu = 0.012;
	double P = 2000;
	double f;

	f = 1 / eta * (0.5 * rho * Cw * Af * 0.44704 * 0.44704 * v * v + mu * m * g) + P / 0.44704 / v;
	return f;
}



int TNM_SNETRF::StochasticShortestPathWOR(RFSNODE *dest, double mLimit, int n, int method)
{
	double minProb = 0.8;
	//double nv = 1;
	//double v[1] = {30};
	//double p[1] = {1.0};
	//double beta[1];
	//double gamma[1];

	//double nv = 2;
	//double v[2] = {30, 40};
	//double p[2] = {0.5, 0.5};
	//double beta[2];
	//double gamma[2];

	//double nv = 3;
	//double v[3] = {30, 40, 50};
	//double p[3] = {0.3, 0.4, 0.3};
	//double beta[3];
	//double gamma[3];

	//double nv = 4;
	//double v[4] = {10, 30,  50,  70};
	//double p[4] = {0.1, 0.35, 0.45, 0.1};
	//double beta[4];
	//double gamma[4];

	//double nv = 5;
	//double v[5] = {20, 30, 40, 50, 60};
	//double p[5] = {0.1, 0.25, 0.3, 0.25, 0.1};
	//double beta[5];
	//double gamma[5];

	//double nv = 6;
	//double v[6] = {20, 30, 40, 50, 60, 70};
	//double p[6] = {0.1, 0.2, 0.2, 0.2, 0.2, 0.1};
	//double beta[6];
	//double gamma[6];

	//double nv = 7;
	//double v[7] = {10, 20, 30, 40, 50, 60, 70};
	//double p[7] = {0.05, 0.1, 0.15, 0.25, 0.2, 0.2, 0.05};
	//double beta[7];
	//double gamma[7];

	double nv = 8;
	double v[8] = {10, 20, 30, 40, 50, 60, 70, 80};
	double p[8] = {0.05, 0.05, 0.15, 0.2, 0.25, 0.20, 0.05, 0.05};
	double beta[8];
	double gamma[8]; 

	//double nv = 9;
	//double v[9] = {10, 20, 30, 40, 50, 60, 70, 80, 90};
	//double p[9] = {0.05, 0.05, 0.15, 0.23, 0.20, 0.20, 0.05, 0.05, 0.02};
	//double beta[9];
	//double gamma[9];

	//double nv = 10;
	//double v[10] = {10, 20, 30, 40, 50, 60, 70, 80, 90, 100};
	//double p[10] = {0.05, 0.05, 0.15, 0.15, 0.23, 0.24, 0.05, 0.05, 0.02, 0.01};
	//double beta[10];
	//double gamma[10];

	double alphaE = 0.03;
	double alphaT = 0.5;
	double delta = mLimit / n;

	for(int i = 0; i < nv; i++)
	{
		gamma[i] = EnergyPerUnitDist(v[i]) / EnergyPerUnitDist(30);
		//cout << "ddd " << gamma[i] << endl;
		beta[i] = 30 / v[i];
	}

	int destID = dest->SNode->id;
	//cout << "destination id is " << destID << endl;
	// Clear the previous labels for all nodes
	for(vector<RFSNODE*>::iterator iter = rfsNodes.begin(); iter !=	rfsNodes.end(); iter++)
		(*iter)->ClearStocLabels();

	for(int i = 0; i <= n; i++)
	{
		StocLABEL *label = new StocLABEL;
		vector<StocLABEL*> *labelVec = new vector<StocLABEL*>();
		label->expCost = 0;
		label->expRange = 0;
		label->probOfArrival = 1;
		label->nextNodeID = 0;  // next node for destination is set to be 0
		labelVec->push_back(label);
		dest->StocLabels.insert(pair<int, vector<StocLABEL*>*>(i, labelVec));
	}

	vector<RFSNODE*> Q;
	Q.push_back(dest);
	RFSNODE *curNode;
	RFSNODE *tailNode;
	TNM_SLINK *curLink;

	vector<StocLABEL*> tempLabels;
	vector<StocLABEL*> tempLabelsPlus;
	double tempProb = 0.0;
	int remainingK; 
	double curLinkCost;
	bool dominated = false;
	bool needUpdate = false;
	bool feasible = false;
	double minCost;
	double range;
	double maxProb;

	while(!Q.empty())
	{
		curNode = *Q.begin();
		Q.erase(Q.begin());
		// iterate over all the tail nodes connected to the current node
		for(PTRTRACE pv = curNode->SNode->backStar.begin(); pv != curNode->SNode->backStar.end(); pv++)
		{
			curLink = *pv;
			tailNode = rfsNodes[curLink->tail->id - 1];
			int minK = int(1.0 * curLink->length / delta + 0.5);
			// iterate over all possible ranges at the tail node
			for(int k = minK; k <= n; k++)
			{
				feasible = false;
				StocLABEL *label = new StocLABEL;
				label->expCost = 0;
				label->expRange = 0;
				label->probOfArrival = 0;
				label->nextNodeID = curNode->SNode->id;
				tempLabels.push_back(label);
				tempProb = 0.0;
				
				// iterate over all possible traffic conditions
				for(int l = 0; l < nv; l++)
				{
					remainingK = k - int(gamma[l] * curLink->length / delta + 0.5);
					if(remainingK >= 0 && curNode->StocLabels.find(remainingK) != curNode->StocLabels.end())
					{
						feasible = true;
						tempProb = tempProb + p[l];

						if(method == 1)
						{
							vector<StocLABEL*>::iterator first = curNode->StocLabels[remainingK]->begin();
							minCost = (*first)->expCost;
							range = (*first)->expRange;
							maxProb = (*first)->probOfArrival;
							for(vector<StocLABEL*>::iterator iter = curNode->StocLabels[remainingK]->begin()+1; iter != curNode->StocLabels[remainingK]->end(); iter++)
							{
								if( (*iter)->expCost < minCost )
								{
									minCost = (*iter)->expCost;
									range = (*iter)->expRange;
								}
								if( (*iter)->probOfArrival > maxProb )
								{
									maxProb = (*iter)->probOfArrival;
								}
							}
							
							curLinkCost = alphaE * gamma[l] * curLink->length + alphaT * beta[l] * curLink->length;
							label->expCost = label->expCost + p[l] * (minCost + curLinkCost);
							label->expRange = label->expRange + p[l] * (range + gamma[l] * curLink->length);
							label->probOfArrival = label->probOfArrival + p[l] * maxProb;
						}

						if(method == 2)
						{
							for(vector<StocLABEL*>::iterator iterI = tempLabels.begin(); iterI != tempLabels.end(); iterI++)
							{
								for(vector<StocLABEL*>::iterator iterJ = curNode->StocLabels[remainingK]->begin(); iterJ != curNode->StocLabels[remainingK]->end(); iterJ++)
								{
									StocLABEL *label = new StocLABEL;
									curLinkCost = alphaE * gamma[l] * curLink->length + alphaT * beta[l] * curLink->length;
									label->expCost = (*iterI)->expCost + p[l] * ( (*iterJ)->expCost + curLinkCost);
									label->expRange = (*iterI)->expRange + p[l] *( (*iterJ)->expRange + gamma[l] * curLink->length);
									label->probOfArrival = (*iterI)->probOfArrival + p[l] * (*iterJ)->probOfArrival;
									label->nextNodeID = curNode->SNode->id;
									tempLabelsPlus.push_back(label);
								}
							}
							// release memory of tempLabels
							for(vector<StocLABEL*>::iterator iter = tempLabels.begin(); iter != tempLabels.end(); iter++)
							{
								delete *iter;
							}
							tempLabels.clear();
							for(vector<StocLABEL*>::iterator iter = tempLabelsPlus.begin(); iter != tempLabelsPlus.end(); iter++)
							{
								tempLabels.push_back(*iter);
							}
							tempLabelsPlus.clear();
						}
					}
				}

				if(feasible)
				{
					StocLABEL *optimalTempLabel;
					vector<StocLABEL*>::iterator optimalIter;
					for(vector<StocLABEL*>::iterator iter = tempLabels.begin(); iter != tempLabels.end(); )
					{
						if((*iter)->probOfArrival < minProb)
						{
							delete *iter;
							tempLabels.erase(iter);
						}
						else
						{
							iter++;
						}
					}

					if(tempLabels.size() > 0)
					{
						optimalTempLabel = tempLabels.front();
						optimalIter = tempLabels.begin();
						for(vector<StocLABEL*>::iterator iter = tempLabels.begin() + 1; iter != tempLabels.end(); iter++)
						{
							if((*iter)->expCost < optimalTempLabel->expCost)
							{
								optimalTempLabel = *iter;
								optimalIter = iter;
							}
						}

						tempLabels.erase(optimalIter);

						// clear tempLabels
						for(vector<StocLABEL*>::iterator iter = tempLabels.begin(); iter != tempLabels.end(); iter++)
						{
							delete *iter;
						}
						tempLabels.clear();

						tempLabels.push_back(optimalTempLabel);
					}

					// update the conditional expected cost
					for(vector<StocLABEL*>::iterator iter = tempLabels.begin(); iter != tempLabels.end(); iter++)
					{
						(*iter)->expCost = (*iter)->expCost / tempProb;
						(*iter)->expRange = (*iter)->expRange / tempProb;
					}
					// check dominated labels
					if(tailNode->StocLabels.find(k) == tailNode->StocLabels.end())
					{
						tailNode->StocLabels[k] = new vector<StocLABEL*>();
					}

					for(vector<StocLABEL*>::iterator iterI = tempLabels.begin(); iterI != tempLabels.end();)
					{
 						dominated = false;
						if( (*iterI)->probOfArrival < minProb )
						{
							iterI++;
							continue;
						}
						for(vector<StocLABEL*>::iterator iterJ = tailNode->StocLabels[k]->begin(); iterJ != tailNode->StocLabels[k]->end();)
						{
							if( (*iterJ)->expCost <= (*iterI)->expCost && (*iterJ)->probOfArrival >= (*iterI)->probOfArrival )
							{
								dominated = true;
								break;
							}
							else if( (*iterI)->expCost <= (*iterJ)->expCost && (*iterI)->probOfArrival >= (*iterJ)->probOfArrival )
							{
								tailNode->StocLabels[k]->erase(iterJ);
							}
							else
							{
								iterJ++;
							}
						}
						
						if(!dominated)
						{
							needUpdate = true;
							tailNode->StocLabels[k]->push_back(*iterI);
							tempLabels.erase(iterI);
						}
						else
						{
							iterI++;
						}
					}
					
					if(tailNode->StocLabels[k]->size() == 0)
					{
						delete tailNode->StocLabels[k]; 
						tailNode->StocLabels.erase(k);
					}

					if(needUpdate)
					{
						if(find(Q.begin(), Q.end(), tailNode) == Q.end())
						{
							Q.push_back(tailNode);
						}
					}
					needUpdate = false;
				}

			    // clear tempLabels
				for(vector<StocLABEL*>::iterator iter = tempLabels.begin(); iter != tempLabels.end(); iter++)
				{
					delete *iter;
				}
				tempLabels.clear();
			}
		}
	}
	//cout << "Finished!" << endl;
	return 1;
}

int TNM_SNETRF::StochasticShortestPathWR(RFSNODE *dest, double mLimit, int n, int method)
{
	clock_t start1, end1;
	double cpuTime;
	double totalSize = 0;
	double avgSize;

	start1 = clock();

	for(int i=0; i < numOfNode; i++)
	{
		SNewNodeID.push_back(-1);
		SOldNodeID.push_back(-1);
	}

	RFSINFO *chargingNode;
	RFSNODE *normNode;
	vector<StocLABEL*> *labels;

	int chargingNodeID;
	int newNodeID = 1;
	// Solve the non-relay stochastic shortest path problem for the charging nodes 
	for(vector<RFSINFO*>::iterator rfsIter = rfsStations.begin(); rfsIter != rfsStations.end(); rfsIter++)
	{
		chargingNode = *rfsIter;
		chargingNodeID = chargingNode->m_nid;
		//cout << "charging node is " << chargingNodeID << endl;
		StochasticShortestPathWOR(rfsNodes[chargingNodeID - 1], mLimit, n, method);
		totalSize += AvgLabelSizeAtFirstStage(n); 

		if(SNewNodeID[chargingNodeID - 1] < 0)
		{
			SNewNodeID[chargingNodeID - 1] = newNodeID;
			SOldNodeID[newNodeID - 1] = chargingNodeID;
			newNodeID++;
		}

		for(vector<RFSNODE*>::iterator iter = rfsNodes.begin(); iter != rfsNodes.end(); iter++)
		{
			normNode = *iter;
			if(normNode->SNode->id != chargingNodeID && normNode->StocLabels.find(n) != normNode->StocLabels.end())
			{
				labels = normNode->StocLabels[n];
				if(SNewNodeID[normNode->SNode->id - 1] < 0)
				{
					SNewNodeID[normNode->SNode->id - 1] = newNodeID;
					SOldNodeID[newNodeID - 1] = normNode->SNode->id;
					newNodeID++;
				}
				sspRecord.insert(pair<pair<int,int>, vector<StocLABEL*>*>(pair<int,int>(SNewNodeID[normNode->SNode->id - 1], SNewNodeID[chargingNodeID - 1]), labels));
				normNode->StocLabels.erase(n);
			}
		}
	}

	// Solve the non-relay stochastic shortest path problem for the destination node 
	if(!dest->IsRefuelNode())
	{
		StochasticShortestPathWOR(dest, mLimit, n, method);
		totalSize += AvgLabelSizeAtFirstStage(n); 
		if(SNewNodeID[dest->SNode->id - 1] < 0)
		{
			SNewNodeID[dest->SNode->id - 1] = newNodeID;
			SOldNodeID[newNodeID - 1] = dest->SNode->id;
			newNodeID++;
		}

		for(vector<RFSNODE*>::iterator iter = rfsNodes.begin(); iter != rfsNodes.end(); iter++)
		{
			normNode = *iter;
			if(normNode->SNode->id != dest->SNode->id && normNode->StocLabels.find(n) != normNode->StocLabels.end())
			{
				labels = normNode->StocLabels[n];
				if(SNewNodeID[normNode->SNode->id - 1] < 0)
				{
					SNewNodeID[normNode->SNode->id - 1] = newNodeID;
					SOldNodeID[newNodeID - 1] = normNode->SNode->id;
					newNodeID++;
				}
				if(method == 1)
				{
					vector<StocLABEL*> *minMaxLabels = new vector<StocLABEL*>;
					StocLABEL *minMaxLabel = new StocLABEL;
					vector<StocLABEL*>::iterator first = labels->begin();
					minMaxLabel->expCost = (*first)->expCost;
					minMaxLabel->expRange = (*first)->expRange;
					minMaxLabel->nextNodeID = (*first)->nextNodeID;
					minMaxLabel->probOfArrival = (*first)->probOfArrival;

					for(vector<StocLABEL*>::iterator minMaxIter = labels->begin()+1; minMaxIter != labels->end(); minMaxIter++)
					{
						if((*minMaxIter)->expCost < minMaxLabel->expCost)
						{
							minMaxLabel->expCost = (*minMaxIter)->expCost;
							minMaxLabel->expRange = (*minMaxIter)->expRange;
							minMaxLabel->nextNodeID = (*minMaxIter)->nextNodeID;
						}
						if((*minMaxIter)->probOfArrival > minMaxLabel->probOfArrival)
						{
							minMaxLabel->probOfArrival = (*minMaxIter)->probOfArrival;
						}
					}

					minMaxLabels->push_back(minMaxLabel);
					sspRecord.insert(pair<pair<int,int>, vector<StocLABEL*>*>(pair<int,int>(SNewNodeID[normNode->SNode->id - 1], SNewNodeID[dest->SNode->id - 1]), minMaxLabels));
				}
				if(method == 2)
				{
					sspRecord.insert(pair<pair<int,int>, vector<StocLABEL*>*>(pair<int,int>(SNewNodeID[normNode->SNode->id - 1], SNewNodeID[dest->SNode->id - 1]), labels));
					normNode->StocLabels.erase(n);
				}
			}
		}
	}

	end1 = clock();
	cpuTime = (end1 - start1) / double(CLOCKS_PER_SEC);
	cout << "The CPU time in the first stage is " << cpuTime << endl;
	totalCPUTime = cpuTime; 

	avgSize = totalSize / (rfsStations.size() + 1);
	cout << "The average label size at first stage is " << avgSize << endl;
	BuildStocMetaNetwork(newNodeID - 1);

	return 1;
}

int TNM_SNETRF::StochasticShortestPathToDest(RFSNODE *dest, int method)
{
	clock_t start, end;
	double cpuTime;
	
	double minProb = 0.8;
	double chargingCostPerUnit = 0.15;
	double fixedChargingCost = 5;

	sMetaNet = new TNM_SNET("E:\\Projects\\VNET\\Networks\\TAPAS\\EX\\smn");
	if (sMetaNet->BuildTAPAS(false) !=0)
	{
		return 1;
	}

	start = clock();

	for(vector<RFSNODE*>::iterator iter = rfsNodes.begin(); iter != rfsNodes.end(); iter++)
		(*iter)->FinalStocLabels = new vector<StocLABEL*>;

	int destID = SNewNodeID[dest->SNode->id - 1];
	StocLABEL *label = new StocLABEL;
	label->expCost = 0;
	label->expRange = 0;
	label->nextNodeID = 0;
	label->probOfArrival = 1;
	dest->FinalStocLabels->push_back(label);

	TNM_SNODE *curNode;
	TNM_SNODE *tailNode;
	vector<TNM_SNODE*> Q;
	Q.push_back(sMetaNet->nodeVector[destID - 1]);
	vector<StocLABEL*> *pathLabels;
	vector<StocLABEL*> *nodeLabels;
	vector<StocLABEL*> candidateLabels;

	bool dominated = false;
	bool needUpdate = false;
	double minCost;
	double range;
	double maxProb;

	int i = 0;
	while(!Q.empty())
	{
		i++;
		curNode = *Q.begin();
		Q.erase(Q.begin());
		for(PTRTRACE pv = curNode->backStar.begin(); pv != curNode->backStar.end(); pv++)
		{
			tailNode = sMetaNet->nodeVector[(*pv)->tail->id - 1];
			pathLabels = sspRecord[pair<int,int>(tailNode->id,curNode->id)];
			nodeLabels = rfsNodes[SOldNodeID[curNode->id - 1] - 1]->FinalStocLabels;
			if(method == 1)
			{
				vector<StocLABEL*>::iterator first = nodeLabels->begin();
				minCost = (*first)->expCost;
				range = (*first)->expRange;
				maxProb = (*first)->probOfArrival;
				for(vector<StocLABEL*>::iterator nodeIter = nodeLabels->begin(); nodeIter != nodeLabels->end(); nodeIter++)
				{
					if( (*nodeIter)->expCost < minCost )
					{
						minCost = (*nodeIter)->expCost;
						range = (*nodeIter)->expRange;
					}
					if( (*nodeIter)->probOfArrival > maxProb )
					{
						maxProb = (*nodeIter)->probOfArrival;
					}
				}

				for(vector<StocLABEL*>::iterator pathIter = pathLabels->begin(); pathIter != pathLabels->end(); pathIter++)
				{
					StocLABEL *label = new StocLABEL;
					if(curNode->id != destID)
					{
						label->expCost = minCost + (*pathIter)->expCost + chargingCostPerUnit * (*pathIter)->expRange + fixedChargingCost;
					}
					else
					{
						label->expCost = minCost + (*pathIter)->expCost;
					}
					label->expRange = range + (*pathIter)->expRange;
					label->probOfArrival = maxProb * (*pathIter)->probOfArrival;
					label->nextNodeID = curNode->id;
					candidateLabels.push_back(label);
				}

			}
			if(method == 2)
			{
				for(vector<StocLABEL*>::iterator nodeIter = nodeLabels->begin(); nodeIter != nodeLabels->end(); nodeIter++)
				{
					for(vector<StocLABEL*>::iterator pathIter = pathLabels->begin(); pathIter != pathLabels->end(); pathIter++)
					{
						StocLABEL *label = new StocLABEL;
						if(curNode->id != destID)
						{
							label->expCost = (*nodeIter)->expCost + (*pathIter)->expCost + chargingCostPerUnit * (*pathIter)->expRange + fixedChargingCost;
						}
						else
						{
							label->expCost = (*nodeIter)->expCost + (*pathIter)->expCost;
						}
						label->expRange = (*nodeIter)->expRange + (*pathIter)->expRange;
						label->probOfArrival = (*nodeIter)->probOfArrival * (*pathIter)->probOfArrival;
						label->nextNodeID = curNode->id;
						candidateLabels.push_back(label);
					}
				}
			}

			nodeLabels = rfsNodes[SOldNodeID[tailNode->id - 1] - 1]->FinalStocLabels;
			for(vector<StocLABEL*>::iterator iterI = candidateLabels.begin(); iterI != candidateLabels.end(); )
			{
				dominated = false;
				if( (*iterI)->probOfArrival < minProb )
				{
					iterI++;
					continue;
				}

				for(vector<StocLABEL*>::iterator iterJ = nodeLabels->begin(); iterJ != nodeLabels->end(); )
				{
					if( (*iterJ)->expCost <= (*iterI)->expCost && (*iterJ)->probOfArrival >= (*iterI)->probOfArrival )
					{
						dominated = true;
						break;
					}
					else if( (*iterI)->expCost <= (*iterJ)->expCost && (*iterI)->probOfArrival >= (*iterJ)->probOfArrival )
					{
						delete *iterJ;
						nodeLabels->erase(iterJ);
					}
					else
					{
						iterJ++;
					}
				}

				if(!dominated)
				{
					needUpdate = true;
					nodeLabels->push_back(*iterI);
					candidateLabels.erase(iterI);
				}
				else
				{
					iterI++;
				}
			}

			if(needUpdate)
			{
				if(find(Q.begin(), Q.end(), tailNode) == Q.end() && rfsNodes[SOldNodeID[tailNode->id - 1] - 1]->IsRefuelNode())
				{
					Q.push_back(tailNode);
				}
			}
			needUpdate =false;

			// Clear the candidateLabels
			for(vector<StocLABEL*>::iterator iter = candidateLabels.begin(); iter != candidateLabels.end(); iter++)
			{
				delete *iter;
			}
			candidateLabels.clear();
		}
	}

	end = clock();
	cpuTime = (end - start) / double(CLOCKS_PER_SEC);
	cout << "The CPU time in the second stage is " << cpuTime << endl;
	totalCPUTime += cpuTime;
	return 1;
}

int TNM_SNETRF::BuildStocMetaNetwork(int numOfNodes)
{
	ofstream newNodeFile;
	newNodeFile.open("E:\\Projects\\VNET\\Networks\\TAPAS\\EX\\smn_nod.dat");
	newNodeFile << "Node      X      Y\n";
	for (int j = 1; j <= numOfNodes; j++)
		newNodeFile << "" << j << "         0      0\n";
	newNodeFile.close();

	vector<StocLABEL*> *labels;
	vector<StocLABEL*>::iterator labelIter;

	ofstream newNetFile;
	newNetFile.open("E:\\Projects\\VNET\\Networks\\TAPAS\\EX\\smn_net.dat");
	newNetFile << "<NUMBER OF ZONES>      " << numOfNodes <<"\n";
	newNetFile << "<NUMBER OF NODES>      " << numOfNodes <<"\n";
	newNetFile << "<FIRST THRU NODE>      1\n";
	newNetFile << "<NUMBER OF LINKS>      " << sspRecord.size() <<"\n";
	newNetFile << "<END OF METADATA>\n\n";
	newNetFile << "~from  to  capacity  length  ftime(min)  B  power  speed(mpm)  toll  type\n\n";
	for (map<pair<int,int>, vector<StocLABEL*>*>::iterator elem = sspRecord.begin(); elem != sspRecord.end(); elem++)
	{
		newNetFile << "" << elem->first.first << "     " << elem->first.second << "     1     1     1";
		newNetFile << "     1     1     1     1     1     ;\n";
		labels = elem->second;
		//cout<< "Path information: Label size is " << labels->size() << endl;
		//cout<< "" << SOldNodeID[elem->first.first - 1] << "     " << SOldNodeID[elem->first.second - 1] << endl;
	}
	newNetFile.close();

	return 1;
}

StocLABEL *TNM_SNETRF::GetOptimalLabel(int orgID)
{
	RFSNODE *curNode;
	StocLABEL *optimalLabel;
	StocLABEL *label;

	curNode = rfsNodes[orgID - 1];

	optimalLabel = curNode->FinalStocLabels->front();
	for(vector<StocLABEL*>::iterator iter = curNode->FinalStocLabels->begin() + 1; iter != curNode->FinalStocLabels->end(); iter++)
	{
		label = *iter;
		if(label->expCost < optimalLabel->expCost)
			optimalLabel = label;
	}

	return optimalLabel;
}

int TNM_SNETRF::GetOptimalChargingPath(int orgID, int destID, bool optimal, int method)
{
	vector<int> path;
	int nextNodeIDNew = -1;
	int nextNodeIDOld;
	int curNodeIDNew;
	path.push_back(orgID);

	RFSNODE *curNode;
	RFSNODE *nextNode;
	curNode = rfsNodes[orgID - 1];
	StocLABEL *optimalLabel;
	StocLABEL *label;
	double curRange;
	double curProb;
	vector<StocLABEL*> *pathLabels;
	vector<StocLABEL*> *nodeLabels;

	if(method == 1)
	{
		nextNodeIDOld = -1;
		while(nextNodeIDOld != destID)
		{
			optimalLabel = curNode->FinalStocLabels->front();
			for(vector<StocLABEL*>::iterator iter = curNode->FinalStocLabels->begin() + 1; iter != curNode->FinalStocLabels->end(); iter++)
			{
				label = *iter;
				if(label->expCost < optimalLabel->expCost)
					optimalLabel = label;
			}

			if(curNode->SNode->id == orgID)
			{
				cout << "The optimal cost is " << optimalLabel->expCost << endl;
				cout << "The expected range is " << optimalLabel->expRange << endl;
				cout << "The probability of arrival is " << optimalLabel->probOfArrival << endl;
			}

			nextNodeIDNew = optimalLabel->nextNodeID;
			nextNodeIDOld = SOldNodeID[nextNodeIDNew - 1];
			path.push_back(nextNodeIDOld);
			curNode = rfsNodes[nextNodeIDOld - 1];
		}

		cout << "The optimal charging path is: " << endl;
		cout << orgID;
		for(vector<int>::iterator iter = path.begin()+1; iter != path.end(); iter++)
		{
			cout << "->" << *iter;
		}
		cout << endl;
	}

	if(method == 2)
	{
		if(optimal)
		{
			optimalLabel = curNode->FinalStocLabels->front();
			for(vector<StocLABEL*>::iterator iter = curNode->FinalStocLabels->begin() + 1; iter != curNode->FinalStocLabels->end(); iter++)
			{
				label = *iter;
				if(label->expCost < optimalLabel->expCost)
					optimalLabel = label;
			}

			nextNodeIDNew = optimalLabel->nextNodeID;
			nextNodeIDOld = SOldNodeID[nextNodeIDNew - 1];
			path.push_back(nextNodeIDOld);
			curRange = optimalLabel->expRange;
			curProb = optimalLabel->probOfArrival;
			curNodeIDNew = SNewNodeID[orgID - 1];
			bool found = false;
			while(nextNodeIDOld != destID)
			{
				found = false;
				pathLabels = sspRecord[pair<int,int>(curNodeIDNew,nextNodeIDNew)];
				for(vector<StocLABEL*>::iterator iterI = pathLabels->begin(); iterI != pathLabels->end(); iterI++)
				{
					nodeLabels = rfsNodes[nextNodeIDOld - 1]->FinalStocLabels;
					for(vector<StocLABEL*>::iterator iterJ = nodeLabels->begin(); iterJ != nodeLabels->end(); iterJ++)
					{
						if(abs(curRange - (*iterI)->expRange - (*iterJ)->expRange) < 0.001 && abs(curProb / (*iterI)->probOfArrival - (*iterJ)->probOfArrival) < 0.001)
						{
							found = true;
							curNodeIDNew = nextNodeIDNew;
							nextNodeIDNew = (*iterJ)->nextNodeID;
							nextNodeIDOld = SOldNodeID[nextNodeIDNew - 1];
							curRange = (*iterJ)->expRange;
							curProb = (*iterJ)->probOfArrival;
							path.push_back(nextNodeIDOld);
							break;
						}
					}
					if(found)
						break;
				}
			}

			cout << "The optimal cost is " << optimalLabel->expCost << endl;
			cout << "The expected range is " << optimalLabel->expRange << endl;
			cout << "The probability of arrival is " << optimalLabel->probOfArrival << endl;
			cout << "The optimal charging path is: " << endl;
			cout << orgID;
			for(vector<int>::iterator iter = path.begin()+1; iter != path.end(); iter++)
			{
				cout << "->" << *iter;
			}
			cout << endl;
		}
		else
		{
			for(vector<StocLABEL*>::iterator iter = curNode->FinalStocLabels->begin(); iter != curNode->FinalStocLabels->end(); iter++)
			{
				optimalLabel = *iter;

				nextNodeIDNew = optimalLabel->nextNodeID;
				nextNodeIDOld = SOldNodeID[nextNodeIDNew - 1];
				path.push_back(nextNodeIDOld);
				curRange = optimalLabel->expRange;
				curProb = optimalLabel->probOfArrival;
				curNodeIDNew = SNewNodeID[orgID - 1];
				bool found = false;
				while(nextNodeIDOld != destID)
				{
					found = false;
					pathLabels = sspRecord[pair<int,int>(curNodeIDNew,nextNodeIDNew)];
					for(vector<StocLABEL*>::iterator iterI = pathLabels->begin(); iterI != pathLabels->end(); iterI++)
					{
						nodeLabels = rfsNodes[nextNodeIDOld - 1]->FinalStocLabels;
						for(vector<StocLABEL*>::iterator iterJ = nodeLabels->begin(); iterJ != nodeLabels->end(); iterJ++)
						{
							if(abs(curRange - (*iterI)->expRange - (*iterJ)->expRange) < 0.001 && abs(curProb / (*iterI)->probOfArrival - (*iterJ)->probOfArrival) < 0.001)
							{
								found = true;
								curNodeIDNew = nextNodeIDNew;
								nextNodeIDNew = (*iterJ)->nextNodeID;
								nextNodeIDOld = SOldNodeID[nextNodeIDNew - 1];
								curRange = (*iterJ)->expRange;
								curProb = (*iterJ)->probOfArrival;
								path.push_back(nextNodeIDOld);
								break;
							}
						}
						if(found)
							break;
					}
				}

				cout << "The expected cost is " << optimalLabel->expCost << endl;
				cout << "The expected range is " << optimalLabel->expRange << endl;
				cout << "The probability of arrival is " << optimalLabel->probOfArrival << endl;
				cout << "The corresponding charging path is: " << endl;
				cout << orgID;
				for(vector<int>::iterator iter = path.begin()+1; iter != path.end(); iter++)
				{
					cout << "->" << *iter;
				}
				cout << endl;
				path.clear();
				path.push_back(orgID);
			}
		}
	}

	return 1;
}

int TNM_SNETRF::PrintLabelsAtFirstStage(int n)
{
	RFSNODE *node;
	for(vector<RFSNODE*>::iterator iterI = rfsNodes.begin(); iterI != rfsNodes.end(); iterI++)
	{
		node = *iterI;
		cout << "Node ID is " << node->SNode->id << endl;
		for(int i = n; i > 0; i--)
		{
			if(node->StocLabels.find(i) != node->StocLabels.end())
			{
				cout << "At remaining range " << i << ", the labels are " << endl;
				cout << "Expected cost      Probability      Next Node" << endl;
				for(vector<StocLABEL*>::iterator iterJ = node->StocLabels[i]->begin(); iterJ != node->StocLabels[i]->end(); iterJ++)
				{
					cout << "" << (*iterJ)->expCost << "   " << (*iterJ)->probOfArrival << "   " << (*iterJ)->nextNodeID << endl;
				}
			}
			else
			{
				break;
			}
		}
		getchar();
	}

	return 1;
}

double TNM_SNETRF::AvgLabelSizeAtFirstStage(int n)
{
	double totalSize = 0;
	double avgSize;

	RFSNODE *node;
	for(vector<RFSNODE*>::iterator iterI = rfsNodes.begin(); iterI != rfsNodes.end(); iterI++)
	{
		node = *iterI;
		for(map<int, vector<StocLABEL*>*>::iterator iterJ = node->StocLabels.begin(); iterJ != node->StocLabels.end(); iterJ++)
		{
			totalSize += (*iterJ).second->size();
		}
	}

	avgSize = totalSize / rfsNodes.size() / n;
	return avgSize;
}

int TNM_SNETRF::AvgLabelSizeAtSecondStage()
{
	double totalSize = 0;
	double avgSize = 0;
	for(vector<RFSNODE*>::iterator iter = rfsNodes.begin(); iter != rfsNodes.end(); iter++)
	{
		totalSize += (*iter)->FinalStocLabels->size();
	}
	avgSize = totalSize / rfsNodes.size();
	cout << "The average label size at second stage is " << avgSize << endl;
	return 1;
}

int TNM_SNETRF::ShortestPathWR_LS(RFSNODE *dest, double mLimit)
{
	clock_t start,end;
	double cpuTime;
	start = clock();
	
    RFSNODE *rfsNode, *rfsTailNode;
	TNM_SNODE *sNode, *sTailNode;
	TNM_SLINK *link;
	LABEL *ilabel = new LABEL;
	

	int destID = dest->SNode->id;
	cout << "destination id is " << destID << endl;
	ilabel->nextNodeID = destID;
	ilabel->subDistance = 0.0;
	ilabel->totalDistance = 0.0;
	dest->Labels.insert(pair<int, LABEL*>(destID,ilabel));
	
	multimap<double, pair<int, int>, less<double>> Q;
	Q.insert(pair<double, pair<int, int>>(0.0, pair<int, int>(destID, destID)));

	multimap<double, pair<int, int>, less<double>>::iterator firstElem;
	int iter = 0;
	int curNodeID;
	int nextRefuelID;
	int tailNodeID;
	
	double subDistance;
	double totalDistance;
	double curTotalDistance;

	while(!Q.empty())
	{
		iter++;
		firstElem = Q.begin();
		curNodeID = firstElem->second.first;
		nextRefuelID = firstElem->second.second;
		Q.erase(firstElem);

		sNode = CatchNodePtr(curNodeID);
		for(PTRTRACE pv = sNode->backStar.begin(); pv != sNode->backStar.end(); pv++)
		{
			link = *pv;
			rfsNode = rfsNodes[curNodeID - 1];
			tailNodeID = link->tail->id;
			LABEL *label;
			label = rfsNode->Labels.find(nextRefuelID)->second;
			rfsTailNode = rfsNodes[tailNodeID - 1];
			subDistance = link->cost + label->subDistance;

			if(subDistance <= mLimit && nextRefuelID != tailNodeID)
			{
				totalDistance = link->cost + label->totalDistance;
				//cout << "total distance is" << totalDistance << endl;
				if(rfsTailNode->Labels.count(nextRefuelID) > 0)
				{
					curTotalDistance = rfsTailNode->Labels.find(nextRefuelID)->second->totalDistance;
					if (totalDistance < curTotalDistance)
					{
						rfsTailNode->Labels.find(nextRefuelID)->second->nextNodeID = curNodeID;
						rfsTailNode->Labels.find(nextRefuelID)->second->subDistance = subDistance;
						rfsTailNode->Labels.find(nextRefuelID)->second->totalDistance = totalDistance;
						DeleteKeyPair(&Q, curTotalDistance, tailNodeID, nextRefuelID);
						Q.insert(pair<double, pair<int, int>>(totalDistance, pair<int, int>(tailNodeID, nextRefuelID)));
					}
				}
				else
				{
					LABEL *newLabel = new LABEL;
					newLabel->nextNodeID = curNodeID;
					newLabel->subDistance = subDistance;
					newLabel->totalDistance = totalDistance;
					rfsTailNode->Labels[nextRefuelID] = newLabel;
					Q.insert(pair<double, pair<int, int>>(totalDistance, pair<int, int>(tailNodeID, nextRefuelID)));
				}
			}

			if(rfsNode->IsRefuelNode() && link->cost <= mLimit)
			{
				totalDistance = link->cost + label->totalDistance;
				if(rfsTailNode->Labels.count(curNodeID) > 0)
				{
					curTotalDistance = rfsTailNode->Labels.find(curNodeID)->second->totalDistance;
					if(totalDistance < curTotalDistance)
					{
						rfsTailNode->Labels.find(curNodeID)->second->nextNodeID = curNodeID;
						rfsTailNode->Labels.find(curNodeID)->second->subDistance = link->cost;
						rfsTailNode->Labels.find(curNodeID)->second->totalDistance = totalDistance;
						DeleteKeyPair(&Q, curTotalDistance, tailNodeID, curNodeID);
						Q.insert(pair<double, pair<int, int>>(totalDistance, pair<int, int>(tailNodeID, curNodeID)));
					}
				}
				else
				{
					LABEL *newLabel2 = new LABEL;
					newLabel2->nextNodeID = curNodeID;
					newLabel2->subDistance = link->cost;
					newLabel2->totalDistance = totalDistance;
					rfsTailNode->Labels[curNodeID] = newLabel2;
					Q.insert(pair<double, pair<int, int>>(totalDistance, pair<int, int>(tailNodeID, curNodeID)));
				}
			}

		}

	}

	end = clock();
	cpuTime = (end - start)/double(CLOCKS_PER_SEC);
	cout << "CPU Time of Label Setting Method is " << cpuTime << endl;
    return 0;
}

int TNM_SNETRF::ShortestPathWR_MN(TNM_SNODE *dest, double mLimit)
{
	clock_t start1,start2,end1,end2;
	double cpuTime;
	start1 = clock();

	for (int i = 0; i < numOfNode; i++)
		NewNodeID.push_back(-1);

	RFSINFO *chargingNode;
	TNM_SNODE *normNode;
	TNM_SPATH *path;
	int chargingNodeID;
	int newNodeID = 1; 
	for(vector<RFSINFO*>::iterator rfsIter = rfsStations.begin(); rfsIter != rfsStations.end(); rfsIter++)
	{
		chargingNode = *rfsIter;
		chargingNodeID = chargingNode->m_nid; 
		UpdateSPR(nodeVector[chargingNodeID - 1]);
		if (NewNodeID[chargingNodeID - 1] < 0)
		{
			NewNodeID[chargingNodeID - 1] = newNodeID;
			newNodeID++;
		}
		
		for (vector<TNM_SNODE*>::iterator nodeIter = nodeVector.begin(); nodeIter != nodeVector.end(); nodeIter++)
		{
			normNode = *nodeIter;
			//cout << "path cost is " << normNode->pathElem->cost << endl;
			if (normNode->id != chargingNodeID && normNode->pathElem->cost <= mLimit)
			{
				path = GetSPath_R_(normNode, nodeVector[chargingNodeID - 1]);
				if (NewNodeID[normNode->id - 1] < 0)
				{
					NewNodeID[normNode->id - 1] = newNodeID;
					newNodeID++;
				}
				spRecord.insert(pair<pair<int,int>, TNM_SPATH*>(pair<int,int>(NewNodeID[normNode->id - 1], NewNodeID[chargingNodeID - 1]), path));
			}
		}
	}
	
	UpdateSPR(dest);
	if (NewNodeID[dest->id - 1] < 0)
	{
		NewNodeID[dest->id - 1] = newNodeID;
		newNodeID++;
	}
	for (vector<TNM_SNODE*>::iterator nodeIter = nodeVector.begin(); nodeIter != nodeVector.end(); nodeIter++)
	{
		normNode = *nodeIter;
		if (normNode->id != dest->id && normNode->pathElem->cost <= mLimit)
		{
			path = GetSPath_R_(normNode, nodeVector[dest->id - 1]);
			if (NewNodeID[normNode->id - 1] < 0)
			{
				NewNodeID[normNode->id - 1] = newNodeID;
				newNodeID++;
			}
			spRecord.insert(pair<pair<int,int>, TNM_SPATH*>(pair<int,int>(NewNodeID[normNode->id - 1], NewNodeID[dest->id - 1]), path));
		}
	}
	
	end1 = clock();

	ofstream newNodeFile;
	newNodeFile.open("E:\\Projects\\VNET\\Networks\\TAPAS\\EX\\mn_nod.dat");
	newNodeFile << "Node      X      Y\n";
	for (int j = 1; j < newNodeID; j++)
		newNodeFile << "" << j << "         0      0\n";
	newNodeFile.close();

	ofstream newNetFile;
	newNetFile.open("E:\\Projects\\VNET\\Networks\\TAPAS\\EX\\mn_net.dat");
	newNetFile << "<NUMBER OF ZONES>      " << newNodeID - 1 <<"\n";
	newNetFile << "<NUMBER OF NODES>      " << newNodeID - 1 <<"\n";
	newNetFile << "<FIRST THRU NODE>      1\n";
	newNetFile << "<NUMBER OF LINKS>      " << spRecord.size() <<"\n";
	newNetFile << "<END OF METADATA>\n\n";
	newNetFile << "~from  to  capacity  length  ftime(min)  B  power  speed(mpm)  toll  type\n\n";
	for (map<pair<int,int>, TNM_SPATH*>::iterator elem = spRecord.begin(); elem != spRecord.end(); elem++)
	{
		newNetFile << "" << elem->first.first << "     " << elem->first.second << "     1     1     1";
		newNetFile << "     1     1     1     " << elem->second->cost << "     1     ;\n";
	}
	newNetFile.close();

	metaNet = new TNM_SNET("E:\\Projects\\VNET\\Networks\\TAPAS\\EX\\mn");
	if (metaNet->BuildTAPAS(false) !=0)
	{
		return 1;
	}
	
	start2 = clock();
	metaNet->UpdateSPR(metaNet->nodeVector[NewNodeID[dest->id - 1] - 1]);
	end2 = clock();
	cpuTime = (end2 - start2 + end1 - start1)/double(CLOCKS_PER_SEC);
	cout << "CPU Time of Meta Network Method is " << cpuTime << endl;
	return 0;
}

TNM_SPATH* TNM_SNETRF::GetSPWR_LS(int orgID, int destID)
{
	TNM_SPATH *path = new TNM_SPATH;
	int nextNodeID = -1;
	int nextRefuelNodeID;
	RFSNODE *rfsNode;
	TNM_SLINK *link;

	rfsNode = rfsNodes[orgID -1];
	map<int, LABEL*>::iterator label;
	double totalDistance = 1e10;
	double curTotalDistance;

	for(label = rfsNode->Labels.begin(); label != rfsNode->Labels.end(); label++)
	{
		curTotalDistance = label->second->totalDistance;
		if(curTotalDistance < totalDistance)
		{
			totalDistance = curTotalDistance;
			nextNodeID = label->second->nextNodeID;
			nextRefuelNodeID = label->first;
		}
	}

	path->cost = totalDistance;
	//cout << path->cost << endl;
	link = CatchLinkPtr(orgID, nextNodeID);
	path->path.push_back(link);

	int curNodeID;
	while(nextNodeID != destID)
	{
		curNodeID = nextNodeID;
		rfsNode = rfsNodes[nextNodeID - 1];
		if(nextNodeID != nextRefuelNodeID)
		{
			nextNodeID = rfsNode->Labels[nextRefuelNodeID]->nextNodeID;
		}
		else
		{
			for(label = rfsNode->Labels.begin(); label != rfsNode->Labels.end(); label++)
			{
				curTotalDistance = label->second->totalDistance;
				if(curTotalDistance < totalDistance)
				{
					totalDistance = curTotalDistance;
					nextNodeID = label->second->nextNodeID;
					nextRefuelNodeID = label->first;
				}
			}
		}
		link = CatchLinkPtr(curNodeID, nextNodeID);
		path->path.push_back(link);
	}

	return path;
}

TNM_SPATH* TNM_SNETRF::GetSPWR_MN(int orgID, int destID)
{
	TNM_SPATH *fullPath = new TNM_SPATH;
    TNM_SPATH *metaPath;
	TNM_SPATH *subPath;
	TNM_SLINK *link;

	int newOrgID;
	int newDestID;

	newOrgID = NewNodeID[orgID - 1];
	newDestID = NewNodeID[destID - 1];
	cout << "org is " << newOrgID << " dest is " << newDestID << endl;
	cout << metaNet->nodeVector[newOrgID - 1]->id << " " << metaNet->nodeVector[newDestID - 1]->id << endl;

	metaPath = metaNet->GetSPath_R_(metaNet->nodeVector[newOrgID - 1], metaNet->nodeVector[newDestID - 1]);
	//for (int i = 1; i < 12982; i++)
	//{
	//	metaPath = metaNet->GetSPath_R_(metaNet->nodeVector[i - 1], metaNet->nodeVector[newDestID - 1]);
	//	if (metaPath)
	//	{
	//		cout << "good org is " << i << endl;
	//		cout << "path cost is " << metaPath->cost << endl;
	//		getchar();
	//	}
	//}
	


	for (vector<TNM_SLINK*>::iterator iter1 = metaPath->path.begin(); iter1 != metaPath->path.end(); iter1++)
	{
		link = *iter1;
		subPath = spRecord[pair<int,int>(link->tail->id, link->head->id)];
		for (vector<TNM_SLINK*>::iterator iter2 = subPath->path.begin(); iter2 != subPath->path.end(); iter2++)
		{
			link = *iter2;
			fullPath->path.push_back(link);
			fullPath->cost = fullPath->PathCostS();
		}
	}

	return fullPath;
}


int TNM_SNETRF::RSGenerator(string netFile, string rfsFile, double mLimit)
{
	TNM_SNET *net = new TNM_SNET(netFile);
	if(net->BuildTAPAS(false) != 0)
	{
		return 1;
	}
	TNM_SLINK *link;
	for(vector<TNM_SLINK*>::iterator iter = net->linkVector.begin(); iter != net->linkVector.end(); iter++)
	{
		link = *iter;
		link->cost = link->length;
	}

	vector<int> rfs;
	rfs.push_back(1);
	double maxMinDistance = 100000;
	double minDistance = 100000;
	TNM_SNODE *normNode;
	TNM_SPATH *sPath;
	int rfsNodeID;
	int rfsCandidateID;
	vector<TNM_SNODE*> nodeVector;

	for(vector<TNM_SNODE*>::iterator iter = net->nodeVector.begin(); iter != net->nodeVector.end(); iter++)
	{
		normNode = *iter;
		nodeVector.push_back(normNode);
	}

	int it = 0;
	while(maxMinDistance > mLimit && nodeVector.size() > 0)
	{
		cout << it << endl;
		it++;
		maxMinDistance = -1;
		for(vector<TNM_SNODE*>::iterator iter1 = nodeVector.begin(); iter1 != nodeVector.end();)
		{
			normNode = *iter1;
			net->UpdateSP(normNode);
			minDistance = 100000;
			int flag = -1;
			for(vector<int>::iterator iter2 = rfs.begin(); iter2 != rfs.end(); iter2++)
			{
				rfsNodeID = *iter2;
				if (normNode->id != rfsNodeID)
				{
					sPath = net->GetSPath_(normNode, net->nodeVector[rfsNodeID - 1]);
					if(sPath && sPath->cost < minDistance)
					{
						minDistance = sPath->cost;
						flag = 1;
					}
				}
			}
			if(minDistance <= mLimit)
			{
				vector<TNM_SNODE*>::iterator pos = find(nodeVector.begin(), nodeVector.end(), normNode);
				if(pos != nodeVector.end())
				{
					nodeVector.erase(pos);
				}
			}
			else
			{
				iter1++;
			}
			if(flag > 0 && minDistance > maxMinDistance)
			{
				maxMinDistance = minDistance;
				rfsCandidateID = normNode->id;
			}
		}

		if(maxMinDistance > mLimit)
		{
			vector<int>::iterator pos = find(rfs.begin(), rfs.end(), rfsCandidateID);
			if(pos != rfs.end())
			{
				TNM_SNODE *rfsNode;
				rfsNode = net->nodeVector[rfsCandidateID - 1];
				net->UpdateSP(rfsNode);
				for(vector<TNM_SNODE*>::iterator iter1 = net->nodeVector.begin(); iter1 != net->nodeVector.end(); iter1++)
				{
					normNode = *iter1;
					sPath = net->GetSPath_(rfsNode,normNode);
					if(sPath && sPath->cost > 0.8 * mLimit && sPath->cost < mLimit)
					{
						rfs.push_back(normNode->id);
						cout << "Occured!" << endl;
						break;
					}
				}
				/*double maxMin = -1;
				double min = 100000;
				int flag2 = -1;
				for(vector<TNM_SNODE*>::iterator iter1 = net->nodeVector.begin(); iter1 != net->nodeVector.end(); iter1++)
				{
					normNode = *iter1;
					net->UpdateSP(normNode);
					flag2 = -1;
					min = 100000;
					vector<int>::iterator pos2 = find(rfs.begin(), rfs.end(), normNode->id);
					if(pos2 == rfs.end())
					{
						for(vector<int>::iterator iter2 = rfs.begin(); iter2 != rfs.end(); iter2++)
						{
							rfsNodeID = *iter2;
							if (normNode->id != rfsNodeID)
							{
								sPath = net->GetSPath_(normNode, net->nodeVector[rfsNodeID - 1]);
								if(sPath && sPath->cost < min)
								{
									min = sPath->cost;
									flag2 = 1;
								}
							}
						}
						if(flag2 > 0 && min > maxMin)
						{
							rfsCandidateID = normNode->id;
						}
					}
				}

				rfs.push_back(rfsCandidateID);*/
			}
			else
			{
				rfs.push_back(rfsCandidateID);
			}
		}
	}

	ofstream newRfsFile;
	newRfsFile.open(rfsFile.c_str());
	for(vector<int>::iterator iter = rfs.begin(); iter != rfs.end(); iter++)
	{
		rfsNodeID = *iter;
		newRfsFile <<"" << rfsNodeID <<"      0\n";
	}
	newRfsFile.close();
	cout << "The refueling node file has been generated." << endl;
	return 0;
}

int TestSPWR()
{
	TNM_SNETRF *net = new TNM_SNETRF("E:\\Projects\\VNET\\Networks\\TAPAS\\EX\\ex");
	if(net->BuildTAPASRF()!=0)
	{
		return 1;
	}
	
	TNM_SLINK *link;

	for(vector<TNM_SLINK*>::iterator iter = net->linkVector.begin(); iter != net->linkVector.end(); iter++)
	{
		link = *iter;
		link->cost = link->length;
	}

	//for Chicago Network
	//double mLimit = 50;
	//for example network
	double mLimit = 25;
	int method = 1;
	//clock_t start,end;
	//double cpuTime;

	// label setting method
	//for chicago network
	//RFSNODE *dest1 = net->rfsNodes[6811];
	//for example network
	RFSNODE *dest1 = net->rfsNodes[17];

	net->ShortestPathWR_LS(dest1, mLimit);
	//for Chicago Network
	//TNM_SPATH *path1 = net->GetSPWR_LS(1,6812);
	//for example network
    TNM_SPATH *path1 = net->GetSPWR_LS(17,18);
	path1->Print(true);

	net->StochasticShortestPathWR(dest1, mLimit,25, method);
	net->StochasticShortestPathToDest(dest1, method);
	cout << "The total CPU time is " << net->totalCPUTime << endl;
	cout << "expected cost      probability" << endl;
	for(vector<StocLABEL*>::iterator iter = net->rfsNodes[16]->FinalStocLabels->begin(); iter != net->rfsNodes[16]->FinalStocLabels->end(); iter++)
	{
		cout << (*iter)->expCost << "     " << (*iter)->probOfArrival << endl;
	}
	net->GetOptimalChargingPath(17,18, false, method);

	// meta network method
	/*TNM_SNODE *dest2 = net->nodeVector[6811];
	net->ShortestPathWR_MN(dest2, mLimit);
	TNM_SPATH *path2 = net->GetSPWR_MN(1,6812);
	path2->Print(true);*/
	return 0;
}

int TestSSPWR()
{
	TNM_SNETRF *net = new TNM_SNETRF("E:\\Projects\\VNET\\Networks\\TAPAS\\CS\\chicagosketch");
	if(net->BuildTAPASRF()!=0)
	{
		return 1;
	}
	
	TNM_SLINK *link;

	for(vector<TNM_SLINK*>::iterator iter = net->linkVector.begin(); iter != net->linkVector.end(); iter++)
	{
		link = *iter;
		link->cost = link->length;
	}

	//for Chicago Network
	double mLimit = 50;

	int method = 1;
	//clock_t start,end;
	//double cpuTime;

	// label setting method
	//for chicago network
	//RFSNODE *dest1 = net->rfsNodes[6811];
	//for example network
	RFSNODE *dest1 = net->rfsNodes[856];

	//net->ShortestPathWR_LS(dest1, mLimit);
	////for Chicago Network
	////TNM_SPATH *path1 = net->GetSPWR_LS(1,6812);
	////for example network
 //   TNM_SPATH *path1 = net->GetSPWR_LS(920,857);
	//path1->Print(true);

	net->StochasticShortestPathWR(dest1, mLimit,100, method);
	net->StochasticShortestPathToDest(dest1, method);
	net->AvgLabelSizeAtSecondStage();
	cout << "The total CPU time is " << net->totalCPUTime << endl;
	cout << "expected cost      probability" << endl;
	for(vector<StocLABEL*>::iterator iter = net->rfsNodes[923]->FinalStocLabels->begin(); iter != net->rfsNodes[923]->FinalStocLabels->end(); iter++)
	{
		cout << (*iter)->expCost << "     " << (*iter)->probOfArrival << endl;
	}
	//vector<StocLABEL*>::iterator iter1 = net->rfsNodes[919]->FinalStocLabels->begin();
	//vector<StocLABEL*>::iterator iter2 = iter1 + 1;
	net->GetOptimalChargingPath(924,857, false, method);

	// output information of Path 1
	//vector<StocLABEL*> *pathLabels;
	//pathLabels = net->sspRecord[pair<int,int>(net->SNewNodeID[924-1],net->SNewNodeID[600-1])];
	//cout << "path 924-600" << endl;
	//for(vector<StocLABEL*>::iterator pathIter = pathLabels->begin(); pathIter != pathLabels->end(); pathIter++)
	//{
	//	cout << "cost: " << (*pathIter)->expCost << " range: " << (*pathIter)->expRange << " prob: " << (*pathIter)->probOfArrival << endl;
	//}

	//pathLabels = net->sspRecord[pair<int,int>(net->SNewNodeID[600-1],net->SNewNodeID[678-1])];
	//cout << "path 600-678" << endl;
	//for(vector<StocLABEL*>::iterator pathIter = pathLabels->begin(); pathIter != pathLabels->end(); pathIter++)
	//{
	//	cout << "cost: " << (*pathIter)->expCost << " range: " << (*pathIter)->expRange << " prob: " << (*pathIter)->probOfArrival << endl;
	//}

	//pathLabels = net->sspRecord[pair<int,int>(net->SNewNodeID[678-1],net->SNewNodeID[857-1])];
	//cout << "path 678-857" << endl;
	//for(vector<StocLABEL*>::iterator pathIter = pathLabels->begin(); pathIter != pathLabels->end(); pathIter++)
	//{
	//	cout << "cost: " << (*pathIter)->expCost << " range: " << (*pathIter)->expRange << " prob: " << (*pathIter)->probOfArrival << endl;
	//}

	// 15 OD pairs
	ofstream outputFile;
	outputFile.open("E:\\Projects\\C++\\Output\\out.txt");
	int orgs[] = {924, 781, 739, 794, 764, 714, 679, 727, 671, 560, 687, 761, 626, 690, 628};
	int orgID;
	vector<int> orgVec(orgs, orgs + sizeof(orgs) / sizeof(int));
	StocLABEL *optimalLabel;
	for(vector<int>::iterator iter = orgVec.begin(); iter != orgVec.end(); iter++)
	{
		orgID = *iter;
		optimalLabel = net->GetOptimalLabel(orgID);
		outputFile << "" << optimalLabel->expCost << "   " << optimalLabel->expRange << "   " << optimalLabel->probOfArrival << "\n";
	}

	// meta network method
	/*TNM_SNODE *dest2 = net->nodeVector[6811];
	net->ShortestPathWR_MN(dest2, mLimit);
	TNM_SPATH *path2 = net->GetSPWR_MN(1,6812);
	path2->Print(true);*/
	return 0;
}

int main()
{
	//TestSPWR();
	TestSSPWR();
	//TNM_SNETRF::RSGenerator("E:\\Projects\\VNET\\Networks\\TAPAS\\CS\\chicagosketch", "E:\\Projects\\VNET\\Networks\\TAPAS\\CS\\chicagosketch_rfs.dat", 50);
 
	getchar();
    return 1;
}
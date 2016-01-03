//sample driver
#include "stdafx.h"
#include "HTSim.h"

void TestNetworkIO()
{
	TNM_SNET *net = new TNM_SNET("..\\..\\resources\\networks\\tapas\\sf\\sf");
	if(net->BuildTAPAS(true)==0) //build function returns 0 when succeeds. 
	{
		net->PrintLinks();
		getchar();
		net->PrintNodes();
		getchar();
	}
	delete net;
}

void TestRandom()
{
	Uniform x;
	x.Set(0.4);
	for(int i = 0;i<10;i++)
		cout<<TNM_FloatFormat(x.Next(), 10,4)<<endl;
}

int TestBBA(const string &type, const string &path, const string &name)
{
	TNM_FloatFormat::SetFormat(12, 3);
    TAP_BBA *bob;
    if(type == "DOB") ///dial algorithm
    {
        TAP_DOB *dob = new TAP_DOB;
        bob = (TAP_BBA*) dob;
    }
    else if(type == "BOB") //bar-gera's algorithm
    {
        TAP_BOB *kob = new TAP_BOB;
        bob = (TAP_BBA*) kob;
    }
    else if(type == "NOB") //Nie'a algorithm
    {
        TAP_NOB *nob = new TAP_NOB;
        bob = (TAP_BBA*) nob;
    }
    else if(type == "QOB") //quick oba
    {
        TAP_QOB *qob = new TAP_QOB;
        bob = (TAP_BBA*) qob;
    }
    else if(type == "BOBX") //bob variant
    {
        TAP_BOBX *bobx = new TAP_BOBX;
        bob = (TAP_BBA*) bobx;
    }
    else if(type == "QOBLUCE") //quick luce
    {
        TAP_QOBLUCE *qobluce = new TAP_QOBLUCE;
        bob = (TAP_BBA*) qobluce;
    }
     else 
    {
        TAP_LUCE *luce = new TAP_LUCE; //luce
        bob = (TAP_BBA*) luce;
    }
    TNM_FloatFormat::SetFormat(12, 6);
    bob->StartWatch(5);
	bob->SetConv(1e-12);
	bob->SetInnerConv(1e-12);
	bob->SetMaxInnerIter(20);
	bob->SetLPF(BPRLK);
    string input, output;
    input = path +   name;
    output = path  + name + "_"+ type;
    if(bob->Build(input, output,NETTAPAS)!=0)
	{
		//cout<<"\tFail to build network object"<<endl;
		return 0;
	}
	bob->SetMaxIter(200);
	bob->reportIterHistory = true;
	bob->reportLinkDetail  = true;
	bob->EnableReportOrgFlows(false);
	bob->EnableReportInnerIters();
	bob->EnableEvRGap();
	bob->SetTollType(TT_NOTOLL); 
	if(bob->Solve()== ErrorTerm)  //solve traffic assignment problem
	{
		//cout<<"something wrong happend in solving the problem"<<endl;
		return	0;
	}
	bob->PrepareReport();
	bob->Report();
    return 0;
	
}

#include <stack>

void TestTAP()
{
	string algor, network;
    cout<<"Please specify algorithm name: ";
    cin>>algor;
    cout<<"Please specify network name: ";
    cin>>network;
    TestBBA(algor, "..\\..\\resources\\networks\\tapas\\" + network + "\\", network);
}
int main()
{
	
	//TestNetworkIO();
	TestRandom();
	HTS::TNM_HelloWorld();

	std::stack<int> stack1;
	if (stack1.empty()) {
		std::cout << "111" << std::endl;
	}
	getchar();
	return 0;
}
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>

using namespace std;

void print(vector<vector<int>> data);
void edf(vector<vector<int>> data);
bool sortdeadline( const vector<int>& v1, const vector<int>& v2);

int main()
{
	int n;
	vector<vector<int>> data;
	ifstream fin;
	fin.open("inp-params.txt");
	fin>>n;
	for(int i = 0; i < n; i++)
	{
		vector<int> temp_v;
		for(int j = 0; j < 4; j++)
		{
			int temp;
			fin>>temp;
			temp_v.push_back(temp);
		}
		temp_v.push_back(temp_v[1]);
		temp_v.push_back(temp_v[2]);
		temp_v.push_back(0);
		data.push_back(temp_v);
	}
	edf(data);
	return 0;
}

void print(vector<vector<int>> data)
{
	for(vector<int> x : data)
	{
		for(int y : x)
		{
			cout<<y<<' ';
		}
		cout<<'\n';
	}
}

void edf(vector<vector<int>> data)
{
	ofstream fout;
	fout.open("EDF-Log.txt");
	for(int i = 0; i < data.size(); i++)
	{
		fout<<"Process id: "<<data[i][0]<<"\t:= Processing_time: "<<data[i][1]<<";\tDeadline: "<<data[i][2]<<";\tPeriod: "<<data[i][2]
			<<";\tJoin the system at Time: "<<0<<".\n";
	}
	int processid = data[0][0];
	int backupid = -2;
	int deadline = 0;
	int total_process = 0;
	for(int i = 0; i < data.size(); i++)
	{
		total_process += data[i][3];
	}
	sort(data.begin(),data.end(),sortdeadline);
	long long int time = 1;
	fout<<"Process id: "<<data[0][0]<<" start's executing at time 0.\n";
	while(1)
	{
		int i = 0;
		while((i < data.size()) && (data[i][4] == 0))
		{
			i++;
		}
		if(i == data.size())
		{
			int flag = 0;
			for(int j = 0; j < data.size(); j++)
			{
				if(data[j][3] > 0)
				{
					flag = 1;
					backupid = processid;
					processid = -1;
					if(backupid != processid)
					{
						if(backupid == -1)
						{
							//cout<<(time-1)<<'\t'<<"CPU IDLE"<<'\n';
							fout<<"CPU is IDLE till time : "<<(time-1)<<".\n";
							fout<<"Process id: "<<processid<<" start's executing from time : "<<(time - 1)<<".\n";
						}
						else
						{
							//cout<<(time-1)<<"\t"<<backupid<<'\n';
							fout<<"Process id: "<<backupid<<" executes till time : "<<(time - 1)<<".\n";
							if(processid != -1)
							{
								fout<<"Process id: "<<processid<<" start's executing from time : "<<(time - 1)<<".\n";
							}
							else
							{
								fout<<"CPU is IDLE from time : "<<(time-1)<<".\n";
							}
						}
					}
					break;
				}
			}
			if(flag == 0)
			{
				if(processid == -1)
				{
					//cout<<(time-1)<<'\t'<<"CPU IDLE"<<'\n';
					fout<<"CPU is IDLE till time : "<<(time-1)<<".\n";
				}
				else
				{
					//cout<<(time-1)<<"\t"<<backupid<<'\n';
					fout<<"Process id: "<<backupid<<" executes till time : "<<(time - 1)<<".\n";
				}
				break;
			}
		}
		else
		{
			backupid = processid;
			processid = data[i][0];
			if(backupid != processid)
			{
				if(backupid == -1)
				{
					//cout<<(time-1)<<'\t'<<"CPU IDLE"<<'\n';
					fout<<"CPU is IDLE till time : "<<(time-1)<<".\n";
					fout<<"Process id: "<<processid<<" start's executing from time : "<<(time - 1)<<".\n";
				}
				else
				{
					//cout<<(time-1)<<"\t"<<backupid<<'\n';
					fout<<"Process id: "<<backupid<<" executes till time : "<<(time - 1)<<".\n";
					if(processid != -1)
					{
						fout<<"Process id: "<<processid<<" start's executing from time : "<<(time - 1)<<".\n";
					}
					else
					{
						fout<<"CPU is IDLE from time : "<<(time-1)<<".\n";
					}
				}
			}
			data[i][4]--;
		}
		if(processid != -1)
		{
			for(int i = 0; i < data.size(); i++)
			{
				if(data[i][0] != processid)
				{
					if(data[i][4] != 0)
					{
						data[i][6]++;
					}
				}
			}
		}
		for(int j = 0; j < data.size(); j++)
		{
			if((time % data[j][2] == 0) && (data[j][3] > 0))
			{
				if(data[j][4] != 0)
				{
					//cout<<"deadline miss for Process"<<data[j][0]<<" at time "<<time<<'\n';
					fout<<"Process id: "<<data[j][0]<<" misses it's deadline at time "<<time<<".\n";
					data[j][4] = 0;
					deadline++;
				}
				if((data[j][3] - 1) > 0) 
				{		
						fout<<"Process id: "<<data[j][0]<<"\t:= Processing_time: "<<data[j][1]<<";\tDeadline: "<<data[j][2]
							<<";\tPeriod: "<<data[j][2]<<";\tAgain joins the system at time: "<<time<<".\n";
						data[j][4] = data[j][1];
				}
				data[j][5] += data[j][2];
				data[j][3]--;
			}
		}
		sort(data.begin(),data.end(),sortdeadline);
		time++;
	}
	long long int avg_time = 0;
	for(int i = 0; i < data.size(); i++)
	{
		avg_time += data[i][5];
	}
	fout.close();
	fout.open("EDF-Stats.txt");
	fout<<"1.Number of Processes that came into system: "<<total_process<<" ;\n";
	fout<<"2.Number of Processes that are completed successfully: "<<total_process - deadline<<" ;\n";
	fout<<"3.Number of Processes that missed their deadline: "<<deadline<<" ;\n";
	fout<<"4.Average waiting time for the given set of Processes: "<<((long double)avg_time / (double)total_process)<<" ;\n";
	fout.close();
}

bool sortdeadline( const vector<int>& v1, const vector<int>& v2)
{ 
 return v1[5] < v2[5]; 
}
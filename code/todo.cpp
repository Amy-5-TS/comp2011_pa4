//submit this file ONLY
//if you need to write your own helper functions, write the functions in this file
//again, do NOT include additional libraries and make sure this todo.cpp can be compiled with the unmodified versions of the other given files in our official compilation environment

#include "todo.h"
using namespace std;

char* substring(char* source, int startIndex, int endIndex)
{
    int size = endIndex - startIndex + 1;
    char* s = new char[size+1];
    strncpy(s, source + startIndex, size); //you can read the documentation of strncpy online
    s[size]  = '\0'; //make it null-terminated
    return s;
}

//******** IMPLEMENTATION OF DayStat *******

DayStat::DayStat(){
	cases=0;deaths=0;
}

DayStat::DayStat(int _cases, int _deaths){
	cases=_cases;deaths=_deaths;
}

DayStat::DayStat(const DayStat &d, double denominator){
// d: another DayStat that stores the raw data.
// denominator: It should be area or population.
	deaths=d.deaths/denominator;
	cases=d.cases/denominator;//datas all set need to call funtions to have csvOutputs
}

double DayStat::mortalityRate() const {
// retured value: mortality (percentage of cases that resulted in death).
	if(cases==0){return 0;}
	double moral=(deaths/cases)*100;
	if(moral>100)return -57;
	return moral;
}

double DayStat::getcases() const {
// retured value: cases.
	return cases;
}

double DayStat::getdeaths() const {
// retured value: deaths
	return deaths;
}

//******** IMPLEMENTATION OF Region *******
Region::Region(){
	delete[] name;
	delete[] raw;
	delete[] normPop;
	delete[] normArea;
	delete[] mortality;
}

void Region::readline(char *line){
// line: char array. A single line read from the csv file.
	int i=0;//the index of '\0' in line
	while(line[i]!='\0'){
		i++;
	}
	int numComma=0,pop=0,are=0,a=0;
	for(;a<=i;a++){
		if(numComma==0&&line[a]==','){
			name=substring(line,0,a-1);
			pop=a+1;numComma++;
			//delete[] name;////还要用 删了后记得删掉这行
		}
		else if(numComma==1&&line[a]==','){
			char* pop_=substring(line,pop,a-1);
			population=atoi(pop_);
			delete[] pop_;//as substring allocates, we deallocates
			are=a+1;numComma++;
		}
		else if(numComma==2&&line[a]==','){
			char* area_=substring(line,are,a-1);
			area=atoi(area_);
			delete[] area_;
			numComma++;
		}
		else if(numComma==3&&(line[a]==','||line[a]=='\0')){
			break;
		}
	}//we have the place of where case1 starts implement raw now
	numComma=0;int keep=a;
	while(line[a]!='\0'){
		if(line[a]==',')numComma++;
		a++;
	}
	nday=numComma/2;
	raw=new DayStat[nday];
	normPop=new DayStat[nday];
	normArea=new DayStat[nday];
	mortality=new double[nday];
	bool readE=0;int _cases=0,_deaths=0,curr=0;
	while(line[keep]!='\0'){
		if(line[keep]==','){
			for(int k=keep+1;k<i;k++){
				if(line[k]==','){
					if(!readE){
						char* ca=substring(line,keep+1,k-1);
						_cases=atoi(ca);
						delete[] ca;
						readE=1;
						break;
					}
					else if(readE){
						char* de=substring(line,keep+1,k-1);
						_deaths=atoi(de);
						delete[] de;
						readE=0;
						DayStat temp(_cases,_deaths);//局部变量……？可能离开之后就被销毁了吧
						raw[curr]=temp;//此处可能有memory leak
						curr++;
						break;
					}
				}
			}

		}
		keep++;
	}
	//normalize by...
	//delete[] raw,moralityRate,normpop...;//还要用 删了后记得删掉这行
}

Region::~Region(){

}

void Region::normalizeByPopulation(){
	int pp=0;
	normPop=new DayStat[nday];
	while(pp<nday){
		DayStat PopNormed(raw[pp],population);
		normPop[pp]=PopNormed;
		pp++;
	}
}

void Region::normalizeByArea(){
	int ts=0;
	normArea=new DayStat[nday];
	while(ts<nday){
		DayStat AreaNormed(raw[ts],area);//will automatically deallocated
	    normArea[ts]=AreaNormed;
		ts++;
	}

}
void Region::computeMortalityRate(){
	int pp=0;
	while(pp<nday){
		double a=raw[pp].mortalityRate();//1 region 3 different versions array of mortality rates
		if(a==-57){cout<<name<<endl;mortality[pp]=0;break;}
		else mortality[pp]=a;
		pp++;
	}

}

void Region::write(Stat stat) const {
// stat: one element of the Enum Stat and indicates which kind of data need to be stored in csv files. See definition of Stat. As you need to generate 7 csv files, this function will be called 7 times for each region in writecsvs().

	switch(stat){
	case(0):{
		ofstream out;
	    out.open("CASESRAW.csv",ios::out|ios::app);
	    out<<name<<',';
	    for(int i=0;i<nday;i++){
	    	out<<std::fixed;
	    	out<<std::setprecision(6)<<raw[i].getcases()<<',';
	    }
	    out<<'\0';
	    out.close();
	    break;
	}
	case(1):{
		ofstream out;
	    out.open("DEATHSRAW.csv",ios::out|ios::app);
	    out<<name<<',';
	    for(int i=0;i<nday;i++){
	    	out<<std::fixed;
	    	out<<std::setprecision(6)<<raw[i].getdeaths()<<',';
	    }
	    out<<endl;
	    out.close();
	    break;
	}
	case(2):{
		ofstream out;
	    out.open("CASESPOP.csv",ios::out|ios::app);
	    out<<name<<',';
	    for(int i=0;i<nday;i++){
	    	out<<std::fixed;
	    	out<<std::setprecision(6)<<normPop[i].getcases()<<',';
	    }
	    out<<endl;
	    out.close();
	    break;
	}
	case(3):{
		ofstream out;
	    out.open("DEATHSPOP.csv",ios::out|ios::app);
	    out<<name<<',';
	    for(int i=0;i<nday;i++){
	    	out<<std::fixed;
	    	out<<std::setprecision(6)<<normPop[i].getdeaths()<<',';
	    }
	    out<<endl;
	    out.close();
	    break;
	}
	case(4):{
		ofstream out;
	    out.open("CASESAREA.csv",ios::out|ios::app);
	    out<<name<<',';
	    for(int i=0;i<nday;i++){
	    	out<<std::fixed;
	    	out<<std::setprecision(6)<<normArea[i].getcases()<<',';
	    }
	    out<<endl;
	    out.close();
	    break;
	}
	case(5):{
		ofstream out;
	    out.open("DEATHSAREA.csv",ios::out|ios::app);
	    out<<name<<',';
	    for(int i=0;i<nday;i++){
	    	out<<std::fixed;
	    	out<<std::setprecision(6)<<normArea[i].getdeaths()<<',';
	    }
	    out<<endl;
	    out.close();
	    break;
	}
	case(6):{
		ofstream out;
	    out.open("MORTALITY.csv",ios::out|ios::app);
	    out<<name<<',';
	    for(int i=0;i<nday;i++){
	    	out<<std::fixed;
	    	out<<std::setprecision(6)<<mortality[i]<<',';
	    }
	    out<<endl;
	    out.close();
	    break;
	}
	}

}

//******** IMPLEMENTATION OF FILE I/O FUNCTIONS *******

int readcsv(Region*& region, const char* csvFileName){
//  region: an array of Region. Each element stores the information of one country (or region).
//  csvFileName: path to the csv file.
//  retured value: the length of the region array.
	ifstream fin(csvFileName);
	if(!fin)cerr<<"can't open the file!"<<endl;
	char line[2048];//not dynamic
	int totalRegions=0;
	while(fin.getline(line, 2048)){
		totalRegions++;
	}
	fin.clear();
	fin.seekg(0, ios::beg);
	region=new Region[totalRegions-1];//new
	for(int i=0;i<totalRegions;i++){
		fin.getline(line, 2048);
		if(i==0)continue;
		region[i-1].readline(line);//DayStat related finish

	}
	fin.close();

	//delete[] region;//还要用 删了后记得删掉这行
	return totalRegions-1;

}

void writecsvs(const Region* region, int nRegions){
//  region: an array of Region. Each element stores the information of one country (or region).
//  nRegions: the length of the region array.ofstream outfile;

	int tspp=0;
    while(tspp<nRegions){
    	region[tspp].write(CASESRAW);
    	region[tspp].write(DEATHSRAW);
    	region[tspp].write(CASESPOP);
    	region[tspp].write(DEATHSPOP);
    	region[tspp].write(CASESAREA);
    	region[tspp].write(DEATHSAREA);
    	region[tspp].write(MORTALITY);
    	tspp++;
    	region[tspp].~Region();
    }

}

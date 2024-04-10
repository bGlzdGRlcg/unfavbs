#include "unfavbs.h"

//定义导出文件数量，以及文件名在bin文件内所占字节大小
int filenamesize,filenumber;

int main(int argc,char* argv[]){
	
	//创建output文件夹
	system("md graph_bs");
	system("cls");

	//打开graph_bs.bin文件
	ifstream binfile;
	binfile.open("graph_bs.bin",ios::in | ios::binary);
	
	//从bin文件读入变量filenumber和filenamesize
	filenumber = readbin(binfile);
	filenamesize = readbin(binfile);
	//cout<<filenumber<<" "<<filenamesize<<endl;
	
	/*
		初始化动态数组bin和unbinname
		bin用来存放对应序列号，文件大小以及起始地址
		unbinname用来存放输出的文件名
	*/
	int **bin = new int *[filenumber];
	string *unbinname = new string[filenumber];
	for(int i = 0;i < filenumber;i++) bin[i] = new int[3];
	
	//从bin文件内读入bin数组
	for(int i = 0;i < filenumber;i++) for(int j = 0;j < 3;j++) bin[i][j] = readbin(binfile);
	
	//排序，使得bin可以与unbinname对应
	sort(bin,bin+filenumber,cmp);
	
	//读入unbinname
	for(int i = 0;i < filenumber;i++) unbinname[i] = getbinname(binfile);
	for(int i = 0;i < filenumber;i++) unbinname[i].erase(unbinname[i].length()-1);
	//for(int i = 0;i < filenumber;i++) cout<<bin[i][0]<<" "<<bin[i][1]<<" "<<bin[i][2]<<" "<<unbinname[i]<<" "<<unbinname[i].size()<<endl;

	//定义一个vecoter用来保存表情文件对应下标
	vector<int>face;

	//保存表情文件下标
	//-79 -19 -57 -23为GBK字符: "表情"
	for(int i = 0;i < filenumber;i++) 
		if(int(unbinname[i][unbinname[i].size()-4])==-79 &&
		int(unbinname[i][unbinname[i].size()-3])==-19 &&
		int(unbinname[i][unbinname[i].size()-2])==-57 &&
		int(unbinname[i][unbinname[i].size()-1])==-23)
			face.push_back(i);
			
	//创建一个map映射文件名-->下标
	map<string,int>binmap;
	for(int i = 0;i < filenumber;i++) binmap[unbinname[i]]=i;

	//定义一个vecoter用来保存每个表情的基底
	vector<int>base;

	//计算每个基底的下标
	for(int i=0;i<face.size();i++){
		string temp="";
		for(int t=0;t<unbinname[face[i]].size()-5;t++) temp+=unbinname[face[i]][t];
		if(unbinname[binmap[temp]]==temp) base.push_back(binmap[temp]);
		else {
			for(int t=i;t<face.size()-1;t++) swap(face[t],face[t+1]);
			face.pop_back();
			i--;
		}
	}
	//for(int i=0;i<face.size();i++) cout<<unbinname[face[i]]<<" "<<unbinname[base[i]]<<" "<<face[i]<<endl;

	//清空之前的缓存
	system("del cachebase");
	system("del cacheface");
	system("cls");
	
	//合成立绘并输出到graph_bs文件夹
	for(int i=0;i<base.size();i++){
		//定义两个流basestream和facestream
		ifstream basestream,facestream;

		//定义临时文件流
		ofstream cachebase,cacheface;
		cachebase.open("cachebase",ios::out | ios::binary);
		cacheface.open("cacheface",ios::out | ios::binary);

		//读入basestream
		unbinout(binfile,cachebase,bin[base[i]][1],bin[base[i]][2]+bin[base[i]][1]);
		basestream.open("cachebase",ios::in | ios::binary);

		//读入facestream
		unbinout(binfile,cacheface,bin[face[i]][1],bin[face[i]][2]+bin[face[i]][1]);
		facestream.open("cacheface",ios::in | ios::binary);

		//读入底图解压后的数据大小
		readbin(basestream);
		int basepngsize=readbin(basestream);

		//读取底图的画布大小
		readbin(basestream);
		readbin(basestream);
		readbin(basestream);
		int basepngx=readbinhelf(basestream),basepngy=readbinhelf(basestream);
		readbin(basestream);
		readbin(basestream);
		readbin(basestream);
		readbin(basestream);
		readbin(basestream);
		istringstream basepngstream=decompressData(basestream,44,basepngsize);

		//创建底图
		Mat basepng(basepngy,basepngx, CV_8UC4);
		for(int j = 0; j < basepngy; ++j) for(int k = 0; k < basepngx; ++k) {
        	basepng.at<cv::Vec4b>(j, k)[0] = int(basepngstream.get());
        	basepng.at<cv::Vec4b>(j, k)[1] = int(basepngstream.get());
        	basepng.at<cv::Vec4b>(j, k)[2] = int(basepngstream.get());
        	basepng.at<cv::Vec4b>(j, k)[3] = int(basepngstream.get());
    	}

		//读入表情解压后的数据大小
		readbin(facestream);
		int facepngsize=readbin(facestream);
		readbin(facestream);
		readbin(facestream);
		readbin(facestream);

		//读表情的画布大小以及偏移坐标
		int facepngx=readbinhelf(facestream),facepngy=readbinhelf(facestream),x=readbinhelf(facestream),y=readbinhelf(facestream);
		readbin(facestream);
		
		//读入表情图片数量
		int pngnumber=readbin(facestream);
		readbin(facestream);
		readbin(facestream);
		istringstream facepngstream=decompressData(facestream,44,facepngsize);

		//合成并输出
		string cmdstr="md .\\graph_bs\\"+unbinname[base[i]];
    	system(cmdstr.c_str());
    	system("cls");
    	for(int n=0;n<pngnumber;n++) {
    		Mat facepng(facepngy,facepngx, CV_8UC4);
    		for(int k = 0; k < facepngy; ++k) for(int j = 0; j < facepngx; ++j) {
        		facepng.at<cv::Vec4b>(k, j)[0] = int(facepngstream.get());
        		facepng.at<cv::Vec4b>(k, j)[1] = int(facepngstream.get());
        		facepng.at<cv::Vec4b>(k, j)[2] = int(facepngstream.get());
        		facepng.at<cv::Vec4b>(k, j)[3] = int(facepngstream.get());
    		}
			Mat pngfile;
			overlayImages(basepng,facepng,pngfile,x,y);
    		imwrite(".\\graph_bs\\"+unbinname[base[i]]+'\\'+unbinname[base[i]]+'_'+to_string(n)+".png",pngfile);
		}
		basestream.close();
		facestream.close();
		system("del cachebase");
		system("del cacheface");
		system("cls");
	}

	//释放动态数组
	for(int i = 0;i < filenumber;i++) delete[] bin[i];
	delete[] bin;
	delete[] unbinname;
	
	//关闭bin文件
	binfile.close();
	
	return 0;
}

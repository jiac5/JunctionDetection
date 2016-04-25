#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <math.h>
#include <limits> 
#include "common.h"
#include "mex.h"
using namespace std;


enum Type
{
	positive, negative
};

class Magic
{
private:
	int map[HEIGHT][WIDTH];


void readMat(string fileName, Type type)
{
	cout << "Reading file : " << fileName << endl;

	FILE* fp;
	fp = fopen(fileName.c_str(), "r");


	for(int i=0; i < HEIGHT; i++)
	{	
		for(int j=0; j<WIDTH; j++)
		{
			int num;
			fscanf(fp, "%d,", &num);

			if(num > 0)
			{
				map[i][j] = (type == Type::positive ? 1 : -1);
			}
		}
	}

	fclose(fp);
}

void writeMat(string fileName)
{
	ofstream fout(fileName.c_str());
	for(int i=0; i < HEIGHT; i++)
	{	
		for(int j=0; j<WIDTH; j++)
		{
			fout << (map[i][j] == -1 ? 1 : 0);
			if(j != WIDTH - 1)
				fout << ",";
		}
		fout << endl;
	}
}


// From a point, cast a ray going up and a ray going down
// get a non-zero intersection point for each ray
// check if they are positive or negative
// return num of positive points, so return value ranges 0..2
int get_num_positive_vertical_neighbors(int row, int col)
{
	int ret = 0;
	// go up 
	int r = row-1;
    while(r >= 0 && map[r][col] == -1)
        r--;
    
	while(r >= 0)
	{
		if(map[r][col] != 0)
		{
			if(map[r][col] == 1)
				ret++;
			break;
		}	

		r--;
	}
	if(r < 0)
		ret++;


	r = row + 1;
    
    while(r < HEIGHT && map[r][col] == -1)
        r++;
	while(r < HEIGHT)
	{
		if(map[r][col] != 0)
		{
			if(map[r][col] == 1)
				ret++;
			break;
		}
		r++;
	}

	if(r == HEIGHT)
		ret++;

	return ret;
}	

void dfs(int row, int col, bool* visited, int& count)
{
	if( get_num_positive_vertical_neighbors(row, col) == 2)
		count++; 
	visited[row*WIDTH + col] = true;
	for(int i=row-1; i <= row +1; i++)
		for(int j=col-1; j<=col+1; j++)
		{
			if(i<0 || i >= HEIGHT) 	continue;
			if(j<0 || j >= WIDTH)	continue;
			if(map[i][j] != -1)
				continue;
			if(visited[i * WIDTH + j])
				continue;

			dfs(i, j, visited, count);
		}
}


void removedfs(int row, int col)
{
	map[row][col] = 0;

	for(int i=row-1; i <= row +1; i++)
		for(int j=col-1; j<=col+1; j++)
		{
			if(i<0 || i >= HEIGHT) 	continue;
			if(j<0 || j >= WIDTH)	continue;
			if(map[i][j] != -1)
				continue;
			removedfs(i, j);
		}
}

public:
	void execute()
	{
		cout << "Magic starts" << endl;
        
        bool* visited = new bool[HEIGHT * WIDTH];
        
        for(int i=0; i<HEIGHT; i++)
			for(int j=0; j<WIDTH; j++)
            {
                visited[i*WIDTH + j] = false;
                map[i][j] = 0;
            }
		
		readMat("out_positive.mat", Type::positive);
		readMat("out_negative.mat", Type::negative);

		
		for(int i=0; i<HEIGHT; i++)
			for(int j=0; j<WIDTH; j++)
			{
                if(visited[i * WIDTH + j])
                    continue;
                
                if(map[i][j] != -1)
                    continue;
                
				if(map[i][j] == -1)
				{	
					int count = 0;
					dfs(i, j, visited, count);
                    
                    cout << count << endl;
					if(count < 3)
                    {
						removedfs(i, j);
                        //cout << "Removing " << i << ", " << j << endl;
                    }
				}
			}
		

		writeMat("out_negative_step4.mat");
	}
};


void mexFunction(int nlhs, mxArray *plhs[], int nrhs, 
  const mxArray *prhs[])
{
	Magic magic;
	magic.execute();
}

int main()
{
	Magic magic;
	magic.execute();
	return 0;
}
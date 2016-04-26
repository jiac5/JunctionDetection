#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <math.h>
#include <limits> 
#include <algorithm>
#include <set>
#include "common.h"

#define FOR_MATLAB

#ifdef FOR_MATLAB
#include "mex.h"
#endif
using namespace std;


class HoleFiller
{
private:
	int map[HEIGHT][WIDTH];

	struct Coordinate
	{
		int Row; 
		int Col;

		Coordinate(int r, int c)
		{
			Row = r;
			Col = c;
		}
	};

	vector<Coordinate> endPoints;
	set<int> endPointsSet;


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

	void writeMat(string fileName, Type type)
	{
	    
	    int target_value = (type == Type::negative ? -1:1);
		ofstream fout(fileName.c_str());
		for(int i=0; i < HEIGHT; i++)
		{	
			for(int j=0; j<WIDTH; j++)
			{
				fout << (map[i][j] == target_value ? 1 : 0);
				if(j != WIDTH - 1)
					fout << ",";
			}
			fout << endl;
		}
	}

    void printCoord(Coordinate coord)
    {
        cout << "(" << coord.Row << "," << coord.Col << ")";
    }

	void drawLine(Coordinate startCoord, Coordinate endCoord, Type type)
	{
        // cout << "drawing line from ";
        // printCoord(startCoord);
        // cout << " to ";
        // printCoord(endCoord);
        // cout << endl;
		if(startCoord.Col > endCoord.Col)
			swap(startCoord, endCoord);

		for(int i= startCoord.Col; i <= endCoord.Col; i++)
		{
			double percentage = (i - startCoord.Col) * 1.0 / (endCoord.Col - startCoord.Col);
			int currentRow = startCoord.Row + (endCoord.Row - startCoord.Row) * percentage;
			map[currentRow][i] = (type == Type::negative ? -1 : 1) ;
		}

		//cout << "end" << endl;
	}

bool contains(const vector<Coordinate>& list, const Coordinate& coord)
{
	for(auto c : list)
		if(c.Row == coord.Row && c.Col == coord.Col)
			return true;
	return false;
}

static bool comp(Coordinate c1, Coordinate c2)
{
    return c1.Col < c2.Col; 
}

int coord_to_id(int row, int col)
{
	return row * WIDTH + col;
}

int coord_to_id(Coordinate c)
{
	return c.Row * WIDTH + c.Col;
}

bool isCoordInRange(int row, int col)
{
	return row >= 0 && row < HEIGHT && col >=0 && col < WIDTH;
}

bool isCoordInRange(const Coordinate& coord)
{
	return isCoordInRange(coord.Row, coord.Col);
}

int getDegree(const Coordinate& coord, Type type)
{
	return getDegree(coord.Row, coord.Col, type);
}

int getDegree(int i, int j, Type type)
{
	int degree = 0;
	bool con[3][3];
	

	for(int ii=i-1; ii<=i+1; ii++)
		for(int jj= j-1; jj <= j+1; jj++)
		{
			con[ii-i+1][jj-j+1] = false;
			if(ii == i && jj == j)
				continue;

			if(map[ii][jj] == (type == Type::negative ? -1:1))
			{
				degree++;
				con[ii-i+1][jj-j+1] = true;
			}	
		}

	for(int ii=0; ii<3; ii++)
		for(int jj=0; jj<3; jj++)
		{
			if(con[ii][jj] == false)
				continue;

			if(ii - 1 >= 0 && con[ii-1][jj] == true)
				degree--;
			if(jj - 1 >= 0 && con[ii][jj-1] == true)
				degree--;
		}
	return degree;
}

void bfs(int row, int col, bool& hasNeg, bool* visited, vector<Coordinate>& ptsInside)
{
	vector<Coordinate> current;
	current.push_back(Coordinate(row, col));

	while(!current.empty())
	{
		vector<Coordinate> temp;
		for(auto pt : current)
		{
			if(!isCoordInRange(pt))
				continue;

			if(visited[coord_to_id(pt)])
				continue;

			if(map[pt.Row][pt.Col] == 1) // boundary
			{
				continue;
			}

			if(map[pt.Row][pt.Col] == 0)
				ptsInside.push_back(pt);

			visited[coord_to_id(pt)] = true;

			if(map[pt.Row][pt.Col] == -1)
				hasNeg = true;

			temp.push_back(Coordinate(pt.Row - 1, pt.Col));
			temp.push_back(Coordinate(pt.Row + 1, pt.Col));
			temp.push_back(Coordinate(pt.Row, pt.Col - 1));
			temp.push_back(Coordinate(pt.Row, pt.Col + 1));
		}

		current = temp;
	}
}

void assign_values(const vector<Coordinate>& pts, int val)
{
	for(auto pt : pts)
	{
		map[pt.Row][pt.Col] = val;
	}
}

void fill_holes()
{
	bool* visited = new bool[HEIGHT * WIDTH];
	for(int i=0; i<HEIGHT*WIDTH; i++)
		visited[i] = false;


	for(int i=0; i<HEIGHT; i++)
		for(int j=0; j<WIDTH; j++)
		{
			
			if(visited[coord_to_id(i, j)])
				continue; // visited

			if(map[i][j] != 0)
				continue;

			//cout << i << ", " << j << endl;

			bool hasNeg = false; // if this region has any negative pixels
			vector<Coordinate> ptsInside; // pts on the border, which should all be positive
			
			bfs(i, j, hasNeg, visited, ptsInside);



			if(!hasNeg)
			{
                assign_values(ptsInside, 1);
			}

			//cout << ptsOnBorder.size() << " pts on the boundary" << endl;
			//cout << "Any neg inside ? " << (hasNeg ? "Yes" : "No") << endl;
		}

	delete[] visited;
}


public:
	void execute()
	{
        for(int i=0; i<HEIGHT; i++)
        for(int j=0; j<WIDTH; j++) 
        {
        	map[i][j] = 0;
        }
        
		readMat("out_positive_step5.mat", Type::positive);
		readMat("out_negative_step5.mat", Type::negative);

		fill_holes();


		writeMat("out_negative_step6.mat", Type::negative);
        writeMat("out_positive_step6.mat", Type::positive);
	}
};

int main()
{
	HoleFiller l;
	l.execute();
}


#ifdef FOR_MATLAB
void mexFunction(int nlhs, mxArray *plhs[], int nrhs, 
  const mxArray *prhs[])
{
	HoleFiller l;
	l.execute();
}
#endif
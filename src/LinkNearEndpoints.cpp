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

class Linker
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

void bfs(Coordinate start, Type type)
{
    //cout << "bfs " << start.Row << "," << start.Col << endl;
	vector<Coordinate> current;
	current.push_back(start);

	bool* visited = new bool[HEIGHT * WIDTH];
    for(int i = 0; i< HEIGHT*WIDTH; i++)
        visited[i] = false;

	while(current.size() != 0)
	{
		vector<Coordinate> temp;

		for(auto c : current)
		{
            //cout << "c : (" << c.Row << ", " << c.Col << ")" << endl;
            if(abs(c.Row - start.Row) > 10)
                continue;
            
            if(c.Row < 0 || c.Row >= HEIGHT)
                continue;
            
            if(c.Col < 0 || c.Col >= WIDTH)
                continue;

			if(map[c.Row][c.Col] == (type == Type::negative ? 1 : -1)) 
			// if meet with the opposite type
				continue;

			if(visited[c.Row * WIDTH + c.Col])
				continue;

			if(
                    (c.Row != start.Row || c.Col != start.Col)&&
                    endPointsSet.find(c.Row * WIDTH + c.Col) != endPointsSet.end())
			{
               
                if(tryLinkEndpoints(start, c, type))
                {
                    delete[] visited;
                    return;
                }
			}

			visited[c.Row * WIDTH + c.Col] = true;

            temp.push_back(Coordinate(c.Row-1, c.Col));
            temp.push_back(Coordinate(c.Row+1, c.Col));
            temp.push_back(Coordinate(c.Row, c.Col-1));
            temp.push_back(Coordinate(c.Row, c.Col+1));
		}
		current = temp;
	}

	delete[] visited;
}


bool tryLinkEndpoints(Coordinate startCoord, Coordinate endCoord, Type type)
{
    
    // if the slope is too large, don't link them
    //if(abs(startCoord.Row - endCoord.Row) > 4 * abs(startCoord.Col - endCoord.Col))
    //    return false;
    
    //cout << "Trying to link (" << startCoord.Row << "," << startCoord.Col << ")";
    //cout << " and (" << endCoord.Row << "," << endCoord.Col << endl;
    
	// check if there is any positive pixel in the way from start to end
	if(startCoord.Col > endCoord.Col)
		swap(startCoord, endCoord);
	bool bBlocked = false;
	for(int i= startCoord.Col+2; i < endCoord.Col-1; i++)
	{
		double percentage = (i - startCoord.Col) * 1.0 / (endCoord.Col - startCoord.Col);
		int currentRow = startCoord.Row + (endCoord.Row - startCoord.Row) * percentage;

		if(map[currentRow][i] != 0)
			bBlocked = true;
        
		if(currentRow - 1 >= 0 && map[currentRow-1][i] != 0)
			bBlocked = true;
		if(currentRow + 1 < HEIGHT && map[currentRow+1][i] != 0)
			bBlocked = true;
	}


	if(!bBlocked)
	{
        cout << "Linking (" << startCoord.Row << "," << startCoord.Col << ")";
        cout << " and (" << endCoord.Row << "," << endCoord.Col << endl;
		drawLine(startCoord, endCoord, type);
		endPointsSet.erase(startCoord.Row * WIDTH + startCoord.Col);
		endPointsSet.erase(endCoord.Row * WIDTH + endCoord.Col);
        return true;
	}
    
    return false;
}

void drawLine(Coordinate startCoord, Coordinate endCoord, Type type)
{
	if(startCoord.Col > endCoord.Col)
		swap(startCoord, endCoord);

	for(int i= startCoord.Col+1; i < endCoord.Col; i++)
	{
		double percentage = (i - startCoord.Col) * 1.0 / (endCoord.Col - startCoord.Col);
		int currentRow = startCoord.Row + (endCoord.Row - startCoord.Row) * percentage;
		map[currentRow][i] = (type == Type::negative ? -1 : 1) ;
        
        //cout << currentRow << ", " << i << endl;
	}
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

void findEndpoints(Type type)
{
	int searchTarget = (type == Type::negative ? -1 : 1);

	endPoints.clear();
    endPointsSet.clear();

	for(int i=0; i<HEIGHT; i++)
	for(int j=0; j<WIDTH; j++)
	{
		if(map[i][j] != searchTarget)
			continue;

		int degree = getDegree(i, j, type);

		if(degree == 1)
			endPoints.push_back(Coordinate(i, j));
	}

	cout << endPoints.size() << " endpoints found" << endl;

	for(auto pt : endPoints)
	{
		endPointsSet.insert(pt.Row * WIDTH + pt.Col);
	}
}

bool contains(const vector<Coordinate>& list, const Coordinate& coord)
{
	for(auto c : list)
		if(c.Row == coord.Row && c.Col == coord.Col)
			return true;
	return false;
}

void dfs(Coordinate coord, 
        vector<Coordinate>& pts, vector<Coordinate>& ends)
{
    //cout << "dfs : " << coord.Row << "," << coord.Col << endl;
    int row = coord.Row, col = coord.Col;
    pts.push_back(coord);
    
    if(endPointsSet.find(row * WIDTH + col) != endPointsSet.end())
    {    
        endPointsSet.erase(row * WIDTH + col);
        ends.push_back(coord);
    }
    for(int i=row-1; i<= row+1; i++)
        for(int j=col-1; j<=col+1; j++)
        {
            if(i<0 || i >= HEIGHT || j < 0 || j >= WIDTH)
                continue;
            if(map[i][j] != -1)
                continue;

            auto next = Coordinate(i, j);
            if(contains(pts, next))
            	continue;
            
            dfs(next, pts, ends);
        }
}

static bool comp(Coordinate c1, Coordinate c2)
{
    return c1.Col < c2.Col; 
}

void break_turnaround()
{
    findEndpoints(Type::negative);
    
    
    for(auto pt : endPoints)
    {
        if(endPointsSet.find(pt.Row * WIDTH + pt.Col) == endPointsSet.end())
            continue; // already processed
        
        vector<Coordinate> pts; // all points in this segment
        vector<Coordinate> ends; // endpoints of this segment
        
        dfs(pt, pts, ends);
        
        sort(pts.begin(), pts.end(), comp);
        
        if(ends.size() == 2)
        {
            // check if it is turnaround
            //    ------|
            //          |
            //----------|
            int right_ends = max(ends[0].Col, ends[1].Col);
            int left_ends = min(ends[0].Col, ends[1].Col);
            
            int right_all = pts[pts.size() - 1].Col;
            int left_all = pts[0].Col;
            
            if(right_all - right_ends > 30)
            {
                for(int i=pts.size()-5; i<pts.size(); i++)
                    map[pts[i].Row][pts[i].Col] = 0; // remove the 5 rightmost points
            }
            
            if(left_ends - left_all > 30)
            {
                for(int i=0; i < 5; i++)
                    map[pts[i].Row][pts[i].Col] = 0; // remove the 5 leftmost points
            }
        }
    }
    
    cout << "Done : break turnarounds" << endl;
}

void link_near_endpoints(Type type)
{
		findEndpoints(type);
        
		for(auto pt : endPoints)
		{
			if(endPointsSet.find(pt.Row * WIDTH + pt.Col) == endPointsSet.end())
				continue;
			bfs(pt, type);
		}
        
        cout << "Done : link near endpoints " << (type == Type::negative ? "negative" : "positive") << endl;
}

public:
	void execute()
	{
        for(int i=0; i<HEIGHT; i++)
        for(int j=0; j<WIDTH; j++) 
        {map[i][j] = 0;}
        
		readMat("out_positive.mat", Type::positive);
		readMat("out_negative_step4.mat", Type::negative);


        link_near_endpoints(Type::negative);
        link_near_endpoints(Type::positive);
        break_turnaround();

		writeMat("out_negative_step5.mat", Type::negative);
        writeMat("out_positive_step5.mat", Type::positive);
	}
};

int main()
{
	Linker l;
	l.execute();
}


#ifdef FOR_MATLAB
void mexFunction(int nlhs, mxArray *plhs[], int nrhs, 
  const mxArray *prhs[])
{
	Linker l;
	l.execute();
}
#endif
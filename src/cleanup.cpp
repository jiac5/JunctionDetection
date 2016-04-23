#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <math.h>
#include <limits> 

#include "mex.h"
using namespace std;




class Cleaner
{
public:
	double type = 1;

private:
	// Rabbit
	const static int height = 1026, width = 1388;

// Hawk
// const int height = 1395, width = 1137;


int map[height][width];

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


void readMat(string fileName)
{
	cout << "Reading file : " << fileName << endl;

	FILE* fp;
	fp = fopen(fileName.c_str(), "r");


	for(int i=0; i < height; i++)
	{	
		for(int j=0; j<width; j++)
		{
			int num;
			fscanf(fp, "%d,", &num);

			map[i][j] = num;
		}
	}

	fclose(fp);
}

void writeMat(string fileName)
{
	ofstream fout(fileName.c_str());
	for(int i=0; i < height; i++)
	{	
		for(int j=0; j<width; j++)
		{
			fout << map[i][j];
			if(j != width - 1)
				fout << ",";
		}
		fout << endl;
	}
}

int getDegree(int i, int j)
{
	int degree = 0;
	bool con[3][3];
	

	for(int ii=i-1; ii<=i+1; ii++)
		for(int jj= j-1; jj <= j+1; jj++)
		{
			con[ii-i+1][jj-j+1] = false;
			if(ii == i && jj == j)
				continue;

			if(map[ii][jj] > 0)
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

void findIntersectionAndEndPoints(vector<Coordinate>& intersections, vector<Coordinate>& endPoints)
{
	intersections.clear();
	endPoints.clear();

	for(int i=0; i<height; i++)
	for(int j=0; j<width; j++)
	{
		if(map[i][j] < 1)
			continue;

		int degree = getDegree(i, j);
		
		if(degree > 2)
			intersections.push_back( Coordinate(i, j) );

		if(degree == 1)
			endPoints.push_back(Coordinate(i, j));
	}

	cout << intersections.size() << " intersections" << endl;
	cout << endPoints.size() << " endPoints" << endl;
}

void writeCoordinates(string fileName, vector<Coordinate> coords)
{
	ofstream fout(fileName.c_str());
	for(auto coord : coords)
	{
		// +1 as matlab uses 1 based coordinates
		fout << coord.Row+1 << "," << coord.Col+1 << endl;
	}
}

bool contains(const vector<Coordinate>& list, const Coordinate& coord)
{
	for(auto c : list)
		if(c.Row == coord.Row && c.Col == coord.Col)
			return true;
	return false;
}

void removeShortBranch(int length, vector<Coordinate>& endPoints, vector<Coordinate>& intersections)
{
	cout << "Removing branches whose length < " << length << endl;

	for(auto endPoint : endPoints)
	{
		
		vector<Coordinate> path;

		path.push_back(endPoint);

		auto cur = endPoint;

		while(path.size() < length)
		{
			int r = cur.Row, c = cur.Col;
			Coordinate next(-1,-1);
			int neighborCount = 0;
			for(int i=r-1; i<=r+1; i++)
				for(int j=c-1; j<=c+1; j++)
				{
					if(i == r && j == c)
						continue;
					if(i < 0 || i >= height || j < 0 || j >= width)
						continue;

					Coordinate coord(i, j);
					if(contains(path, coord))
						continue;

					if(map[i][j] > 0)
					{
						next = coord;
						neighborCount++;
					}
				}

			if(neighborCount > 1) // cur is a intersection
			{
				for(auto node : path)
				{
					map[node.Row][node.Col] = 0;
				}
				break;
			}
			else if(neighborCount == 0)
			{
				break;
			}
			else // neighbor count == 1
			{
				path.push_back(cur);
				cur = next;
			}
		}
	}
}

bool isVertical(Coordinate& start, Coordinate& end)
{
	if(start.Col == end.Col)
		return true;

	if(abs(start.Row - end.Row) > abs(start.Col - end.Col) )
		return true;
	return false;

}

void RemovePath(std::vector<Coordinate> path, int startIndex)
{
	// the first one is the start point
	for(int i=startIndex; i<path.size(); i++)
	{
		auto coord = path[i];
		map[coord.Row][coord.Col] = 0;
	}
}

bool isNeighbor(Coordinate& center, Coordinate& pt)
{
	if(abs(pt.Row - center.Row) > 1)
		return false;
	if(abs(pt.Col - center.Col) > 1)
		return false;
	if(pt.Row == center.Row && pt.Col == center.Col)
		return false;
	return true;
}

bool isMeetSpecialRequirement(Coordinate& center, Coordinate& current)
{
	Coordinate left = Coordinate(current.Row, current.Col - 1);
	Coordinate right = Coordinate(current.Row, current.Col + 1);
	Coordinate up = Coordinate(current.Row-1, current.Col);
	Coordinate down = Coordinate(current.Row+1, current.Col);

	int count = 0;
	if(isNeighbor(center, left) && map[left.Row][left.Col])
		count++;
	if(isNeighbor(center, right) && map[right.Row][right.Col])
		count++;
	if(isNeighbor(center, up) && map[up.Row][up.Col])
		count++;
	if(isNeighbor(center, down) && map[down.Row][down.Col])
		count++;

	return count == 0;
}

void dfs(Coordinate coord, vector<Coordinate>& path)
{
	
	int degree = getDegree(coord.Row, coord.Col);

	//cout << "dfs in " << coord.Row << "," << coord.Col << " with degree : " << degree << endl;
	if(degree > 2) // intersection
		return;

	path.push_back(coord);
	if(degree == 1) // end point
		return;
	
	int r = coord.Row, c = coord.Col;
	for(int i=r-1; i <= r+1; i++)
		for(int j=c-1; j<=c+1; j++)
		{

			//cout << i << " " << j << endl;
			if(i<0 || i>=height || j<0 || j>=width)
				continue;

			if(i == r && j == c)
				continue;

			if(map[i][j] < 1)
				continue;

			Coordinate current(i, j);
			if(contains(path, current))
				continue;

			// deal with a special case here 
			// X
			// XX

			if(isMeetSpecialRequirement(coord, current))	
				dfs(current, path);
		}
}


void breakIntersections(vector<Coordinate>& intersections)
{
	cout << "Breaking intersections" << endl;
	for(auto intersection : intersections)
	{
		int r = intersection.Row, c = intersection.Col;
		if(getDegree(r, c) <= 2)
			continue;

		for(int i = r-1; i <= r+1; i++)
		for(int j= c-1; j <= c+1; j++)
		{
			if(i<0 || i>=height || j<0 || j>=width)
				continue;

			if(i == r && j == c)
				continue;

			if(map[i][j] < 0.1)
				continue;


			vector<Coordinate> path;
			path.push_back(intersection);
			dfs(Coordinate(i, j), path);	

			if(path.size() > 2 && isVertical(intersection, path[path.size() - 1]))
				RemovePath(path, 1);
		}		
	}
}

// the return value indicates if at the end reach a end point
bool dfs2(Coordinate coord, vector<Coordinate>& path, int depth )
{
	int degree = getDegree(coord.Row, coord.Col);
	if(degree > 2) // intersection
		return false;

	path.push_back(coord);
	if(depth > 0 && degree == 1) // end point
		return true;
	
	int r = coord.Row, c = coord.Col;
	for(int i=r-1; i <= r+1; i++)
		for(int j=c-1; j<=c+1; j++)
		{

			//cout << i << " " << j << endl;
			if(i<0 || i>=height || j<0 || j>=width)
				continue;

			if(i == r && j == c)
				continue;

			if(map[i][j] < 1)
				continue;

			Coordinate current(i, j);
			if(contains(path, current))
				continue;

			if(dfs2(current, path, depth + 1))
				return true;
			else
				return false;
		}

	return false;
}

void removeTrivialSegment(vector<Coordinate>& endPoints, int threshold)
{
	for(auto pt : endPoints)
	{

		vector<Coordinate> path;
		if(dfs2(pt, path, 0 ) && path.size() < threshold)
		{
			//cout << "removing end points " << path.size() << endl;
			RemovePath(path, 0);
		}
	}
}

void dfs_for_remove_intersection(const Coordinate& current, int depth, vector<Coordinate>& path, const vector<Coordinate> visited)
{
	if(depth == 0)
		return;

	path.push_back(current);
	
	int r = current.Row, c = current.Col;
	for(int i=r-1; i <= r+1; i++)
		for(int j=c-1; j<=c+1; j++)
		{
			if(i<0 || i>=height || j<0 || j>=width)
				continue;

			if(i == r && j == c)
				continue;

			if(map[i][j] < 1)
				continue;

			Coordinate temp(i, j);
			if(contains(path, temp))
				continue;

			if(contains(visited, temp))
				continue;

			dfs_for_remove_intersection(temp, depth - 1, path, visited);
			return;
		}
}

void printPath(const vector<Coordinate>& path)
{
	for(auto c : path)
	{
		cout << "(" << c.Row << ", " << c.Col << ")";
	}

	cout << endl;
}


void removeIntersection(const vector<Coordinate>& intersections)
{
	
	for(auto coord : intersections)
	{

		vector<Coordinate> visited;
		vector<vector<Coordinate>> paths;
		int r = coord.Row, c = coord.Col;
		for(int i=r-1; i <= r+1; i++)
		for(int j=c-1; j<=c+1; j++)
		{

			//cout << i << " " << j << endl;
			if(i<0 || i>=height || j<0 || j>=width)
				continue;

			if(i == r && j == c)
				continue;

			if(map[i][j] < 1)
				continue;

			vector<Coordinate> path;
			Coordinate current(r, c);
			path.push_back(current);
			dfs_for_remove_intersection(Coordinate(i,j), 10, path, visited);
			path.erase(path.begin());
			if(path.size() > 0)
			{
				paths.push_back(path);

				// workaround there
				for(auto cc : path)
					visited.push_back(cc);
			}
		}

		// if this is not an intersection, return
		if(paths.size() < 3)
		{
			//cout << "not a intersection" << endl;
			return;
		}

		// find the path that is most close to vertical
		double largestSlope = -9999;
		int chosenIndex = -1;
		for(int i=0; i<paths.size(); i++)
		{
			auto lastCoord = paths[i][paths[i].size()-1];

			if(fabs(lastCoord.Col - coord.Col) < 0.1)
			{	
				cout << "vertical path found, " << i << endl;
				chosenIndex = i;
				break;
			}
			double slope = fabs((double)(lastCoord.Row - coord.Row) / (double)(lastCoord.Col - coord.Col));
			//printPath(paths[i]);
			//cout << "slope:" << slope << endl;
			if(slope > largestSlope)
			{
				largestSlope = slope;
				chosenIndex = i;
			}
		}

		//cout <<"chosenIndex: " << chosenIndex << endl;
		if(chosenIndex != -1)
		{
			auto path = paths[chosenIndex];
			//cout << "Removing a path with " << path.size() << " nodes" << endl;
			for(auto c : path)
			{
				map[c.Row][c.Col] = 0;
			}
		}
	}	
}

void mergeNearEndPoints(vector<Coordinate>& endPoints)
{
	cout << "merging near end points" << endl;
	vector<pair<Coordinate, Coordinate>> toMerge;

	int n = endPoints.size();
	for(int i=0; i<n; i++)
		for(int j=i+1; j<n; j++)
		{
			auto p1 = endPoints[i];
			auto p2 = endPoints[j];

			if( abs(p1.Row - p2.Row) <= 6 && abs(p1.Col - p2.Col) < 15)
			{
				toMerge.push_back(make_pair(p1, p2));
			}
		}

	for(auto pair : toMerge)
	{
		auto startCoord = pair.first;
		auto endCoord = pair.second;
		if(pair.first.Col > pair.second.Col)
			swap(startCoord, endCoord);

		for(int i= startCoord.Col+1; i < endCoord.Col; i++)
		{
			double percentage = (i - startCoord.Col) * 1.0 / (endCoord.Col - startCoord.Col);
			int currentRow = startCoord.Row + (endCoord.Row - startCoord.Row) * percentage;
			map[currentRow][i] = 1;
		}
	}
}

public:
int clean()
{
    string fileName = "negative.mat";
    string outputFileName = "out_negative.mat";
    string endPintsFileName = "endpoints_negative.mat";
    string intersectionsFileName = "intersections_negative.mat";
    if(type == 1)
    {
        fileName = "positive.mat";
        outputFileName = "out_positive.mat";
        endPintsFileName = "endpoints_positive.mat";
        intersectionsFileName = "intersections_positive.mat";
    }
    
    readMat(fileName);

    vector<Coordinate> intersections;
    vector<Coordinate> endPoints;


    findIntersectionAndEndPoints(intersections, endPoints);

    int intersectionsCount = intersections.size();
    while(true)
    {
        removeShortBranch(5, endPoints, intersections);
        findIntersectionAndEndPoints(intersections, endPoints);

        if(intersections.size() == intersectionsCount)
            break;
        intersectionsCount = intersections.size();
    }

    if(type == 2)
    {
        while(true)
        {
            breakIntersections(intersections);
            findIntersectionAndEndPoints(intersections, endPoints);
            if(intersectionsCount == intersections.size())
                break;
            intersectionsCount = intersections.size();
        }
    }

    removeTrivialSegment(endPoints, 6);
    findIntersectionAndEndPoints(intersections, endPoints);

    
    if(type == 2)
    {
        while(intersections.size() > 0)
        {	
            removeIntersection(intersections);
            findIntersectionAndEndPoints(intersections, endPoints);
        }
    }

    removeTrivialSegment(endPoints, 15);
    findIntersectionAndEndPoints(intersections, endPoints);

    mergeNearEndPoints(endPoints);
    findIntersectionAndEndPoints(intersections, endPoints);

    intersectionsCount = intersections.size();
    while(true)
    {
        removeShortBranch(5, endPoints, intersections);
        findIntersectionAndEndPoints(intersections, endPoints);

        if(intersections.size() == intersectionsCount)
            break;
        intersectionsCount = intersections.size();
    }

    writeMat(outputFileName);
    writeCoordinates(endPintsFileName, endPoints);
    writeCoordinates(intersectionsFileName, intersections);
	return 0;
}

};



void mexFunction(int nlhs, mxArray *plhs[], int nrhs, 
  const mxArray *prhs[])
{
    if(nrhs < 1)
    {
        cout << "Error: cleanup type not specified" << endl;
        cout << "1:positive   2: negative" << endl;
        return;
    }

    Cleaner cleaner;
    cleaner.type = *mxGetPr(prhs[0]);
    cleaner.clean();
}
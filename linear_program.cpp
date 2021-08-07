#include <iostream>
#include <algorithm>
#include <vector>
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include <cmath>
#include <set>
//#include <map>
#include <fstream>
#include <sstream>
#include "matplotlibcpp.h"
namespace plt = matplotlibcpp;

using namespace std;

class Point {
	private: 
		float x;
		float y;
	public:
		Point(){
			this->x = 0;
			this->y = 0;
		}
		Point(float x,float y){
			this->x = x;
			this->y = y;
		}
		float getX(){
			return this->x;
		}
		float getY(){
			return this->y;
		}
		void setX(float x){
			this->x = x;
		}
		void setY(float x){
			this->y = y;
		}		
};

class KMeans{
	private:
		int no_of_clusters;
		vector<Point*> points;
		vector<Point> centroids;
		vector<int> clusters; 
	public:
		KMeans(int clusters,int size, vector<vector<float> *>& arr){
			this->no_of_clusters = clusters;
			
			int i = 1;
			while(i <= size){
				this->points.push_back(new Point(arr[i-1]->at(0),arr[i-1]->at(1)));
				i++;
			}
			
			/* initialize random seed: */
  			srand (time(NULL));
			
			/* generate random number between 0 and length of the vector array of points */
			i = 0;
			vector<int> initial_centroids_index;
			while(true){
				int index = rand() % this->points.size(); 
				/***** checks for duplication of random number *****/
				if(	find(initial_centroids_index.begin(), initial_centroids_index.end(), index) == initial_centroids_index.end())  
				{
					initial_centroids_index.push_back(index);
					this->centroids.push_back(Point(this->points[index]->getX(),this->points[index]->getY()));
					i++;
				}	
				if(i == clusters){
					break;
				}
			}
				
		}
	    
		vector<Point> get_initital_centroids(){
	    	return this->centroids;
		}
		
		/**** 
			calculating euclidean distance between two points 
		****/
		float calcDistance(Point* P1, Point P2){
			float X = (P1->getX() - P2.getX());
			float X_square = pow(X,2);
			
			float Y = (P1->getY() - P2.getY());
			float Y_square = pow(Y,2);
			
			return sqrt(X_square + Y_square);
		}
		
		/**** 	
			calculates the closest centroids and pushes it into the 'distance' array. 
			At the end, corresponding minimum centroid index is pushed into the 'assigned_centroid_index' array.
			
			returns the centroids index of the corresponding points
		****/
		
		vector<int> findClosestCentroids(){
			
			vector<int> assigned_centroid_index;
			for (int i=0;i<this->points.size();i++){
				vector<float> distance;
				for(int j=0;j<this->centroids.size();j++){
					distance.push_back(calcDistance(this->points[i],this->centroids[j]));
				}
				
				vector<float>::iterator it = min_element(distance.begin(), distance.end());
				int min_index = std::distance(distance.begin(),it);
				
				assigned_centroid_index.push_back(min_index);
			}
			return assigned_centroid_index;
		}
		
		void calcCentroids(){
			vector<Point> new_centroids;
			vector<int> old_centroids = this->findClosestCentroids();
			vector<pair<Point*,int> > points_with_cluster;
				
			for(int i=0;i<this->points.size();i++){
				points_with_cluster.push_back(make_pair(this->points[i],old_centroids[i]));
			}
			
									
			set<int> S (old_centroids.begin(),old_centroids.end());

			set<int>::iterator it;
			for (it = S.begin(); it != S.end(); ++it) {
   
				vector<Point*>  cluster_points;
				for(int j=0;j<points_with_cluster.size();j++){
					
					if(points_with_cluster[j].second == *it){
						cluster_points.push_back(new Point(points_with_cluster[j].first->getX(),points_with_cluster[j].first->getY()));
					}
				}
			
				float X = 0;
				float Y = 0;
				for(int i=0;i<cluster_points.size();i++){
					X += cluster_points[i]->getX();
					Y += cluster_points[i]->getY();
				}
				
				Point cluster_mean = Point(X/cluster_points.size(),Y/cluster_points.size());
				
				new_centroids.push_back(cluster_mean);
			}
			
			
			this->centroids.clear();
			this->clusters.assign(old_centroids.begin(),old_centroids.end());
			this->centroids.assign(new_centroids.begin(),new_centroids.end());
			
		}
		vector<int> getClusters(){
			return this->clusters;
		}
		
		~KMeans(){
			for(int i=0;i< this->points.size();i++){
				delete this->points[i];
				//Points[i] = NULL;
			}
			this->points.clear();
			
		}
};

int main(){
	/***********************
		
		Initialization step starts
	
	************************/	
	ifstream file("data.csv");
	vector<vector <float>*> data;
	string line;
	int t = 0;
	int no_of_points = 0;
	while (getline(file,line)){
		no_of_points++;
		stringstream numbers(line);
		vector<float> pts;
	        string values;
	        while (getline(numbers, values,',')){
	       	pts.push_back(stof(values));
	        }
	        data.push_back(new vector<float> (pts));
	}
	
	int clusters;
	KMeans* K_means= new KMeans(clusters = 3, no_of_points, data);
	
  	/***********************
		
		Initialization step ends
	
	***********************/
	
	for(int i=0;i<10;i++){
		K_means->calcCentroids();
	}
	
	vector<Point> centroids = K_means -> get_initital_centroids();
	
	vector<float> x_centroids ;
	vector<float> y_centroids ;
	
	vector<float> x_points ;
	vector<float> y_points ;
	
	int s;
	for(int i=0;i<centroids.size();i++){
		x_centroids.push_back(centroids[i].getX());
		y_centroids.push_back(centroids[i].getY());
	}
	for(int i=0;i<data.size();i++){
		x_points.push_back(data[i]->at(0));
		y_points.push_back(data[i]->at(1));
	}
	
	for(int i=0;i<centroids.size();i++){
		cout << "("<< centroids[i].getX() << "," << centroids[i].getY() << ")\n" ;
	}
	
	plt::scatter_colored(x_points, y_points, K_means->getClusters() , s=30);
	plt::scatter(x_centroids, y_centroids, s=100, {{"color" , "red"}});
	plt::show();
	
	
	delete K_means;

}

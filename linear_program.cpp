#include <iostream>
#include <algorithm>
#include <vector>
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include <cmath>
#include <set>
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
	public:
		KMeans(int clusters, vector<vector<float> >& arr){
			this->no_of_clusters = clusters;
			
			int i = 1;
			while(i<=30){
				this->points.push_back(new Point(arr[i-1][0],arr[i-1][1]));
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
			this->centroids.assign(new_centroids.begin(),new_centroids.end());
			
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
		
		Initialization step
	
	************************/
	int rows = 30;
	float array[30][2] = {{25,79},{34,51},{22,53},{27,78},{33,59},{33,74},{31,73},{22,57},{35,69},{34,75},{67,51},{54,32},{57,40},{43,47},{50,53},{57,36},{59,35},{52,58},{65,59},{47,50},
						 {49,25},{48,20},{35,14},{33,12},{44,20},{45,5},{38,29},{43,27},{51,8},{46,7}};
	vector<vector<float> > data(rows,vector<float> (2,0));
	
	for(int i=0;i<rows;i++)
		for(int j=0;j<2;j++)
			data[i][j] = array[i][j];
	
	int clusters;
	KMeans* K_means= new KMeans(clusters = 3,data);	
	
  	/***********************
		
		Initialization step 
	
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
	string color;
	for(int i=0;i<centroids.size();i++){
		x_centroids.push_back(centroids[i].getX());
		y_centroids.push_back(centroids[i].getY());
	}
	
	for(int i=0;i<data.size();i++){
		x_points.push_back(data[i][0]);
		y_points.push_back(data[i][1]);
	}
	
	for(int i=0;i<centroids.size();i++){
		cout << "("<< centroids[i].getX() << "," << centroids[i].getY() << ")\n" ;
	}
	
	plt::scatter(x_points,y_points,s=30);
	plt::scatter(x_centroids,y_centroids,s=100);
	plt::show();
	
	//cout << centroids.size() << '\n';
	
	
	delete K_means;

}

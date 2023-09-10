#include <iostream>
#include <algorithm>
#include <vector>
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include <cmath>
#include <set>
#include <omp.h>
#include <fstream>
#include <sstream>
#include "matplotlibcpp.h"


#define ITERATIONS 100
#define NO_OF_CLUSTERS 3
#define CENTROIDS_SIZE 100
#define POINTS_SIZE 30
#define CENTROIDS_COLOR "red"

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
            // allocate memory to the vector
            this->points.resize(size);

			this->no_of_clusters = clusters;
			
			int i = 1;
            #pragma omp parallel for schedule(static)
            for (int i=0; i< size; i++){
               this->points[i] = new Point(arr[i]->at(0),arr[i]->at(1)); 
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
			vector<int> assigned_centroid_index(this->points.size());
			#pragma omp parallel for schedule(static)
			for (int i=0;i<this->points.size();i++){	
				vector<float> distance;	
				for(int j=0;j<this->centroids.size();j++){
					distance.push_back(calcDistance(this->points[i],this->centroids[j]));
				}
				vector<float>::iterator it = min_element(distance.begin(), distance.end());
				int min_index = std::distance(distance.begin(),it);
				assigned_centroid_index[i] = min_index;
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
		
		int getClusterOfPoint(int i){
			return this->clusters[i];
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
	omp_set_nested(1);
	// start wall clock time of the program
	double start = omp_get_wtime();	
	
	/***********************
		
		Initialization step starts
	
	************************/
		
	ifstream file("./dataset/data.csv");
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
	KMeans* K_means= new KMeans(clusters = NO_OF_CLUSTERS, no_of_points, data);
	
  	/***********************
		
		Initialization step ends
	
	***********************/  
	
	#pragma omp parallel for schedule(static)
	for(int i=0;i<ITERATIONS;i++){
		K_means->calcCentroids();	
	}
	
	vector<Point> centroids = K_means -> get_initital_centroids();
	
	vector<float> x_centroids ;
	vector<float> y_centroids ;
	
	vector<float> x_points(data.size()) ;
	vector<float> y_points(data.size());
	
	vector<int> ClusterOfPoints(data.size()) ;
	
	int s;
	
	for(int i=0;i<centroids.size();i++){
		x_centroids.push_back(centroids[i].getX());
		y_centroids.push_back(centroids[i].getY());
	}

	#pragma omp parallel for schedule(static)
	for(int i=0;i<data.size();i++){
		x_points[i] = data[i]->at(0);
		y_points[i] = data[i]->at(1);
		ClusterOfPoints[i] = K_means->getClusterOfPoint(i);
	}
	
	for(int i=0;i<centroids.size();i++){
		cout << "("<< centroids[i].getX() << "," << centroids[i].getY() << ")\n" ;
	}
	
	double end = omp_get_wtime();	
	cout << "Elapsed time = " << end - start << " sec\n" ; 
	
	plt::scatter_colored(x_points, y_points, ClusterOfPoints , s=POINTS_SIZE);
	plt::scatter(x_centroids, y_centroids, s=CENTROIDS_SIZE, {{"color" , CENTROIDS_COLOR}});
	plt::show();
	
	
	delete K_means;

}


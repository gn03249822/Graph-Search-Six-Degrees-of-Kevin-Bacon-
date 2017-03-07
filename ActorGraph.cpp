/*
 * ActorGraph.cpp
 * Author: Daniel Huang
 * Date:   3/02/2017
 *
 * This file is meant to exist as a container for starter code that you can use to read the input file format
 * defined in movie_casts.tsv. Feel free to modify any/all aspects as you wish.
 */

#include <fstream>

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <iterator>
#include "ActorGraph.h"


using namespace std;

ActorGraph::ActorGraph() {
    //this->actorList=new  unordered_map<string,Actor*>();
    //this->movieList=new unordered_map<string,Movie*>();
}

//public getter
unordered_map<string,Actor*>* ActorGraph:: getActorList() {
    return this->actorList;
}

//public getter
unordered_map<string,Movie*>* ActorGraph:: getMovieList() {
    return this->movieList;
}

bool ActorGraph::loadFromFile(const char* in_filename, bool use_weighted_edges) {
    //TODO: use_weighted_edges not yet implemented!!!
    
    // Initialize the file stream
    ifstream infile;
    infile.open(in_filename);
    bool have_header = false;
    // keep reading lines until the end of file is reached
    while (infile) {
        string s;
        
        // get the next line
        if (!getline( infile, s )) break;
        
        
        if (!have_header) {
            // skip the header
            have_header = true;
            continue;
        }
        
        istringstream ss( s );
        vector <string> record;
        
        
        while (ss) {
            string next;
            
            // get the next string before hitting a tab character and put it in 'next'
            if (!getline( ss, next, '\t' )) break;
            
            record.push_back( next );
        }
        
        if (record.size() != 3) {
            // we should have exactly 3 columns
            continue;
        }
        
        string actor_name(record[0]);
        string movie_title(record[1]);
        int movie_year = stoi(record[2]);
        // we have an actor/movie relationship, now what?
        
        Actor* newActor=new Actor(actor_name);
        
        
        Movie* newMovie=new Movie(movie_title,movie_year);
        //Concatenate the name and year of the movie for comparison
        string namePlusYear=newMovie->getName()+"#@"+to_string(movie_year);
        
        pair<string,Actor*> actorPair (newActor->getName(), newActor);
        
        
        if(movieList->find(namePlusYear)==movieList->end()) {
            //if movie is not yet in the list, add it to the movieList
            
            pair<string,Movie*> moviePair (namePlusYear,newMovie);
            this->movieList->insert(moviePair);
            
            
            unordered_map<string,Movie*>::iterator it=this->getMovieList()->find(namePlusYear);
            
           
            
            //if the actor is already in the actor ist
            if(getActorList()->find(actor_name)!=actorList->end()){
                std::pair<Movie*,Actor*> pair((*it).second, (*actorList->find(actor_name)).second);

                (actorList->find(actor_name))->second->edges.push_back(pair);
                (*it).second->addToCast((*actorList->find(actor_name)).second); //add the actor to the cast of the movie
                

            }
            else{
               
                this->actorList->insert(actorPair); // insert the actor to the list
                unordered_map<string,Actor*>::iterator it2=this->getActorList()->find(actor_name);

                pair<Movie*,Actor*> pair((*it).second,(*it2).second);

                newActor->edges.push_back(pair); //add the movie to the actor's edges
                (*it).second->addToCast((*it2).second);
            }
            
        }
        else { //the movie is already added in the list previously,
            // then just go ahead and add the actor to its cast
            unordered_map<string,Movie*>::iterator it=this->getMovieList()->find(namePlusYear);
           
        
            
            //if the actor is already in the actor ist
            if(actorList->find(actor_name)!=actorList->end()){
                pair<Movie*,Actor*> pair((*it).second, (actorList->find(actor_name))->second);
                (actorList->find(actor_name))->second->edges.push_back(pair);
                 (*it).second->addToCast(actorList->find(actor_name)->second);
            }
            else{
                std::pair<Movie*,Actor*> pair((*it).second, newActor);
                this->actorList->insert(actorPair);
                newActor->edges.push_back(pair); //add the movie to the actor's edges
                (*it).second->addToCast(newActor);
            }
            
            
        }
        
        free(newActor);
        free(newMovie);
    }
    //at theis point, we should have all the actors and movies stores to our two sets
    
    if (!infile.eof()) {
        cerr << "Failed to read " << in_filename << "!\n";
        return false;
    }
    infile.close();
    
    return true;
    
}




/** Build the graph with the information read from the file
 *  PRECONDITION: loadFromFile has been called
 *  POSTCONDITION: A graph representation of movies and actors is built
 */
void ActorGraph::buildGraph() {
    
    //iterate through every movie in the movieList and check their casts
    for(unordered_map<string,Movie*>::iterator it=this->getMovieList()->begin(); it!=this->getMovieList()->end(); it++){
               
        if((*it).second->getCast()->size()>1) {
            //if there are more than one actor in a movie, connect them all with edges
            
            for(int i=0; i<(*it).second->getCast()->size();i++) {
                
                for(int j=0; j<(*it).second->getCast()->size();j++){
                    if(i!=j) {
                        
                        for(int k=0; k<(*it).second->getCast()->at(i)->edges.size();k++){
                           
                            
                            if((*it).second->getCast()->at(i)->edges[k].first==(*it).second){
                                
                                
                                std::pair<Movie*,Actor*> pair((*it).second->getCast()->at(i)->edges.at(k).first, (*it).second->getCast()->at(j));
                                (*it).second->getCast()->at(i)->edges.at(k)=pair;
                                
                                std::pair<Movie*,Actor*> pair2((*it).second->getCast()->at(j)->edges[k].first, (*it).second->getCast()->at(i));
                                
                                
                                (*it).second->getCast()->at(j)->edges[k]=pair2;
                                
                                
                            }
                        }
                        
                        
                    }
                }
            }
            
            
        }
    }
    
}


/** The bread-first search algorithm that would traverse the graph and find the
 *  shortest path from the start actor s
 */
void ActorGraph::BFS(Actor* s) {
    queue<Actor*> queue; //initialize the queue to store actor nodes
    queue.push(s);
    Actor* curr;
    s->setDist(0);
    
    while(!queue.empty()) {
        curr=queue.front(); //set the curr node
        queue.pop();
        
        for(int i=0; i<curr->edges.size();i++) {
            //for each of curr's neighbors, n
            if(curr->edges.at(i).second->getDist()==-1){
                curr->edges.at(i).second->setDist((curr->getDist())+1);
                curr->edges.at(i).second->prev=curr;
                queue.push(curr->edges.at(i).second); //enqueue n
            }
        }
    }
    //at this point we have finished the search and done exploring from S
}



ActorGraph::~ActorGraph() {
    delete movieList;
    delete actorList;
    
}


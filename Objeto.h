#ifndef OBJECTO_H
#define OBJECTO_H
#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

class Objeto
{
	private:
		
		
	public:
		vector<float> vertice;
		//vector<Vertice> vn;
		int faces;
		vector<int> indices;
			//Objeto(vector<Vertice> vertice, vector<Face> face);
		Objeto(vector<float> vertiec, int faces, vector<int> indices);
		Objeto(string objFile);
		vector<string> separa(string linea, char separador);
		int cantidadCaras();


		//vector<Face> getFace();
		vector<float> getVertice();
		//Face getFace(int index);
		float getVertice(int index);
		int getCantidadVertices();

};


#endif


#include "Objeto.h"

#include <vector>
#include <fstream>
#include <sstream>
#include <iostream> 
#include <string>

using namespace std;


float Objeto::getVertice(int index)
{
	return this->vertice[index];
}

int Objeto::getCantidadVertices()
{
	return this->vertice.size();
}

vector<float> Objeto::getVertice()
{
	return this->vertice;
}

int Objeto::cantidadCaras()
{
	return this->indices.size();
}

Objeto::Objeto(vector<float> vertice, int faces ,vector<int> indices)
{
	this->vertice = vertice;
	this->faces = faces;
	this->indices = indices;
}

Objeto::Objeto(string objFile)
{
	ifstream file(objFile);
	

	if (file.is_open())
	{
		string line;
		int count = 0;
		//cout << line << endl;
		while(getline(file, line))
		{
			//vector<string> vec_line;
			vector<string> vec_line = separa(line, ' ');
			if (vec_line[0] == "v")
			{
				float x = stod(vec_line[1]);
				float y = stod(vec_line[2]);
				float z = stod(vec_line[3]);
				//Vertice ver(x, y, z);

				
				vertice.push_back(x);
				vertice.push_back(y);
				vertice.push_back(z);

				//cout << "v " << "\t" << vertice[count].getX() << " " << "\t" << vertice[count].getY() << " " << "\t" << vertice[count].getZ() << endl;
				count++;
			}
			else if (vec_line[0] == "f")
			{
				vector<int> caras;
				vector<string> numeros;
				int vector;
				//int vn;

				for (int i = 1; i < vec_line.size(); i++)
				{
					//cout << vec_line[i] << endl;
					 numeros = separa(vec_line[i], '/');
					 vector = stoi(numeros[0]);
					 this->indices.push_back(vector);
					 //vn = stoi(numeros[2]);
					 //caras.push_back(vn);
					 //cout << numeros[0] << " ";
				}
				//cout << endl;
				//this->indices.push_back(caras);

				//Face facesitas(caras);
				//this->face.push_back(facesitas);
			}
		}
		//this->vn.resize(this->face.size());
		file.close();
	}
	else
	{
		printf("Ocurrió un error!\n");
	}
}

vector<string> Objeto::separa(string linea, char separador)
{
	vector<string> tokens;
	stringstream ss(linea);
	string word;

	while (!ss.eof()) {
		getline(ss, word, separador);
		tokens.push_back(word);
	}

	return tokens;
}

